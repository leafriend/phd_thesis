#pragma once

#include <math.h>

#define DATA_FILE_PREFIX                "res\\"

#define MEGA                            1048576
#define ON                              1
#define OFF                             0

#define BW                              20000000            // 전체 Bandwidth, Hz 단위
#define BW_PER_RB                         180000            // 할당하는 단위 자원의 Bandwidth, Hz 단위, noise 계산시 사용
#define NUM_RB                               100            // Resource Block 갯수

#define NUM_MACRO                       7
#define NUM_PICO                        15
#define NUM_MOBILE                      50
#define NUM_MM                          350                 // = NUM_MACRO * NUM_MOBILE
#define NUM_PM                          750                 // = NUM_PICO * NUM_MOBILE
#define NUM_RM                          5000                // = NUM_RB * NUM_MOBILE

#define TX_POWER_MACRO                  40                  // watt
#define TX_POWER_PICO                   1                   // watt
#define QOS_FACTOR                      0.0001              // 각 유저가 갖는 utility의 QoS 값
#define QOS                             log(QOS_FACTOR)

#define RANGE_MACRO                     1000                // 모바일과 매크로, 간섭 관계 설정, 거리 기준
#define RANGE_PICO                      1000                // 모바일과 피코, 간섭 관계 설정, 거리 기준

#define MP_INT_DIST                     1000                // 매크로와 피코 간섭 범위

#define LN_SHAD                         8                   // log-normal shadowing (dB)
#define PATH_LOSS_EXPO                  4                   // Path Loss Exponent

#define NOISE_FACTOR                    -174                // dB
#define NOISE                           (BW_PER_RB * powl(10, (NOISE_FACTOR / 10)))

#define SIMULATION_TIME                 2000000
//#define SIMULATION_TIME                 100000
//#define SIMULATION_TIME                 10000
//#define SIMULATION_TIME                 100
#define LOG_INTERVAL_TIME               1000
#define LOG_PROGRESS_TIME               10

#define STEPSIZE2                       0.0005
#define STEPSIZE3                       0.0001
#define STEPSIZE4                       0.00005
#define RATE_MAX                        100                 // lambda = 0 일 경우 rate Ru 가 갖게되는 값
