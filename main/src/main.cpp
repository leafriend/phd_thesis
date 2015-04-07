#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <float.h>
#include <math.h>

#include "func.h"
#include "macro.h"
#include "pico.h"
#include "mobile.h"
#include "macro_mobile.h"
#include "pico_mobile.h"

void pa1(int* best_mobile_states, Macro** macros, Pico** picos, Mobile** mobiles);

int main(int argc, char** argv) {

	FILE* fp;

	//printf("/ Start ===============================\\\n");

	//printf("= Macro ================================\n");
	Macro* macros[NUM_MACRO];
	fp = fopen(DATA_FILE_PREFIX "macro.txt", "r");
	for (int mac = 0; mac < NUM_MACRO; mac++) {
		double x, y, tx_power = TX_POWER_MACRO;
		fscanf(fp, "%lf", &x);
		fscanf(fp, "%lf", &y);
		//printf("(%12.6f, %12.6f): %6.3f\n", x, y, tx_power);
		macros[mac] = new Macro(mac, x, y, tx_power);
	}
	fclose(fp);

	//printf("= Pico =================================\n");
	Pico* picos[NUM_PICO];
	fp = fopen(DATA_FILE_PREFIX "pico.txt", "r");
	for (int pic = 0; pic < NUM_PICO; pic++) {
		double x, y, tx_power = TX_POWER_MACRO;
		fscanf(fp, "%lf", &x);
		fscanf(fp, "%lf", &y);
		//printf("(%12.6f, %12.6f): %6.3f\n", x, y, tx_power);
		picos[pic] = new Pico(pic, x, y, tx_power);
	}
	fclose(fp);

	Macro_Mobile* macro_mobiles[NUM_MM];
	Pico_Mobile* pico_mobiles[NUM_PICO * NUM_MOBILE];

	//printf("= Mobile ===============================\n");
	Mobile* mobiles[NUM_MOBILE];
	fp = fopen(DATA_FILE_PREFIX "mobile.txt", "r");
	for (int mob = 0; mob < NUM_MOBILE; mob++) {
		double x, y, qos = QOS;
		fscanf(fp, "%lf", &x);
		fscanf(fp, "%lf", &y);
		//printf("(%12.6f, %12.6f): %6.3f\n", x, y, qos);
		Mobile* mobile = new Mobile(mob, x, y, qos);
		mobiles[mob] = mobile;

		for (int mac = 0; mac < NUM_MACRO; mac++) {
			macro_mobiles[mob * NUM_MACRO + mac] = new Macro_Mobile(macros[mac], mobile);
		}

		for (int pic = 0; pic < NUM_PICO; pic++) {
			pico_mobiles[mob * NUM_PICO + pic] = new Pico_Mobile(picos[pic], mobile);
		}

		if (mobile->macro == NULL) printf("Mobile at (%12.6f, %12.6f): %6.3f has no service Macro\n", x, y, qos);
		if (mobile->pico == NULL) printf("Mobile at (%12.6f, %12.6f): %6.3f has no service Pico\n", x, y, qos);

	}
	fclose(fp);

	// /////////////////////////////////////////////////////////////////////////

	// Time Check //////////////////////////////////////////////////////////////
	TIME start, stop, ellapse;
	TIME_GET(&start);
	printf("/ Start ===============================\\\n");
	printf("                         %6d.%06d s\n", TIME_GETSEC(&start) - BASE_SEC, TIME_MSEC(&start));

	TIME t_start, t_stop;
	TIME_GET(&t_start);
	// ////////////////////////////////////////////////////////////////////// */

	for (int t = 0; t < SIMULATION_TIME; t++) {

		// Time Check //////////////////////////////////////////////////////////
		/*
		if ((t + 1) % LOG_INTERVAL_TIME == 0) {
			printf("\n");
			printf("    / T:%8d ======================\\\n", (t + 1));
			printf("                         %6d.%06d s\n", t_start.tv_sec - BASE_SEC, t_start.tv_usec);
		}
		//*/
		// /////////////////////////////////////////////////////////////////////

		for (int mm = 0; mm < NUM_MM; mm++)
			macro_mobiles[mm]->generate_channel_gain();

		for (int pm = 0; pm < NUM_MM; pm++)
			pico_mobiles[pm]->generate_channel_gain();

		for (int mob = 0; mob < NUM_MOBILE; mob++)
			mobiles[mob]->calculate_throughputs();

		// /////////////////////////////////////////////////////////////////////

		for (int pic = 0; pic < NUM_PICO; pic++)
			picos[pic]->sort_mobiles();

		for (int mac = 0; mac < NUM_MACRO; mac++)
			macros[mac]->sort_mobiles();

		// /////////////////////////////////////////////////////////////////////
		// PROCESS ALGORITHM ///////////////////////////////////////////////////

		int best_mobile_states[NUM_MOBILE];
		pa1(best_mobile_states, macros, picos, mobiles);

		// /////////////////////////////////////////////////////////////////////

		for (int mob = 0; mob < NUM_MOBILE; mob++) {
			Mobile* mobile = mobiles[mob];

			switch (best_mobile_states[mob]) {
				case 0:
					mobile->instant_rate = 0.0;
				case 1:
					mobile->instant_rate = mobile->macro_throughput;
					break;
				case 2:
					mobile->instant_rate = mobile->abs_pico_throughput;
					break;
				case 3:
				case 4:
					mobile->instant_rate = mobile->pico_throughput;
					break;
			}
			mobile->result_throughput += mobile->instant_rate;

			// /////////////////////////////////////////////////////////////////

			if (mobile->lambda == 0.0)
				mobile->rate_user = RATE_MAX;
			else
				mobile->rate_user
					= 0.8 * mobile->rate_user
					+ 0.2 * (1.0 + mobile->mu) / mobile->lambda
				;

			// /////////////////////////////////////////////////////////////////

			const double step_size = 1.0 / ((double)(t + 1));
			const double step_size2
				= (t > 100000)
				? STEPSIZE4
				:	( (t < 10000)
					? STEPSIZE2
					: STEPSIZE3
					)
			;

			//*
			double lambda_temp, mu_temp;
			if ((abs(mobile->result_throughput / (1 + t) - mobile->rate_user) * mobile->lambda < 0.05))
				lambda_temp = mobile->lambda - step_size  * (mobile->instant_rate - mobile->rate_user);
			else
				lambda_temp = mobile->lambda - step_size2 * (mobile->instant_rate - mobile->rate_user);
			//if (lambda_temp < 0) printf("%lf\n", lambda_temp);
			//printf("%lf\n", mobile->lambda);
			mobile->lambda = lambda_temp > 0 ? lambda_temp : 0.0; //(0.0 > lambda_temp) ? 0.0 : lambda_temp;

			//*
			if ((abs(log(mobile->rate_user) - mobile->qos) * mobile->mu < 0.01))
				mu_temp = mobile->mu - step_size * (log(mobile->rate_user) - mobile->qos);
			else
				mu_temp = mobile->mu - step_size2 * (log(mobile->rate_user) - mobile->qos);
			mobile->mu = (0.0 > mu_temp) ? 0.0 : mu_temp;
			//*/
		}

		// /////////////////////////////////////////////////////////////////////

		// for (int mob = 0; mob < NUM_MOBILE; mob++) printf("%d", best_mobile_states[mob]); printf("\n");

		for (int mac = 0; mac < NUM_MACRO; mac++) {
			Macro* macro = macros[mac];
			for (int mob = 0; mob < macro->num_mobiles_in_range; mob++) {
				Mobile* mobile = (Mobile*) macro->mobiles_in_range[mob]->mobile;
				if (best_mobile_states[mobile->idx] == 1) {
					macro->allocation_count++;
					break;
				}
			}
		}

		// /////////////////////////////////////////////////////////////////////

		if (t % 250 == 0)
			printf(".");

		// /////////////////////////////////////////////////////////////////////

		if ((t + 1) % LOG_INTERVAL_TIME == 0) {

			// Time Check //////////////////////////////////////////////////////
			TIME_GET(&t_stop);
			// ////////////////////////////////////////////////////////////// */

			printf("\n");

			for (int mob = 0; mob < NUM_MOBILE; mob++) {
				//printf("%f\t%f\t%f\t%f\n", mobiles[mob]->rate_user_PA1, log(mobiles[mob]->rate_user_PA1), (mobiles[mob]->thrp_result_PA1 / (1 + t)), log(mobiles[mob]->thrp_result_PA1 / (1 + t)));
				printf("%f\t%f\t%f\t%f\t%f\t%f\n",
					mobiles[mob]->rate_user,
					mobiles[mob]->result_throughput / (1 + t),
					log(mobiles[mob]->rate_user),
					log(mobiles[mob]->result_throughput / (1 + t)),
					mobiles[mob]->lambda,
					mobiles[mob]->mu
				);
			}

			for (int mac = 0; mac < NUM_MACRO; mac++) {
				printf("%8d (%f) ", macros[mac]->allocation_count, macros[mac]->allocation_count / (double) (t + 1));
			}
			printf("\n");
			printf("Time: %d\t", (1 + t));
			printf("QOS: %f\t", QOS);

			/*
			printf("\n");
			for (int mac = 0; mac < MACRO_NUM; mac++)
			{
				printf("%d\t", abs_count_macro[mac]);
				printf("%f\n", (double)abs_count_macro[mac] / ((double) t + 1));
			}
			//*/

			// Time Check //////////////////////////////////////////////////////
			subtract_timeval(&ellapse, &t_stop, &t_start);
			printf("Execution Time:%6d.%06d\t", TIME_GETSEC(&ellapse), TIME_MSEC(&ellapse));

			subtract_timeval(&ellapse, &t_stop, &start);
			printf("Elapsed Time:%6d.%06d\t", TIME_GETSEC(&ellapse), TIME_MSEC(&ellapse));
			// ////////////////////////////////////////////////////////////// */

			double sum_utility = 0.0;
			for (int mob = 0; mob < NUM_MOBILE; mob++) {
				double utility = log(mobiles[mob]->result_throughput / (1 + t));
				if (!isinf(utility))
					sum_utility += utility;
			}
			printf("Sum Utility: %f\n", sum_utility);

			/*
			gettimeofday(&t_stop, NULL);
			printf("                         %6d.%06d s\n", t_stop.tv_sec - BASE_SEC, t_stop.tv_usec);
			printf("    \\__________________________________/\n");

			subtract_timeval(&ellapse, &t_stop, &t_start);
			printf("    ---------------------%6d.%06d s\n", ellapse.tv_sec, ellapse.tv_usec);

			subtract_timeval(&ellapse, &t_stop, &start);
			printf("    =====================%6d.%06d s\n", ellapse.tv_sec, ellapse.tv_usec);
			printf("\n");
			//*/

			TIME_GET(&t_start);

		}
		// /////////////////////////////////////////////////////////////////////

	}

	// Time Check //////////////////////////////////////////////////////////////
	TIME_GET(&stop);
	printf("                         %6d.%06d s\n", TIME_GETSEC(&stop) - BASE_SEC, TIME_MSEC(&stop));
	printf("\\______________________________________/\n");

	subtract_timeval(&ellapse, &stop, &start);
	printf("=========================%6d.%06d s\n", TIME_GETSEC(&ellapse), TIME_MSEC(&ellapse));
	// ////////////////////////////////////////////////////////////////////// */

	// /////////////////////////////////////////////////////////////////////////

	for (int mm = 0; mm < NUM_MM; mm++)
		delete macro_mobiles[mm];

	for (int pm = 0; pm < NUM_PM; pm++)
		delete pico_mobiles[pm];

	for (int mac = 0; mac < NUM_MACRO; mac++)
		delete macros[mac];

	for (int pic = 0; pic < NUM_PICO; pic++)
		delete picos[pic];

	for (int mob = 0; mob < NUM_MOBILE; mob++)
		delete mobiles[mob];

	system("pause");
	return 0;

}

