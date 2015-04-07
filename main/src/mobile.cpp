#include <stdio.h>

#include <stdlib.h>

#include "func.h"
#include "macro_mobile.h"
#include "pico_mobile.h"
#include "macro.h"
#include "pico.h"
#include "mobile.h"

#define THOURGHPUT(channel_gain, interference, noise) \
	(BW * log(1 + ((channel_gain) / ((interference) + (noise)))))

Mobile::Mobile(int idx, double x, double y, double qos)
: idx(idx), x(x), y(y), qos(qos)
{
	macro = NULL;
	pico = NULL;

	result_throughput = 0.0;
	rate_user = 0.0;
	instant_rate = 0.0;

	lambda = 0.1;
	mu = 0.0;

	num_macros_interfered = 0;
	num_picos_interfered = 0;
}

void Mobile::calculate_throughputs()
{

	double sum_macro_channel_gain = 0.0;
	for (int mac = 0; mac < num_macros_interfered; mac++) {
		sum_macro_channel_gain += macros_interfered[mac]->get_channel_gain();
	}

	double sum_pico_channel_gain = 0.0;
	for (int pic = 0; pic < num_picos_interfered; pic++) {
		sum_pico_channel_gain += picos_interfered[pic]->get_channel_gain();
	}

	// /////////////////////////////////

	// TODO NULL

	double macro_channel_gain = macro == NULL ? 0 : macro->get_channel_gain();
	//printf("macro_channel_gain: %lf\n", macro_channel_gain);
	macro_throughput = THOURGHPUT(
		macro_channel_gain,
		sum_macro_channel_gain + sum_pico_channel_gain - macro_channel_gain,
		NOISE
	) / MEGA;

	double pico_channel_gain = pico == NULL ? 0 : pico->get_channel_gain();

	pico_throughput = THOURGHPUT(
		pico_channel_gain,
		sum_macro_channel_gain + sum_pico_channel_gain - pico_channel_gain,
		NOISE
	) / MEGA;

	// TODO
	abs_pico_throughput = THOURGHPUT(
		pico_channel_gain,
		/*sum_macro_channel_gain +*/ sum_pico_channel_gain - pico_channel_gain,
		NOISE
	) / MEGA;

}

double Mobile::get_macro_throughput() const {
	return macro_throughput;
}

double Mobile::get_pico_throughput() const {
	return pico_throughput;
}

double Mobile::get_abs_pico_throughput() const {
	return abs_pico_throughput;
}

Macro_Mobile* Mobile::get_macro() const {
	return macro;
}

Pico_Mobile* Mobile::get_pico() const {
	return pico;
}
