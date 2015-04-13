#pragma once

#include "config.h"

#include "mobile.h"
#include "macro_mobile.h"
//class Mobile;
//class Macro_Mobile;

enum MacroState {
	OFF,
	ON,
};

class Macro {

	friend class Macro_Mobile;

public:
	const int idx;
	const double x;
	const double y;
	const double tx_power;

private:

	Macro_Mobile* mobiles_to_service[NUM_MM];
	int num_mobiles_to_service;

	Macro_Mobile* mobiles_interfered[NUM_PM];
	int num_mobiles_interfered;
	
	int allocation_count;

public:
	MacroState state;

	////////////////////////////////////////////////////////////////////////////
	// PA3                                                                    //

public:
	double pa3_lambda_r;

	// PA3                                                                    //
	////////////////////////////////////////////////////////////////////////////

public:

	Macro(int idx, double x, double y, double tx_power);

	int get_num_mobiles_to_service() const;
	Macro_Mobile** get_mobiles_to_service();

	void count_allocation();

	int get_allocation_count();

};
