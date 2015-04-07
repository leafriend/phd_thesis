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

#define PRINT_TIME(elap, exec)                              \
	printf(                                                 \
		"Elapsed %8.3f\t"                                    \
		"Execute %8.3f\n",                                   \
		(float) (clock() - elap) / CLOCKS_PER_SEC,          \
		(float) (clock() - exec) / CLOCKS_PER_SEC           \
	);                                                      \
	t_execute = clock();

int main(int argc, char** argv) {

	// Time Check //////////////////////////////////////////////////////////////
	clock_t t_elapsed = clock();
	clock_t t_execute = clock();
	// /////////////////////////////////////////////////////////////////////////

	// Initialize //////////////////////////////////////////////////////////////
	Macro* macros[NUM_MACRO];
	Pico* picos[NUM_PICO];
	Macro_Mobile* macro_mobiles[NUM_MM];
	Pico_Mobile* pico_mobiles[NUM_PICO * NUM_MOBILE];
	Mobile* mobiles[NUM_MOBILE];
	initialize(macros, picos, mobiles, macro_mobiles, pico_mobiles);

	PRINT_TIME(t_elapsed, t_execute)

	for (int t = 1; t <= SIMULATION_TIME; t++) {

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

		// Time Check //////////////////////////////////////////////////////
		if (t % LOG_INTERVAL_TIME == 0) {

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
				printf("%7d(%5.2f%%)\t", macros[mac]->get_allocation_count(), 100 * macros[mac]->get_allocation_count() / (double) t);
			}
			printf("\n");
			printf("Time    %8d\t", t);
			printf("QOS     %8.3f\t", QOS);
			double sum_utility = 0.0;
			FOREACH_MOBILES {
				double utility = log(mobiles[mob]->result_throughput / t);
				//if (!isinf(utility))
					sum_utility += utility;
			}
			printf("Utility %8.3f\t", sum_utility);

			/*
			printf("\n");
			for (int mac = 0; mac < MACRO_NUM; mac++)
			{
				printf("%d\t", abs_count_macro[mac]);
				printf("%f\n", (double)abs_count_macro[mac] / ((double) t));
			}
			//*/


			PRINT_TIME(t_elapsed, t_execute)

		}

	}

	PRINT_TIME(t_elapsed, t_execute)

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
