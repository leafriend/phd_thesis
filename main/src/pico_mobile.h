#pragma once

#include "config.h"

class Pico;
class Mobile;

class Pico_Mobile {

private:

	const Pico* pico;
	const Mobile* mobile;
	const double distance;

	// Time Context

	const double channel_gain_factor;
	double channel_gain[NUM_RB];

public:

	Pico_Mobile(Pico* pico, Mobile* mobile);

	void register_mobile();

	void generate_channel_gain();
	double get_channel_gain(int ri);

	const Pico* get_pico() const;
	const Mobile* get_mobile() const;
	double get_distance() const;

};
