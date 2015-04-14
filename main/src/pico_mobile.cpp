#include <stdio.h>

#include <stdlib.h>
#include <math.h>

#include "config.h"
#include "func.h"
#include "pico.h"
#include "mobile.h"
#include "pico_mobile.h"

Pico_Mobile::Pico_Mobile(Pico* pico, Mobile* mobile)
	: pico(pico), Edge(EDGE_PICO, pico, mobile)
{

}

void Pico_Mobile::register_mobile() {
	Pico* pico = (Pico*) this->pico;
	if (mobile->pico == this)
		pico->mobiles_to_service[pico->num_mobiles_to_service++] = this;
}
