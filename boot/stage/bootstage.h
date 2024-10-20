#ifndef __BOOTSTAGE__
#define __BOOTSTAGE__

#include "stdint.h"
#include "vesa.h"

struct BootStage_Header {
    VBE_Info* vesa_info;
    VBE_Mode_Info* vesa_mode_info;
};

#endif
