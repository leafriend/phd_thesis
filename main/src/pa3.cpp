#include <math.h>
#include <limits>
#include <float.h>

#include "func.h"
#include "pa2.h"

void pa3(Macro** macros, Pico** picos, Mobile** mobiles) {

	// �� Mobile �� Macro�� �������� Cell Association ����
	// ���⼭�� �ٸ� ���� �޸� Pico�� ABS����(is_abs())�� Ȯ������ �ʰ�
	// ������ non-ABS ���� ���Ѵ�
	//...

	double best_sum_lambda_r = -std::numeric_limits<double>::infinity();
	bool best_macro_states[NUM_MACRO];

	FOREACH_MACROS
		macros[mac]->set_state(false);

	// ������ ��� Macro ����(2 ^ NUM_MACRO = 1 << NUM_MACRO)�� ���� �ݺ���
	int num_macro_state = 1 << NUM_MACRO;
	for (int s = 0; s < num_macro_state; s++) {

		// Macro ����(ON/OFF) ����
		FOREACH_MACROS
			macros[mac]->set_state(1 == ((1 << mac) & s) >> mac);

		FOREACH_MOBILES {

			// Mobile�� Macro�� �����ٸ�
			// ������ ���� Cell Association�� ���� lambda_r ����
			// �� ���� ä�κ� �Ҵ� ��� ����

			// Mobile�� Macro�� �����ٸ�
			// Mobile�� Pico�� ABS ���ο� ���� lambda_r ����

		}

		// �������� �ְ� lambda_r�� �� ������ lambda_r ��

	}

	// ���� ä�� �� throughput ���

	// Macro/Pico lambda_r

}
