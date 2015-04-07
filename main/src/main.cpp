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
#include "main.h"

int main(int argc, char** argv) {

	// Initialize //////////////////////////////////////////////////////////////
	Macro* macros[NUM_MACRO];
	Pico* picos[NUM_PICO];
	Macro_Mobile* macro_mobiles[NUM_MM];
	Pico_Mobile* pico_mobiles[NUM_PICO * NUM_MOBILE];
	Mobile* mobiles[NUM_MOBILE];
	initialize(macros, picos, mobiles, macro_mobiles, pico_mobiles);

	// Time Check //////////////////////////////////////////////////////////////
	TIME start, stop, ellapse;
	TIME_GET(&start);
	//printf("/ Start ===============================\\\n");
	//printf("                         %6d.%06d s\n", TIME_GETSEC(&start) - BASE_SEC, TIME_MSEC(&start));

	TIME t_start, t_stop;
	clock_t ccstart = clock(), ccstop;
	clock_t c_start = clock(), c_stop;
	TIME_GET(&t_start);
	// ////////////////////////////////////////////////////////////////////// */

	for (int t = 1; t <= SIMULATION_TIME; t++) {

		// Time Check //////////////////////////////////////////////////////////
		/*
		if (t % LOG_INTERVAL_TIME == 0) {
			printf("\n");
			printf("    / T:%8d ======================\\\n", t);
			printf("                         %6d.%06d s\n", t_start.tv_sec - BASE_SEC, t_start.tv_usec);
		}
		//*/
		// /////////////////////////////////////////////////////////////////////

		for (int mm = 0; mm < NUM_MM; mm++)
			macro_mobiles[mm]->generate_channel_gain();

		for (int pm = 0; pm < NUM_PM; pm++)
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

		if (t % LOG_INTERVAL_TIME == 0) {

			// Time Check //////////////////////////////////////////////////////
			TIME_GET(&t_stop);
			c_stop = clock();
			// ////////////////////////////////////////////////////////////// */

			printf("\n");

			printf(
				"Rate User  (log)\t"
				"Throughput (log)\t"
				"lmabda  \t"
				"mu\n");
			FOREACH_MOBILES {
				Mobile* mobile = mobiles[mob];
				printf(
					"%10.6f (%10.6f)\t"
					"%10.6f (%10.6f)\t"
					"%f\t"
					"%f\n",
					mobile->rate_user,
					log(mobile->rate_user),
					mobile->result_throughput / t,
					log(mobile->result_throughput / t),
					mobile->lambda,
					mobile->mu
				);
			}

			FOREACH_MACROS {
				printf("%7d(%5.2f%%)\t", macros[mac]->get_allocation_count(), 100 *	macros[mac]->get_allocation_count() / (double) t);
			}
			printf("\n");
			printf("Time: %d\t", t);
			printf("QOS: %f\t", QOS);

			/*
			printf("\n");
			for (int mac = 0; mac < MACRO_NUM; mac++)
			{
				printf("%d\t", abs_count_macro[mac]);
				printf("%f\n", (double)abs_count_macro[mac] / ((double) t));
			}
			//*/

			// Time Check //////////////////////////////////////////////////////
			subtract_timeval(&ellapse, &t_stop, &t_start);
			//printf("Execution Time:%6d.%06d\t", TIME_GETSEC(&ellapse), TIME_MSEC(&ellapse));
			printf("Execution Time:%13.6f\t", (float)(c_stop - c_start) / CLOCKS_PER_SEC);

			subtract_timeval(&ellapse, &t_stop, &start);
			//printf("Elapsed Time:%6d.%06d\t", TIME_GETSEC(&ellapse), TIME_MSEC(&ellapse));
			printf("Elapsed Time:%13.6f\t", (float)(c_stop - ccstart) / CLOCKS_PER_SEC);
			// ////////////////////////////////////////////////////////////// */

			double sum_utility = 0.0;
			FOREACH_MOBILES {
				double utility = log(mobiles[mob]->result_throughput / t);
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

			c_start = clock();
			TIME_GET(&t_start);

		}
		// /////////////////////////////////////////////////////////////////////

	}

	// Time Check //////////////////////////////////////////////////////////////
	ccstop = clock();
	TIME_GET(&stop);
	//printf("                         %6d.%06d s\n", TIME_GETSEC(&stop) - BASE_SEC, TIME_MSEC(&stop));
	//printf("\\______________________________________/\n");

	subtract_timeval(&ellapse, &stop, &start);
	//printf("=========================%6d.%06d s\n", TIME_GETSEC(&ellapse), TIME_MSEC(&ellapse));
	//printf("=========================%13.6f s\n", (float)(ccstop - ccstart) / CLOCKS_PER_SEC);

	// Finalize ///////////////////////////////////////////////////////////// */
	finalize(macros, picos, mobiles, macro_mobiles, pico_mobiles);

	// /////////////////////////////////////////////////////////////////////////

	system("pause");
	return 0;

}

void initialize(Macro** macros, Pico** picos, Mobile** mobiles,
	Macro_Mobile** macro_mobiles, Pico_Mobile** pico_mobiles) {

	FILE* fp;

	//printf("/ Start ===============================\\\n");

	//printf("= Macro ================================\n");
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
	fp = fopen(DATA_FILE_PREFIX "pico.txt", "r");
	FOREACH_PICOS {
		double x, y, tx_power = TX_POWER_MACRO;
		fscanf(fp, "%lf", &x);
		fscanf(fp, "%lf", &y);
		//printf("(%12.6f, %12.6f): %6.3f\n", x, y, tx_power);
		picos[pic] = new Pico(pic, x, y, tx_power);
	}
	fclose(fp);

	//printf("= Mobile ===============================\n");
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

}

void finalize(Macro** macros, Pico** picos, Mobile** mobiles,
	Macro_Mobile** macro_mobiles, Pico_Mobile** pico_mobiles) {

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

}
