#pragma once

//#include "pico.h"
//#include "mobile.h"

class Pico;
class Mobile;

class Pico_Mobile {

public:
private: public:

	const Pico* pico;
	const Mobile* mobile;
	const double distance;

	// Time Context

	const double channel_gain_factor;
	double channel_gain;

public:

	Pico_Mobile(Pico* pico, Mobile* mobile);

	bool is_in_range();

	void generate_channel_gain();
	double get_channel_gain();

	const Pico* get_pico() const;
	const Mobile* get_mobile() const;
	double get_distance() const;

};
