#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>
#include <stdio.h>
#include <gmp.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <sys/stat.h>

#include "numtheory.h"
#include "randstate.h"
#include "rsa.h"

#define OPTIONS "b:i:n:d:s:vh"

// ***** Cite: Eugene, Christian, Eric

gmp_randstate_t state;

int main(int argc, char **argv) {

    int opt = 0;
    //bool no_input = true;
    bool h = false;
    bool v = false;

    uint64_t bits = 255;
    uint64_t iters = 50;

    FILE *pbfile = NULL;
    FILE *pvfile = NULL;

    char *pbfilename = "rsa.pub";
    char *pvfilename = "rsa.priv";

    uint64_t seed = time(NULL);

    mpz_t p, q, n, e, d, username, m;
    mpz_inits(p, q, n, e, d, username, m, 0);

    // Command line operator
    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {

        switch (opt) {
        case 'b': bits = (uint64_t) optarg; break;
        case 'i': iters = (uint64_t) optarg; break;
        case 'n': pbfilename = optarg; break;
        case 'd': pvfilename = optarg; break;
        case 's': seed = (uint64_t) optarg; break;
        case 'v': v = true; break;
        case 'h': h = true; break;
        }
    }

    // helper function
    if (h) {
        printf("SYNOPSIS\n");
        printf("   Generates an RSA public/private key pair.\n");

        printf("USAGE\n");
        printf("   ./keygen [-hv] [-b bits] -n pbfile -d pvfile\n");

        printf("OPTIONS\n");
        printf("   -h              Display program help and usage.\n");
        printf("   -v              Display verbose program output.\n");
        printf("   -b bits         Minimum bits needed for public key n (default: 256).\n");
        printf("   -i confidence   Miller-Rabin iterations for testing primes (default: 50).\n");
        printf("   -n pbfile       Public key file (default: rsa.pub).\n");
        printf("   -d pvfile       Private key file (default: rsa.priv).\n");
        printf("   -s seed         Random seed for testing.\n");
        mpz_clears(p, q, n, e, d, username, m, NULL);
        return 0;
    }

    // implemented using the asgn pdf
    // opens public and private files

    if (fopen(pbfilename, "w") == NULL) {
        printf("\nERROR!\n");
        mpz_clears(p, q, n, e, d, username, m, NULL);
        return 1;
    } else {
        pbfile = fopen(pbfilename, "w");
    }

    if (fopen(pvfilename, "w") == NULL) {
        printf("\nERROR!\n");
        mpz_clears(p, q, n, e, d, username, m, NULL);
        return 1;
    } else {
        pvfile = fopen(pvfilename, "w");
    }

    // private key file permissions are set to 0600
    fchmod(fileno(pvfile), 0600);

    // Initialize the random state
    randstate_init(seed);

    // makes keys
    rsa_make_pub(q, p, n, e, bits, iters);
    rsa_make_priv(d, e, q, p);

    // cite: Erik the TA
    // Get the current user’s name as a string and converts into mpz_t string
    char *user = getenv("USER");
    mpz_set_str(username, user, 62);

    // compute the signature of the username
    rsa_sign(username, m, d, n);

    // Write the computed public and private key to their designated files
    rsa_write_pub(n, e, m, user, pbfile);
    rsa_write_priv(n, d, pvfile);

    // enables verbose
    if (v) {
        printf("user %s\n", user);
        gmp_fprintf(stderr, "s (%zu bits) = %Zd\n", mpz_sizeinbase(m, 2), m);
        gmp_fprintf(stderr, "p (%zu bits) = %Zd\n", mpz_sizeinbase(p, 2), p);
        gmp_fprintf(stderr, "q (%zu bits) = %Zd\n", mpz_sizeinbase(q, 2), q);
        gmp_fprintf(stderr, "n (%zu bits) = %Zd\n", mpz_sizeinbase(n, 2), n);
        gmp_fprintf(stderr, "e (%zu bits) = %Zd\n", mpz_sizeinbase(e, 2), e);
        gmp_fprintf(stderr, "d (%zu bits) = %Zd\n", mpz_sizeinbase(d, 2), d);
    }

    // closes all files and clears mpz values
    fclose(pbfile);
    fclose(pvfile);
    randstate_clear();
    mpz_clears(p, q, n, e, d, username, m, NULL);
    return 0;
}
