#pragma once

#include "config.h"

class Macro;
class Mobile;

class Macro_Mobile {

public:

	const Macro* macro;
	const Mobile* mobile;
	const double distance;

	// Time Context
	
private:

	const double channel_gain_factor;
	double channel_gain[NUM_RB];

public:

	Macro_Mobile(Macro* macro, Mobile* mobile);

	void register_mobile();

	void generate_channel_gain();
	double get_channel_gain(int ri);

};
