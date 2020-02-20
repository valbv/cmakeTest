#include "MarginsToStr.h"

#include <cassert>

#include <QVector>

namespace Util {

QMargins  strToMargins(const QString& str)
{
	return strToMargins(&str);
}

QMargins strToMargins(const QStringRef & str)
{
	const QString fixedStr = str.toString().simplified();
	const QVector<QStringRef> splitted = fixedStr.splitRef(' ', QString::SkipEmptyParts);
	auto val = [&splitted](int index) {
		return splitted[index].toInt();
	};
	//t,r,b,l -> l,t,r,b
	switch (splitted.size())
	{
	case 0: return QMargins();
	case 1: return QMargins(val(0), val(0), val(0), val(0));// '1'       -> top = 1 right = 1 bottom = 1 left = 1
	case 2: return QMargins(val(1), val(0), val(1), val(0));// '1 2'     -> top = 1 right = 2 bottom = 1 left = 2
	case 3: return QMargins(val(1), val(0), val(1), val(2));// '1 2 3'   -> top = 1 right = 2 bottom = 3 left = 2
	case 4: return QMargins(val(3), val(0), val(1), val(2));// '1 2 3 4' -> top = 1 right = 2 bottom = 3 left = 4
	}
	assert(!"toMargins: wrong param count");
	return QMargins();
}

QMarginsF strToMarginsF(const QString & str)
{
	return strToMarginsF(&str);
}

QMarginsF strToMarginsF(const QStringRef & str)
{
	const QString fixedStr = str.toString().simplified();
	const QVector<QStringRef> splitted = fixedStr.splitRef(' ', QString::SkipEmptyParts);
	auto val = [&splitted](int index) {
		return splitted[index].toDouble();
	};
	//t,r,b,l -> l,t,r,b
	switch (splitted.size())
	{
	case 0: return QMarginsF();
	case 1: return QMarginsF(val(0), val(0), val(0), val(0));// '1'       -> top = 1 right = 1 bottom = 1 left = 1
	case 2: return QMarginsF(val(1), val(0), val(1), val(0));// '1 2'     -> top = 1 right = 2 bottom = 1 left = 2
	case 3: return QMarginsF(val(1), val(0), val(1), val(2));// '1 2 3'   -> top = 1 right = 2 bottom = 3 left = 2
	case 4: return QMarginsF(val(3), val(0), val(1), val(2));// '1 2 3 4' -> top = 1 right = 2 bottom = 3 left = 4
	}
	assert(!"toMargins: wrong param count");
	return QMarginsF();
}

QString marginsToStr(const QMargins & margins)
{
	return QString("%1 %2 %3 %4").arg(margins.top()).arg(margins.right()).arg(margins.bottom()).arg(margins.left());
}

QString marginsToStr(const QMarginsF & margins)
{
	return QString("%1 %2 %3 %4").arg(margins.top()).arg(margins.right()).arg(margins.bottom()).arg(margins.left());
}

}
