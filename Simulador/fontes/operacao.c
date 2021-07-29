#include <math.h>


int pega_pedaco(int bits, int mais_significativo, int menos_significativo) {
	int pedaco = pow(2, mais_significativo - menos_significativo + 1) - 1;
	pedaco &= bits >> menos_significativo;

	return pedaco;
}
