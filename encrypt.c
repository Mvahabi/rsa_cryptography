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

// ****** Cite: Eugene, Christian, Eric
int main(int argc, char **argv) {
    int opt = 0;
    FILE *in_file = stdin;
    FILE *out_file = stdout;
    FILE *public_file;
    char *in = "stdin";
    char *out = "stdout";
    bool v = false;
    bool h = false;
    char *public_name = "rsa.pub";
    char *user = NULL;
    mpz_t n, e, s, d;
    mpz_inits(n, e, s, d, 0);

    // command prompt
    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
        switch (opt) {
        case 'i': in = optarg; break;
        case 'o': out = optarg; break;
        case 'n': public_name = optarg; break;
        case 'v': v = true; break;
        case 'h': h = true; break;
        }
    }

    // helper function
    if (h) {
        printf("SYNOPSIS\n");
        printf("   Encrypts data using RSA encryption.\n");
        printf("   Encrypted data is decrypted by the decrypt program.\n");

        printf("\nUSAGE\n");
        printf("   ./encrypt [-hv] [-i infile] [-o outfile] -n pubkey\n");

        printf("\nOPTIONS\n");
        printf("   -h              Enables program synopsis and usage.\n");
        printf("   -v              Enables verbose output.\n");
        printf("   -i              Specifies the input file to encrypt (default: stdin).\n");
        printf("   -o              Specifies the output file to encrypt (default: stdout).\n");
        printf("   -n              Specifies the file containing the public key (default: "
               "rsa.pub).\n");
        mpz_clears(n, e, s, d, NULL);
        return 0;
    }

    // opens the public file
    if (fopen(public_name, "w") == NULL) {
        printf("\nERROR!\n");
        mpz_clears(n, e, s, d, NULL);
        return 1;
    } else {
        public_file = fopen(public_name, "w");
    }

    // reads public file
    rsa_read_pub(n, e, s, user, public_file);

    // prints verbose
    if (v) {
        printf("user %s\n", user);
        gmp_fprintf(stderr, "s (%zu bits) = %Zd\n", mpz_sizeinbase(s, 2), s);
        gmp_fprintf(stderr, "n (%zu bits) = %Zd\n", mpz_sizeinbase(n, 2), n);
        gmp_fprintf(stderr, "e (%zu bits) = %Zd\n", mpz_sizeinbase(e, 2), e);
    }
    //convert username to mpz variable d
    user = getenv("USER");
    mpz_set_str(d, user, 62);

    // verifies user signature
    if (!rsa_verify(d, s, e, n)) {
        printf("\nERROR: CAN NOT VERIFY!\n");
        mpz_clears(n, e, s, d, NULL);
        exit(1);
    }

    // encrypts file, closes all files, empties mpz_variables
    rsa_encrypt_file(in_file, out_file, n, e);
    fclose(public_file);
    fclose(in_file);
    fclose(out_file);
    mpz_clears(n, e, s, d, NULL);

    return 0;
}
