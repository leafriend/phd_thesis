#pragma once

#include "config.h"

//#include "macro.h"
//#include "macro_mobile.h"
//#include "pico_mobile.h"

class Macro;
class Macro_Mobile;
class Pico_Mobile;

using namespace std;

class Mobile {

	friend class Macro_Mobile;
	friend class Pico_Mobile;

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

	////////////////////////////////////////////////////////////////////////////
	// PA1                                                                    //

	double macro_throughput[NUM_RB];

	double pico_throughput[NUM_RB];

	double abs_pico_throughput[NUM_RB];

	int states[NUM_RB];
	double result_throughput;
	double rate_user;
public:
	double instant_rate;

	// PA1                                                                    //
	////////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////////
	// PA2                                                                    //
public:
	bool conn_macro;

	// PA1                                                                    //
	////////////////////////////////////////////////////////////////////////////

public:

	Mobile(int idx, double x, double y, double tx_power);

	void locate_on_macro(Macro* marco);

	////////////////////////////////////////////////////////////////////////////
	// PA1                                                                    //

	void generate_channel_gain();

	void calculate_throughput();

	void calculate_rate_user();

	void calculate_dual_variable(const int t);

	double get_macro_throughput(int ri) const;

	double get_pico_throughput(int ri) const;

	double get_abs_pico_throughput(int ri) const;

	void set_state(int ri, int state);

	int get_state(int ri) const;

	double get_rate_user();

	double get_result_throughput();

	// PA1                                                                    //
	////////////////////////////////////////////////////////////////////////////

	Macro_Mobile* get_macro() const;
	Pico_Mobile* get_pico() const;

};
