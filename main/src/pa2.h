#pragma once

#include "macro.h"
#include "pico.h"
#include "mobile.h"

void pa2(Macro** macros, Pico** picos, Mobile** mobiles);

double pa2_calculate_lambda_r(long mobile_state, Macro_Mobile** mmobiles, int NUM_MOBILE_TS, int* curr_allocated_mob);
