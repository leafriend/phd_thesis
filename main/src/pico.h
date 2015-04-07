#pragma once

#include "config.h"

//#include "mobile.h"
//#include "macro.h"
//#include "pico_mobile.h"

class Mobile;
class Macro;
class Pico_Mobile;

class Pico {

	friend class Pico_Mobile;

public:
private: public:

	const int idx;
	const double x;
	const double y;
	const double tx_power;

	Pico_Mobile* mobiles_in_range[NUM_PM];
	int num_mobiles_in_range;
	Pico_Mobile* mobiles_interfered[NUM_PM];
	int num_mobiles_interfered;

	////////////////////////////////////////////////////////////////////////////
	// PA1                                                                    //

	Mobile* first_mobile;
	Mobile* second_mobile;

	Mobile* abs_first_mobile;
	Mobile* abs_second_mobile;

	// PA1                                                                    //
	////////////////////////////////////////////////////////////////////////////

public:

	Pico(int idx, double x, double y, double tx_power);

	bool is_neighbor(Macro* macro);

	void sort_mobiles();

	////////////////////////////////////////////////////////////////////////////
	// PA1                                                                    //

	Mobile* get_first_mobile();
	Mobile* get_second_mobile();

	Mobile* get_abs_first_mobile();
	Mobile* get_abs_second_mobile();

	// PA1                                                                    //
	////////////////////////////////////////////////////////////////////////////

};
