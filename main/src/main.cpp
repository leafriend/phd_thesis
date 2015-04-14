#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <float.h>
#include <math.h>

#include "func.h"
#include "macro.h"
#include "pico.h"
#include "mobile.h"
#include "pa1.h"
#include "pa2.h"
#include "pa3.h"
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

	printf("Proposed Algorithm %d ===========================================================\n", ALGORITHM);

	// Time Check //////////////////////////////////////////////////////////////
	clock_t t_elapsed = clock();
	clock_t t_execute = clock();
	// /////////////////////////////////////////////////////////////////////////

	// Initialize //////////////////////////////////////////////////////////////
	Macro* macros[NUM_MACRO];
	Pico* picos[NUM_PICO];
	Edge* macro_mobiles[NUM_MM];
	Edge* pico_mobiles[NUM_PICO * NUM_MOBILE];
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

		// /////////////////////////////////////////////////////////////////////
		// PROCESS ALGORITHM

#if ALGORITHM == 1
		pa1(macros, picos, mobiles);
#elif ALGORITHM == 2
		pa2(macros, picos, mobiles);
#elif ALGORITHM == 3
		pa3(macros, picos, mobiles);
#endif

		// /////////////////////////////////////////////////////////////////////

		FOREACH_MOBILES
			mobiles[mob]->calculate_throughput();

		//FOREACH_MOBILES mobiles[mob]->count_cell_association(best_mobile_states[mob]));

		FOREACH_MOBILES
			mobiles[mob]->calculate_rate_user();

		FOREACH_MOBILES
			mobiles[mob]->calculate_dual_variable(t);

		// /////////////////////////////////////////////////////////////////////

		// FOREACH_MOBILES printf("%d", best_mobile_states[mob]); printf("\n");

		FOREACH_MACROS {
			macros[mac]->count_allocation();
		}

		// /////////////////////////////////////////////////////////////////////

		if (t % LOG_PROGRESS_TIME == 0)
			printf(
				"\b\b\b\b\b\b\b\b"
				"\b"
				"\b\b\b\b\b\b\b\b"
				"\b\b\b\b\b\b\b"
				"%8d"
				"/"
				"%8d"
				"       ",
				((t % LOG_INTERVAL_TIME) / LOG_PROGRESS_TIME) == 0
				? (LOG_INTERVAL_TIME / LOG_PROGRESS_TIME)
				: ((t % LOG_INTERVAL_TIME) / LOG_PROGRESS_TIME),
				(LOG_INTERVAL_TIME / LOG_PROGRESS_TIME)
			);

		// /////////////////////////////////////////////////////////////////////

		// Time Check //////////////////////////////////////////////////////
		if (t % LOG_INTERVAL_TIME == 0) {

			printf("\n");

			printf(
				"Rate User  (log)\t"
				"Throughput (log)\t"
				"lmabda  \t"
				"mu      \n");
			FOREACH_MOBILES {
				Mobile* mobile = mobiles[mob];
				const char* split = mob & 1 ? "\n" : "\t";
				printf(
					"%10.6f (%10.6f)\t"
					"%10.6f (%10.6f)\t"
					"%f\t"
					"%f\n",
					mobile->get_rate_user(),
					log(mobile->get_rate_user()),
					mobile->get_result_throughput() / t,
					log(mobile->get_result_throughput() / t),
					mobile->lambda,
					mobile->mu
				);
			}

			FOREACH_MACROS {
				int count = t - macros[mac]->get_allocation_count();
				printf("%7d(%5.2f%%)\t", count, 100 * count / (double) t);
			}
			printf("\n");
			printf("Time    %8d\t", t);
			printf("QOS     %8.3f\t", QOS);
			double sum_utility = 0.0;
			FOREACH_MOBILES {
				double utility = log(mobiles[mob]->get_result_throughput() / t);
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
	Edge** macro_mobiles, Edge** pico_mobiles) {

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

		FOREACH_MACROS
			picos[pic]->check_interfering(macros[mac]);
	}
	fclose(fp);

	//printf("= Mobile ===============================\n");
	//*
	fp = fopen(DATA_FILE_PREFIX "mobile.txt", "r");
	FOREACH_MOBILES {
		double x, y, lambda, mu, rate_user, qos = QOS;
		fscanf(fp, "%lf", &x);
		fscanf(fp, "%lf", &y);
		fscanf(fp, "%lf", &lambda);
		fscanf(fp, "%lf", &mu);
		fscanf(fp, "%lf", &rate_user);
		//printf("(%12.6f, %12.6f): %6.3f\n", x, y, qos);
		Mobile* mobile = new Mobile(mob, x, y, lambda, mu, rate_user, qos);
		mobiles[mob] = mobile;

		FOREACH_MACROS macro_mobiles[mob * NUM_MACRO + mac] = new Edge(EDGE_MACRO, macros[mac], mobile);

		FOREACH_PICOS pico_mobiles[mob * NUM_PICO + pic] = new Edge(EDGE_PICO, picos[pic], mobile);

		if (mobile->get_macro() == NULL) printf("Mobile at (%12.6f, %12.6f): %6.3f has no service Macro\n", x, y, qos);
		if (mobile->get_pico() == NULL) printf("Mobile at (%12.6f, %12.6f): %6.3f has no service Pico\n", x, y, qos);

	}
	//*/
	
	/*
	FOREACH_MOBILES {
		double x = (500 + pow(-10.0, mob)) * cos(2 * M_PI * mob / 50);
		double y = (500 + pow(-10.0, mob)) * sin(2 * M_PI * mob / 50);
		mobiles[mob] = new Mobile(mob, x, y, 0.1, 0.0, 0.0, QOS);
		
		FOREACH_MACROS macro_mobiles[mob * NUM_MACRO + mac] = new Edge(EDGE_MACRO, macros[mac], mobiles[mob]);
		FOREACH_PICOS pico_mobiles[mob * NUM_PICO + pic] = new Edge(EDGE_PICO, picos[pic], mobiles[mob]);
	}
	*/

	FOREACH_MOBILES {
		FOREACH_MACROS
			macro_mobiles[mob * NUM_MACRO + mac]->register_mobile();
		FOREACH_PICOS
			pico_mobiles[mob * NUM_PICO + pic]->register_mobile();
	}

	/*
	FOREACH_MACROS {
		printf("\n%d\t", macros[mac]->num_mobiles_to_service);
		for (int i = 0; i < macros[mac]->num_mobiles_to_service; i++)
			printf("%d\t", macros[mac]->mobiles_to_service[i]->get_mobile()->idx);
	}
	FOREACH_PICOS {
		printf("\n%d\t", picos[pic]->num_mobiles_to_service);
		for (int i = 0; i < picos[pic]->num_mobiles_to_service; i++)
			printf("%d\t", picos[pic]->mobiles_to_service[i]->get_mobile()->idx);
	}
	//*/

	fclose(fp);

}

void finalize(Macro** macros, Pico** picos, Mobile** mobiles,
	Edge** macro_mobiles, Edge** pico_mobiles) {

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
