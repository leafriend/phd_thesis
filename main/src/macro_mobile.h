#pragma once

#include "config.h"
#include "edge.h"

class Macro;
class Mobile;

class Macro_Mobile : public Edge {

public:

	const Macro* macro;

public:

	Macro_Mobile(Macro* macro, Mobile* mobile);

	void register_mobile();

};
