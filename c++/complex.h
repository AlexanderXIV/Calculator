#pragma once
#include "defs.h"
#include "tokenizer.h"

#include <iomanip>
#include <math.h>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <assert.h>
#include <array>

using std::abs;
using std::array;
using std::make_pair;
using std::make_unique;
using std::map;
using std::max;
using std::min;
using std::ostream;
using std::pair;
using std::string;
using std::swap;
using std::vector;

template <typename T>
struct Complex
{
  bool is_kartesian;
  T real, img;
  Complex(T r, T i = 0, bool is_p = true);

  template <typename T1>
  friend ostream &operator<<(ostream &os, const Complex<T> &t);

  inline Complex<T> operator+(const Complex<T> &b);

  inline Complex<T> operator-(const Complex<T> &b);

  inline Complex<T> operator*(const Complex<T> &b);

  Complex<T> operator/(const Complex<T> &b);

  void cartesian_to_polar();

  void polar_to_cartesian();
};

typedef Complex<double> Complex64;

// template struct Complex<int>;
// template struct Complex<float>;
// template struct Complex<double>;

template <typename T>
pair<Complex<T>, Complex<T>> sqrt(Complex<T> a);

template <typename T>
Complex<T> sqrt_only_first(Complex<T> a);

template <typename T>
array<Complex<T>, 3> cbrt(Complex<T> a);

template <typename T>
vector<Complex<T>> n_root(Complex<T> a, int n);

template <typename T>
Complex<T> ln(Complex<T> a);

template <typename T>
ostream &operator<<(ostream &os, const Complex<T> &c);

template <typename T>
ostream &operator<<(ostream &os, const vector<Complex<T>> &v);

template <typename T>
ostream &operator<<(ostream &os, const pair<Complex<T>, Complex<T>> &p);

template <typename T>
pair<Complex<T>, Complex<T>> abc(T a, T b, T c);

template <typename T>
array<Complex<T>, 3> cardan(T a1, T b1, T c1, T d1);

template <typename T>
array<Complex<T>, 4> quart(T a1, T b1, T c1, T d1, T e1);