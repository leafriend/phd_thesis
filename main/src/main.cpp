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
#include "pa1.h"

int main(int argc, char** argv) {

	FILE* fp;

	//printf("/ Start ===============================\\\n");

	//printf("= Macro ================================\n");
	Macro* macros[NUM_MACRO];
	fp = fopen(DATA_FILE_PREFIX "macro.txt", "r");
	FOREACH_MACROS {
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
	FOREACH_PICOS {
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
	FOREACH_MOBILES {
		double x, y, qos = QOS;
		fscanf(fp, "%lf", &x);
		fscanf(fp, "%lf", &y);
		//printf("(%12.6f, %12.6f): %6.3f\n", x, y, qos);
		Mobile* mobile = new Mobile(mob, x, y, qos);
		mobiles[mob] = mobile;

		FOREACH_MACROS {
			macro_mobiles[mob * NUM_MACRO + mac] = new Macro_Mobile(macros[mac], mobile);
		}

		FOREACH_PICOS {
			pico_mobiles[mob * NUM_PICO + pic] = new Pico_Mobile(picos[pic], mobile);
		}

		if (mobile->get_macro() == NULL) printf("Mobile at (%12.6f, %12.6f): %6.3f has no service Macro\n", x, y, qos);
		if (mobile->get_pico() == NULL) printf("Mobile at (%12.6f, %12.6f): %6.3f has no service Pico\n", x, y, qos);

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

		FOREACH_MOBILES
			mobiles[mob]->generate_channel_gain();

		// /////////////////////////////////////////////////////////////////////

		FOREACH_PICOS
			picos[pic]->sort_mobiles();

		FOREACH_MACROS
			macros[mac]->sort_mobiles();

		// /////////////////////////////////////////////////////////////////////
		// PROCESS ALGORITHM ///////////////////////////////////////////////////

		int best_mobile_states[NUM_MOBILE];
		pa1(best_mobile_states, macros, picos, mobiles);

		// /////////////////////////////////////////////////////////////////////

		FOREACH_MOBILES
			mobiles[mob]->calculate_throughput(best_mobile_states[mob]);
		
		//FOREACH_MOBILES mobiles[mob]->count_cell_association(best_mobile_states[mob]));

		FOREACH_MOBILES
			mobiles[mob]->calculate_rate_user();

		FOREACH_MOBILES
			mobiles[mob]->calculate_dual_variable(t);

		// /////////////////////////////////////////////////////////////////////

		// FOREACH_MOBILES printf("%d", best_mobile_states[mob]); printf("\n");

		FOREACH_MACROS {
			macros[mac]->count_allocation(best_mobile_states);
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

			FOREACH_MOBILES {
				//printf("%f\t%f\t%f\t%f\n", mobiles[mob]->get_rate_user_()PA1, log(mobiles[mob]->get_rate_user_()PA1), (mobiles[mob]->get_thrp_result_()PA1 / (1 + t)), log(mobiles[mob]->get_thrp_result_()PA1 / (1 + t)));
				printf("%f\t%f\t%f\t%f\t%f\t%f\n",
					mobiles[mob]->rate_user,
					mobiles[mob]->result_throughput / (1 + t),
					log(mobiles[mob]->rate_user),
					log(mobiles[mob]->result_throughput / (1 + t)),
					mobiles[mob]->lambda,
					mobiles[mob]->mu
				);
			}

			FOREACH_MACROS {
				printf("%8d (%f) ", macros[mac]->get_allocation_count(), macros[mac]->get_allocation_count() / (double) (t + 1));
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
			FOREACH_MOBILES {
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

	FOREACH_MACROS
		delete macros[mac];

	FOREACH_PICOS
		delete picos[pic];

	FOREACH_MOBILES
		delete mobiles[mob];

	system("pause");
	return 0;

}
