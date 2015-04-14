#include <stdio.h>
#include <stdlib.h>

#include "func.h"
#include "basestation.h"
#include "mobile.h"
#include "edge.h"

Edge::Edge(EdgeType type, BaseStation* bs, Mobile* mobile)
	: type(type), bs(bs), mobile(mobile), distance(sqrt(pow((bs->x - mobile->x), 2) + pow((bs->y - mobile->y), 2)))
, channel_gain_factor(bs->tx_power * pow((1 / distance), (double) PATH_LOSS_EXPO))
{
	switch (type)
	{
	case EDGE_MACRO:
		mobile->macros_interfered[mobile->num_macros_interfered++] = this;

		if (mobile->macro == NULL || distance < mobile->macro->distance)
			mobile->macro = this;
		break;
	case EDGE_PICO:
		mobile->picos_interfered[mobile->num_picos_interfered++] = this;

		if (mobile->pico == NULL || distance < mobile->pico->distance)
			if (RANGE_PICO < 0 || distance < RANGE_PICO)
				mobile->pico = this;
		break;
	default:
		fprintf(stderr, "ERROR: Unexpected Edge Type: %d\n", type);
		exit(-1);
		break;
	}
}

void Edge::generate_channel_gain() {
	FOREACH_RBS
		channel_gain[ri]
			= channel_gain_factor
			* RAYLEIGH()
			* LOG_NORMAL()
		;
}

double Edge::get_channel_gain(int ri) {
	return channel_gain[ri];
}

void Edge::register_mobile() {
	switch (type)
	{
	case EDGE_MACRO:
		if (mobile->macro == this) {
			BaseStation* bs = (BaseStation*) this->bs;
			bs->mobiles_to_service[bs->num_mobiles_to_service++] = this;
		}
		break;
	case EDGE_PICO:
		if (mobile->pico == this) {
			BaseStation* bs = (BaseStation*) this->bs;
			bs->mobiles_to_service[bs->num_mobiles_to_service++] = this;
		}
		break;
	default:
		fprintf(stderr, "ERROR: Unexpected Edge Type: %d\n", type);
		exit(-1);
		break;
	}
}
