#pragma once

#include <QDialog>
#include <QScopedPointer>

#include "MovaviWidgetLib.h"

class QPlainTextEdit;

namespace Ui { class MRuntimeStylesheetEditor; }

class MOVAVIWIDGET_API MRuntimeStylesheetEditor : public QDialog
{
	Q_OBJECT

public:
	MRuntimeStylesheetEditor(QWidget *parent = 0);

	~MRuntimeStylesheetEditor();

private:
	/// @brief Возвращает стили для защиты своего окна от стилей приложения
	QString getAntiStyle();

private slots:
	void browseFile();
	void loadFromFile();
	void saveToFile();
	void applyStyle();
	void readStyle();
	void setAutoApplyStyle(bool enable);

private:
	QScopedPointer<Ui::MRuntimeStylesheetEditor> ui;
	QPlainTextEdit *m_styleTextEdit;
};
