#pragma once

#include <QResizeEvent>
#include <QWidget>

/// @brief Данный шаблон позволяет любой виджет разместить поверх другого
/// с автоматической подстройкой размера.
/// Может быть использован для отображения полупрозрачного виджета поверх другого.
/// @note WIDGET должен быть наследником QWidget.
template <class WIDGET> class TOverlayWidget : public WIDGET
{
public:
	template<typename ...ARGS> explicit TOverlayWidget(ARGS... args) : WIDGET(args...)
	{
		auto parent = this->parentWidget();
		if (!parent)
			return;
		parent->installEventFilter(this);
		this->raise();
	}
protected:
	// Catches resize and child events from the parent widget&
	bool eventFilter(QObject * obj, QEvent * ev) override
	{
		if (obj == this->parent())
		{
			if (ev->type() == QEvent::Resize)
			{
				QResizeEvent * rev = static_cast<QResizeEvent*>(ev);
				this->resize(rev->size());
			}
			else if (ev->type() == QEvent::ChildAdded)
			{
				this->raise();
			}
		}
		return WIDGET::eventFilter(obj, ev);
	}
	// Tracks parent widget changes
	bool event(QEvent* ev) override
	{
		if (ev->type() == QEvent::ParentAboutToChange)
		{
			if (this->parent())
				this->parent()->removeEventFilter(this);
		}
		else if (ev->type() == QEvent::ParentChange)
		{
			if (this->parent())
			{
				this->parent()->installEventFilter(this);
				this->raise();
			}
		}
		return WIDGET::event(ev);
	}

	void showEvent(QShowEvent * event) override
	{
		WIDGET::showEvent(event);
		if (auto parent = this->parentWidget())
			this->resize(parent->size());
	}
};
