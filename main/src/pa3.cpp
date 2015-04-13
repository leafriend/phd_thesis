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

	double best_sum_lambda_r = -std::numeric_limits<double>::infinity();
	bool best_macro_states[NUM_MACRO];

	FOREACH_MACROS
		macros[mac]->state = OFF;

	// ������ ��� Macro ����(2 ^ NUM_MACRO = 1 << NUM_MACRO)�� ���� �ݺ���
	int num_macro_state = 1 << NUM_MACRO;
	for (int s = 0; s < num_macro_state; s++) {

		// Macro ����(ON/OFF) ����
		FOREACH_MACROS
			macros[mac]->state
				= ((1 << mac) & s) >> mac
				? ON
				: OFF;

		FOREACH_MACROS_TS {

			int* mobile_states = (int*) malloc(sizeof(int) * NUM_MOBILE_TS * NUM_RB);

			FOREACH_RBS {

				FOREACH_MOBILES_TS {

					const Mobile* mobile = mmobiles[mob]->mobile;

					if (mobile->get_macro()->macro->state == ON) {
						// Mobile�� Macro�� �����ٸ�
						// ������ ���� Cell Association�� ���� lambda_r ����
						// �� ���� ä�κ� �Ҵ� ��� ����

						if (conn_macros[mob]) {

						} else {

						}

					} else {
						// Mobile�� Macro�� �����ٸ�
						// Mobile�� Pico�� ABS ���ο� ���� lambda_r ����

					}

				}
			}

			// �������� �ְ� lambda_r�� �� ������ lambda_r ��

			free(mobile_states);

		}

	} END

	// ���� ä�� �� throughput ���

	// Macro/Pico lambda_r

}
