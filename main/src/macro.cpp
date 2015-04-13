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
: idx(idx), x(x)
, y(y)
, tx_power(tx_power)
{
	num_mobiles_to_service = 0;
	num_mobiles_interfered = 0;
	allocation_count = 0;

	pa3_lambda_r = 1.0;
}

int Macro::get_num_mobiles_to_service() const {
	return num_mobiles_to_service;
}

Macro_Mobile** Macro::get_mobiles_to_service() {
	return mobiles_to_service;
}

void Macro::set_state(bool state) {
	this->state = state;
}

bool Macro::get_state() const {
	return state;
}

void Macro::count_allocation() {
	if (state)
		allocation_count++;
}

int Macro::get_allocation_count() {
	return allocation_count;
}
