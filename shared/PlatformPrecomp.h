#ifndef _PLATFORMPRECOMP_H
#define _PLATFORMPRECOMP_H

//#define _CRTDBG_MAP_ALLOC
//#include <stdlib.h>
//#include <crtdbg.h>

#if defined __cplusplus 

	#include "PlatformSetup.h"
	#ifndef _CONSOLE
	#include "BaseApp.h"
	#else
	bool IsTabletSize();
	#endif
	#include<shiny.h>
#endif

#endif
