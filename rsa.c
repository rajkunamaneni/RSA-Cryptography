#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <gmp.h>

#include "randstate.h"
#include "numtheory.h"
#include "rsa.h"

void rsa_make_pub(mpz_t p, mpz_t q, mpz_t n, mpz_t e, uint64_t nbits, uint64_t iters) {

    mpz_t totn, gcdcompute, pminone, qminone;
    mpz_inits(totn, gcdcompute, pminone, qminone, NULL);
    uint64_t bit_p = ((random() % (nbits / 2)) + (nbits / 4)); //get pbits with a random range
    uint64_t bit_q = (nbits - bit_p); //take difference of pbits and nbits to get qbits

    make_prime(p, bit_p, iters); //calculate pbits with make prime
    make_prime(q, bit_q, iters); //calcualte qbits with make prime
    mpz_sub_ui(pminone, p, 1);
    mpz_sub_ui(qminone, q, 1);

    mpz_mul(n, p, q); //n = p * q

    mpz_mul(totn, pminone, qminone); //totent(n)= (p-1)*(q-1)

    do {
        mpz_urandomb(e, state, nbits); //urandmob and calculate gcd
        gcd(gcdcompute, e, totn);
    } while (mpz_cmp_ui(gcdcompute, 1) != 0); //gcd value != 1

    mpz_clears(totn, gcdcompute, pminone, qminone, NULL);
    return;
}

void rsa_write_pub(mpz_t n, mpz_t e, mpz_t s, char username[], FILE *pbfile) {
    gmp_fprintf(pbfile, "%Zx\n", n);
    gmp_fprintf(pbfile, "%Zx\n", e);
    gmp_fprintf(pbfile, "%Zx\n", s);
    fprintf(pbfile, "%s\n", username);
    return;
}

void rsa_read_pub(mpz_t n, mpz_t e, mpz_t s, char username[], FILE *pbfile) {
    gmp_fscanf(pbfile, "%Zx\n", n);
    gmp_fscanf(pbfile, "%Zx\n", e);
    gmp_fscanf(pbfile, "%Zx\n", s);
    fscanf(pbfile, "%s\n", username);
    return;
}

void rsa_make_priv(mpz_t d, mpz_t e, mpz_t p, mpz_t q) {

    mpz_t mulnq, pminone, qminone;
    mpz_inits(mulnq, pminone, qminone, NULL); //init values

    mpz_sub_ui(pminone, p, 1); //p - 1
    mpz_sub_ui(qminone, q, 1); //q - 1

    mpz_mul(mulnq, pminone, qminone); //totent(n)= (p-1)*(q-1)

    mod_inverse(d, e, mulnq); //call mod inverse with totent(n) and public exponent

    mpz_clears(mulnq, pminone, qminone, NULL);
    return;
}

void rsa_write_priv(mpz_t n, mpz_t d, FILE *pvfile) {
    gmp_fprintf(pvfile, "%Zx\n", n);
    gmp_fprintf(pvfile, "%Zx\n", d);

    return;
}

void rsa_read_priv(mpz_t n, mpz_t d, FILE *pvfile) {
    gmp_fscanf(pvfile, "%Zx\n", n);
    gmp_fscanf(pvfile, "%Zx\n", d);

    return;
}

void rsa_encrypt(mpz_t c, mpz_t m, mpz_t e, mpz_t n) {
    pow_mod(c, m, e, n);
    return;
}

void rsa_encrypt_file(FILE *infile, FILE *outfile, mpz_t n, mpz_t e) {
    mpz_t c, m;
    mpz_inits(c, m, NULL);

    //calculate (log base 2 of n - 1)/8
    size_t k = (mpz_sizeinbase(n, 2) - 1) / 8; 

    uint8_t *array = malloc(k); 

    array[0] = 0xFF; 

    size_t j = 0;

    while ((j = fread(&array[1], sizeof(uint8_t), (k - 1), infile)), j > 0) { 
        mpz_import(m, j + 1, 1, sizeof(uint8_t), 1, 0, array);
        rsa_encrypt(c, m, e, n);
        gmp_fprintf(outfile, "%Zx\n", c);
    }

    //clear and free array
    mpz_clears(c, m, NULL);
    free(array);
    return;
}

void rsa_decrypt(mpz_t m, mpz_t c, mpz_t d, mpz_t n) {
    pow_mod(m, c, d, n);
    return;
}

void rsa_decrypt_file(FILE *infile, FILE *outfile, mpz_t n, mpz_t d) {
    mpz_t c, m;
    mpz_inits(c, m, NULL);

    //calculate (log base 2 of n - 1)/8
    size_t k = (mpz_sizeinbase(n, 2) - 1) / 8; 

    uint8_t *block = malloc(k); 

    size_t j = 0;

    while (!feof(infile)) { //step 4
        int results = gmp_fscanf(infile, "%Zx", c); 
        if (results > 0) { 
            rsa_decrypt(m, c, d, n); //decrypt the contents
            mpz_export(block, &j, 1, sizeof(uint8_t), 1, 0, m); //convert back to bytes
            fwrite(&block[1], sizeof(uint8_t), j - 1, outfile); //write to outfile
        }
    }

    //clear and free
    mpz_clears(c, m, NULL);
    free(block);

    return;
}

void rsa_sign(mpz_t s, mpz_t m, mpz_t d, mpz_t n) {
    pow_mod(s, m, d, n);
    return;
}

bool rsa_verify(mpz_t m, mpz_t s, mpz_t e, mpz_t n) {
    mpz_t t;
    mpz_init(t);

    pow_mod(t, s, e, n);

    if (mpz_cmp(t, m) == 0) { //t == m
        mpz_clear(t);
        return true;
    }
    mpz_clear(t);
    return false; 
}
