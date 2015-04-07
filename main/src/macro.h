#pragma once

#include "config.h"

#include "mobile.h"
#include "macro_mobile.h"
//class Mobile;
//class Macro_Mobile;

class Macro {

	friend class Macro_Mobile;

public:
	const int idx;
	const double x;
	const double y;
	const double tx_power;

private:

	Macro_Mobile* mobiles_in_range[NUM_MM];
	int num_mobiles_in_range;

	Macro_Mobile* mobiles_interfered[NUM_PM];
	int num_mobiles_interfered;

	////////////////////////////////////////////////////////////////////////////
	// PA1                                                                    //

	Mobile* first_mobile[NUM_RB];

	int allocation_count;

	// PA1                                                                    //
	////////////////////////////////////////////////////////////////////////////

public:

	Macro(int idx, double x, double y, double tx_power);

	void sort_mobiles();

	void count_allocation();

	Mobile* get_first_mobile(int ri);

	int get_allocation_count();

};
