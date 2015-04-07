#include <stdio.h>

#include <stdlib.h>
#include <math.h>

#include "config.h"
#include "func.h"
#include "macro.h"
#include "mobile.h"
#include "macro_mobile.h"

Macro_Mobile::Macro_Mobile(Macro* macro, Mobile* mobile)
: macro(macro)
, mobile(mobile)
, distance(sqrt(pow((macro->x - mobile->x), 2) + pow((macro->y - mobile->y), 2)))
, channel_gain_factor(macro->tx_power * pow((1 / distance), (double) PATH_LOSS_EXPO))
{

	macro->mobiles_interfered[macro->num_mobiles_interfered++] = this;
	mobile->macros_interfered[mobile->num_macros_interfered++] = this;

	if (distance < RANGE_MACRO)
		macro->mobiles_in_range[macro->num_mobiles_in_range++] = this;

	if (mobile->macro == NULL || distance < mobile->macro->distance)
		mobile->macro = this;

}

void Macro_Mobile::generate_channel_gain() {
	channel_gain
		= channel_gain_factor
		* RAYLEIGH()
		* LOG_NORMAL()
	;
	/*
	printf("M (%12.6f, %12.6f) - (%12.6f, %12.6f): %12.6f - %f <- %lf:%lf:%lf = %lf * pow((1 / %lf), %d)\n",
		macro->x, macro->y,
		mobile->x, mobile->y,
		distance,
		channel_gain,
		channel_gain_factor,
		(macro->get_tx_power() * pow((1 / distance), (double) PATH_LOSS_EXPO)),
		(pow(1 / distance, PATH_LOSS_EXPO)),
		macro->get_tx_power(), distance, PATH_LOSS_EXPO
	);
	//*/
}

double Macro_Mobile::get_channel_gain() {
	return channel_gain;
}

const Macro* Macro_Mobile::get_macro() const {
	return macro;
}

const Mobile* Macro_Mobile::get_mobile() const {
	return mobile;
}

double Macro_Mobile::get_distance() const {
	return distance;
}
