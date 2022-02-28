#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <gmp.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <inttypes.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "randstate.h"
#include "numtheory.h"
#include "rsa.h"

#define OPTIONS "hvb:i:n:d:s:"

void program_usage(void) { //prints help message
    fprintf(stderr, "SYNOPSIS\n");
    fprintf(stderr, "   Generates an RSA public/private key pair.\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "USAGE\n");
    fprintf(stderr, "   ./keygen [-hv] [-b bits] -n pbfile -d pvfile\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "OPTIONS\n");
    fprintf(stderr, "   -h              Display program help and usage.\n");
    fprintf(stderr, "   -v              Display verbose program output.\n");
    fprintf(stderr, "   -b bits         Minimum bits needed for public key n (default: 256).\n");
    fprintf(
        stderr, "   -i confidence   Miller-Rabin iterations for testing primes (default: 50).\n");
    fprintf(stderr, "   -n pbfile       Public key file (default: rsa.pub).\n");
    fprintf(stderr, "   -d pvfile       Private key file (default: rsa.priv).\n");
    fprintf(stderr, "   -s seed         Random seed for testing.\n");
}

int bitcounter(mpz_t x) { //For verbose printing. Print the number of bits.
    mpz_t floor;
    mpz_init(floor);
    mpz_set(floor, x); //set temp var floor = x
    int count = 1; //counter for the number of times

    while (mpz_cmp_ui(floor, 0) > 0) {
        mpz_fdiv_q_ui(floor, floor, 2); //do floor division
        count += 1; //increment counter
    }

    mpz_clear(floor); //clear mpz values
    return count;
}

int main(int argc, char **argv) {

    int opt = 0;
    bool test_v = false; 
    bool openpubfile = false; 
    bool openprivfile = false;
    FILE *public;
    FILE *private;
    
    uint64_t b = 256; //the minimum bits for modulus n
    uint64_t i = 50; //number of MR iterations for primes
    uint64_t seed = time(NULL); //set seed to time module.

    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
        switch (opt) {
        case 'h': program_usage(); exit(0);
        case 'v': test_v = true; break;
        case 'b': b = strtoull(optarg, NULL, 10); break; //takes new min bits from user
        case 'i': i = strtoull(optarg, NULL, 10); break; //takes iterations num from user
        case 'n': public = fopen(optarg, "w");
                  openpubfile = true; //does not open default file created
                  break;
        case 'd':
            //open private file from user
                  private = fopen(optarg, "w");
                  openprivfile = true; //stops default file from opening
                  break;
        case 's': seed = strtoull(optarg, NULL, 10); break; //make seed to user inputs
        default: program_usage(); exit(1);
        }
    }

    if (openpubfile == false) {
        //open default public file
        public = fopen("rsa.pub", "w");
    }

    if (openprivfile == false) {
        //open default private file
        private = fopen("rsa.priv", "w");
    }

    int fd = fileno(private); //create file desc for fchmod

    fchmod(fd, 0600); //set 0600 which is read and write permissions only for user

    randstate_init(seed); //create Mersenne Twister with seed

    mpz_t m, s, str, d, p, q, n, e;
    mpz_inits(m, s, str, d, p, q, n, e, NULL); //inits all the values

    rsa_make_pub(p, q, n, e, b, i); //create a public key
    rsa_make_priv(d, e, p, q); //create a private key

    char *username = getenv("USER"); 

    mpz_set_str(str, username, 62); 
    rsa_sign(s, str, d, n); //sign the username to show it was checked by keygen

    rsa_write_pub(n, e, s, username, public); //write to the public file
    rsa_write_priv(n, d, private); //write to the private file

    if (test_v) { //print verbose 
        printf("user = %s\n", username); //username
        gmp_printf("s (%d bits) = %Zd\n", bitcounter(s), s); //signature
        gmp_printf("p (%d bits) = %Zd\n", bitcounter(p), p); //first large prime
        gmp_printf("q (%d bits) = %Zd\n", bitcounter(q), q); //second large prime
        gmp_printf("n (%d bits) = %Zd\n", bitcounter(n), n); //pub mod
        gmp_printf("e (%d bits) = %Zd\n", bitcounter(e), e); //pub exponenet
        gmp_printf("d (%d bits) = %Zd\n", bitcounter(d), d); //private key
    }

    //clear MT, clear mpz, and close all files
    randstate_clear();
    mpz_clears(m, s, str, d, p, q, n, e, NULL);
    fclose(public);
    fclose(private);

    return 0;
}
