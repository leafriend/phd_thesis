#pragma once

#include "config.h"

//#include "macro.h"
//#include "mobile.h"

class BaseStation;
class Mobile;

enum EdgeType {
	EDGE_MACRO,
	EDGE_PICO,
};

class Edge {

public:

	const EdgeType type;
	const BaseStation* bs;
	const Mobile* mobile;
	const double distance;

	// Time Context
	
private:

	const double channel_gain_factor;
	double channel_gain[NUM_RB];

public:

	Edge(EdgeType type, BaseStation* macro, Mobile* mobile);

	void register_mobile();

	void generate_channel_gain();
	double get_channel_gain(int ri);

};
