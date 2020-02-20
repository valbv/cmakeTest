#pragma once

#include <QWidget>
#include <QLayout>
#include <QPointer>

#include "MovaviWidgetLib.h"

class MOVAVIWIDGET_API AbstractLayoutController
{
public:
	AbstractLayoutController() = default;
	virtual ~AbstractLayoutController() = default;

	// Примеры: qproperty-layoutMargin: 1, qproperty-layoutMargin: '1 2', qproperty-layoutMargin: '1 2 3', qproperty-layoutMargin: '1 2 3 4'
	QString layoutMargin() const;
	void setLayoutMargin(const QString&);

	int layoutMarginLeft() const;
	void setLayoutMarginLeft(int marginLeft);

	int layoutMarginRight() const;
	void setLayoutMarginRight(int marginRight);

	int layoutMarginTop() const;
	void setLayoutMarginTop(int marginTop);

	int layoutMarginBottom() const;
	void setLayoutMarginBottom(int marginBottom);

	// Примеры: qproperty-layoutSpacing: 1, qproperty-layoutSpacing: '1 2' (horizontal, vertical)
	QString layoutSpacing() const;
	void setLayoutSpacing(const QString&);

	int layoutHSpacing() const;
	void setLayoutHSpacing(int hSpacing);

	int layoutVSpacing() const;
	void setLayoutVSpacing(int vSpacing);

	Qt::Alignment layoutAlign() const;
	void setLayoutAlign(Qt::Alignment);

	// Позволяют задать align для элементов layout, которые содержат виджеты (синтаксис схож с Selector Types из https://doc.qt.io/qt-5/stylesheet-syntax.html)
	// 1) по имени виджета:                                         '[#widget1 #widget2] AlignLeft, [#widget3] AlignHCenter | AlignBottom'
	// 2) для всех безымянных виджетов:                             '[#] AlignLeft'
	// 3) по типу виджета (для указанного типа и его наследников):  '[MyNamespace--MyButton] AlignTop'
	// 4) по типу виджета (наследники игнорируются):                '[.MyNamespace--MyButton] AlignTop'
	// 5) для всех виджетов:                                        '[*]' - сбросит всем виджетам align на дефолтное значение
	QString layoutWidgetAlign() const;
	void setLayoutWidgetAlign(const QString&);

	// Методы для управления минимальными размерами и stretch-ами столбцов/строк QGridLayout
	// 1) [*] 100 - задаст значение всем столбцам/строкам лэйаута
	// 2) [0 2] 100, [1] 50 - задаст значение столбцам/строкам с указанными индексами
	QString layoutColMinWidth() const;
	void    setLayoutColMinWidth(const QString&);

	QString layoutRowMinHeight() const;
	void    setLayoutRowMinHeight(const QString&);

	QString layoutColStretch() const;
	void    setLayoutColStretch(const QString&);

	QString layoutRowStretch() const;
	void    setLayoutRowStretch(const QString&);

	// Методы для управлениями stretch-ами QBoxLayout-а
	// 1) [*] 1 - задаст значение всем элементам
	// 2) [0 2] 1, [1] 2 - задаст значение элементам с указанными индексами
	QString layoutStretch() const;
	void    setLayoutStretch(const QString&);

	QLayout::SizeConstraint layoutSizeConstraint() const;
	void setLayoutSizeConstraint(QLayout::SizeConstraint);

	virtual QLayout * getLayout() const = 0;
};

#define DECLARE_LAYOUT_CONTROLLER_PROPERTIES                                                                           \
Q_PROPERTY(QString layoutMargin         READ layoutMargin       WRITE setLayoutMargin)                                 \
Q_PROPERTY(int     layoutMarginLeft     READ layoutMarginLeft   WRITE setLayoutMarginLeft)                             \
Q_PROPERTY(int     layoutMarginRight    READ layoutMarginRight  WRITE setLayoutMarginRight)                            \
Q_PROPERTY(int     layoutMarginTop      READ layoutMarginTop    WRITE setLayoutMarginTop)                              \
Q_PROPERTY(int     layoutMarginBottom   READ layoutMarginBottom WRITE setLayoutMarginBottom)                           \
                                                                                                                       \
Q_PROPERTY(QString layoutSpacing        READ layoutSpacing      WRITE setLayoutSpacing)                                \
Q_PROPERTY(int     layoutHSpacing       READ layoutHSpacing     WRITE setLayoutHSpacing)                               \
Q_PROPERTY(int     layoutVSpacing       READ layoutVSpacing     WRITE setLayoutVSpacing)                               \
                                                                                                                       \
Q_PROPERTY(Qt::Alignment layoutAlign       READ layoutAlign        WRITE setLayoutAlign)                               \
Q_PROPERTY(QString       layoutWidgetAlign READ layoutWidgetAlign  WRITE setLayoutWidgetAlign)                         \
                                                                                                                       \
Q_PROPERTY(QString       layoutColMinWidth  READ layoutColMinWidth  WRITE setLayoutColMinWidth)                        \
Q_PROPERTY(QString       layoutRowMinHeight READ layoutRowMinHeight WRITE setLayoutRowMinHeight)                       \
                                                                                                                       \
Q_PROPERTY(QString       layoutStretch      READ layoutStretch      WRITE setLayoutStretch)                            \
Q_PROPERTY(QString       layoutColStretch   READ layoutColStretch   WRITE setLayoutColStretch)                         \
Q_PROPERTY(QString       layoutRowStretch   READ layoutRowStretch   WRITE setLayoutRowStretch)                         \
                                                                                                                       \
Q_PROPERTY(QLayout::SizeConstraint layoutSizeConstraint READ layoutSizeConstraint WRITE setLayoutSizeConstraint)
