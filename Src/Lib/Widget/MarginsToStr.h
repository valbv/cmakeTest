#pragma once

#include <QString>
#include <QMargins>
#include <QMarginsF>

#include "MovaviWidgetLib.h"

namespace Util {

// поведение аналогично Box Lengths, https://doc.qt.io/qt-5/stylesheet-reference.html#box-lengths
// единицы измерения - px
// '1'       -> top = 1 right = 1 bottom = 1 left = 1
// '1 2'     -> top = 1 right = 2 bottom = 1 left = 2
// '1 2 3'   -> top = 1 right = 2 bottom = 3 left = 2
// '1 2 3 4' -> top = 1 right = 2 bottom = 3 left = 4
QMargins  MOVAVIWIDGET_API strToMargins (const QStringRef&);
QMargins  MOVAVIWIDGET_API strToMargins (const QString&);
QMarginsF MOVAVIWIDGET_API strToMarginsF(const QStringRef&);
QMarginsF MOVAVIWIDGET_API strToMarginsF(const QString&);

QString MOVAVIWIDGET_API marginsToStr(const QMargins&);
QString MOVAVIWIDGET_API marginsToStr(const QMarginsF&);

}
