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

	if (mobile->macro == NULL || distance < mobile->macro->distance)
		mobile->macro = this;

}

void Macro_Mobile::register_mobile() {
	Macro* macro = (Macro*) this->macro;
	if (mobile->macro == this)
		macro->mobiles_to_service[macro->num_mobiles_to_service++] = this;
}

void Macro_Mobile::generate_channel_gain() {
	FOREACH_RBS
		channel_gain[ri]
			= channel_gain_factor
			* RAYLEIGH()
			* LOG_NORMAL()
		;
}

double Macro_Mobile::get_channel_gain(int ri) {
	return channel_gain[ri];
}
