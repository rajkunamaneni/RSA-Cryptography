#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <gmp.h>

#include "randstate.h"
#include "numtheory.h"

void gcd(mpz_t d, mpz_t a, mpz_t b) {
    mpz_t zero, temp_b, temp_a, t;
    mpz_inits(zero, temp_b, temp_a, t, NULL); //initalize values

    mpz_set(temp_a, a); 
    mpz_set(temp_b, b); 
    while (mpz_cmp_ui(temp_b, 0) != 0) { //check if b != zero
        mpz_set(t, temp_b); //t = b
        mpz_mod(temp_b, temp_a, temp_b); //b = a % b
        mpz_set(temp_a, t); //a = t
    }

    mpz_set(d, temp_a); 
    mpz_clears(zero, temp_b, temp_a, t, NULL); //clear init values
    return;
}

void mod_inverse(mpz_t i, mpz_t a, mpz_t n) {
    mpz_t finalt, finalvaluet, tempi, value, value_1, r, r_singlequote, t, t_singlequote, q, tempr,
        tempr_singlequote, tempt, tempt_singlequote; //create values
    mpz_inits(finalt, finalvaluet, tempi, value, value_1, r, r_singlequote, t, t_singlequote, q,
        tempr, tempr_singlequote, tempt, tempt_singlequote, NULL); //initalize values

    mpz_set(r, n); //r = n
    mpz_set(r_singlequote, a); //r' = a

    mpz_set_ui(t, 0); //t = 0
    mpz_set_ui(t_singlequote, 1); //t' = 1

    while (mpz_cmp_ui(r_singlequote, 0) != 0) { //r' != 0

        mpz_fdiv_q(q, r, r_singlequote); //floor div for r/r' = q

        mpz_set(tempr, r); //tempr = r
        mpz_set(tempr_singlequote, r_singlequote); //temp_r' = r'

        mpz_set(r, r_singlequote); //r = r'

        mpz_mul(value, q, tempr_singlequote); //value = q*r'
        mpz_sub(value_1, tempr, value); //value_1 = r - value
        mpz_set(r_singlequote, value_1); //r' = r - q*r'

        mpz_set(tempt, t); //tempt = t
        mpz_set(tempt_singlequote, t_singlequote); //temp_t' = t'

        mpz_set(t, tempt_singlequote); //t = t'

        mpz_mul(finalt, q, tempt_singlequote); //value = q*t' reuse value from prev
        mpz_sub(finalvaluet, tempt, finalt); //value_1 = t - value
        mpz_set(t_singlequote, finalvaluet); //t' = t - q*t'
    }

    if (mpz_cmp_ui(r, 1) > 0) { //r > 1
        mpz_set_ui(i, 0); // return no inverse
        mpz_clears(finalt, finalvaluet, tempi, value, value_1, r, r_singlequote, t, t_singlequote,
            q, tempr, tempr_singlequote, tempt, tempt_singlequote, NULL); //clear values
        return;
    }

    if (mpz_cmp_ui(t, 0) < 0) { //t < 0
        mpz_set(tempi, t);
        mpz_add(t, tempi, n);
    }

    mpz_set(i, t); //return t
    mpz_clears(finalt, finalvaluet, tempi, value, value_1, r, r_singlequote, t, t_singlequote, q,
        tempr, tempr_singlequote, tempt, tempt_singlequote, NULL); //clear values
    return;
}

void pow_mod(mpz_t out, mpz_t base, mpz_t exponent, mpz_t modulus) {
    mpz_t modtemp, tempd, two, storep, vmul, modv, v, p;
    mpz_inits(modtemp, tempd, two, storep, vmul, modv, v, p, NULL); //init values

    mpz_set_ui(v, 1); //v = 1
    mpz_set(p, base); //p = base
    mpz_set_ui(two, 2); //this is for checking if exponent is odd
    mpz_set(tempd, exponent);
    mpz_set(modtemp, modulus);

    while (mpz_cmp_ui(tempd, 0) > 0) { //while exponent > 0
        mpz_mod(modv, tempd, two); //exponent % 2 != 0 checks for odd

        if (mpz_cmp_ui(modv, 0) != 0) { //check if odd
            mpz_mul(vmul, v, p); //v * p
            mpz_mod(v, vmul, modtemp); //v = (v*p) % modulus
        }

        mpz_mul(storep, p, p); //p*p
        mpz_mod(p, storep, modtemp); // p = (p*p) % modulus

        mpz_fdiv_q_ui(tempd, tempd, 2); //floor div for exponent/2 = exponent
    }

    mpz_set(out, v); //return v
    mpz_clears(modtemp, tempd, two, storep, vmul, modv, v, p, NULL); //clear values
    return;
}

bool is_prime(mpz_t n, uint64_t iters) {
    mpz_t a, r, y, randupvalue, two, modv, nminone;
    mpz_inits(a, r, y, randupvalue, two, modv, nminone, NULL);

    if ((mpz_cmp_ui(n, 2) == 0) || ((mpz_cmp_ui(n, 3) == 0))) { //check if n == 2 or n == 3
        mpz_clears(a, r, y, randupvalue, two, modv, nminone, NULL); //return true
        return true;
    }

    mpz_set_ui(two, 2);
    mpz_mod(modv, n, two);

    if ((mpz_cmp_ui(modv, 0) == 0) || ((mpz_cmp_ui(n, 1) == 0))) { //check if n % 2 == 0 or n == 1
        mpz_clears(a, r, y, randupvalue, two, modv, nminone, NULL); //return false
        return false;
    }

    mp_bitcnt_t s = 2;
    mpz_sub_ui(nminone, n, 1);

    while (mpz_divisible_2exp_p(nminone, s)) {
        s += 1;
    }
    s -= 1; //removes the power which it was div

    mpz_tdiv_q_2exp(r, nminone, s);

    for (uint64_t i = 0; i < iters; i += 1) {
        mpz_sub_ui(randupvalue, n, 3); //n-3
        mpz_urandomm(a, state, randupvalue);
        mpz_add_ui(a, a, 2);
        pow_mod(y, a, r, n);

        if ((mpz_cmp_ui(y, 1) != 0) && (mpz_cmp(y, nminone) != 0)) { //y != 1 and y != n - 1
            for (uint64_t j = 1; j <= (s - 1) && (mpz_cmp(y, nminone) != 0); j += 1) {
                pow_mod(y, y, two, n);
                if (mpz_cmp_ui(y, 1) == 0) { //y == 1
                    mpz_clears(a, r, y, randupvalue, two, modv, nminone, NULL);
                    return false; //clear mpz and return false
                }
            }
            if (mpz_cmp(y, nminone) != 0) { //y != n - 1
                mpz_clears(a, r, y, randupvalue, two, modv, nminone, NULL);
                return false;
            }
        }
    }
    mpz_clears(a, r, y, randupvalue, two, modv, nminone, NULL);
    return true;
}

void make_prime(mpz_t p, uint64_t bits, uint64_t iters) {

    bits += 1; //create an offset due to urandomb
    size_t k; //init k to size_t
    do {
        mpz_urandomb(p, state, bits);
        k = (mpz_sizeinbase(p, 2)); // p in base of
    } while (is_prime(p, iters) == false || k < bits);

    return;
}
