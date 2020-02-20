#include "MRuntimeStylesheetEditor.h"

#include <algorithm>
#include <functional>

#include <QApplication>
#include <QFileDialog>
#include <QFile>
#include <QPlainTextEdit>
#include <QFontMetrics>
#include <QPainter>
#include <QTextBlock>
#include <QPointer>
#include <QStyle>

#include "ui_MRuntimeStylesheetEditor.h"

namespace {

constexpr int LINE_NUMBER_AREA_MARGIN = 3;
constexpr int CODE_EDITOR_TAB_WIDTH   = 8;

class SearchDialog;

class CodeEditor : public QPlainTextEdit
{
public:
	using SaveCallback = std::function<void()>;

	CodeEditor(QWidget *parent = nullptr);

	void RepaintLineNumbersArea(const QRect &rect);
	int LineNumbersAreaWidth();
	void ShowSearchDialog();
	void SetSaveCallback(const SaveCallback &saveCallback);

protected:
	void resizeEvent(QResizeEvent *event) override;
	void changeEvent(QEvent *event) override;
	void keyPressEvent(QKeyEvent *event) override;
	void hideEvent(QHideEvent *event) override;

private:
	enum class IndentOperation
	{
		IncreaseLevel,
		DecreaseLevel,
	};

	void UpdateLineNumbersAreaWidth();
	void UpdateLineNumbersArea(const QRect &, int);
	bool ToggleCommentSelection();
	bool IndentSelectedLines(IndentOperation operation);

private:
	QWidget *m_lineNumberArea;
	QPointer<SearchDialog> m_searchDialog;
	SaveCallback m_saveCallback;
};

class LineNumberArea : public QWidget
{
public:
	LineNumberArea(CodeEditor *editor)
		: QWidget(editor)
		, m_codeEditor(editor)
	{}

	QSize sizeHint() const override
	{
		return QSize(m_codeEditor->LineNumbersAreaWidth(), m_codeEditor->sizeHint().height());
	}

protected:
	void paintEvent(QPaintEvent *event) override
	{
		m_codeEditor->RepaintLineNumbersArea(event->rect());
	}

private:
	CodeEditor *m_codeEditor;
};

class SearchDialog : public QDialog
{
public:
	SearchDialog(CodeEditor *editor)
		: QDialog(editor)
		, m_codeEditor(editor)
		, m_searchLineEdit(new QLineEdit(this))
		, m_searchPushButton(new QPushButton("Find", this))
		, m_findBackwardCheckBox(new QCheckBox("Find Backward", this))
		, m_findCaseSensitivelyCheckBox(new QCheckBox("Find Case Sensitively", this))
		, m_findWholeWordsCheckBox(new QCheckBox("Find Whole Words", this))
	{
		setWindowTitle("Search");
		setWindowFlags(Qt::Dialog
					 | Qt::MSWindowsFixedSizeDialogHint
					 | Qt::CustomizeWindowHint
					 | Qt::WindowTitleHint
					 | Qt::WindowCloseButtonHint);
		QVBoxLayout *layout = new QVBoxLayout(this);
		layout->addWidget(m_searchLineEdit);
		layout->addWidget(m_findBackwardCheckBox);
		layout->addWidget(m_findCaseSensitivelyCheckBox);
		layout->addWidget(m_findWholeWordsCheckBox);
		layout->addWidget(m_searchPushButton);
		connect(m_searchPushButton, &QPushButton::clicked, this, &SearchDialog::OnSearchClicked);
	}

	void OnSearchClicked()
	{
		const QString searchString = m_searchLineEdit->text();
		if(searchString.isEmpty())
			return;
		QTextDocument *document = m_codeEditor->document();
		QTextCursor cursor = m_codeEditor->textCursor();
		QTextDocument::FindFlags flags;
		if(m_findBackwardCheckBox->isChecked())
			flags |= QTextDocument::FindBackward;
		if(m_findCaseSensitivelyCheckBox->isChecked())
			flags |= QTextDocument::FindCaseSensitively;
		if(m_findWholeWordsCheckBox->isChecked())
			flags |= QTextDocument::FindWholeWords;
		QTextCursor newCursor = document->find(searchString, cursor, flags);
		if(!newCursor.isNull())
			m_codeEditor->setTextCursor(newCursor);
	}

