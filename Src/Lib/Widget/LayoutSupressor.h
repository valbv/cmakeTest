#pragma once

class QLayout;

#include "MovaviWidgetLib.h"

// RAII-блокировщик перестроения лэйаута. При создании блокирует лэйаут, при удалении - разблокирует и перестраивает принудительно
class MOVAVIWIDGET_API LayoutSupressor
{
public:
	LayoutSupressor(QLayout * layout);
	~LayoutSupressor();
private:
	QLayout * m_layout = nullptr;
};
