#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <gmp.h>
#include <fcntl.h>
#include <unistd.h>
#include <inttypes.h>
#include <sys/types.h>

#include "randstate.h"
#include "rsa.h"
#include "numtheory.h"

#define OPTIONS "hvi:o:n:"

void program_usage(void) { //prints help message
    fprintf(stderr, "SYNOPSIS\n");
    fprintf(stderr, "   Decrypts data using RSA decryption.\n");
    fprintf(stderr, "   Encrypted data is encrypted by the encrypt program.\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "USAGE\n");
    fprintf(stderr, "   ./decrypt [-hv] [-i infile] [-o outfile] -n privkey\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "OPTIONS\n");
    fprintf(stderr, "   -h              Display program help and usage.\n");
    fprintf(stderr, "   -v              Display verbose program output.\n");
    fprintf(stderr, "   -i infile       Input file of data to decrypt (default: stdin).\n");
    fprintf(stderr, "   -o outfile      Output file for decrypted data (default: stdout).\n");
    fprintf(stderr, "   -n pvfile       Private key file (default: rsa.priv).\n");
}

int bitcounter(mpz_t x) { //For verbose printing. Print the number of bits.
    mpz_t floor;
    mpz_init(floor);
    mpz_set(floor, x); //set temp var floor = x
    int count = 1; //counter for the number of times

    while (mpz_cmp_ui(floor, 0) > 0) {
        mpz_fdiv_q_ui(floor, floor, 2); //do floor division
        count += 1; //increment the counter
    }

    mpz_clear(floor); //clear all the mpz
    return count; //counter return
}

int main(int argc, char **argv) {

    int opt = 0;
    bool test_v = false; //checks for verbose printing
    bool openprivfile = false;
    FILE *infile = stdin;
    FILE *outfile = stdout;
    FILE *pvfile; //private file

    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
        switch (opt) {
        case 'h': program_usage(); exit(0);
        case 'v': test_v = true; break; 
        case 'i': infile = fopen(optarg, "r"); break; 
        case 'o': outfile = fopen(optarg, "w"); break;
        case 'n':
            pvfile = fopen(optarg, "r"); 
            openprivfile = true; //does not open default file
            break;
        default: program_usage(); exit(1);
        }
    }

    //open default file rsa.priv
    if (openprivfile == false) { //this is to check if no file was inputed from the user
        pvfile = fopen("rsa.priv", "r"); //default open rsa.priv
    }

    if (!pvfile) { //not able to open, show error message
        perror("Error");
        fclose(infile); //close files
        fclose(outfile);
        return 1;
    }

    mpz_t n, e;
    mpz_inits(n, e, NULL);

    rsa_read_priv(n, e, pvfile); //read in from the private file

    if (test_v) { //use bitcounter to count bits and print the verbose options
        gmp_printf("n (%d bits) = %Zd\n", bitcounter(n), n); //public mod
        gmp_printf("e (%d bits) = %Zd\n", bitcounter(e), e); //private key
    }

    rsa_decrypt_file(infile, outfile, n, e); //decrypt the file by writing to outfile

    //clear and close all the files
    mpz_clears(n, e, NULL);
    fclose(pvfile);
    fclose(infile);
    fclose(outfile);

    return 0;
}
