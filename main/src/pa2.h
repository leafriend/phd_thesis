#pragma once

#include "macro.h"
#include "pico.h"
#include "mobile.h"

void pa2(Macro** macros, Pico** picos, Mobile** mobiles);

void pa2_find_best_mobile_state(Macro* macro, double* best_sum_lambda_r);
