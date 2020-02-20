#pragma once

#include <type_traits>

#include <QString>

#include "MovaviWidgetLib.h"

class QLayout;
class QWidget;
class QVariant;
class QAbstractSlider;
class QAbstractButton;
class QSpinBox;
class QDoubleSpinBox;
class QScrollArea;
class QFrame;
class MLabel;
class MFrame;
class MDoubleSlider;
class LayoutController;

namespace Util {

// создает виджеты
// версии с M_WIDGET используются для виджетов-полей класса, назначает имя такое же как у поля класса, отбрасывая префикс m_
#define CREATE_WIDGET(WIDGET, ...)                  ::Util::createWidget<std::remove_pointer_t<decltype(WIDGET)>>(QStringLiteral(#WIDGET)                          , ##__VA_ARGS__)
#define CREATE_M_WIDGET(WIDGET, ...)                ::Util::createWidget<std::remove_pointer_t<decltype(WIDGET)>>(::Util::Details::removeClassMemberPrefix(#WIDGET), ##__VA_ARGS__)
#define CREATE_NAMED_WIDGET(WIDGET, NAME, ...)      ::Util::createWidget<std::remove_pointer_t<decltype(WIDGET)>>(NAME                                             , ##__VA_ARGS__)

// для создания виджетов в списке иницализации конструктора
#define CONSTRUCT_WIDGET(WIDGET, ...)               WIDGET(CREATE_WIDGET      (WIDGET,       ##__VA_ARGS__))
#define CONSTRUCT_M_WIDGET(WIDGET, ...)             WIDGET(CREATE_M_WIDGET    (WIDGET,       ##__VA_ARGS__))
#define CONSTRUCT_NAMED_WIDGET(WIDGET, NAME, ...)   WIDGET(CREATE_NAMED_WIDGET(WIDGET, NAME, ##__VA_ARGS__))

// для объявления и создания виджетов как поле класса или как локальную переменную
#define DECLARE_WIDGET(TYPE, WIDGET, ...)             TYPE * WIDGET = CREATE_WIDGET  (WIDGET, __VA_ARGS__)
#define DECLARE_M_WIDGET(TYPE, WIDGET, ...)           TYPE * WIDGET = CREATE_M_WIDGET(WIDGET, __VA_ARGS__)

// для упрощения задания objectName
#define NAME_WIDGET(WIDGET)                           WIDGET->setObjectName(QStringLiteral(#WIDGET))

template <typename Widget, typename ... ARGS>
Widget * createWidget(const QString & name, ARGS && ... args)
{
	auto widget = new Widget(std::forward<ARGS>(args)...);
	widget->setObjectName(name);
	return widget;
}

template <typename Layout, typename ... ARGS>
Layout * createZeroLayout(ARGS && ... args)
{
	auto layout = new Layout(std::forward<ARGS>(args)...);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setSpacing(0);
	return layout;
}

MOVAVIWIDGET_API LayoutController * installLayoutController(QWidget * target);
MOVAVIWIDGET_API LayoutController * installLayoutController(QLayout * target, QWidget * parent);

// создает QFrame, который можно использовать как spacer и управлять им из стилей (QSpacerItem из стилей недоступен)
MOVAVIWIDGET_API QFrame * createSpacer(QWidget * parent, const QString & name);

// очищает лэйаут. Виджеты извлекаются, QLayoutItem-ы удаляются (spacer-ы, вложенные лэйауты)
MOVAVIWIDGET_API void clearLayout(QLayout *);
// удаляет все содержимое лэйаута (виджеты и QLayoutItem-ы)
MOVAVIWIDGET_API void cleanupLayout(QLayout *);

// делает unpolish/polish стилей для widget
MOVAVIWIDGET_API void repolishWidget(QWidget *);
// применяет repolish рекурсивно ко всем дочерним виджетам
MOVAVIWIDGET_API void repolishWidgetRecursive(QWidget *);

// устанавливает указанному свойству виджета значение и вызывает repolish
MOVAVIWIDGET_API void applyProperty(QWidget *, const char * name, const QVariant & value);
MOVAVIWIDGET_API void applyStyleHint(QWidget *, const QString & value);

// устанавливает необходимые коннекты между виджетами для поддержки синхронности значений
MOVAVIWIDGET_API void bindWidgets(QAbstractSlider *, QSpinBox *);
MOVAVIWIDGET_API void bindWidgets(MDoubleSlider *, QDoubleSpinBox *);
// используется для привязки MLabel к QCheckBox и QRadioButton
MOVAVIWIDGET_API void bindWidgets(MLabel *, QAbstractButton *);

// оборачивает виджет в QScollArea. Полезно при создании страниц для TabWIdget, StackedWidget
MOVAVIWIDGET_API QScrollArea * wrapWithScrollArea (QWidget *, Qt::ScrollBarPolicy hPolicy = Qt::ScrollBarAsNeeded, Qt::ScrollBarPolicy vPolicy = Qt::ScrollBarAsNeeded);
MOVAVIWIDGET_API QScrollArea * wrapWithHScrollArea(QWidget *, Qt::ScrollBarPolicy hPolicy = Qt::ScrollBarAsNeeded);
MOVAVIWIDGET_API QScrollArea * wrapWithVScrollArea(QWidget *, Qt::ScrollBarPolicy vPolicy = Qt::ScrollBarAsNeeded);

// оборачивает виджет в MFrame. Может быть полезно при стилизации виджетов в GridLayout (для задания разных отступов между ячейками)
// если указать stretchSide, то создаст VBoxLayout или HBoxLayout и добавит с указанной стороны stretch
MOVAVIWIDGET_API MFrame * wrapWithMFrame(QWidget *, const QString & name, Qt::Edge stretchSide = {});
MOVAVIWIDGET_API MFrame * wrapWithMFrame(QWidget *, Qt::Edge stretchSide = {}); // создает MFrame с именем widget->objectName() + "Frame"

// создаёт виджет с property delimiter "horizontal" или "vertical" в зависимости от ориентации
MOVAVIWIDGET_API QWidget * createDelimiter(Qt::Orientation, QWidget * parent = nullptr);
MOVAVIWIDGET_API QWidget * createDelimiter(Qt::Orientation, const QString & name, QWidget * parent = nullptr);

// создает фокус цепочку для списка виджетов в порядке их перечисления
MOVAVIWIDGET_API void setTabOrder(const QList<QWidget*> & widgets);

// Иногда не сбрасывается состояние hovered, например если у QToolButton скрыть
// popup меню нажатием за его пределами
// подробнее https://bugreports.qt.io/browse/QTBUG-36862
MOVAVIWIDGET_API void resetHoverState(QWidget *);

}

namespace Util { namespace Details {

MOVAVIWIDGET_API QString removeClassMemberPrefix(const char * widgetName);

}}
