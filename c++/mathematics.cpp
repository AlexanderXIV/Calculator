#include "mathematics.h"

number my_faculty(const number value)
{
  number result = 1;
  for (auto i = 1; i <= value; ++i)
    result *= i;
  return result;
}

decimal my_sin(decimal x, const decimal accuracy)
{
  decimal res = 0, x_sqr = x * x, new_val;
  for (auto i = 0; i < sin_lookup_max; ++i)
  {
    new_val = x / sin_lookup[i];
    if (i & 1)
      res -= new_val;
    else
      res += new_val;
    if (ACCURACY_STANDARD && new_val < accuracy)
      return res;
    x *= x_sqr;
  }
  return res;
}

decimal my_cos(decimal x, const decimal accuracy)
{
  decimal res = 0, x_sqr = x * x, new_val;
  x = 1;
  for (auto i = 0; i < cos_lookup_max; ++i)
  {
    new_val = x / cos_lookup[i];
    if (i & 1)
      res -= new_val;
    else
      res += new_val;
    if (ACCURACY_STANDARD && new_val < accuracy)
      return res;
    x *= x_sqr;
  }
  return res;
}

number my_gcd(number a, number b)
{
  number t;
  while (b)
  {
    t = b;
    b = a % b;
    a = t;
  }

  return a;
}

number n_k(number n, number k)
{
  assert(0 <= k && k <= n && "range");
  number a = 1, b = 1;

  if (k < n - k)
  {
    for (auto i = n - k + 1; i <= n; ++i)
      a *= i;
    for (auto i = 2; i <= k; ++i)
      b *= i;
  }
  else
  {
    for (auto i = k + 1; i <= n; ++i)
      a *= i;
    for (auto i = 2; i <= n - k; ++i)
      b *= i;
  }

  return (number)a / b;
}

number my_lcm(number a, number b)
{
  return (a * b) / my_gcd(a, b);
}

u_number my_fibonacci(u_number x)
{
  if (x == 0)
    return 0;
  if (x == 1)
    return 1;

  u_number res, h1 = 0, h2 = 1;
  for (u_number i = 1; i < x; ++i)
  {
    res = h1 + h2;
    h1 = h2;
    h2 = res;
  }

  return res;
}

decimal my_pow(decimal x, number n)
{
  decimal res = 1;
  while (n > 0)
  {
    if (n & 1)
      res *= x;
    n >>= 1;
    x *= x;
  }
  return res;
}

number my_pow(number x, number n)
{
  number res = 1;
  while (n > 0)
  {
    if (n & 1)
      res *= x;
    n >>= 1;
    x *= x;
  }
  return res;
}

decimal two_even_Bernoulli_number(number n)
{
  decimal v = 2 * my_faculty(2 * n) / (decimal)pow(PI2, 2 * (double)n);
  if (!(n & 1))
    v = -v;
  decimal res = 0;
  for (auto i = 1; i < 30; ++i)
    res += 1 / (decimal)pow(i, 2 * (double)n);
  return v * res;
}

decimal my_arcsin_slow(decimal x)
{
  decimal res = 0, powers2 = x, x_sqr = x * x;
  number powers = 1, aa = 1, bb = 1;

  for (u_number i = 0; i < 30; ++i)
  {
    if (i > 0)
    {
      aa *= 2 * i - 1;
      bb *= i;
    }

    res += (aa / (bb * powers)) * powers2 / (2 * i + 1);

    powers <<= 1;
    powers2 *= x_sqr;
  }
  return res;
}

decimal my_arcsin(decimal x)
{
  decimal res = 0, powers2 = x, x_sqr = x * x;
  for (u_number i = 0; i < asin_lookup_max; ++i)
  {
    res += asin_lookup[i] * powers2 / (2 * i + 1);
    powers2 *= x_sqr;
  }
  return res;
}

decimal my_arccos(decimal x)
{
  return M_PI_2 - my_arcsin(x);
}

decimal my_tan(decimal x, const decimal accuracy)
{
  return my_sin(x, accuracy) / (my_cos(x, accuracy));
}
// 1 / (1 / x - 1)

// decimal Euler_numbers(decimal x, const decimal accuracy) // Eulerschen Zahlen
// {
//   decimal res = 0;
//   int n;

//   int a0 = 1, a1 = 1;
//   n = 2;
//   int tmp;
//   for (auto i = 0; i < 10; ++i) {
//     tmp
//   }
//   int a2 = 1 / (2 * n) *
//   return res;
// }

// decimal my_tan(decimal x, const decimal accuracy)
// {
//   decimal res = 0;
//   return res;
// }

decimal my_exp(decimal x, const decimal accuracy)
{
  decimal res = 0, x_tmp = 1, new_val;
  for (auto i = 0; i < exp_lookup_max; ++i)
  {
    new_val = x_tmp / exp_lookup[i];
    res += new_val;
    if (ACCURACY_STANDARD && new_val < accuracy)
      return res;
    x_tmp *= x;
  }
  return res;
}

decimal my_ln(decimal x, const decimal accuracy)
{
  assert(x > 0 && "ln <= 0 not defined");
  x = (x - 1) / (x + 1);
  decimal res = 0, x_sqr = x * x, tmp = 1, new_val;
  for (auto i = 1; i < ln_max; i += 2)
  {
    new_val = tmp / i;
    res += new_val;
    if (ACCURACY_STANDARD && new_val < accuracy)
      return 2 * x * res;
    tmp *= x_sqr;
  }
  return 2 * x * res;
}