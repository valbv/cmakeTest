#pragma once

#include <QObject>
#include <QStack>

#include "MovaviWidgetLib.h"

class QUndoCommand;

/**
 * @brief MUndoStack по поведению подобен QUndoStack.
 * Но предоставляет вложенные транзакции и возможность undo / redo во время транакции
 * Если какого-то функционала QUndoStack не хватает, его можно добавить
 */
class MOVAVIWIDGET_API MUndoStack : public QObject
{
	Q_OBJECT

	class TransactionCommand;

public:
	MUndoStack(QObject *parent = 0);
	~MUndoStack();

	bool canRedo() const;
	bool canUndo() const;

	void push(QUndoCommand *);

public slots:
	void undo();
	void redo();

	void    startTransaction();
	void   commitTransaction();
	void rollbackTransaction();

	void clear();

protected:
	int count() const;
	QUndoCommand * command(int index);

	void clearNoDelete(); // Очистить список комманд, но не удалять их

private:
	void removeUndoCommands();
	void removeRedoCommands();

	const MUndoStack * currentStack() const;
	      MUndoStack * currentStack();

	void undoImpl();
	void redoImpl();

	bool canRedoImpl() const;
	bool canUndoImpl() const;

	void pushImpl(QUndoCommand *);
private:
	QStack<QUndoCommand *> m_undoCommands;
	QStack<QUndoCommand *> m_redoCommands;
	QStack<MUndoStack   *> m_transactions;
};
