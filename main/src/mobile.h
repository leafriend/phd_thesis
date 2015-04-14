#pragma once

#include "config.h"

//#include "macro.h"
//#include "macro_mobile.h"
//#include "pico_mobile.h"

#define STATE_CONN_NOTHING    0
#define STATE_CONN_MACRO      1
#define STATE_CONN_ABS_PICO   2
#define STATE_CONN_NON_PICO_1 3
#define STATE_CONN_NON_PICO_2 4

class Macro;
class Macro_Mobile;
class Pico_Mobile;
class Edge;

using namespace std;

enum MobileConnection {
	NOTHING,    // 연결 안됨
	MACRO,      // Macro에 연결
	ABS_PICO,   // Pico에 ABS로 연결
	NON_PICO_1, // Pico에 non-ABS 첫 번째로 연결
	NON_PICO_2, // Pico에 non-ABS 두 번째로 연결
};

class Mobile {

	friend class Macro_Mobile;
	friend class Pico_Mobile;
	friend class Edge;

public:

	const int idx;
	const double x;
	const double y;
	const double qos;

	////////////////////////////////////////////////////////////////////////////
	// State Variable                                                         //

	double lambda;
	double mu;

	// State Variable                                                         //
	////////////////////////////////////////////////////////////////////////////

private:

	Macro_Mobile* macro;
	Pico_Mobile* pico;

	Macro_Mobile* macros_interfered[NUM_MACRO];
	int num_macros_interfered;
	Pico_Mobile* picos_interfered[NUM_PICO];
	int num_picos_interfered;

	double macro_throughput[NUM_RB];
	double macro_lambda_r[NUM_RB];

	double non_pico_throughput[NUM_RB];
	double non_pico_lambda_r[NUM_RB];

	double abs_pico_throughput[NUM_RB];
	double abs_pico_lambda_r[NUM_RB];

	double result_throughput;
	double rate_user;

	double instant_rate;

public:
	MobileConnection conns[NUM_RB];

public:

	Mobile(int idx, double x, double y, double lambda, double mu, double rate_user, double tx_power);

	void locate_on_macro(Macro* marco);

	void generate_channel_gain();

	void calculate_throughput();

	void calculate_rate_user();

	void calculate_dual_variable(const int t);

	double get_macro_throughput(int ri) const;

	double get_macro_lambda_r(int ri) const;

	double get_non_pico_throughput(int ri) const;

	double get_non_pico_lambda_r(int ri) const;

	double get_abs_pico_throughput(int ri) const;

	double get_abs_pico_lambda_r(int ri) const;

	double get_rate_user();

	double get_result_throughput();

	Macro_Mobile* get_macro() const;
	Pico_Mobile* get_pico() const;

};
