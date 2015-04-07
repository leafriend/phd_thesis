#include <math.h>
#include <stdlib.h>

#include "func.h"

// http://www.gnu.org/software/libc/manual/html_node/Elapsed-Time.html
int subtract_timeval(TIME* result, TIME* x, TIME* y) {

	// Perform the carry for the later subtraction by updating y.
	if (TIME_MSEC(x) < TIME_MSEC(y)) {
		int nsec = (TIME_MSEC(y) - TIME_MSEC(x)) / 1000000 + 1;
		TIME_MSEC(y) -= 1000000 * nsec;
		TIME_SEC(y) += nsec;
	}
	if (TIME_MSEC(x) - TIME_MSEC(y) > 1000000) {
		int nsec = (TIME_MSEC(x) - TIME_MSEC(y)) / 1000000;
		TIME_MSEC(y) += 1000000 * nsec;
		TIME_SEC(y) -= nsec;
	}

	// Compute the time remaining to wait tv_usec is certainly positive.
	TIME_SEC(result) = TIME_SEC(x) - TIME_SEC(y);
	TIME_MSEC(result) = TIME_MSEC(x) - TIME_MSEC(y);

	// Return 1 if result is negative.
	return TIME_SEC(x) < TIME_SEC(y);
}

double uniform(void) {
	return ((double) (rand() & RAND_MAX) / (double) RAND_MAX);
}

// http://phoxis.org/2013/05/04/generating-random-numbers-from-normal-distribution-in-c/
double gaussian(double mu, double sigma)
{
	double U1, U2, W, mult;
	static double X1, X2;
	static int call = 0;

	if (call == 1) {
		call = !call;
		return (mu + sigma * (double) X2);
	}

	do {
		U1 = -1 + ((double) rand () / RAND_MAX) * 2;
		U2 = -1 + ((double) rand () / RAND_MAX) * 2;
		W = pow (U1, 2) + pow (U2, 2);
	}
	while (W >= 1 || W == 0);

	mult = sqrt ((-2 * log (W)) / W);
	X1 = U1 * mult;
	X2 = U2 * mult;

	call = !call;

	return (mu + sigma * (double) X1);
}
