﻿#pragma once

#include "config.h"

double uniform(void);
double gaussian(double mu, double sigma);

#define RAYLEIGH()                      \
    sqrt(                               \
        pow(gaussian(0, 1 / 1.2533), 2) \
        +                               \
        pow(gaussian(0, 1 / 1.2533), 2) \
    )

#define LOG_NORMAL()                    \
    pow(10, gaussian(0, LN_SHAD) * 0.1)

#ifndef isnan
#define isnan(x) _isnan(x)
#endif

#ifndef isinf
#define isinf(x) (!_finite(x))
#endif

#define FOREACH_MACROS \
    for (int mac = 0; mac < NUM_MACRO; mac++)

#define FOREACH_MACROS_OPEN \
    for (int mac = 0; mac < NUM_MACRO; mac++) { \
        Macro* macro = macros[mac];

#define FOREACH_MACROS_TS_OPEN \
    for (int mac = 0; mac < NUM_MACRO; mac++) { \
        Macro* macro = macros[mac]; \
        const int NUM_MOBILE_TS = macro->get_num_mobiles_to_service(); \
        Edge** mmobiles = macro->get_mobiles_to_service();

#define FIND_BEST_MACRO_STATE_OPEN(macros, best_macro_states) {                 \
    double best_sum_lambda_r = -std::numeric_limits<double>::infinity();        \
    MacroState best_macro_states[NUM_MACRO];                                    \
                                                                                \
    FOREACH_MACROS                                                              \
        macros[mac]->state = OFF;                                               \
                                                                                \
    /* 가능한 모든 Macro 상태(2 ^ NUM_MACRO = 1 << NUM_MACRO)에 대한 반복문 */      \
    int num_macro_state = 1 << NUM_MACRO;                                       \
    for (int s = 0; s < num_macro_state; s++) {                                 \
                                                                                \
        /* Macro 상태(ON/OFF) 지정 */                                            \
        FOREACH_MACROS                                                          \
            macros[mac]->state = ((1 << mac) & s) >> mac ? ON : OFF;            \
                                                                                \
        double curr_sum_lambda_r = 0.0;

#define FIND_BEST_MACRO_STATE_CLOSE(best_macro_states)                          \
        }                                                                       \
        FOREACH_MACROS                                                          \
            macros[mac]->state = best_macro_states[mac];                        \
    }

#define FOREACH_PICOS \
    for (int pic = 0; pic < NUM_PICO; pic++)

#define FOREACH_MOBILES \
    for (int mob = 0; mob < NUM_MOBILE; mob++)

#define FOREACH_MOBILES_TS \
    for (int mob = 0; mob < NUM_MOBILE_TS; mob++)

#define FOREACH_MOBILES_TS_OPEN \
    for (int mob = 0; mob < NUM_MOBILE_TS; mob++) { \
        const Mobile* mobile = mmobiles[mob]->mobile;

#define FOREACH_RBS \
    for (int ri = 0; ri < NUM_RB; ri++)

#define CLOSE \
    }