	void SetText(const QString &text)
	{
		m_searchLineEdit->setText(text);
	}

protected:
	void showEvent(QShowEvent *event) override
	{
		m_searchLineEdit->setFocus();
		QDialog::showEvent(event);
	}

private:
	CodeEditor *m_codeEditor;
	QLineEdit *m_searchLineEdit;
	QPushButton *m_searchPushButton;
	QCheckBox *m_findBackwardCheckBox;
	QCheckBox *m_findCaseSensitivelyCheckBox;
	QCheckBox *m_findWholeWordsCheckBox;
};

CodeEditor::CodeEditor(QWidget *parent)
	: QPlainTextEdit(parent)
	, m_lineNumberArea(new LineNumberArea(this))
{
	connect(this, &QPlainTextEdit::blockCountChanged, this, &CodeEditor::UpdateLineNumbersAreaWidth);
	connect(this, &QPlainTextEdit::updateRequest,     this, &CodeEditor::UpdateLineNumbersArea);
	UpdateLineNumbersAreaWidth();
}

int CodeEditor::LineNumbersAreaWidth()
{
	int digits = 1;
	for(int max = qMax(1, blockCount()); max >= 10; max /= 10)
		digits++;
	return 2 * LINE_NUMBER_AREA_MARGIN + fontMetrics().width('M') * digits;
}

void CodeEditor::ShowSearchDialog()
{
	if(m_searchDialog.isNull())
		m_searchDialog = new SearchDialog(this);
	const QString selectedText = textCursor().selectedText();
	if(!selectedText.isEmpty())
		m_searchDialog->SetText(selectedText);
	const bool dialogIsHidden = m_searchDialog->isHidden();
	m_searchDialog->show();
	m_searchDialog->raise();
	m_searchDialog->activateWindow();
	if(dialogIsHidden)
	{
		const QSize dialogHalfSize = m_searchDialog->frameSize() / 2;
		const QPoint dialogLocalPos = geometry().center() - QPoint(dialogHalfSize.width(), dialogHalfSize.height());
		m_searchDialog->move(mapToGlobal(dialogLocalPos));
	}
}

void CodeEditor::SetSaveCallback(const CodeEditor::SaveCallback &saveCallback)
{
	m_saveCallback = saveCallback;
}

void CodeEditor::UpdateLineNumbersAreaWidth()
{
	setViewportMargins(LineNumbersAreaWidth(), 0, 0, 0);
}

void CodeEditor::UpdateLineNumbersArea(const QRect &rect, int dy)
{
	if(dy)
		m_lineNumberArea->scroll(0, dy);
	else
		m_lineNumberArea->update(0, rect.y(), m_lineNumberArea->width(), rect.height());
	if(rect.contains(viewport()->rect()))
		UpdateLineNumbersAreaWidth();
}

bool CodeEditor::ToggleCommentSelection()
{
	QTextCursor cursor = textCursor();
	if(!cursor.hasSelection())
		return false;

	int startPos = cursor.anchor();
	int endPos = cursor.position();
	if(startPos > endPos)
		std::swap(startPos, endPos);

	static const QString beginComment = "/*";
	static const QString endComment = "*/";

	cursor.setPosition(startPos, QTextCursor::MoveAnchor);
	cursor.setPosition(startPos + beginComment.length(), QTextCursor::KeepAnchor);
	const QString startText = cursor.selectedText();

	cursor.setPosition(endPos, QTextCursor::MoveAnchor);
	cursor.setPosition(endPos - endComment.length(), QTextCursor::KeepAnchor);
	const QString endText = cursor.selectedText();

	cursor.beginEditBlock();
	if(startText == beginComment && endText == endComment)
	{
		cursor.setPosition(startPos, QTextCursor::MoveAnchor);
		cursor.setPosition(startPos + beginComment.length(), QTextCursor::KeepAnchor);
		cursor.removeSelectedText();

		cursor.setPosition(endPos - beginComment.length(), QTextCursor::MoveAnchor);
		cursor.setPosition(endPos - beginComment.length() - endComment.length(), QTextCursor::KeepAnchor);
		cursor.removeSelectedText();

		cursor.setPosition(startPos, QTextCursor::MoveAnchor);
		cursor.setPosition(endPos - beginComment.length() - endComment.length(), QTextCursor::KeepAnchor);
	}
	else
	{
		cursor.setPosition(startPos, QTextCursor::MoveAnchor);
		cursor.insertText(beginComment);

		cursor.setPosition(endPos + beginComment.length(), QTextCursor::MoveAnchor);
		cursor.insertText(endComment);

		cursor.setPosition(startPos, QTextCursor::MoveAnchor);
		cursor.setPosition(endPos + beginComment.length() + endComment.length(), QTextCursor::KeepAnchor);
	}
	cursor.endEditBlock();

	setTextCursor(cursor);
	return true;
}

bool CodeEditor::IndentSelectedLines(IndentOperation operation)
{
	QTextCursor cursor = textCursor();
	if(!cursor.hasSelection())
		return false;

	int startPos = cursor.anchor();
	int endPos = cursor.position();
	if(startPos > endPos)
		std::swap(startPos, endPos);

	cursor.setPosition(startPos, QTextCursor::MoveAnchor);
	if(cursor.atBlockEnd())
	{
		cursor.movePosition(QTextCursor::NextBlock, QTextCursor::MoveAnchor);
		cursor.movePosition(QTextCursor::StartOfBlock, QTextCursor::MoveAnchor);
	}
	const int startBlock = cursor.block().blockNumber();
	startPos = cursor.anchor();

	cursor.setPosition(endPos, QTextCursor::MoveAnchor);
	if(cursor.atBlockStart())
	{
		cursor.movePosition(QTextCursor::PreviousBlock, QTextCursor::MoveAnchor);
		cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::MoveAnchor);
	}
	const int endBlock = cursor.block().blockNumber();
	endPos = cursor.anchor();

