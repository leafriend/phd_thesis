#include <stdlib.h>

#include "func.h"
#include "basestation.h"
#include "mobile.h"
#include "edge.h"

Edge::Edge(EdgeType type, BaseStation* bs, Mobile* mobile)
	: type(type), bs(bs), mobile(mobile), distance(sqrt(pow((bs->x - mobile->x), 2) + pow((bs->y - mobile->y), 2)))
, channel_gain_factor(bs->tx_power * pow((1 / distance), (double) PATH_LOSS_EXPO))
{
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
