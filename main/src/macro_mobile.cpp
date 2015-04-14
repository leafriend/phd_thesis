#include <stdio.h>

#include <stdlib.h>
#include <math.h>

#include "config.h"
#include "func.h"
#include "macro.h"
#include "mobile.h"
#include "macro_mobile.h"

Macro_Mobile::Macro_Mobile(Macro* macro, Mobile* mobile)
	: macro(macro), Edge(macro, mobile)
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
