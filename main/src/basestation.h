#pragma once

#include "macro_mobile.h"
#include "pico_mobile.h"
#include "mobile.h"

class BaseStation {

	friend class Macro_Mobile;
	friend class Pico_Mobile;

public:
	const int idx;
	const double x;
	const double y;
	const double tx_power;

private:

	Macro_Mobile* mobiles_to_service[NUM_MM];
	int num_mobiles_to_service;

	////////////////////////////////////////////////////////////////////////////
	// PA3                                                                    //

public:
	double pa3_lambda_r;

	// PA3                                                                    //
	////////////////////////////////////////////////////////////////////////////

public:

	BaseStation(int idx, double x, double y, double tx_power);

	int get_num_mobiles_to_service() const;
	Macro_Mobile** get_mobiles_to_service();

};
