#pragma once
#include <limits>

#define watch(x) cout << #x << " is " << x << endl

#ifndef INFINITY
#define INFINITY 1e8
#endif

#ifndef M_PI
#define M_PI 3.14159265358979323846 /* pi */
#endif

#ifndef M_PI_2
#define M_PI_2 1.57079632679489661923 /* pi/2 */
#endif

#ifndef M_E
#define M_E 2.7182818284590452354 /* e */
#endif

typedef long double decimal;
typedef long long int number;
typedef unsigned long long int u_number;
const decimal decimal_epsilon = std::numeric_limits<decimal>::epsilon();
const decimal PI2 = 2 * M_PI;