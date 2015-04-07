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

	// TODO private으로 변경
	Macro_Mobile* mobiles_in_range[NUM_MM];
	int num_mobiles_in_range;

private:

	Macro_Mobile* mobiles_interfered[NUM_PM];
	int num_mobiles_interfered;

	////////////////////////////////////////////////////////////////////////////
	// PA1                                                                    //

	Mobile* first_mobile;
public:
	int allocation_count;
private:

	// PA1                                                                    //
	////////////////////////////////////////////////////////////////////////////

public:

	Macro(int idx, double x, double y, double tx_power);

	void sort_mobiles();

	Mobile* get_first_mobile();

};
