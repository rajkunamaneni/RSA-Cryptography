#include <stdint.h>
#include <gmp.h>

gmp_randstate_t state; //define variable

void randstate_init(uint64_t seed) {

    gmp_randinit_mt(state); //create the Mersenne Twister algo
    gmp_randseed_ui(state, seed); //state = seed
    return;
}

void randstate_clear(void) { //destructor
    gmp_randclear(state);
    return;
}
