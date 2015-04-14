#pragma once

#include "config.h"

//#include "macro.h"
//#include "mobile.h"

class BaseStation;
class Mobile;

class Edge {

public:

	const BaseStation* macro;
	const Mobile* mobile;
	const double distance;

	// Time Context
	
private:

	const double channel_gain_factor;
	double channel_gain[NUM_RB];

public:

	Edge(BaseStation* macro, Mobile* mobile);

	void register_mobile();

	void generate_channel_gain();
	double get_channel_gain(int ri);

};
