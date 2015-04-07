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

	if (mobile->pico == NULL || distance < mobile->pico->distance)
		mobile->pico = this;

}

void Pico_Mobile::register_mobile() {
	Pico* pico = (Pico*) this->pico;
	if (mobile->pico == this)
		pico->mobiles_to_service[pico->num_mobiles_to_service++] = this;
}

void Pico_Mobile::generate_channel_gain() {
	FOREACH_RBS
		channel_gain[ri]
			= channel_gain_factor
			* RAYLEIGH()
			* LOG_NORMAL()
		;
}

double Pico_Mobile::get_channel_gain(int ri) {
	return channel_gain[ri];
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
