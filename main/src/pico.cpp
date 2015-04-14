#include <stdio.h>

#include <stdlib.h>
#include <float.h>

#include <limits>

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
	: BaseStation(idx, x, y, tx_power)
{
	num_macros_interfering = 0;

	pa3_lambda_r = 1.0;
}

void Pico::check_interfering(Macro* macro) {
	if (sqrt(pow((macro->x - x), 2) + pow((macro->y - y), 2)) < MP_INT_DIST) {
		macros_interfering[num_macros_interfering++] = macro;
	}
}

bool Pico::is_abs() const {
	for (int i = 0; i < num_macros_interfering; i++)
		if (macros_interfering[i]->state == ON)
			return false;
	return true;
}

void Pico::sort_mobiles(int ri, Edge** items, int size, Edge** sorted, int cmp(int, const Mobile*, const Mobile*)) {

	if (size == 0)
		return;

	Edge* former[NUM_PM];
	int former_count = 0;
	Edge* latter[NUM_PM];
	int latter_count = 0;

	const Mobile* base = items[0]->mobile;

	for (int i = 1; i < size; i++) {
		const Mobile* mobile = items[i]->mobile;
		if (cmp(ri, base, mobile) < 0) {
			former[former_count++] = items[i];
		} else {
			latter[latter_count++] = items[i];
		}
	}

	Edge* sorted_former[NUM_PM];
	sort_mobiles(ri, former, former_count, sorted_former, cmp);
	Edge* sorted_latter[NUM_PM];
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

		/*
		printf("%d:\n", num_mobiles_to_service);
		double non_max = std::numeric_limits<double>::infinity();
		double abs_max = std::numeric_limits<double>::infinity();
		for (int mob = 0; mob < num_mobiles_to_service; mob++) {

			const Mobile* mobile = mobiles_to_service[mob]->mobile;
			printf("%2d:%f/%f\t", mobile->idx,
				mobile->get_non_pico_lambda_r(ri),
				mobile->get_abs_pico_lambda_r(ri)
			);

			const Mobile* non_mobile = non_sorted_mobiles[ri][mob]->mobile;
			double non = non_mobile->get_non_pico_lambda_r(ri);
			printf("%2d:%f\t", non_mobile->idx, non);
			if (non_max > non)
				non_max = non;
			else
				exit(-1);
			
			const Mobile* abs_mobile = abs_sorted_mobiles[ri][mob]->mobile;
			double abs = abs_mobile->get_abs_pico_lambda_r(ri);
			printf("%2d:%f\n", abs_mobile->idx, abs);
			if (abs_max > abs)
				abs_max = abs;
			else
				exit(-1);

		}
		//*/

	}

}

Edge* Pico::get_non_sorted_mobile(int ri, int mob) const {
	return non_sorted_mobiles[ri][mob];
}

Edge* Pico::get_abs_sorted_mobile(int ri, int mob) const {
	return abs_sorted_mobiles[ri][mob];
}
