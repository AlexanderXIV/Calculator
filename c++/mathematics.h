#pragma once
#include "defs.h"
#include <assert.h>
#include <math.h> // only for pow()

#define ACCURACY_STANDARD false

const decimal standard_accuracy = 10e-10;
const int sin_lookup_max = 10;
const number sin_lookup[sin_lookup_max] = {
    1,
    6,
    120,
    5040,
    362880,
    39916800,
    6227020800,
    1307674368000,
    355687428096000,
    121645100408832000};

const int cos_lookup_max = 11;
const number cos_lookup[cos_lookup_max] = {
    1,
    2,
    24,
    720,
    40320,
    3628800,
    479001600,
    87178291200,
    20922789888000,
    6402373705728000,
    2432902008176640000};

const int exp_lookup_max = 21;
const number exp_lookup[exp_lookup_max] = {
    1,
    1,
    2,
    6,
    24,
    120,
    720,
    5040,
    40320,
    362880,
    3628800,
    39916800,
    479001600,
    6227020800,
    87178291200,
    1307674368000,
    20922789888000,
    355687428096000,
    6402373705728000,
    121645100408832000,
    2432902008176640000};
const int ln_max = 2 * 50 + 1;

number my_faculty(const number value);

decimal my_sin(decimal x, const decimal accuracy = standard_accuracy);

decimal my_cos(decimal x, const decimal accuracy = standard_accuracy);

number my_gcd(number a, number b);

number n_k(number n, number k);

number my_lcm(number a, number b);

u_number my_fibonacci(u_number x);

decimal my_pow(decimal x, number n);

number my_pow(number x, number n);

decimal two_even_Bernoulli_number(number n);

const int asin_lookup_max = 17;
const decimal asin_lookup[asin_lookup_max]{
    1,
    0.5,
    0.375,
    0.3125,
    0.2734375,
    0.24609375,
    0.2255859375,
    0.20947265625,
    0.196380615234375,
    0.1854705810546875,
    0.176197052001953125,
    0.1681880950927734375,
    0.1611802577972412109375,
    0.15498101711273193359375,
    0.1494459807872772216796875,
    0.14446444809436798095703125,
    0.1399499340914189815521240234375};

decimal my_arcsin_slow(decimal x);

decimal my_arcsin(decimal x);

decimal my_arccos(decimal x);

decimal my_tan(decimal x, const decimal accuracy = standard_accuracy);

decimal my_exp(decimal x, const decimal accuracy = standard_accuracy);

decimal my_ln(decimal x, const decimal accuracy = standard_accuracy);