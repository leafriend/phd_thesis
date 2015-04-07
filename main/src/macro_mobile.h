#pragma once

//#include "macro.h"
//#include "mobile.h"

class Macro;
class Mobile;

class Macro_Mobile {

public:
private: public:

	const Macro* macro;
	const Mobile* mobile;
	const double distance;

	// Time Context

	const double channel_gain_factor;
	double channel_gain;

public:

	Macro_Mobile(Macro* macro, Mobile* mobile);

	void generate_channel_gain();
	double get_channel_gain();

	const Macro* get_macro() const;
	const Mobile* get_mobile() const;
	double get_distance() const;

};
