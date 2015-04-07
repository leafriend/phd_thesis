#pragma once

#include "config.h"

#ifdef _MSC_VER
#include <Windows.h>
typedef SYSTEMTIME TIME;
#define TIME_GET(t) GetLocalTime(t)
#define TIME_GETSEC(t) ((t)->wSecond + (t)->wMinute * 60 + (t)->wHour * 60 * 60)
#define TIME_SEC(t) (t)->wSecond
#define TIME_MSEC(t) (t)->wMilliseconds
#define BASE_SEC 0
#else
#include <sys/time.h>
typedef struct timeval TIME;
#define TIME_GET(t) gettimeofday(t, NULL)
#define TIME_GETSEC(t) (t)->tv_sec
#define TIME_SEC(t) (t)->tv_sec
#define TIME_MSEC(t) (t)->tv_usec
#define BASE_SEC                        (16532 * 60 * 60 * 24 - 60 * 60 * 9)
#endif
int subtract_timeval(TIME* result, TIME* x, TIME* y);

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

#define FOREACH_MACROS for (int mac = 0; mac < NUM_MACRO; mac++)
#define FOREACH_PICOS for (int pic = 0; pic < NUM_PICO; pic++)
#define FOREACH_MOBILES for (int mob = 0; mob < NUM_MOBILE; mob++)

//
// Foreach.h C++ helper
// (C) 2010 Artur Sowiński <artur.sowinski@gmail.com>
// Released under MIT license as defined at
// http://pleasanthacking.com/license/

// plain vanilla iterator version
#define foreach(IT,X) for ( typeof( X->begin() ) IT = X->begin(); IT != X->end(); ++IT )
#define foreach_(IT,X) for ( typeof( X.begin() ) IT = X.begin(); IT != X.end(); ++IT )

// vanilla enforced with secure deleting of element from within the loop
#define foreachdel(IT,X) for ( typeof( X->begin() ) IT = X->begin(); typeof( IT ) IT##_bup = IT; IT != X->end(); (IT == IT##_bup) IT##_bup = ++IT : IT##_bup = IT  )
#define foreachdel_(IT,X) for ( typeof( X.begin() ) IT = X.begin(); typeof( IT ) IT##_bup = IT; IT != X.end(); (IT == IT##_bup) IT##_bup = ++IT : IT##_bup = IT  )
