#include <stdio.h>

#include <stdlib.h>

#include <math.h>
#include <limits>

#include "func.h"
#include "macro_mobile.h"
#include "pico_mobile.h"
#include "mobile.h"
#include "pico.h"
#include "macro.h"

Macro::Macro(int idx, double x, double y, double tx_power)
	: BaseStation(idx, x, y, tx_power)
{
	num_mobiles_interfered = 0;
	allocation_count = 0;

	pa3_lambda_r = 1.0;
}

void Macro::count_allocation() {
	if (state == ON)
		allocation_count++;
}

int Macro::get_allocation_count() {
	return allocation_count;
}
