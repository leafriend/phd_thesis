#pragma once

#include "config.h"
#include "basestation.h"

//#include "mobile.h"
//#include "macro.h"
//#include "pico_mobile.h"

class Mobile;
class Macro;

int non_cmp(int ri, const Mobile* a, const Mobile* b);

int abs_cmp(int ri, const Mobile* a, const Mobile* b);

class Pico : public BaseStation {

private:

	Macro* macros_interfering[NUM_MACRO];
	int num_macros_interfering;

	Edge* non_sorted_mobiles[NUM_RB][NUM_PM];
	Edge* abs_sorted_mobiles[NUM_RB][NUM_PM];

	////////////////////////////////////////////////////////////////////////////
	// PA3                                                                    //

public:
	double pa3_lambda_r;

	// PA3                                                                    //
	////////////////////////////////////////////////////////////////////////////

public:

	Pico(int idx, double x, double y, double tx_power);

	void check_interfering(Macro* macro);

	bool is_abs() const;

	void sort_mobiles();

	void sort_mobiles(int ri, Edge** items, int size, Edge** sorted, int cmp(int, const Mobile*, const Mobile*));

	int get_num_mobiles_to_service() const;

	Edge* get_non_sorted_mobile(int ri, int mob) const;

	Edge* get_abs_sorted_mobile(int ri, int mob) const;

};
