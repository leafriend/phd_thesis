#pragma once

#include "config.h"

#include "mobile.h"
#include "macro_mobile.h"
#include "basestation.h"

enum MacroState {
	OFF,
	ON,
};

class Macro : public BaseStation {

private:

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

	void count_allocation();

	int get_allocation_count();

};
