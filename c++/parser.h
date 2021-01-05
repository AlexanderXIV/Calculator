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

enum NodeKind
{
  NodeValue,
  NodeAdd,
  NodeMul,
  NodeDiv,
  NodeSub,
  NodeVar,
  NodePow,
  NodeSin,
  NodeCos,
  NodeTan,
  NodeAsin,
  NodeAcos,
  NodeAtan,
  NodeLn,
  NodeLog,
  NodeCon
};

struct Node;

typedef std::unique_ptr<Node> NodePtr;

struct Node
{
  NodeKind kind;
  decimal value;
  int varId;
  string varName;

  // log(a, b) -> ln(4) == log(e, 4)
  NodePtr a /* base */, b /* value */;
  bool a_var, b_var; // true if in sub-tree a or b variable is used

  friend ostream &operator<<(ostream &os, const Node &t);

  Node(NodeKind _kind, decimal _value) : kind(_kind), value(_value), a_var(false), b_var(false){};
  Node(int _varId, string _varName) : kind(NodeVar), varId(_varId), varName(_varName), a_var(true), b_var(true){};
  Node(NodeKind _kind, NodePtr _a) : kind(_kind), a(move(_a)), a_var(a.get()->a_var || a.get()->b_var), b_var(false){};
  Node(NodeKind _kind, NodePtr _a, NodePtr _b) : kind(_kind), a(move(_a)), b(move(_b)), a_var(a.get()->a_var || a.get()->b_var), b_var(b.get()->a_var || b.get()->b_var){};
};

inline decimal rad(decimal value);

inline decimal deg(decimal value);

inline NodePtr to_node(decimal value);

inline NodePtr const_e();

inline NodePtr const_pi();

inline NodePtr operator+(NodePtr a, NodePtr b);

inline NodePtr operator-(NodePtr a, NodePtr b);

inline NodePtr operator*(NodePtr a, NodePtr b);

inline NodePtr operator/(NodePtr a, NodePtr b);

inline NodePtr operator^(NodePtr a, NodePtr b);

inline NodePtr log(NodePtr a, NodePtr b);

inline NodePtr sin(NodePtr a);

inline NodePtr cos(NodePtr a);

inline NodePtr tan(NodePtr a);

inline NodePtr asin(NodePtr a);

inline NodePtr acos(NodePtr a);

inline NodePtr atan(NodePtr a);

inline NodePtr ln(NodePtr a);

inline NodePtr variable(int varId, string varName);

inline NodePtr sqrt(NodePtr a);

inline bool operator==(const NodePtr &a, const NodeKind &k);

inline bool operator==(const NodeKind &k, const NodePtr &a);

inline bool operator==(const NodePtr &a, const decimal &f);

inline bool operator==(const decimal &f, const NodePtr &a);

bool operator==(const NodePtr &a, const NodePtr &b);

inline bool operator!=(const NodePtr &a, const NodeKind &k);

inline bool operator!=(const NodeKind &k, const NodePtr &a);

inline bool operator!=(const NodePtr &a, const decimal &f);

inline bool operator!=(const decimal &f, const NodePtr &a);

inline bool operator!=(const NodePtr &a, const NodePtr &b);

inline bool is_const_e(decimal value);

inline bool is_const_pi(decimal value);

inline bool is_const_e(const NodePtr &a);

inline bool is_const_pi(const NodePtr &a);

ostream &stringify(ostream &os, const Node &t, int level);

ostream &operator<<(ostream &os, const Node &t);

ostream &operator<<(ostream &os, const NodePtr &t);

struct TokenIter
{
  size_t cur;
  vector<Token> tokens;

  TokenIter(vector<Token> _tokens, size_t _cur) : cur(_cur), tokens(_tokens){};

  bool next(const TokenKind kind) { return cur < tokens.size() && tokens[cur].kind == kind; }

  bool take(TokenKind kind)
  {
    if (next(kind))
    {
      cur++;
      return true;
    }
    return false;
  }

  Token token() { return tokens[cur - 1]; }

  void back() { cur--; }
};

NodePtr parse(TokenIter &iter, map<string, int> &vars, bool next_negated, int level = 0);

NodePtr parse(string str, map<string, int> &vars);

decimal eval(const NodePtr &ptr_node, const vector<decimal> &vars);

NodePtr copy2(const NodePtr &p);

NodePtr copy(const NodePtr &p);

NodePtr derive(const NodePtr &ptr_node, int varId);

void flatten(NodePtr node, NodeKind node_kind, vector<NodePtr> &nds);

void flatten(NodePtr node, vector<NodePtr> &nds);

NodePtr unflatten(vector<NodePtr> &terms, NodeKind node_kind);

NodePtr simplify(const NodePtr &ptr_node);

int digits_before_dot(decimal a);

int digits(decimal a);

NodePtr stern_brocot_tree(decimal x, const int limit = 15, const decimal accuracy = 10e-10);

decimal deriveAt(const NodePtr &ptr_node, vector<decimal> &vars, int varId, const decimal accuracy = 1e8);

struct range
{
  double min_val, max_val;
  range(double _min_val, double _max_val) : min_val(_min_val), max_val(_max_val){}; // l1, u1 inclusive
  friend ostream &operator<<(ostream &os, const range &r);
};

ostream &operator<<(ostream &os, const range &r);

range getLimits(const NodePtr &ptr_node);

NodePtr inverse_var(const NodePtr &p, int varId);

bool cmpr(const NodePtr &a, const NodePtr &b);

bool axis_symmetic(const NodePtr &node_ptr, int varId);

bool point_symmetic(const NodePtr &node_ptr, int varId);

bool brents_fun(std::function<double(double)> f, const double lower, const double upper, const double tol, const unsigned int max_iter, double &s);

int find_all_roots(std::function<double(double)> f, const double lower, const double upper, const double tol, vector<double> &roots);

template <typename T>
struct Complex
{
  bool is_kartesian;
  T real, img;
  Complex(T r, T i = 0, bool is_p = true) : real(r), img(i), is_kartesian(is_p){};

  template <typename T1>
  friend ostream &operator<<(ostream &os, const Complex<T> &t);

  inline Complex<T> operator+(const Complex<T> &b)
  {
    assert(is_kartesian && "wrong format");
    return Complex<T>(real + b.real, img + b.img);
  }

  inline Complex<T> operator-(const Complex<T> &b)
  {
    assert(is_kartesian && "wrong format");
    return Complex<T>(real - b.real, img - b.img);
  }

  inline Complex<T> operator*(const Complex<T> &b)
  {
    assert(is_kartesian && "wrong format");
    return Complex<T>(real * b.real - img * b.img, real * b.img + img * b.real);
  }

  Complex<T> operator/(const Complex<T> &b)
  {
    assert(is_kartesian && "wrong format");
    T d1 = b.real * b.real + b.img * b.img;
    return Complex<T>((real * b.real - img * b.img) / d1, (real * b.img + img * b.real) / d1);
  }

  void cartesian_to_polar()
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

  void polar_to_cartesian()
  {
    assert(!is_kartesian && "wrong format");
    is_kartesian = true;
    T r = real;
    real *= cos(img);
    img = r * sin(img);
  }
};

typedef Complex<double> Complex64;

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