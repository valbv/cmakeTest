#import <Cocoa/Cocoa.h>
#import <objc/runtime.h>

#include <Carbon/Carbon.h>

#include "MSliderButton.h"

void* RealWindowIdFromWId(WId windowId)
{
	void *realWinId = [reinterpret_cast<NSView *>(windowId) window];

	return realWinId;
}

NSWindow *GetNSWindowFromQWidgetWId(WId windowId)
{
	return reinterpret_cast<NSWindow *>( RealWindowIdFromWId(windowId) );
}

void MSliderButton::setShadow(bool shadow)
{
	if (m_shadow == shadow)
		return;
	m_shadow = shadow;

	NSWindow *wId = GetNSWindowFromQWidgetWId(m_sliderFrame->winId());
	[wId setHasShadow: shadow];
	[wId invalidateShadow];
}