	if(startPos >= endPos)
		return false;

	static const QString indentString = "\t";

	cursor.setPosition(startPos, QTextCursor::MoveAnchor);
	cursor.beginEditBlock();
	for(int i = 0, blocksDelta = endBlock - startBlock; i <= blocksDelta; ++i)
	{
		cursor.movePosition(QTextCursor::StartOfBlock, QTextCursor::MoveAnchor);
		switch(operation)
		{
		case IndentOperation::IncreaseLevel:
			if(!cursor.block().text().simplified().isEmpty())
				cursor.insertText(indentString);
			break;
		case IndentOperation::DecreaseLevel:
			if(cursor.block().text().startsWith(indentString))
			{
				cursor.setPosition(cursor.position() + indentString.length(), QTextCursor::KeepAnchor);
				cursor.removeSelectedText();
			}
			break;
		}
		cursor.movePosition(QTextCursor::NextBlock, QTextCursor::MoveAnchor);
	}
	cursor.endEditBlock();

	cursor.setPosition(startPos, QTextCursor::MoveAnchor);
	cursor.movePosition(QTextCursor::StartOfBlock, QTextCursor::MoveAnchor);
	while(cursor.block().blockNumber() < endBlock)
		cursor.movePosition(QTextCursor::NextBlock, QTextCursor::KeepAnchor);
	cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);

	setTextCursor(cursor);
	return true;
}

void CodeEditor::resizeEvent(QResizeEvent *event)
{
	QPlainTextEdit::resizeEvent(event);
	const QRect cr = contentsRect();
	m_lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), LineNumbersAreaWidth(), cr.height()));
}

void CodeEditor::changeEvent(QEvent *event)
{
	QPlainTextEdit::changeEvent(event);
	if(event->type() == QEvent::StyleChange)
		setTabStopWidth(CODE_EDITOR_TAB_WIDTH * fontMetrics().width('M'));
}

void CodeEditor::keyPressEvent(QKeyEvent *event)
{
	if(event->matches(QKeySequence::Find))
		ShowSearchDialog();
	else if(m_saveCallback && event->matches(QKeySequence::Save))
		m_saveCallback();
	else if(event->key() == Qt::Key_Slash && event->modifiers().testFlag(Qt::ControlModifier) && ToggleCommentSelection())
		return;
	else if(((event->key() == Qt::Key_Tab && event->modifiers().testFlag(Qt::ShiftModifier)) || (event->key() == Qt::Key_Backtab)) && IndentSelectedLines(IndentOperation::DecreaseLevel))
		return;
	else if(event->key() == Qt::Key_Tab && IndentSelectedLines(IndentOperation::IncreaseLevel))
		return;
	else
		QPlainTextEdit::keyPressEvent(event);
}

void CodeEditor::hideEvent(QHideEvent *event)
{
	if(!m_searchDialog.isNull())
		m_searchDialog->close();
	QPlainTextEdit::hideEvent(event);
}

void CodeEditor::RepaintLineNumbersArea(const QRect &rect)
{
	QPainter painter(m_lineNumberArea);
	painter.fillRect(rect, Qt::lightGray);
	painter.setPen(Qt::black);

	QTextBlock block = firstVisibleBlock();
	int blockNumber = block.blockNumber();
	int top = static_cast<int>(blockBoundingGeometry(block).translated(contentOffset()).top());
	int bottom = top + static_cast<int>(blockBoundingRect(block).height());

	while(block.isValid() && top <= rect.bottom())
	{
		blockNumber++;
		if(block.isVisible() && bottom >= rect.top())
		{
			const QRect textRect = QRect(0, top, m_lineNumberArea->width() - LINE_NUMBER_AREA_MARGIN, fontMetrics().height());
			painter.drawText(textRect, Qt::AlignRight, QString::number(blockNumber));
		}
		block = block.next();
		top = bottom;
		bottom = top + static_cast<int>(blockBoundingRect(block).height());
	}
}

} // namespace

