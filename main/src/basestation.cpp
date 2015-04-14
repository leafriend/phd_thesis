#include "basestation.h"

BaseStation::BaseStation(int idx, double x, double y, double tx_power)
: idx(idx), x(x), y(y), tx_power(tx_power)
{
	num_mobiles_to_service = 0;

	pa3_lambda_r = 1.0;
}

int BaseStation::get_num_mobiles_to_service() const {
	return num_mobiles_to_service;
}

Edge** BaseStation::get_mobiles_to_service() {
	return mobiles_to_service;
}