void pa1(int* best_mobile_states, Macro** macros, Pico** picos, Mobile** mobiles) {

	//int best_mobile_states[NUM_MOBILE];
	for (int mob = 0; mob < NUM_MOBILE; mob++)
		best_mobile_states[mob] = 0;

	double best_sum_lambda_r = DBL_MIN;

	bool best_macro_states[NUM_MACRO];
	for (int mac = 0; mac < NUM_MACRO; mac++)
		best_macro_states[mac] = false;

	int num_macro_state = 1 << NUM_MACRO;

	//struct timeval start, stop, ellapse;
	//gettimeofday(&start, NULL);

	for (int s = 0; s < num_macro_state; s++) {

		double curr_sum_lambda_r = 0.0;

		bool curr_macro_states[NUM_MACRO];
		for (int mac = 0; mac < NUM_MACRO; mac++)
			curr_macro_states[mac] = ((1 << mac) & s) >> mac;

		int curr_mobile_states[NUM_MOBILE];
		for (int mob = 0; mob < NUM_MOBILE; mob++)
			curr_mobile_states[mob] = 0;

		//printf("SATAE   "); for (int mac = NUM_MACRO; mac --> 0;) printf("%d", curr_macro_states[mac]); printf("\n");

		for (int mac = 0; mac < NUM_MACRO; mac++) {
			if (curr_macro_states[mac]) {
				Mobile* mobile = macros[mac]->first_mobile;

				curr_sum_lambda_r += mobile->lambda * mobile->macro_throughput;
				curr_mobile_states[mobile->idx] = 1;
			}
		}

		for (int pic = 0; pic < NUM_PICO; pic++) {

			Pico* pico = picos[pic];

			bool is_abs = true;

			for (int mac = 0; mac < NUM_MACRO; mac++) {
				if (curr_macro_states[mac] == ON && pico->is_neighbor(macros[mac])) {
					is_abs = false;
					break;
				}
			}

			if (is_abs) {

				Mobile* abs_first = pico->abs_first_mobile;
				Mobile* abs_second = pico->abs_second_mobile;

				if (abs_first != NULL) {

					Macro* macro = abs_first->macro == NULL ? NULL : (Macro*) abs_first->macro->macro;

					if (macro != NULL
					&& ((curr_macro_states[macro->idx] == OFF
					||  macro->first_mobile != abs_first
					))) {
						curr_sum_lambda_r += abs_first->lambda * abs_first->abs_pico_throughput;
						curr_mobile_states[abs_first->idx] = 2;
						//printf("ABS - %2d = 2\n", abs_first->idx);

					} else if (abs_second != NULL) {
						curr_sum_lambda_r += abs_second->lambda * abs_second->abs_pico_throughput;
						curr_mobile_states[abs_second->idx] = 2;
						//printf("ABS - %2d = 2\n", abs_second->idx);

					}

				}

			} else {

				Mobile* first = pico->first_mobile;
				Macro* first_macro = first->macro == NULL ? NULL : (Macro*) first->macro->macro;

				if (first_macro != NULL
				&&  curr_macro_states[first_macro->idx] == ON
				&&  first_macro->first_mobile == first
				) {

					Mobile* second = pico->second_mobile;
					if (second != NULL && curr_mobile_states[second->idx] != 1) {
						curr_sum_lambda_r += second->lambda * second->pico_throughput;
						curr_mobile_states[second->idx] = 4;
						//printf("non - %2d = 4\n", second->idx);
					} else {
						//printf("non\n");
					}

				} else {

					curr_sum_lambda_r += first->lambda * first->pico_throughput;
					//if (curr_mobile_states[first->idx] != 3) {
					//	printf("non - %2d @ %d[%d] = 3   ", first->idx, first_macro->idx, curr_macro_states[first_macro->idx]);
					//	for (int mac = NUM_MACRO; mac --> 0;) printf("%d", curr_macro_states[mac]);
					//	printf(" (%p == %p)\n", first_macro->first_mobile, first);
					//}
					curr_mobile_states[first->idx] = 3;

				}

			}

			//for (int mob = 0; mob < NUM_MOBILE; mob++) printf("%d", curr_mobile_states[mob]); printf("\n");

		}

		if (curr_sum_lambda_r > best_sum_lambda_r) {

			//printf("BETTER  %12.6lf > %12.6lf - ", curr_sum_lambda_r, best_sum_lambda_r);
			//for (int mac = NUM_MACRO; mac --> 0;) printf("%d", curr_macro_states[mac]);

			best_sum_lambda_r = curr_sum_lambda_r;

			for (int mac = 0; mac < NUM_MACRO; mac++)
				best_macro_states[mac] = curr_macro_states[mac];

			for (int mob = 0; mob < NUM_MOBILE; mob++)
				best_mobile_states[mob] = curr_mobile_states[mob];

			//printf(" .\n");

		}

	}

	//gettimeofday(&stop, NULL);
	//subtract_timeval(&ellapse, &stop, &start);
	//printf("                        :%6d.%06d s\n", ellapse.tv_sec, ellapse.tv_usec);

	//printf("BEST    %12.6lf - ", best_sum_lambda_r); for (int mac = NUM_MACRO; mac --> 0;) printf(best_macro_states[mac] ? "%d" : "-", best_macro_states[mac]); printf("\n");
	//printf("    MOBILE             "); for (int mac = NUM_MACRO; mac --> 0;) printf(best_macro_states[mac] ? "%d" : "-", best_mobile_states[ macros[mac]->first_mobile->idx ]); printf("\n");
	//printf("\n");

}
