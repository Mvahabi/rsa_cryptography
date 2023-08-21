#include "rsa.h"
#include "numtheory.h"
#include "randstate.h"
#include <gmp.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

// ****** Cite: Eugene, Eric

gmp_randstate_t state;

void rsa_make_pub(mpz_t p, mpz_t q, mpz_t n, mpz_t e, uint64_t nbits, uint64_t iters) {

    // implemented using Eugen's pseudo
    uint64_t pbits = (random() % (nbits / 2)) + (nbits / 4);
    uint64_t qbits = nbits - pbits;

    make_prime(p, pbits, iters);
    make_prime(q, qbits, iters);
    mpz_t phi, pmin, qmin, rand, result;
    mpz_mul(n, p, q);

    mpz_init(phi);
    mpz_init_set(pmin, p);
    mpz_init_set(qmin, q);
    mpz_sub_ui(pmin, pmin, 1);
    mpz_sub_ui(qmin, qmin, 1);
    mpz_mul(phi, pmin, qmin);

    mpz_inits(rand, result, 0);
    bool loop = true;

    while (loop) {
        mpz_urandomb(rand, state, nbits);
        gcd(result, rand, phi);
        if (mpz_cmp_ui(result, 1) == 0) {
            loop = false;
        }
    }

    // acts as return "e"
    mpz_set(e, rand);

    // clears space
    mpz_clears(phi, pmin, qmin, rand, result, NULL);
}

void rsa_write_pub(mpz_t n, mpz_t e, mpz_t s, char username[], FILE *pbfile) {
    // prints all public keys
    gmp_fprintf(pbfile, "%Zx\n", n);
    gmp_fprintf(pbfile, "%Zx\n", e);
    gmp_fprintf(pbfile, "%Zx\n", s);
    fprintf(pbfile, "%s\n", username);
}

void rsa_read_pub(mpz_t n, mpz_t e, mpz_t s, char username[], FILE *pbfile) {
    // reads all public keys
    gmp_fscanf(pbfile, "%Zx\n", n);
    gmp_fscanf(pbfile, "%Zx\n", e);
    gmp_fscanf(pbfile, "%Zx\n", s);
    fscanf(pbfile, "%s\n", username);
}

void rsa_make_priv(mpz_t d, mpz_t e, mpz_t p, mpz_t q) {

    // implemented using Eugen's pseudo and asgn pdf
    mpz_t phi, pmin, qmin;

    mpz_init(phi);
    mpz_init_set(pmin, p);
    mpz_init_set(qmin, q);
    mpz_sub_ui(pmin, pmin, 1);
    mpz_sub_ui(qmin, qmin, 1);
    mpz_mul(phi, pmin, qmin);

    mod_inverse(d, e, phi);

    // clears space
    mpz_clears(phi, pmin, qmin, NULL);
}

void rsa_write_priv(mpz_t n, mpz_t d, FILE *pvfile) {
    // prints private keys
    gmp_fprintf(pvfile, "%Zx\n", n);
    gmp_fprintf(pvfile, "%Zx\n", d);
}

void rsa_read_priv(mpz_t n, mpz_t d, FILE *pvfile) {
    // reads private keys
    gmp_fscanf(pvfile, "%Zx\n", n);
    gmp_fscanf(pvfile, "%Zx\n", d);
}

void rsa_encrypt(mpz_t c, mpz_t m, mpz_t e, mpz_t n) {
    // Performs RSA encryption
    pow_mod(c, m, e, n);
}

void rsa_encrypt_file(FILE *infile, FILE *outfile, mpz_t n, mpz_t e) {
    mpz_t c, m;
    mpz_inits(c, m, 0);

    // Encrypts the contents of infile, writing the encrypted contents to outfile
    size_t k = (mpz_sizeinbase(n, 2) - 1) / 8;
    uint8_t *block = calloc(k, sizeof(uint8_t));
    block[0] = 0xFF;

    while (feof(infile) == 0) {
        size_t j = fread(block + 1, sizeof(uint8_t), k - 1, infile);
        mpz_import(m, j + 1, 1, sizeof(uint8_t), 1, 0, block);
        rsa_encrypt(c, m, e, n);
        gmp_fprintf(outfile, "%Zx\n", c);
    }

    // free space
    mpz_clears(c, m, NULL);
    free(block);
}

void rsa_decrypt(mpz_t m, mpz_t c, mpz_t d, mpz_t n) {
    // Performs RSA decryption
    pow_mod(m, c, d, n);
}

void rsa_decrypt_file(FILE *infile, FILE *outfile, mpz_t n, mpz_t d) {
    mpz_t c, m;
    mpz_inits(c, m, 0);

    // Decrypts the contents of infile, writing the decrypted contents to outfile
    size_t k = (mpz_sizeinbase(n, 2) - 1) / 8;
    uint8_t *block = calloc(k, sizeof(uint8_t));

    while (feof(infile) == 0) {
        gmp_fscanf(infile, "%Zx\n", c);
        if (mpz_cmp_ui(c, 0) > 0) {
            rsa_decrypt(m, c, d, n);
            size_t j;
            mpz_export(block, &j, 1, sizeof(uint8_t), 1, 0, m);
            fwrite(block + 1, sizeof(uint8_t), j - 1, outfile);
        }
    }

    // free space
    mpz_clears(c, m, NULL);
    free(block);
}

void rsa_sign(mpz_t s, mpz_t m, mpz_t d, mpz_t n) {
    // Performs RSA signing
    pow_mod(m, s, d, n);
}

bool rsa_verify(mpz_t m, mpz_t s, mpz_t e, mpz_t n) {
    //Performs RSA verification
    mpz_t temp;
    mpz_init_set(temp, m);
    pow_mod(temp, s, e, n);
    bool compare = mpz_cmp(temp, m) == 0;

    // clears space
    mpz_clear(temp);
    return compare;
}
