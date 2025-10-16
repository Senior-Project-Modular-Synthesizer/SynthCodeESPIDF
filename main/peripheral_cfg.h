#pragma once

#define USE_PCB 0

#if USE_PCB
    #include "peripheral_cfg_pcb.h"
#else
    #include "peripheral_cfg_prototype.h"
#endif