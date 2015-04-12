#include <stdio.h>

#include <stdlib.h>
#include <float.h>

#include "func.h"

#include "pico_mobile.h"
#include "macro.h"
#include "mobile.h"
#include "pico.h"

int non_cmp(int ri, const Mobile* a, const Mobile* b) {
	double cmp
		= ((Mobile*) a)->get_non_pico_lambda_r(ri)
		-  ((Mobile*) b)->get_non_pico_lambda_r(ri);
	if (cmp < 0.0)
		return -1;
	else if (cmp > 0.0)
		return 1;
	else
		return 0;
}

int abs_cmp(int ri, const Mobile* a, const Mobile* b) {
	double cmp
		= ((Mobile*) a)->get_abs_pico_lambda_r(ri)
		- ((Mobile*) b)->get_abs_pico_lambda_r(ri);
	if (cmp < 0.0)
		return -1;
	else if (cmp > 0.0)
		return 1;
	else
		return 0;
}


Pico::Pico(int idx, double x, double y, double tx_power)
: idx(idx), x(x), y(y), tx_power(tx_power)
{
	num_mobiles_to_service = 0;
	num_mobiles_interfered = 0;
	num_macros_interfering = 0;
}

void Pico::check_interfering(Macro* macro) {
	if (sqrt(pow((macro->x - x), 2) + pow((macro->y - y), 2)) < MP_INT_DIST) {
		macros_interfering[num_macros_interfering++] = macro;
	}
}

bool Pico::is_abs() const {
	for (int i = 0; i < num_macros_interfering; i++)
		if (macros_interfering[i]->get_state())
			return false;
	return true;
}

void Pico::sort_mobiles(int ri, Pico_Mobile** items, int size, Pico_Mobile** sorted, int cmp(int, const Mobile*, const Mobile*)) {

	if (size == 0)
		return;

	Pico_Mobile* former[NUM_PM];
	int former_count = 0;
	Pico_Mobile* latter[NUM_PM];
	int latter_count = 0;

	const Mobile* base = items[0]->get_mobile();

	for (int i = 1; i < size; i++) {
		const Mobile* mobile = items[i]->get_mobile();
		if (cmp(ri, base, mobile) < 0) {
			former[former_count++] = items[i];
		} else {
			latter[latter_count++] = items[i];
		}
	}

	Pico_Mobile* sorted_former[NUM_PM];
	sort_mobiles(ri, former, former_count, sorted_former, cmp);
	Pico_Mobile* sorted_latter[NUM_PM];
	sort_mobiles(ri, latter, latter_count, sorted_latter, cmp);

	for (int i = 0; i < former_count; i++)
		sorted[i] = sorted_former[i];
	sorted[former_count] = items[0];
	for (int i = 0; i < latter_count; i++)
		sorted[former_count + 1 + i] = sorted_latter[i];

}

void Pico::sort_mobiles() {

	FOREACH_RBS {
		
		sort_mobiles(ri, mobiles_to_service, num_mobiles_to_service,
			non_sorted_mobiles[ri], non_cmp);
		sort_mobiles(ri, mobiles_to_service, num_mobiles_to_service,
			abs_sorted_mobiles[ri], abs_cmp);

		first_mobile[ri] = NULL;
		second_mobile[ri] = NULL;

		double first_lambda_r = DBL_MIN;
		double second_lambda_r = DBL_MIN;

		abs_first_mobile[ri] = NULL;
		abs_second_mobile[ri] = NULL;

		double abs_first_lambda_r = DBL_MIN;
		double abs_second_lambda_r = DBL_MIN;

		//printf("num_mobiles_to_service: %d\n", num_mobiles_to_service);
		for (int pm = 0; pm < num_mobiles_to_service; pm++) {

			Mobile* mobile = (Mobile*) mobiles_to_service[pm]->get_mobile();

			const double lambda_r = mobile->lambda * mobile->get_abs_pico_throughput(ri);

			if (first_mobile[ri] == NULL) {

				first_mobile[ri] = mobile;
				first_lambda_r = lambda_r;

			} else {

				if (lambda_r > first_lambda_r) {

					second_mobile[ri] = first_mobile[ri];
					second_lambda_r = first_lambda_r;

					first_mobile[ri] = mobile;
					first_lambda_r = lambda_r;

				} else if (lambda_r > second_lambda_r) {

					second_mobile[ri] = mobile;
					second_lambda_r = lambda_r;

				} else {

					// DO NOTHING

				}

			}

			// /////////////////////////////////////////////////////////////////////
			// ABS

			const double abs_lambda_r = mobile->lambda * mobile->get_abs_pico_throughput(ri);

			if (abs_first_mobile[ri] == NULL) {

				abs_first_mobile[ri] = mobile;
				abs_first_lambda_r = lambda_r;

			} else {

				if (abs_lambda_r > abs_first_lambda_r) {

					abs_second_mobile[ri] = abs_first_mobile[ri];
					abs_second_lambda_r = abs_first_lambda_r;

					abs_first_mobile[ri] = mobile;
					abs_first_lambda_r = abs_lambda_r;

				} else if (abs_lambda_r > abs_second_lambda_r) {

					abs_second_mobile[ri] = mobile;
					abs_second_lambda_r = abs_lambda_r;

				} else {

					// DO NOTHING

				}

			}

		}

	}

}

int Pico::get_num_mobiles_to_service() const {
	return num_mobiles_to_service;
}

Pico_Mobile* Pico::get_non_sorted_mobile(int ri, int mob) const {
	return non_sorted_mobiles[ri][mob];
}

Pico_Mobile* Pico::get_abs_sorted_mobile(int ri, int mob) const {
	return abs_sorted_mobiles[ri][mob];
}

Mobile* Pico::get_first_mobile(int ri) {
	return first_mobile[ri];
}

Mobile* Pico::get_second_mobile(int ri) {
	return second_mobile[ri];
}

Mobile* Pico::get_abs_first_mobile(int ri) {
	return abs_first_mobile[ri];
}

Mobile* Pico::get_abs_second_mobile(int ri) {
	return abs_second_mobile[ri];
}
