#include <stdio.h>

#include <stdlib.h>
#include <math.h>

#include "config.h"
#include "func.h"
#include "pico.h"
#include "mobile.h"
#include "pico_mobile.h"

Pico_Mobile::Pico_Mobile(Pico* pico, Mobile* mobile)
	: pico(pico), Edge(pico, mobile)
{

	mobile->picos_interfered[mobile->num_picos_interfered++] = this;

	if (mobile->pico == NULL || distance < mobile->pico->distance)
		if (RANGE_PICO < 0 || distance < RANGE_PICO)
			mobile->pico = this;

}

void Pico_Mobile::register_mobile() {
	Pico* pico = (Pico*) this->pico;
	if (mobile->pico == this)
		pico->mobiles_to_service[pico->num_mobiles_to_service++] = this;
}
