#pragma once

#include <math.h>

#define DATA_FILE_PREFIX                "res\\"

#define MEGA                            1048576
#define ON                              1
#define OFF                             0

#define BW                              20000000            // ��ü Bandwidth, Hz ����
#define BW_PER_RB                         180000            // �Ҵ��ϴ� ���� �ڿ��� Bandwidth, Hz ����, noise ���� ���
#define NUM_RB                               100            // Resource Block ����

#define NUM_MACRO                       7
#define NUM_PICO                        15
#define NUM_MOBILE                      50
#define NUM_MM                          350                 // = NUM_MACRO * NUM_MOBILE
#define NUM_PM                          750                 // = NUM_PICO * NUM_MOBILE
#define NUM_RM                          5000                // = NUM_RB * NUM_MOBILE

#define TX_POWER_MACRO                  40                  // watt
#define TX_POWER_PICO                   1                   // watt
#define QOS_FACTOR                      0.0001              // �� ������ ���� utility�� QoS ��
#define QOS                             log(QOS_FACTOR)

#define RANGE_MACRO                     1000                // ����ϰ� ��ũ��, ���� ���� ����, �Ÿ� ����
#define RANGE_PICO                      1000                // ����ϰ� ����, ���� ���� ����, �Ÿ� ����

#define MP_INT_DIST                     1000                // ��ũ�ο� ���� ���� ����

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
#define RATE_MAX                        100                 // lambda = 0 �� ��� rate Ru �� ���ԵǴ� ��
