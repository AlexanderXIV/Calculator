#include "complex.h"

template <typename T>
Complex<T>::Complex(T r, T i, bool is_p) : real(r), img(i), is_kartesian(is_p){};

template <typename T>
inline Complex<T> Complex<T>::operator+(const Complex<T> &b)
{
  assert(is_kartesian && "wrong format");
  return Complex<T>(real + b.real, img + b.img);
}

template <typename T>
inline Complex<T> Complex<T>::operator-(const Complex<T> &b)
{
  assert(is_kartesian && "wrong format");
  return Complex<T>(real - b.real, img - b.img);
}

template <typename T>
inline Complex<T> Complex<T>::operator*(const Complex<T> &b)
{
  assert(is_kartesian && "wrong format");
  return Complex<T>(real * b.real - img * b.img, real * b.img + img * b.real);
}

template <typename T>
Complex<T> Complex<T>::operator/(const Complex<T> &b)
{
  assert(is_kartesian && "wrong format");
  T d1 = b.real * b.real + b.img * b.img;
  return Complex<T>((real * b.real - img * b.img) / d1, (real * b.img + img * b.real) / d1);
}

template <typename T>
void Complex<T>::cartesian_to_polar()
{
  assert(is_kartesian && "wrong format");
  assert((img != 0 || real != 0) && "error, not a complex num");
  is_kartesian = false;
  T r = sqrt(real * real + img * img);
  if (img >= 0)
  {
    if (real > 0)
      img = atan(img / real);
    else if (real < 0)
      img = M_PI - abs(atan(img / real));
  }
  else if (real > 0)
    img = M_PI + atan(img / real);
  else if (real < 0)
    img = PI2 - abs(atan(img / real));
  real = r;
}

template <typename T>
void Complex<T>::polar_to_cartesian()
{
  assert(!is_kartesian && "wrong format");
  is_kartesian = true;
  T r = real;
  real *= cos(img);
  img = r * sin(img);
}

// dringend ueberarbeiten
// inline Complex operator+(const decimal &a, const Complex &b) { return Complex(a, 0) + b; }
// inline Complex operator-(const decimal &a, const Complex &b) { return Complex(a, 0) - b; }
// inline Complex operator*(const decimal &a, const Complex &b) { return Complex(a, 0) * b; }
// inline Complex operator/(const decimal &a, const Complex &b) { return Complex(a, 0) / b; }
// inline Complex operator+(const Complex &a, const decimal &b) { return a + Complex(b, 0); }
// inline Complex operator-(const Complex &a, const decimal &b) { return a - Complex(b, 0); }
// inline Complex operator*(const Complex &a, const decimal &b) { return a * Complex(b, 0); }
// inline Complex operator/(const Complex &a, const decimal &b) { return a / Complex(b, 0); }

template <typename T>
pair<Complex<T>, Complex<T>> sqrt(Complex<T> a)
{
  a.cartesian_to_polar();
  a.is_kartesian = true;
  T r = sqrt(a.real);
  a.img *= 0.5;
  a.real = r * cos(a.img);
  a.img = r * sin(a.img);
  return make_pair(a, Complex<T>(-1 * a.real, -1 * a.img));
}

template <typename T>
Complex<T> sqrt_only_first(Complex<T> a) // returns only first solution (second in principal identical)
{
  a.cartesian_to_polar();
  a.is_kartesian = true;
  T r = sqrt(a.real);
  a.img *= 0.5;
  a.real = r * cos(a.img);
  a.img = r * sin(a.img);
  return a;
}

template <typename T>
array<Complex<T>, 3> cbrt(Complex<T> a) // array
{
  a.cartesian_to_polar();
  a.is_kartesian = true;
  T r = cbrt(a.real);
  a.img /= 3;
  const T img1 = a.img + PI2 / 3;
  const T img2 = a.img + 2 * PI2 / 3;
  return array<Complex<T>, 3>{Complex<T>(r * cos(a.img), r * sin(a.img)), Complex<T>(r * cos(img1), r * sin(img1)), Complex<T>(r * cos(img2), r * sin(img2))};
}

template <typename T>
vector<Complex<T>> n_root(Complex<T> a, int n)
{
  assert(n > 0);
  a.cartesian_to_polar();
  a.is_kartesian = true;
  T r = pow(a.real, 1 / (double)n);
  a.img /= n;
  vector<Complex<T>> solutions;
  T tmp;
  for (int i = 0; i < n; ++i)
  {
    tmp = a.img + (PI2 * i) / n;
    solutions.push_back(Complex<T>(r * cos(tmp), r * sin(tmp)));
  }
  return solutions;
}

// returns only one solution (k = 0), all solutions: a.img += 2 * M_PI * k, k E Z
template <typename T>
Complex<T> ln(Complex<T> a)
{
  a.cartesian_to_polar();
  a.is_kartesian = true;
  a.real = log(a.real);
  return a;
}

template <typename T>
ostream &operator<<(ostream &os, const Complex<T> &c)
{
  if (c.real == 0)
  {
    if (c.img == 0)
      os << '0';
    else if (c.img == 1)
      os << "i";
    else if (c.img == -1)
      os << "(-i)";
    else
      os << '(' << c.img << "i)";
  }
  else
  {
    if (c.img == 0)
      os << '(' << c.real << ')';
    else if (c.img == 1)
      os << '(' << c.real << " + i)";
    else if (c.img == -1)
      os << '(' << c.real << " - i)";
    else if (c.img < 0)
      os << '(' << c.real << " - " << -1 * c.img << "i)";
    else if (c.img > 0)
      os << '(' << c.real << " + " << c.img << "i)";
  }
  return os;
}

template <typename T>
ostream &operator<<(ostream &os, const vector<Complex<T>> &v)
{
  os << '[';
  bool is_first = true;
  for (auto e : v)
    if (is_first)
      is_first = false, os << e;
    else
      os << ", " << e;
  os << ']';
  return os;
}

template <typename T>
ostream &operator<<(ostream &os, const pair<Complex<T>, Complex<T>> &p)
{
  os << '[' << p.first << ", " << p.second << ']';
  return os;
}

template <typename T>
pair<Complex<T>, Complex<T>> abc(T a, T b, T c)
{
  assert(a != 0 && "not abc Formula");
  auto d1 = sqrt_only_first(Complex<T>(b * b - 4 * a * c));
  const T a2 = 2 * a, pre_calc_img = d1.img / a2;
  return make_pair(Complex<T>((d1.real - b) / a2, pre_calc_img), Complex<T>((-d1.real - b) / a2, -pre_calc_img));
}

template <typename T>
array<Complex<T>, 3> cardan(T a1, T b1, T c1, T d1)
{
  assert(a1 != 0);

  T a, b, c, p, q, d, tmp;
  a = b1 / a1, b = c1 / a1, c = d1 / a1;
  tmp = a * a / 3.0;
  p = b - tmp;
  q = a * (2 * tmp / 3.0 - b) / 3.0 + c;
  d = q * q / 4 + p * p * p / 27;

  tmp = a / 3;
  if (abs(d) < 10e-6) // naja, globaler Vegleich
  {
    T first = 3 * q / p - tmp;
    b = a1 * first + b1;

    Complex<T> d1 = sqrt_only_first(Complex<T>(b * b - 4 * a1 * (b * first + c1)));
    const T a2 = 2 * a1, pre_calc_img = d1.img / a2;
    return array<Complex<T>, 3>{Complex<T>(first), Complex<T>(((d1.real - b) / a2), pre_calc_img), Complex<T>((-d1.real - b) / a2, -pre_calc_img)};
  }
  else if (d > 0)
  {
    d = sqrt(d);
    T first = q / 2;
    first = cbrt(d - first) - cbrt(d + first) - tmp;
    b = a1 * first + b1;

    Complex<T> d1 = sqrt_only_first(Complex<T>(b * b - 4 * a1 * (b * first + c1)));
    const T a2 = 2 * a1, pre_calc_img = d1.img / a2;
    return array<Complex<T>, 3>{Complex<T>(first), Complex<T>((d1.real - b) / a2, pre_calc_img), Complex<T>((-d1.real - b) / a2, -pre_calc_img)};
  }
  else
  {
    T r = sqrt(-4.0 * p / 3.0);
    T phi = acos(-q / 2.0 * sqrt(-27.0 / (p * p * p))) / 3.0;

    return array<Complex<T>, 3>{Complex<T>(r * cos(phi) - tmp), Complex<T>(r * cos(phi + 2.0 / 3.0 * M_PI) - tmp), Complex<T>(r * cos(phi + 4.0 / 3.0 * M_PI) - tmp)};
  }
}

template <typename T>
array<Complex<T>, 4> quart(T a1, T b1, T c1, T d1, T e1)
{
  assert(a1 != 0);
  T a, b, c, p, q, r, tmp;
  a = b1 / a1, b = c1 / a1, c = d1 / a1, tmp = a * a;

  p = -3.0 / 8.0 * tmp + b;
  q = (tmp * a) / 8.0 - a * b / 2.0 + c;
  r = tmp * (-3.0 / 256.0 * tmp + b / 16.0) - c * a / 4.0 + e1 / a1;

  auto cd = cardan(1.0, -2.0 * p, p * p - 4.0 * r, q * q);

  Complex<T> z1 = sqrt_only_first(Complex<T>(0) - cd[0]), z2 = sqrt_only_first(Complex<T>(0) - cd[1]), z3 = sqrt_only_first(Complex<T>(0) - cd[2]);
  Complex<T> omega = (q > 0 == (z1 * z2 * z3).real > 0) ? Complex<T>(0.5) : Complex<T>(-0.5);
  Complex<T> tmp2 = a / 4.0;
  return array<Complex<T>, 4>{Complex<T>(omega * (z1 + z2 - z3) - tmp2), Complex<T>(omega * (z1 - z2 + z3) - tmp2), Complex<T>(omega * (z2 - z1 + z3) - tmp2) /* entspricht dem ersten??? */, Complex<T>(omega * (Complex<T>(0) - z1 - z2 - z3) - tmp2) /* entsprciht dem zweiten??? */};
}