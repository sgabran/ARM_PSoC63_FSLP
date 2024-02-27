// revision 2024-2-27-1

#ifndef _SG_FSLP_4PIN_H
#define _SG_FSLP_4PIN_H

#include "project.h"

#define FSLP_FORCE_THRESHOLD 25
#define FSLP_read_delay 75
    
//void FSLP_init(void);
uint32_t FSLP_read_force(void);
uint32_t FSLP_read_position(void);

#endif
