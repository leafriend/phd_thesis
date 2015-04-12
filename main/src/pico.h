#pragma once

#include "config.h"

//#include "mobile.h"
//#include "macro.h"
//#include "pico_mobile.h"

class Mobile;
class Macro;
class Pico_Mobile;

int non_cmp(int ri, const Mobile* a, const Mobile* b);

int abs_cmp(int ri, const Mobile* a, const Mobile* b);

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
	Pico_Mobile* non_sorted_mobiles[NUM_RB][NUM_PM];
	Pico_Mobile* abs_sorted_mobiles[NUM_RB][NUM_PM];
	Pico_Mobile* mobiles_interfered[NUM_PM];
	int num_mobiles_interfered;

public:

	Pico(int idx, double x, double y, double tx_power);

	void check_interfering(Macro* macro);

	bool is_abs() const;

	void sort_mobiles();

	void sort_mobiles(int ri, Pico_Mobile** items, int size, Pico_Mobile** sorted, int cmp(int, const Mobile*, const Mobile*));

	int get_num_mobiles_to_service() const;

	Pico_Mobile* get_non_sorted_mobile(int ri, int mob) const;

	Pico_Mobile* get_abs_sorted_mobile(int ri, int mob) const;

};
