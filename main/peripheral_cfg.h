#pragma once

#define USE_PCB 1
#if USE_PCB
    #include "peripheral_cfg_pcb.h"
#else
    #include "peripheral_cfg_prototype.h"
#endif

// Buffer Waiting Bits
#define INPUT_BUF1_READY (1 << 0)
#define INPUT_BUF1_WAIT  (1 << 1)
#define INPUT_BUF2_READY (1 << 2)
#define INPUT_BUF2_WAIT  (1 << 3)
#define OUTPUT_BUF1_READY (1 << 4)
#define OUTPUT_BUF1_WAIT  (1 << 5)
#define OUTPUT_BUF2_READY (1 << 6)
#define OUTPUT_BUF2_WAIT  (1 << 7)
