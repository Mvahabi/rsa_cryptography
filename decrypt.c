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

#define OPTIONS "ion:vh"

// ********** Cite: Eugene, Christian, Eric

gmp_randstate_t state;

int main(int argc, char **argv) {
    int opt = 0;
    FILE *in_file = stdin;
    FILE *out_file = stdout;
    FILE *pvfile;
    char *in = "stdin";
    char *out = "stdout";
    bool v = false;
    bool h = false;
    char *privatefile = "rsa.priv";

    mpz_t n, e;
    mpz_inits(n, e, 0);

    // Command line prompt
    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
        switch (opt) {
        case 'i': in = optarg; break;
        case 'o': out = optarg; break;
        case 'n': privatefile = optarg; break;
        case 'v': v = true; break;
        case 'h': h = true; break;
        }
    }

    // helper function
    if (h) {
        printf("SYNOPSIS\n");
        printf("   Decrypts data using RSA decryption.\n");
        printf("   Encrypted data is encrypted by the encrypt program.\n");

        printf("USAGE\n");
        printf("   ./decrypt [-hv] [-i infile] [-o outfile] -n privkey\n");

        printf("OPTIONS\n");
        printf("   -h              Enables program synopsis and usage.\n");
        printf("   -v              Enables verbose output.\n");
        printf("   -i              Specifies the input file to decrypt (default: stdin).\n");
        printf("   -o              Specifies the output file to decrypt (default: stdout).\n");
        printf("   -n              Specifies the file containing the private key (default: "
               "rsa.priv).\n");
        mpz_clears(n, e, NULL);
        return 0;
    }

    // opens private file
    if (fopen(privatefile, "w") == NULL) {
        printf("\nERROR\n");
        mpz_clears(n, e, NULL);
        return 1;
    } else {
        pvfile = fopen(privatefile, "w");
    }

    // read private file
    rsa_read_priv(n, e, pvfile);

    // prints verbose
    if (v) {
        gmp_fprintf(stderr, "n (%zu bits) = %Zd\n", mpz_sizeinbase(n, 2), n);
        gmp_fprintf(stderr, "d (%zu bits) = %Zd\n", mpz_sizeinbase(e, 2), e);
    }

    // decrypts file, closes files, clears mpz values
    rsa_decrypt_file(in_file, out_file, n, e);
    fclose(pvfile);
    fclose(in_file);
    fclose(out_file);
    mpz_clears(n, e, NULL);

    return 0;
}
