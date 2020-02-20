#include <windows.h>

#include "MSliderButton.h"

void MSliderButton::setShadow(bool shadow)
{
	if (m_shadow == shadow)
		return;
	m_shadow = shadow;

	HWND hWnd = reinterpret_cast<HWND>(m_sliderFrame->winId());
	DWORD windowStyle = GetClassLongW(hWnd, GCL_STYLE);
	if ((windowStyle & CS_DROPSHADOW) && !shadow)
	{
		windowStyle ^= CS_DROPSHADOW;
		SetClassLongW(hWnd, GCL_STYLE, windowStyle);
	}
}
