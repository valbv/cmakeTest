#include "MUndoStack.h"

#include <cassert>

#include <QUndoCommand>

class MUndoStack::TransactionCommand : public QUndoCommand
{
	QStack<QUndoCommand *> commands;

public:
	TransactionCommand() { }
	~TransactionCommand()
	{
		for (auto command : commands)
			delete command;
		commands.clear();
	}

	void undo() override
	{
		for (int i = commands.size() - 1; i >= 0; --i)
			commands[i]->undo();
	}
	void redo() override
	{
		for (int i = 0; i < commands.size(); ++i)
			commands[i]->redo();
	}
	void push(QUndoCommand * cmd)
	{
		commands.push(cmd);
	}
	bool isEmpty()
	{
		return commands.isEmpty();
	}
};


MUndoStack::MUndoStack(QObject *parent)
	: QObject(parent)
{ }

MUndoStack::~MUndoStack()
{
	clear();
}

void MUndoStack::undo()
{
	if (!canUndo())
		return;

	auto stack = currentStack();
	stack->undoImpl();
}

void MUndoStack::redo()
{
	if (!canRedo())
		return;

	auto stack = currentStack();
	stack->redoImpl();
}

bool MUndoStack::canUndo() const
{
	return currentStack()->canUndoImpl();
}

bool MUndoStack::canRedo() const
{
	return currentStack()->canRedoImpl();
}

void MUndoStack::push(QUndoCommand * command)
{
	currentStack()->pushImpl(command);
}

void MUndoStack::startTransaction()
{
	m_transactions.push(new MUndoStack(this));
}

void MUndoStack::commitTransaction()
{
	bool transactionAvailable = !m_transactions.isEmpty();
	assert(transactionAvailable);
	if (!transactionAvailable)
		return;

	auto transaction = m_transactions.pop();
	auto command     = new TransactionCommand;

	for (int i = 0; i < transaction->count(); ++i)
		command->push(transaction->command(i));

	if (!command->isEmpty()) push(command);
	else                     delete command;

	transaction->clearNoDelete();
	delete transaction;
}

void MUndoStack::rollbackTransaction()
{
	bool transactionAvailable = !m_transactions.isEmpty();
	assert(transactionAvailable);
	if (!transactionAvailable)
		return;

	auto transaction = m_transactions.pop();
	while(transaction->canUndo())
		transaction->undo();
	delete transaction;
}

void MUndoStack::clear()
{
	removeUndoCommands();
	removeRedoCommands();
}

int MUndoStack::count() const
{
	return m_undoCommands.size();
}

QUndoCommand * MUndoStack::command(int index)
{
	bool indexValid = index >= 0 && index < count();
	assert(indexValid);
	if (!indexValid)
		return nullptr;

	return m_undoCommands[index];
}

void MUndoStack::clearNoDelete()
{
	m_undoCommands.clear();
	m_redoCommands.clear();
}

void MUndoStack::removeUndoCommands()
{
	while(!m_undoCommands.isEmpty())
		delete m_undoCommands.pop();
}

void MUndoStack::removeRedoCommands()
{
	while (!m_redoCommands.isEmpty())
		delete m_redoCommands.pop();
}

const MUndoStack * MUndoStack::currentStack() const
{
	return !m_transactions.isEmpty()
		? m_transactions.top()
		: this;
}

MUndoStack * MUndoStack::currentStack()
{
	return !m_transactions.isEmpty()
		? m_transactions.top()
		: this;
}

void MUndoStack::undoImpl()
{
	if (m_undoCommands.isEmpty())
		return;

	auto command = m_undoCommands.pop();
	m_redoCommands.push(command);
	command->undo();
}

void MUndoStack::redoImpl()
{
	if (m_redoCommands.isEmpty())
		return;

	auto command = m_redoCommands.pop();
	m_undoCommands.push(command);
	command->redo();
}

bool MUndoStack::canUndoImpl() const
{
	return !m_undoCommands.isEmpty();
}

bool MUndoStack::canRedoImpl() const
{
	return !m_redoCommands.isEmpty();
}

void MUndoStack::pushImpl(QUndoCommand * command)
{
	m_undoCommands.push(command);
	removeRedoCommands();
}
