/* Copyright 2010-2020 <>< Charles Lohr and other various authors as attributed.
	Licensed under the MIT/x11 or NewBSD License you choose.

	CN Foundational Graphics Main Header File.  This is the main header you
	should include.  See README.md for more details.
*/

#include "CNFGFunctions.h"

#ifdef CNFG_IMPLEMENTATION
#include "CNFG.c"
#ifdef CNFG3D
#include "CNFG3D.c"
#endif
#endif

#ifdef CNFG3D
#include "CNFG3D.h"
#endif

