#include <stdio.h>

#include <stdlib.h>
#include <math.h>

#include "config.h"
#include "func.h"
#include "pico.h"
#include "mobile.h"
#include "pico_mobile.h"

Pico_Mobile::Pico_Mobile(Pico* pico, Mobile* mobile)
: pico(pico)
, mobile(mobile)
, distance(sqrt(pow((pico->x - mobile->x), 2) + pow((pico->y - mobile->y), 2)))
, channel_gain_factor(pico->tx_power * pow((1 / distance), PATH_LOSS_EXPO))
{

	pico->mobiles_interfered[pico->num_mobiles_interfered++] = this;
	mobile->picos_interfered[mobile->num_picos_interfered++] = this;

	if (distance < RANGE_PICO) {
		pico->mobiles_in_range[pico->num_mobiles_in_range++] = this;
	}
	if (mobile->pico == NULL || distance < mobile->pico->distance)
		mobile->pico = this;

}

void Pico_Mobile::generate_channel_gain() {
	channel_gain
		= channel_gain_factor
		* RAYLEIGH()
		* LOG_NORMAL()
	;
	/*
	printf("P (%12.6f, %12.6f) - (%12.6f, %12.6f): %12.6f - %f\n",
		pico->x, pico->y,
		mobile->x, mobile->y,
		distance,
		channel_gain
	);
	//*/
}

double Pico_Mobile::get_channel_gain() {
	return channel_gain;
}

const Pico* Pico_Mobile::get_pico() const {
	return pico;
}

const Mobile* Pico_Mobile::get_mobile() const {
	return mobile;
}

double Pico_Mobile::get_distance() const {
	return distance;
}
