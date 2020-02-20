// The following ifdef block is the standard way of creating macros which make exporting
// from a DLL simpler. All files within this DLL are compiled with the PRESETSLIB_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see
// PRESETSLIB_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.

#pragma once

#if defined (_MSC_VER)

#pragma warning(disable : 4251)
#pragma warning(disable : 4275)

#ifdef Widget_EXPORTS
	#define MOVAVIWIDGET_API __declspec(dllexport)
#else
	#define MOVAVIWIDGET_API __declspec(dllimport)
#endif

#elif defined (__GNUC__)

	#define MOVAVIWIDGET_API __attribute__((visibility("default")))

#else

	#define MOVAVIWIDGET_API

#endif
