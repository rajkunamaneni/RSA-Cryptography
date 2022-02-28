#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <gmp.h>
#include <fcntl.h>
#include <unistd.h>
#include <inttypes.h>
#include <sys/types.h>
#include <limits.h>

#include "randstate.h"
#include "rsa.h"
#include "numtheory.h"

#define OPTIONS "hvi:o:n:"

void program_usage(void) { //prints help message
    fprintf(stderr, "SYNOPSIS\n");
    fprintf(stderr, "   Encrypts data using RSA encryption.\n");
    fprintf(stderr, "   Encrypted data is decrypted by the decrypt program.\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "USAGE\n");
    fprintf(stderr, "   ./encrypt [-hv] [-i infile] [-o outfile] -n pubkey\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "OPTIONS\n");
    fprintf(stderr, "   -h              Display program help and usage.\n");
    fprintf(stderr, "   -v              Display verbose program output.\n");
    fprintf(stderr, "   -i infile       Input file of data to encrypt (default: stdin).\n");
    fprintf(stderr, "   -o outfile      Output file for encrypted data (default: stdout).\n");
    fprintf(stderr, "   -n pbfile       Public key file (default: rsa.pub).\n");
}

int bitcounter(mpz_t x) { //For verbose printing. Print the number of bits.
    mpz_t floor;
    mpz_init(floor);
    mpz_set(floor, x);
 
    int count = 1;
    while (mpz_cmp_ui(floor, 0) > 0) {
        mpz_fdiv_q_ui(floor, floor, 2); //do floor division in mpz
        count += 1; 
    }
    mpz_clear(floor); //clear mpz var
    return count;
}

int main(int argc, char **argv) {

    int opt = 0;
    bool test_v = false;
    bool openpubfile = false;
    FILE *infile = stdin;
    FILE *outfile = stdout;
    FILE *pbfile; //public file

    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
        switch (opt) {
        case 'h': program_usage(); exit(0);
        case 'v': test_v = true; break; 
        case 'i': infile = fopen(optarg, "r"); break;
        case 'o': outfile = fopen(optarg, "w"); break;
        case 'n':
            pbfile = fopen(optarg, "r"); 
            openpubfile = true; 
            break;
        default: program_usage(); exit(1);
        }
    }

    if (openpubfile == false) { 
        pbfile = fopen("rsa.pub", "r");
    }

    //print error message and quits the program
    if (!pbfile) {
        perror("Error");
        fclose(infile);
        fclose(outfile);
        return 1;
    }

    mpz_t str, m, n, e, s; //create vars with mpz
    mpz_inits(str, m, n, e, s, NULL);

    char username[_POSIX_LOGIN_NAME_MAX]; //holds the username of the user

    rsa_read_pub(n, e, s, username, pbfile); //reads file and stores variable to mpz values

    if (test_v) { //prints verbose options and use bitcounter to print the bits
        printf("user = %s\n", username); //username
        gmp_printf("s (%d bits) = %Zd\n", bitcounter(s), s); //signature
        gmp_printf("n (%d bits) = %Zd\n", bitcounter(n), n); //pub modulus
        gmp_printf("e (%d bits) = %Zd\n", bitcounter(e), e); //pub exponent
    }

    mpz_set_str(str, username, 62); //set username to mpz values

    if (rsa_verify(str, s, e, n) == false) { 
        fprintf(stderr, "Error: invalid key.\n"); //if not valid print message and close files
        mpz_clears(str, m, n, e, s, NULL);
        fclose(pbfile);
        fclose(infile);
        fclose(outfile);
        exit(1);
    }

    rsa_encrypt_file(infile, outfile, n, e); //encrypt the files if key is valid

    //clear and close files
    mpz_clears(str, m, n, e, s, NULL);
    fclose(pbfile);
    fclose(infile);
    fclose(outfile);

    return 0;
}
