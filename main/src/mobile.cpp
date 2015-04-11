#include <stdio.h>

#include <stdlib.h>

#include "func.h"
#include "macro_mobile.h"
#include "pico_mobile.h"
#include "macro.h"
#include "pico.h"
#include "mobile.h"

#define THOURGHPUT(bandwidth, channel_gain, interference, noise) \
	((bandwidth) * log(1 + ((channel_gain) / ((interference) + (noise)))))

Mobile::Mobile(int idx, double x, double y, double lambda, double mu, double rate_user, double qos)
: idx(idx), x(x), y(y), lambda(lambda), mu(mu), rate_user(rate_user), qos(qos)
{
	macro = NULL;
	pico = NULL;

	result_throughput = 0.0;
	instant_rate = 0.0;

	num_macros_interfered = 0;
	num_picos_interfered = 0;
}

void Mobile::generate_channel_gain()
{

	FOREACH_RBS {

		double sum_macro_channel_gain = 0.0;
		for (int mac = 0; mac < num_macros_interfered; mac++) {
			sum_macro_channel_gain += macros_interfered[mac]->get_channel_gain(ri);
		}

		double sum_pico_channel_gain = 0.0;
		for (int pic = 0; pic < num_picos_interfered; pic++) {
			sum_pico_channel_gain += picos_interfered[pic]->get_channel_gain(ri);
		}

		// /////////////////////////////////

		// TODO NULL

		macro_throughput[ri] = THOURGHPUT(
			BW_PER_RB,
			macro->get_channel_gain(ri),
			sum_macro_channel_gain + sum_pico_channel_gain - macro->get_channel_gain(ri),
			NOISE
		) / MEGA;

		if (pico == NULL) {
			
			non_pico_throughput[ri] = 0.0;

			// TODO
			abs_pico_throughput[ri] = 0.0;

		} else {
			
			non_pico_throughput[ri] = THOURGHPUT(
				BW_PER_RB,
				pico->get_channel_gain(ri),
				sum_macro_channel_gain + sum_pico_channel_gain - pico->get_channel_gain(ri),
				NOISE
			) / MEGA;

			// TODO
			abs_pico_throughput[ri] = THOURGHPUT(
				BW_PER_RB,
				pico->get_channel_gain(ri),
				/*sum_macro_channel_gain +*/ sum_pico_channel_gain - pico->get_channel_gain(ri),
				NOISE
			) / MEGA;

		}

	}

}

void Mobile::calculate_throughput() {
	instant_rate = 0.0;
	FOREACH_RBS {
		switch (states[ri]) {
			case 0:
				instant_rate += 0.0;
			case 1:
				instant_rate += macro_throughput[ri];
				break;
			case 2:
				instant_rate += abs_pico_throughput[ri];
				break;
			case 3:
			case 4:
				instant_rate += non_pico_throughput[ri];
				break;
		}
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

	const double step_size = 1.0 / ((double) t);
	const double step_size2
		= (t > 100000)
		? STEPSIZE4
		:	( (t < 10000)
			? STEPSIZE2
			: STEPSIZE3
			)
	;

	double lambda_temp;
	if ((abs(result_throughput / t - rate_user) * lambda < 0.05))
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

double Mobile::get_macro_throughput(int ri) const {
	return macro_throughput[ri];
}

double Mobile::get_non_pico_throughput(int ri) const {
	return non_pico_throughput[ri];
}

double Mobile::get_abs_pico_throughput(int ri) const {
	return abs_pico_throughput[ri];
}

Macro_Mobile* Mobile::get_macro() const {
	return macro;
}

Pico_Mobile* Mobile::get_pico() const {
	return pico;
}

void Mobile::set_state(int ri, int state) {
	states[ri] = state;
}

int Mobile::get_state(int ri) const {
	return states[ri];
}

double Mobile::get_rate_user() {
	return rate_user;
}

double Mobile::get_result_throughput() {
	return result_throughput;
}