MRuntimeStylesheetEditor::MRuntimeStylesheetEditor(QWidget *parent)
	: QDialog(parent)
	, ui(new Ui::MRuntimeStylesheetEditor)
{
	ui->setupUi(this);
	setWindowFlags(Qt::Window);

	CodeEditor *styleTextEdit = new CodeEditor(ui->styleTextFrame);
	m_styleTextEdit = styleTextEdit;
	ui->styleTextFrame->layout()->addWidget(styleTextEdit);
	styleTextEdit->SetSaveCallback([this]{ applyStyle(); });

	connect(ui->browseFile,     &QAbstractButton::clicked, this, &MRuntimeStylesheetEditor::browseFile);
	connect(ui->loadFromFile,   &QAbstractButton::clicked, this, &MRuntimeStylesheetEditor::loadFromFile);
	connect(ui->saveToFile,     &QAbstractButton::clicked, this, &MRuntimeStylesheetEditor::saveToFile);
	connect(ui->applyStyle,     &QAbstractButton::clicked, this, &MRuntimeStylesheetEditor::applyStyle);
	connect(ui->readAppStyle,   &QAbstractButton::clicked, this, &MRuntimeStylesheetEditor::readStyle);
	connect(ui->autoApplyStyle, &QAbstractButton::toggled, this, &MRuntimeStylesheetEditor::setAutoApplyStyle);
	connect(ui->search,         &QAbstractButton::clicked, styleTextEdit, &CodeEditor::ShowSearchDialog);

	readStyle();
	setStyleSheet(getAntiStyle());

	styleTextEdit->setFocus();
}

MRuntimeStylesheetEditor::~MRuntimeStylesheetEditor()
{
}

QString MRuntimeStylesheetEditor::getAntiStyle()
{
	QString retval;
	if(!ui->protectStyleEditor->isChecked())
		return retval;
	retval += "/*BEGIN_ANTISTYLE*/";
	retval += "MRuntimeStylesheetEditor, MRuntimeStylesheetEditor * "
	"{"
	"color: black;"
	"background: white;"
	"selection-background-color: darkblue;"
	"selection-color: white;"
#ifndef Q_OS_MAC
	"font-family: \"Courier\"; font-size: 10pt;"
#else
	"font-family: \"Courier\"; font-size: 12pt;"
#endif
	"}"
	;
	retval += "/*END_ANTISTYLE*/";
	return retval;
}

void MRuntimeStylesheetEditor::browseFile()
{
	auto filename = QFileDialog::getSaveFileName();
	if(filename.isEmpty())
		return;
	ui->filenameLineEdit->setText(filename);
}

void MRuntimeStylesheetEditor::loadFromFile()
{
	auto filename =ui->filenameLineEdit->text();
	if(filename.isEmpty())
		return;

	QFile file(filename);
	if(!file.open(QFile::ReadOnly))
		return;

	auto filetext = QString::fromUtf8(file.readAll());
	m_styleTextEdit->setPlainText(filetext);
}

void MRuntimeStylesheetEditor::saveToFile()
{
	auto filename = ui->filenameLineEdit->text();
	if(filename.isEmpty())
		return;

	QFile file(filename);
	if(!file.open(QFile::WriteOnly))
		return;

	auto filetext = m_styleTextEdit->toPlainText();
	file.write(filetext.toUtf8());
}

void MRuntimeStylesheetEditor::applyStyle()
{
	qApp->setStyleSheet(m_styleTextEdit->toPlainText());// + getAntiStyle());
	qApp->style()->unpolish(qApp);
	qApp->style()->polish(qApp);

	for (const auto w : qApp->allWidgets())
	{
//		if(!w->parentWidget())
		{
			w->update();
			w->repaint();
		}
	}
	this->setStyleSheet(getAntiStyle());
}

void MRuntimeStylesheetEditor::readStyle()
{
	m_styleTextEdit->setPlainText(qApp->styleSheet());//.remove(getAntiStyle()));
}

void MRuntimeStylesheetEditor::setAutoApplyStyle(bool enable)
{
	if(enable)
		connect(m_styleTextEdit, &QPlainTextEdit::textChanged, this, &MRuntimeStylesheetEditor::applyStyle);
	else
		disconnect(m_styleTextEdit, &QPlainTextEdit::textChanged, this, &MRuntimeStylesheetEditor::applyStyle);
}
