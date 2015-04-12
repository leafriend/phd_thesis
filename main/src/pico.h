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

	const int idx;
	const double x;
	const double y;
	const double tx_power;

private:

	Macro* macros_interfering[NUM_MACRO];
	int num_macros_interfering;

	Pico_Mobile* mobiles_to_service[NUM_PM];
	int num_mobiles_to_service;
	Pico_Mobile* non_sorted_mobile[NUM_RB][NUM_PM];
	Pico_Mobile* abs_sorted_mobile[NUM_RB][NUM_PM];
	Pico_Mobile* mobiles_interfered[NUM_PM];
	int num_mobiles_interfered;

	////////////////////////////////////////////////////////////////////////////
	// PA1                                                                    //

	Mobile* first_mobile[NUM_RB];
	Mobile* second_mobile[NUM_RB];

	Mobile* abs_first_mobile[NUM_RB];
	Mobile* abs_second_mobile[NUM_RB];

	// PA1                                                                    //
	////////////////////////////////////////////////////////////////////////////

public:

	Pico(int idx, double x, double y, double tx_power);

	void check_interfering(Macro* macro);

	bool is_abs() const;

	void sort_mobiles();

	int get_num_mobiles_to_service() const;

	Pico_Mobile* get_non_sorted_mobile(int ri, int mob) const;

	Pico_Mobile* get_abs_sorted_mobile(int ri, int mob) const;

	////////////////////////////////////////////////////////////////////////////
	// PA1                                                                    //

	Mobile* get_first_mobile(int ri);
	Mobile* get_second_mobile(int ri);

	Mobile* get_abs_first_mobile(int ri);
	Mobile* get_abs_second_mobile(int ri);

	// PA1                                                                    //
	////////////////////////////////////////////////////////////////////////////

};
