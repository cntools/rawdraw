#include <CNFG.h>

void CNFGClearFrameInternal( uint32_t bgcolor );

void CNFGClearFrame()
{
	CNFGClearFrameInternal( CNFGBGColor );
}
