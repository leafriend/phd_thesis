#include <math.h>
#include <limits>
#include <float.h>

#include "pico_mobile.h"
#include "macro_mobile.h"
#include "func.h"
#include "pa2.h"

void pa3(Macro** macros, Pico** picos, Mobile** mobiles) {

	// �� Mobile �� Macro�� �������� Cell Association ����
	// ���⼭�� �ٸ� ���� �޸� Pico�� ABS����(is_abs())�� Ȯ������ �ʰ�
	// ������ non-ABS ���� ���Ѵ�
	bool conn_macros[NUM_MOBILE];
	FOREACH_MOBILES {

		Mobile* mobile = mobiles[mob];

		double sum_macro_lambda_r = 0.0;
		FOREACH_RBS {
			sum_macro_lambda_r += mobile->get_macro_lambda_r(ri);
		}
		double ratio_macro_lambda_r
			= sum_macro_lambda_r
			/ mobile->get_macro()->macro->pa3_lambda_r;

		double sum_pico_lambda_r = 0.0;
		FOREACH_RBS {
			sum_pico_lambda_r += mobile->get_non_pico_lambda_r(ri);
		}
		double ratio_pico_lambda_r
			= sum_pico_lambda_r
			/ mobile->get_pico()->pico->pa3_lambda_r;

		conn_macros[mob] = ratio_macro_lambda_r > ratio_pico_lambda_r;

	}

	FIND_BEST_MACRO_STATE_OPEN(macros, best_macro_states) {

		best_macro_states[0] = 1;

		FOREACH_MACROS_TS_OPEN {

			int* mobile_states = (int*) malloc(sizeof(int) * NUM_MOBILE_TS * NUM_RB);








			if (macro->state == ON) {
				// Mobile�� Macro�� �����ٸ�
				// ������ ���� Cell Association�� ���� lambda_r ����
				// �� ���� ä�κ� �Ҵ� ��� ����

				//if (conn_macros[mobile->idx]) {

				//} else {

				//}

			} else {
				// Mobile�� Macro�� �����ٸ�
				// Mobile�� Pico�� ABS ���ο� ���� lambda_r ����

			}

			// �������� �ְ� lambda_r�� �� ������ lambda_r ��

			free(mobile_states);

		} CLOSE

	} FIND_BEST_MACRO_STATE_CLOSE

	// ���� ä�� �� throughput ���

	// Macro/Pico lambda_r

}





























