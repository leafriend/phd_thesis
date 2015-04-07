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

void Mobile::calculate_throughput(int mobile_state) {
	switch (mobile_state) {
		case 0:
			instant_rate = 0.0;
		case 1:
			instant_rate = macro_throughput;
			break;
		case 2:
			instant_rate = abs_pico_throughput;
			break;
		case 3:
		case 4:
			instant_rate = pico_throughput;
			break;
	}
	result_throughput += instant_rate;
}


void Mobile::calculate_rate_user() {
	if (lambda == 0.0)
		rate_user = RATE_MAX;
	else
		rate_user
			= 0.8 * rate_user
			+ 0.2 * (1.0 + mu) / lambda
		;
}

void Mobile::calculate_dual_variable(const int t) {

	const double step_size = 1.0 / ((double)(t + 1));
	const double step_size2
		= (t > 100000)
		? STEPSIZE4
		:	( (t < 10000)
			? STEPSIZE2
			: STEPSIZE3
			)
	;

	double lambda_temp;
	if ((abs(result_throughput / (1 + t) - rate_user) * lambda < 0.05))
		lambda_temp = lambda - step_size  * (instant_rate - rate_user);
	else
		lambda_temp = lambda - step_size2 * (instant_rate - rate_user);
	this->lambda = lambda_temp > 0 ? lambda_temp : 0.0;

	double mu_temp;
	if ((abs(log(rate_user) - qos) * mu < 0.01))
		mu_temp = mu - step_size * (log(rate_user) - qos);
	else
		mu_temp = mu - step_size2 * (log(rate_user) - qos);
	this->mu = (0.0 > mu_temp) ? 0.0 : mu_temp;

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
