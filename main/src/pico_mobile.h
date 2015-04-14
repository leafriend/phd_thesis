#pragma once

#include "config.h"
#include "edge.h"

class Pico;
class Mobile;

class Pico_Mobile : public Edge {
	
public:

	const Pico* pico;

public:

	Pico_Mobile(Pico* pico, Mobile* mobile);

	void register_mobile();

};
