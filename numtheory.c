#include "numtheory.h"
#include "randstate.h"
#include <gmp.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

// ******** Cite: Christian

gmp_randstate_t state;

void gcd(mpz_t d, mpz_t a, mpz_t b) {

    mpz_t alpha, beta, temp;

    // implemented gcd function from asgn pdf
    mpz_init_set(alpha, a);
    mpz_init_set(beta, b);
    mpz_init(temp);

    while (mpz_sgn(beta) != 0) {
        mpz_set(temp, beta);
        mpz_mod(beta, alpha, beta);
        mpz_set(alpha, temp);
    }

    // acts as return "g"
    mpz_set(d, alpha);
    mpz_clears(alpha, beta, temp, NULL);
}

void mod_inverse(mpz_t i, mpz_t a, mpz_t n) {
    mpz_t r, rinv, t, tinv, q, temp;

    // implement inverse from asgn pdf
    mpz_init_set(r, n);
    mpz_init_set(rinv, a);
    mpz_init_set_ui(t, 0);
    mpz_init_set_ui(tinv, 1);
    mpz_init(q);
    mpz_init(temp);
    while (mpz_sgn(rinv) != 0) {
        mpz_fdiv_q(q, r, rinv);
        mpz_set(temp, r);
        mpz_set(r, rinv);
        mpz_mul(rinv, q, rinv);
        mpz_sub(rinv, temp, rinv);

        mpz_set(temp, t);
        mpz_set(t, tinv);
        mpz_mul(tinv, q, tinv);
        mpz_sub(tinv, temp, tinv);
    }
    if ((mpz_cmp_ui(r, 1) > 0)) {
        mpz_set_ui(i, 0);
        mpz_clears(r, rinv, t, tinv, q, temp, NULL);
        return;
    }
    if (mpz_cmp_ui(t, 0) < 0) {
        mpz_add(t, t, n);
    }

    // acts as return "i"
    mpz_set(i, t);

    // clearing space
    mpz_clears(r, rinv, t, tinv, q, temp, NULL);
}

void pow_mod(mpz_t out, mpz_t base, mpz_t exponent, mpz_t modulus) {

    mpz_t v, p, exp, n;

    // implemented power-modulus function from asgn pdf
    mpz_init_set_ui(v, 1);
    mpz_init_set(p, base);
    mpz_init_set(exp, exponent);
    mpz_init_set(n, modulus);
    while (mpz_sgn(exp) == 1) {
        if (mpz_divisible_ui_p(exp, 2) == 0) {
            mpz_mul(v, v, p);
            mpz_mod(v, v, modulus);
        }
        mpz_mul(p, p, p);
        mpz_mod(p, p, modulus);
        mpz_fdiv_q_ui(exp, exp, 2);
    }

    // acts as return "output"
    mpz_set(out, v);

    // clears space
    mpz_clears(v, p, exp, n, NULL);
}

bool is_prime(mpz_t n, uint64_t iters) {

    mpz_t nn, nmod2, nmin1, s, r, a, y, temp1, two, j, smin1, tempmod;

    // implemented miller-rabin using asgn pdf
    mpz_init_set(nn, n);
    mpz_init(nmod2);
    mpz_mod_ui(nmod2, nn, 2);

    // initialize nmin to n -1 without changing value of n
    mpz_init(nmin1);
    mpz_sub_ui(nmin1, n, 1);

    // initialize s to 0; n - 1 = (2^s) * r
    mpz_init_set_ui(s, 0);

    mpz_init(tempmod);

    // initialize r in n - 1 = (2^s) * r; set it to 1 till value for r is solved
    mpz_init_set_ui(r, 1);
    // initialize "a" to 0 for later usage of random number selection
    mpz_init(a);
    mpz_init(y);
    // set nn to basical n - 4 for later range selection
    mpz_sub_ui(nn, nn, 4);

    // set another variable "temp1" to n-1 for later needs
    mpz_init(temp1);
    mpz_sub_ui(temp1, n, 1);

    mpz_init_set_ui(j, 1);

    // smin = s-1
    mpz_init(smin1);
    mpz_sub_ui(smin1, s, 1);

    // set two = 2 for pow mode variable later on
    mpz_init_set_ui(two, 2);

    // check if n is 2 (prime number) and return true
    if (mpz_cmp_ui(n, 2) == 0) {
        mpz_clears(nn, nmod2, nmin1, s, r, a, y, temp1, two, j, smin1, tempmod, NULL);
        return true;
    }
    // if n is even, or n=1, return false
    if (mpz_cmp_ui(nmod2, 0) == 0 || mpz_cmp_ui(n, 1) == 0) {
        mpz_clears(nn, nmod2, nmin1, s, r, a, y, temp1, two, j, smin1, tempmod, NULL);
        return false;
    }
    bool loopp = true;
    while (loopp) {
        // if n-1 is 0 return false
        if (mpz_sgn(nmin1) == 0) {
            mpz_clears(nn, nmod2, nmin1, s, r, a, y, temp1, two, j, smin1, tempmod, NULL);
            return false;
        }
        // set "tempmod" to (n-1)%2
        mpz_mod_ui(tempmod, nmin1, 2);
        if (mpz_cmp_ui(tempmod, 0) == 0) {
            // s++
            mpz_add_ui(s, s, 1);
            // nmin1/2
            mpz_divexact_ui(nmin1, nmin1, 2);
        } else {
            loopp = false;
            break;
        }
    }

    mpz_set(r, nmin1);
    // set smin1 to s-1 again in case s has changed
    mpz_sub_ui(smin1, s, 1);

    for (uint64_t i = 1; i <= iters; i++) {

        // random number "a" from 0 to n - 4
        mpz_urandomm(a, state, nn);
        // add two to "a" so the rang eof numbers is now 2 to n-2
        mpz_add_ui(a, a, 2);

        pow_mod(y, a, r, n);

        if (mpz_cmp_ui(y, 1) != 0 && mpz_cmp(y, temp1) != 0) {
            mpz_set_ui(j, 1);
            while (mpz_cmp(j, smin1) <= 0 && mpz_cmp(y, temp1) != 0) {
                pow_mod(y, y, two, n);
                if (mpz_cmp_ui(y, 1) == 0) {
                    mpz_clears(nn, nmod2, nmin1, s, r, a, y, temp1, two, j, smin1, tempmod, NULL);
                    return false;
                }
                mpz_add_ui(j, j, 1);
            }
            if (mpz_cmp(y, temp1) != 0) {
                mpz_clears(nn, nmod2, nmin1, s, r, a, y, temp1, two, j, smin1, tempmod, NULL);
                return false;
            }
        }
    }

    // clears space
    mpz_clears(nn, nmod2, nmin1, s, r, a, y, temp1, two, j, smin1, tempmod, NULL);
    return true;
}

void make_prime(mpz_t p, uint64_t bits, uint64_t iters) {

    // implemented using Christian's pseudo
    while (!is_prime(p, iters) || mpz_sizeinbase(p, 2) < bits) {
        mpz_urandomb(p, state, (mp_bitcnt_t)(bits + 2));
    }
    return;
}
