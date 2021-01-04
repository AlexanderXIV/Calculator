#include <iostream> // cin, cout, endl
#include <iomanip>  // setprecision
#include <cmath>    // min, max
#include <string>   // string
#include <vector>   // vector
#include <chrono>
#include <map>
#include <algorithm>
#include <functional>
#include <fstream>
#include <assert.h>
#include <array>

#ifndef _WIN32
#include <png++/image.hpp>
#include <png++/rgb_pixel.hpp>
#endif

using std::abs;
using std::array;
using std::cin;
using std::cout;
using std::endl;
using std::make_pair;
using std::make_tuple;
using std::make_unique;
using std::map;
using std::max;
using std::min;
using std::ostream;
using std::pair;
using std::string;
using std::swap;
using std::tuple;
using std::vector;

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

// hint: use exceptions instead of assert()
// hint: Performance over code redundancy

#pragma region Tokenizer
enum TokenKind
{
  TokenAdd,
  TokenSub,
  TokenMul,
  TokenDiv,
  TokenPot,
  TokenSin,
  TokenCos,
  TokenTan,
  TokenAsin,
  TokenAcos,
  TokenAtan,
  TokenLog,
  TokenLn,
  TokenVar,
  TokenOpen,
  TokenClose,
  TokenValue,
  TokenComma,
  TokenSqrt
};

struct Token
{
  TokenKind kind;
  decimal value;
  string name;

  Token(TokenKind _kind) : kind(_kind){};
  Token(decimal _value) : kind(TokenValue), value(_value){};
  Token(string _name) : kind(TokenVar), name(_name){};

  friend ostream &operator<<(ostream &os, const Token &t);
};

ostream &operator<<(ostream &os, const Token &t)
{
  switch (t.kind)
  {
  case TokenAdd:
    os << " + ";
    break;
  case TokenSub:
    os << " - ";
    break;
  case TokenMul:
    os << " * ";
    break;
  case TokenDiv:
    os << " / ";
    break;
  case TokenPot:
    os << " ^ ";
    break;
  case TokenSin:
    os << "sin";
    break;
  case TokenCos:
    os << "cos";
    break;
  case TokenTan:
    os << "tan";
    break;
  case TokenAsin:
    os << "asin";
    break;
  case TokenAcos:
    os << "acos";
    break;
  case TokenAtan:
    os << "atan";
    break;
  case TokenLog:
    os << "log";
    break;
  case TokenLn:
    os << "ln";
    break;
  case TokenVar:
    os << t.name;
    break;
  case TokenOpen:
    os << "(";
    break;
  case TokenClose:
    os << ")";
    break;
  case TokenValue:
    os << t.value;
    break;
  case TokenComma:
    os << "','";
    break;
  case TokenSqrt:
    os << "sqrt";
    break;
  }
  return os;
};

ostream &operator<<(ostream &os, const vector<Token> &tokens)
{
  bool writted = false;
  os << "[";
  for (const Token &t : tokens)
  {
    if (writted)
      os << ", ";
    else
      writted = true;
    os << t;
  }
  os << "]";
  return os;
}

bool is_decimal(const string &str)
{
  if (str.empty())
    return false;
  bool point = false;
  char chr;
  for (size_t i = 0; i < str.size(); i++)
  {
    chr = str[i];
    if (!isdigit(chr) && !(i == 0 && (chr == '+' || chr == '-') && str.size() > 1))
    {
      if (chr == '.' && !point)
        point = true;
      else
        return false;
    }
  }
  return true;
}

Token make_token(const string &str)
{
  if (str == "sqrt")
    return Token(TokenSqrt);
  if (str == "sin")
    return Token(TokenSin);
  if (str == "cos")
    return Token(TokenCos);
  if (str == "tan")
    return Token(TokenTan);
  if (str == "asin")
    return Token(TokenAsin);
  if (str == "acos")
    return Token(TokenAcos);
  if (str == "atan")
    return Token(TokenAtan);
  if (str == "log")
    return Token(TokenLog);
  if (str == "ln")
    return Token(TokenLn);
  if (is_decimal(str))
    return Token(stof(str));
  return Token(str);
}

void tokenize(string str, vector<Token> &result)
{
  string cur = "";
  for (char chr : str)
  {
    if (chr == '(' || chr == ')' || chr == '+' || chr == '-' || chr == '*' || chr == '/' || chr == '^' || chr == ',' || chr == ' ' || chr == '\n')
    {
      if (!cur.empty())
      {
        result.push_back(make_token(cur));
        cur = "";
      }

      if (chr == '(')
        result.push_back(Token(TokenOpen));
      else if (chr == ')')
        result.push_back(Token(TokenClose));
      else if (chr == ',')
        result.push_back(Token(TokenComma));
      else if (chr == '-')
        result.push_back(Token(TokenSub));
      else if (chr == '+')
        result.push_back(Token(TokenAdd));
      else if (chr == '*')
        result.push_back(Token(TokenMul));
      else if (chr == '/')
        result.push_back(Token(TokenDiv));
      else if (chr == '^')
        result.push_back(Token(TokenPot));
    }
    else if (('a' <= chr && chr <= 'z') || ('A' <= chr && chr <= 'Z') || ('0' <= chr && chr <= '9') || '.')
    {
      if (is_decimal(cur) && !is_decimal(cur + chr))
      {
        result.push_back(make_token(cur));
        result.push_back(Token(TokenMul));
        cur = "";
      }
      cur += chr;
    }
    else
      assert(0 && "unallowed Character");
  }

  if (!cur.empty())
    result.push_back(make_token(cur));
}
#pragma endregion

#pragma region Calculator
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

number my_faculty(const number value)
{
  number result = 1;
  for (auto i = 1; i <= value; ++i)
    result *= i;
  return result;
}

decimal my_sin(decimal x, const decimal accuracy = standard_accuracy)
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

decimal my_cos(decimal x, const decimal accuracy = standard_accuracy)
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

decimal my_tan(decimal x, const decimal accuracy = standard_accuracy)
{
  return my_sin(x, accuracy) / (my_cos(x, accuracy));
}
// 1 / (1 / x - 1)

// decimal Euler_numbers(decimal x, const decimal accuracy = standard_accuracy) // Eulerschen Zahlen
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

// decimal my_tan(decimal x, const decimal accuracy = standard_accuracy)
// {
//   decimal res = 0;
//   return res;
// }

decimal my_exp(decimal x, const decimal accuracy = standard_accuracy)
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

decimal my_ln(decimal x, const decimal accuracy = standard_accuracy)
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
#pragma endregion

#pragma region Parser
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

inline decimal rad(decimal value) { return value * M_PI / 180; }
inline decimal deg(decimal value) { return value * 180 / M_PI; }

inline NodePtr to_node(decimal value) { return make_unique<Node>(NodeValue, value); }
inline NodePtr const_e() { return make_unique<Node>(NodeCon, M_E); }
inline NodePtr const_pi() { return make_unique<Node>(NodeCon, M_PI); }

inline NodePtr operator+(NodePtr a, NodePtr b) { return make_unique<Node>(Node(NodeAdd, move(a), move(b))); }
inline NodePtr operator-(NodePtr a, NodePtr b) { return make_unique<Node>(Node(NodeSub, move(a), move(b))); }
inline NodePtr operator*(NodePtr a, NodePtr b) { return make_unique<Node>(Node(NodeMul, move(a), move(b))); }
inline NodePtr operator/(NodePtr a, NodePtr b) { return make_unique<Node>(Node(NodeDiv, move(a), move(b))); }
inline NodePtr operator^(NodePtr a, NodePtr b) { return make_unique<Node>(Node(NodePow, move(a), move(b))); }
inline NodePtr log(NodePtr a, NodePtr b) { return make_unique<Node>(NodeLog, move(a), move(b)); }
inline NodePtr sin(NodePtr a) { return make_unique<Node>(NodeSin, move(a)); }
inline NodePtr cos(NodePtr a) { return make_unique<Node>(NodeCos, move(a)); }
inline NodePtr tan(NodePtr a) { return make_unique<Node>(NodeTan, move(a)); }
inline NodePtr asin(NodePtr a) { return make_unique<Node>(NodeAsin, move(a)); }
inline NodePtr acos(NodePtr a) { return make_unique<Node>(NodeAcos, move(a)); }
inline NodePtr atan(NodePtr a) { return make_unique<Node>(NodeAtan, move(a)); }
inline NodePtr ln(NodePtr a) { return make_unique<Node>(NodeLn, move(a)); }
inline NodePtr variable(int varId, string varName) { return make_unique<Node>(varId, varName); }
inline NodePtr sqrt(NodePtr a) { return make_unique<Node>(NodePow, move(a), to_node(0.5)); }
// NodePtr log(NodePtr a) { return make_unique<Node>(NodeLog, a, 10); }

inline bool operator==(const NodePtr &a, const NodeKind &k) { return a.get()->kind == k; }

inline bool operator==(const NodeKind &k, const NodePtr &a) { return a.get()->kind == k; }

inline bool operator==(const NodePtr &a, const decimal &f) { return a.get()->kind == NodeValue && a.get()->value == f; }

inline bool operator==(const decimal &f, const NodePtr &a) { return a.get()->kind == NodeValue && a.get()->value == f; }

bool operator==(const NodePtr &a, const NodePtr &b)
{
  if (a.get()->kind != b.get()->kind)
    return false;

  switch (a.get()->kind)
  {
  case NodeValue:
  case NodeCon:
    return a.get()->value == b.get()->value;
  case NodeVar:
    return a.get()->varId == b.get()->varId;
  case NodeAdd:
  case NodeMul:
  case NodeDiv:
  case NodeSub:
  case NodePow:
  case NodeLog:
    return a.get()->a == b.get()->a && a.get()->b == b.get()->b; // Vertauschungen hinzufügen? -> Problem: (4 + 3 == 3 + 4) -> false
  case NodeSin:
  case NodeCos:
  case NodeTan:
  case NodeAsin:
  case NodeAcos:
  case NodeAtan:
  case NodeLn:
    return a.get()->a == b.get()->a;
  }
  assert(0 && "error");
  return false;
}

inline bool operator!=(const NodePtr &a, const NodeKind &k) { return !(a == k); }

inline bool operator!=(const NodeKind &k, const NodePtr &a) { return !(k == a); }

inline bool operator!=(const NodePtr &a, const decimal &f) { return !(a == f); }

inline bool operator!=(const decimal &f, const NodePtr &a) { return !(f == a); }

inline bool operator!=(const NodePtr &a, const NodePtr &b) { return !(a == b); }

inline bool is_const_e(decimal value) { return abs(value - (decimal)M_E) <= decimal_epsilon; }

inline bool is_const_pi(decimal value) { return abs(value - (decimal)M_PI) <= decimal_epsilon; }

inline bool is_const_e(const NodePtr &a) { return a.get()->kind == NodeCon && is_const_e(a.get()->value); }

inline bool is_const_pi(const NodePtr &a) { return a.get()->kind == NodeCon && is_const_pi(a.get()->value); }

// #define ALWAYS_CLIPS
ostream &stringify(ostream &os, const Node &t, int level)
{
  switch (t.kind)
  {
  case NodeCon:
    if (is_const_e(t.value))
      os << "e";
    else if (is_const_pi(t.value))
      os << "pi";
    else
      assert(0 && "unknown const");
    break;
  case NodeValue:
    if (t.value < 0)
      os << "(" << t.value << ")";
    else
      os << t.value;
    break;
  case NodeVar:
    os << t.varName;
    break;
  case NodeAdd:
    if (level > 1)
      os << "(";

    stringify(os, *t.a.get(), 1);
    os << " + ";
    stringify(os, *t.b.get(), 1);

    if (level > 1)
      os << ")";
    break;
  case NodeSub:
    if (level > 1)
      os << "(";

    stringify(os, *t.a.get(), 1);
    os << " - ";
    stringify(os, *t.b.get(), 2);

    if (level > 1)
      os << ")";
    break;
  case NodeSin:
    os << "sin(";
    stringify(os, *t.a.get(), 1);
    os << ")";
    break;
  case NodeCos:
    os << "cos(";
    stringify(os, *t.a.get(), 1);
    os << ")";
    break;
  case NodeTan:
    os << "tan(";
    stringify(os, *t.a.get(), 1);
    os << ")";
    break;
  case NodeAsin:
    os << "asin(";
    stringify(os, *t.a.get(), 1);
    os << ")";
    break;
  case NodeAcos:
    os << "acos(";
    stringify(os, *t.a.get(), 1);
    os << ")";
    break;
  case NodeAtan:
    os << "atan(";
    stringify(os, *t.a.get(), 1);
    os << ")";
    break;
  case NodeLn:
    os << "ln(";
    stringify(os, *t.a.get(), 1);
    os << ")";
    break;
  case NodeLog:
    os << "log(";
    stringify(os, *t.a.get(), 1);
    os << ", ";
    stringify(os, *t.b.get(), 1);
    os << ")";
    break;
  case NodeMul:
// level = 10;
#ifndef ALWAYS_CLIPS
    if (level > 2)
#endif
      os << "(";

    // proversorisch, dringend ueberarbeiten
    if (t.a == -1)
    {
      os << "-";
      stringify(os, *t.b.get(), 2);
    }
    else if (t.b == -1)
    {
      os << "-";
      stringify(os, *t.a.get(), 2);
    }
    else if ((t.b == NodeVar && t.a != NodeVar) || (t.a == NodeValue && (t.b == NodeCon || t.b == NodeSin || t.b == NodeCos || t.b == NodeLog)) || ((t.a == NodeCon || t.a == NodeSin || t.a == NodeCos || t.a == NodeLog) && (t.b == NodeCon || t.b == NodeSin || t.b == NodeCos || t.b == NodeLog)))
    {
      stringify(os, *t.a.get(), 2); // or 1 ???
      stringify(os, *t.b.get(), 2); // or 1 ???
    }
    else if ((t.a == NodeVar && t.b != NodeVar) || (t.b == NodeValue && (t.a == NodeCon || t.a == NodeSin || t.a == NodeCos || t.a == NodeLog)))
    {
      stringify(os, *t.b.get(), 2); // or 1 ???
      stringify(os, *t.a.get(), 2); // or 1 ???
    }
    else
    {
      stringify(os, *t.a.get(), 2);
      os << " * ";
      stringify(os, *t.b.get(), 2);
    }

#ifndef ALWAYS_CLIPS
    if (level > 2)
#endif
      os << ")";
    break;
  case NodeDiv:
    // level = 10;
#ifndef ALWAYS_CLIPS
    if (level > 2)
#endif
      os << "(";

    stringify(os, *t.a.get(), 3);
    os << " / ";
    stringify(os, *t.b.get(), 3);

#ifndef ALWAYS_CLIPS
    if (level > 2)
#endif
      os << ")";
    break;
  case NodePow:
    // level = 10;
#ifndef ALWAYS_CLIPS
    if (level > 2)
#endif
      os << "(";

    stringify(os, *t.a.get(), 4);
    os << " ^ ";
    stringify(os, *t.b.get(), 4);

#ifndef ALWAYS_CLIPS
    if (level > 2)
#endif
      os << ")";
    break;
  }
  return os;
}

ostream &operator<<(ostream &os, const Node &t)
{
  stringify(os, t, 0);
  return os;
}

ostream &operator<<(ostream &os, const NodePtr &t)
{
  os << *t.get();
  return os;
}

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

int operators(TokenKind kind)
{
  switch (kind)
  {
  case TokenAdd:
    return 1;
  case TokenSub:
    return 1;
  case TokenMul:
    return 2;
  case TokenDiv:
    return 2;
  case TokenPot:
    return 3;
  default:
    assert(0 && "error parsing");
    return 0;
  }
}

NodePtr parse(TokenIter &iter, map<string, int> &vars, bool next_negated, int level = 0)
{
  NodePtr result = nullptr;
  if (iter.take(TokenValue))
    result = to_node(iter.token().value);
  else if (iter.take(TokenLn))
  {
    assert(iter.take(TokenOpen) && "failed parsing");
    result = ln(parse(iter, vars, false));
    assert(iter.take(TokenClose) && "failed parsing");
  }
  else if (iter.take(TokenSqrt))
  {
    assert(iter.take(TokenOpen) && "failed parsing");
    result = sqrt(parse(iter, vars, false));
    assert(iter.take(TokenClose) && "failed parsing");
  }
  else if (iter.take(TokenSin))
  {
    assert(iter.take(TokenOpen) && "failed parsing");
    result = sin(parse(iter, vars, false));
    assert(iter.take(TokenClose) && "failed parsing");
  }
  else if (iter.take(TokenCos))
  {
    assert(iter.take(TokenOpen) && "failed parsing");
    result = cos(parse(iter, vars, false));
    assert(iter.take(TokenClose) && "failed parsing");
  }
  else if (iter.take(TokenTan))
  {
    assert(iter.take(TokenOpen) && "failed parsing");
    result = tan(parse(iter, vars, false));
    assert(iter.take(TokenClose) && "failed parsing");
  }
  else if (iter.take(TokenAsin))
  {
    assert(iter.take(TokenOpen) && "failed parsing");
    result = asin(parse(iter, vars, false));
    assert(iter.take(TokenClose) && "failed parsing");
  }
  else if (iter.take(TokenAcos))
  {
    assert(iter.take(TokenOpen) && "failed parsing");
    result = acos(parse(iter, vars, false));
    assert(iter.take(TokenClose) && "failed parsing");
  }
  else if (iter.take(TokenAtan))
  {
    assert(iter.take(TokenOpen) && "failed parsing");
    result = atan(parse(iter, vars, false));
    assert(iter.take(TokenClose) && "failed parsing");
  }
  else if (iter.take(TokenLog))
  {
    assert(iter.take(TokenOpen) && "failed parsing");
    NodePtr tmp = parse(iter, vars, false);
    assert(iter.take(TokenComma) && "failed parsing");
    result = log(move(tmp), parse(iter, vars, false));
    assert(iter.take(TokenClose) && "failed parsing");
  }
  else if (iter.take(TokenVar))
  {
    string name = iter.token().name;

    if (name == "e")
      result = const_e();
    else if (name == "pi")
      result = const_pi();
    else
    {
      map<string, int>::iterator it = vars.find(name);
      if (it != vars.end())
        result = variable(it->second, name);
      else
      {
        size_t index = vars.size();
        vars.emplace(name, index);
        result = variable((int)index, name);
      }
    }
  }
  else if (iter.take(TokenOpen))
  {
    result = parse(iter, vars, false);
    assert(iter.take(TokenClose) && "Clips not closed");
  }
  else if (iter.take(TokenSub))
    result = parse(iter, vars, true);
  else if (iter.take(TokenAdd))
    result = parse(iter, vars, false);

  assert(result && "should be a error ???");

  if (next_negated)
    result = to_node(-1) * move(result);

  while (iter.take(TokenAdd) || iter.take(TokenSub) || iter.take(TokenMul) || iter.take(TokenDiv) || iter.take(TokenPot))
  {
    TokenKind kind = iter.token().kind;
    int lv = operators(kind);
    if (lv <= level)
    {
      iter.back();
      return result;
    }

    if (kind == TokenAdd)
      result = move(result) + parse(iter, vars, false, lv);
    else if (kind == TokenSub)
      result = move(result) - parse(iter, vars, false, lv);
    else if (kind == TokenMul)
      result = move(result) * parse(iter, vars, false, lv);
    else if (kind == TokenDiv)
      result = move(result) / parse(iter, vars, false, lv);
    else
      result = move(result) ^ parse(iter, vars, false, lv);
  }

  return result;
}

NodePtr parse(string str, map<string, int> &vars)
{
  vector<Token> result;
  tokenize(str, result);
  TokenIter iter = TokenIter(result, 0);
  return parse(iter, vars, false);
}

decimal eval(const NodePtr &ptr_node, const vector<decimal> &vars) // array
{
  Node *node = ptr_node.get();
  switch (node->kind)
  {
  case NodeValue:
    return node->value;
  case NodeCon:
    return node->value;
  case NodeAdd:
    return eval(node->a, vars) + eval(node->b, vars);
  case NodeSub:
    return eval(node->a, vars) - eval(node->b, vars);
  case NodeMul:
    return eval(node->a, vars) * eval(node->b, vars);
  case NodeDiv:
    return eval(node->a, vars) / eval(node->b, vars);
  case NodePow:
    return pow((double)eval(node->a, vars), (double)eval(node->b, vars));
  case NodeSin:
    return sin((double)eval(node->a, vars));
  case NodeCos:
    return cos((double)eval(node->a, vars));
  case NodeTan:
    return tan((double)eval(node->a, vars));
  case NodeAsin:
    return asin((double)eval(node->a, vars));
  case NodeAcos:
    return acos((double)eval(node->a, vars));
  case NodeAtan:
    return atan((double)eval(node->a, vars));
  case NodeLn:
    return log((double)eval(node->a, vars));
  case NodeLog:
    return log((double)eval(node->b, vars)) / log((double)eval(node->a, vars));
  case NodeVar:
    assert((size_t)node->varId <= vars.size() && "Variable gibt es nicht");
    return vars[node->varId];
  }
  assert(0 && "error");
  return 0;
}

NodePtr copy2(const NodePtr &p)
{
  switch (p.get()->kind)
  {
  case NodeValue:
    return to_node(p.get()->value);
  case NodeAdd:
    return copy2(p.get()->a) + copy2(p.get()->b);
  case NodeMul:
    return copy2(p.get()->a) * copy2(p.get()->b);
  case NodeDiv:
    return copy2(p.get()->a) / copy2(p.get()->b);
  case NodeSub:
    return copy2(p.get()->a) - copy2(p.get()->b);
  case NodeVar:
    return variable(p.get()->varId, p.get()->varName);
  case NodePow:
    return copy2(p.get()->a) ^ copy2(p.get()->b);
  case NodeSin:
    return sin(copy2(p.get()->a));
  case NodeCos:
    return cos(copy2(p.get()->a));
  case NodeTan:
    return tan(copy2(p.get()->a));
  case NodeAsin:
    return asin(copy2(p.get()->a));
  case NodeAcos:
    return acos(copy2(p.get()->a));
  case NodeAtan:
    return atan(copy2(p.get()->a));
  case NodeLn:
    return ln(copy2(p.get()->a));
  case NodeLog:
    return log(copy2(p.get()->a), copy2(p.get()->b));
  case NodeCon:
    return make_unique<Node>(NodeCon, p.get()->value);
  }
  assert(0 && "error");
  return nullptr;
}

NodePtr copy(const NodePtr &p)
{
  NodePtr c = copy2(p);
  c.get()->a_var = p.get()->a_var;
  c.get()->b_var = p.get()->b_var;
  return c;
}

// derive and (partly) simplify a function
// error: sqrt(x^2)' = x -> should be abs(x)
// Todo: abs()
NodePtr derive(const NodePtr &ptr_node, int varId) // varId according to which is derived
{
  Node *node = ptr_node.get();
  switch (node->kind)
  {
  case NodeValue:
  case NodeCon:
    return to_node(0);
  case NodeAdd:
    if (node->a_var)
      return (node->b_var) ? derive(node->a, varId) + derive(node->b, varId) : derive(node->a, varId);
    else
      return (node->b_var) ? derive(node->b, varId) : to_node(0);
  case NodeSub:
    if (node->a_var)
      return (node->b_var) ? derive(node->a, varId) - derive(node->b, varId) : derive(node->a, varId);
    else
      return (node->b_var) ? to_node(-1) * derive(node->b, varId) : to_node(0);
  case NodeMul:
    if (node->a_var)
      return (node->b_var) ? copy(node->a) * derive(node->b, varId) + derive(node->a, varId) * copy(node->b) : derive(node->a, varId) * copy(node->b);
    else
      return (node->b_var) ? copy(node->a) * derive(node->b, varId) : to_node(0);
  case NodeDiv:
    if (node->a_var)
      return (node->b_var) ? (copy(node->b) * derive(node->a, varId) - derive(node->b, varId) * copy(node->a)) / (copy(node->b) ^ to_node(2)) : derive(node->a, varId) / copy(node->b);
    else
      return (node->b_var) ? (to_node(-1) * derive(node->b, varId) * copy(node->a)) / (copy(node->b) ^ to_node(2)) : to_node(0);
  case NodePow:
    if (node->a_var)
      return (node->b_var) ? (ln(copy(node->a)) * derive(node->b, varId) + copy(node->b) / copy(node->a) * derive(node->a, varId)) * copy(ptr_node) : copy(node->b) * (copy(node->a) ^ (copy(node->b) - to_node(1)));
    else
      return (node->b_var) ? ln(copy(node->a)) * derive(node->b, varId) * copy(ptr_node) : to_node(0);
  case NodeSin:
    return (node->a_var) ? cos(copy(node->a)) * derive(node->a, varId) : to_node(0);
  case NodeCos:
    return (node->a_var) ? to_node(-1) * sin(copy(node->a)) * derive(node->a, varId) : to_node(0);
  case NodeTan:
    return (node->a_var) ? derive(node->a, varId) * ((tan(copy(node->a)) ^ to_node(2)) + to_node(1)) : to_node(0);
  case NodeAsin:
    return derive(node->a, varId) / sqrt(to_node(1) - (copy(node->a) ^ to_node(2)));
  case NodeAcos:
    return (to_node(-1) * derive(node->a, varId)) / sqrt(to_node(1) - (copy(node->a) ^ to_node(2)));
  case NodeAtan:
    return derive(node->a, varId) / (to_node(1) + (copy(node->a) ^ to_node(2)));
  case NodeLog: // Annahme : log a(b) == log(a, b)
  {
    if (node->a_var)
    {
      if (node->b_var)
        return (ln(copy(node->a)) * (derive(node->b, varId) / copy(node->b)) - (derive(node->a, varId) / copy(node->a)) * ln(copy(node->b))) / (ln(copy(node->a)) ^ to_node(2));
      else
        return (to_node(-1) * (derive(node->a, varId) / copy(node->a)) * ln(copy(node->b))) / (ln(copy(node->a)) ^ to_node(2));
    }
    else
      return (node->b_var) ? derive(node->b, varId) / (copy(node->b) * ln(copy(node->a))) : to_node(0);
  }
  case NodeLn:
    return (node->a_var) ? derive(node->a, varId) / copy(node->a) : to_node(0);
  case NodeVar:
    return to_node(node->varId == varId);
  }
  assert(0 && "error");
  return nullptr;
}

void flatten(NodePtr node, NodeKind node_kind, vector<NodePtr> &nds)
{
  if (node.get()->kind == node_kind)
  {
    flatten(move(node.get()->a), node_kind, nds);
    flatten(move(node.get()->b), node_kind, nds);
  }
  else
    nds.push_back(move(node));
}

void flatten(NodePtr node, vector<NodePtr> &nds) { return flatten(move(node), node.get()->kind, nds); }

NodePtr unflatten(vector<NodePtr> &terms, NodeKind node_kind)
{
  NodePtr cur = move(terms[0]);

  for (size_t i = 1; i < terms.size(); ++i)
    if (node_kind == NodeAdd || node_kind == NodeSub || node_kind == NodeMul || node_kind == NodeDiv || node_kind == NodePow)
      cur = make_unique<Node>(node_kind, move(cur), move(terms[i]));
  return cur;
}

#define EXPERIMENTAL_FEATURE
NodePtr simplify(const NodePtr &ptr_node)
{
  Node *node = ptr_node.get();
  switch (node->kind)
  {
  case NodeValue:
    return to_node(node->value);
  case NodeCon:
    return make_unique<Node>(NodeCon, node->value);
  case NodeVar:
    return variable(node->varId, node->varName);
  case NodeSin:
  {
    NodePtr a = simplify(node->a);
    if (a == NodeAsin) // ATTENTION : should be definition area, value range
      return move(a.get()->a);
    else if (a == NodeAcos)
      return sqrt(to_node(1) - (move(a.get()->a) ^ to_node(2)));
    else if (a == NodeMul && a.get()->a == NodeValue && a.get()->a.get()->value == -1)
      return to_node(-1) * sin(move(a.get()->b));
    else if (a == NodeMul && a.get()->b == NodeValue && a.get()->b.get()->value == -1)
      return to_node(-1) * sin(move(a.get()->a));
    return sin(move(a));
  }
  case NodeCos: // sqrt(1-f^2) == sin(arccos(f)) == cos(arcsin(f))
  {
    NodePtr a = simplify(node->a);
    if (a == NodeAcos) // ATTENTION : should be definition area, value range
      return move(a.get()->a);
    else if (a == NodeAsin)
      return sqrt(to_node(1) - (move(a.get()->a) ^ to_node(2)));
    else if (a == NodeMul && a.get()->a == NodeValue && a.get()->a.get()->value == -1)
      return cos(move(a.get()->b));
    else if (a == NodeMul && a.get()->b == NodeValue && a.get()->b.get()->value == -1)
      return cos(move(a.get()->a));
    return cos(move(a));
  }
  case NodeTan:
    return tan(simplify(node->a));
  case NodeAsin:
    return asin(simplify(node->a));
  case NodeAcos:
    return acos(simplify(node->a));
  case NodeAtan:
    return atan(simplify(node->a));
  case NodePow:
  {
    NodePtr a = simplify(node->a);
    NodePtr b = simplify(node->b);
    if (a == 1)
      return to_node(1);
    if (b == NodeValue)
    {
      if (b.get()->value == 0)
        return to_node(1);
      else if (b.get()->value == 1)
        return a;
    }
    if (a == NodePow) // (a0 ^ a1) ^ b = a0 ^ (a1 * b)
      return simplify(move(a.get()->a) ^ (move(a.get()->b) * move(b)));
    return move(a) ^ move(b);
  }
  case NodeLog:
  {
    NodePtr a = simplify(node->a); // base
    NodePtr b = simplify(node->b); // value
    if (b == 0)
      assert(0 && "log undefined for this value");
    else if (b == 1)
      return to_node(0);
    else if (a == b)
      return to_node(1);
    return log(move(a), move(b));
  }
  case NodeLn:
  {
    NodePtr a = simplify(node->a); // value
    if (a == 0)
      assert(0 && "ln undefined for this value");
    else if (a == 1)
      return to_node(0);
    else if (is_const_e(a))
      return to_node(1);
    return ln(move(a));
  }
  case NodeMul:
  {
    NodePtr a = simplify(node->a);
    NodePtr b = simplify(node->b);

    if (a == 0 || b == 0)
      return to_node(0);
    else if (a == 1)
      return b;
    else if (b == 1)
      return a;
    else if (a == NodeValue && b == NodeValue)
      return to_node(a.get()->value * b.get()->value);
    else if (a == NodeMul && a.get()->a == NodeValue && b == NodeValue)
      return simplify(move(a.get()->b) * to_node(a.get()->a.get()->value * b.get()->value));
    else if (a == NodeMul && a.get()->b == NodeValue && b == NodeValue)
      return simplify(move(a.get()->a) * to_node(a.get()->b.get()->value * b.get()->value));
    else if (b == NodeMul && b.get()->a == NodeValue && a == NodeValue)
      return simplify(move(b.get()->b) * to_node(b.get()->a.get()->value * a.get()->value));
    else if (b == NodeMul && b.get()->b == NodeValue && a == NodeValue)
      return simplify(move(b.get()->a) * to_node(b.get()->b.get()->value * a.get()->value));
    else if (a == b)
      return simplify(move(a) ^ to_node(2));

    // if (a == NodeValue && b == NodeMul)

    return move(a) * move(b);
  }
  case NodeAdd:
  {
    NodePtr a = simplify(node->a);
    NodePtr b = simplify(node->b);
    if (a == 0)
      return b;
    else if (b == 0)
      return a;

#ifdef EXPERIMENTAL_FEATURE
    vector<NodePtr> terms;
    flatten(move(a) + move(b), terms);

    decimal value_sum = 0;
    vector<NodePtr> rest;
    for (NodePtr &term : terms)
      if (term == NodeValue)
        value_sum += term.get()->value;
      else
        rest.push_back(move(term));
    if (value_sum != 0 || rest.empty())
      rest.push_back(to_node(value_sum));

    return unflatten(rest, NodeAdd);
#endif

    return move(a) + move(b);
  }
  case NodeSub:
  {
    NodePtr a = simplify(node->a);
    NodePtr b = simplify(node->b);
    if (a == NodeValue && b == NodeValue)
      return to_node(a.get()->value - b.get()->value);
    else if (b == 0)
      return a;
    else if (a == 0)
      return simplify(to_node(-1) * move(b));
    return move(a) - move(b);
  }
  case NodeDiv:
  {
    NodePtr a = simplify(node->a);
    NodePtr b = simplify(node->b);
    if (a == 0)
      return to_node(0);
    else if (b == 0)
      assert(0 && "can`t divide by 0");
    return move(a) / move(b);
  }
  }
  assert(0 && "error");
  return nullptr;
}

int digits_before_dot(decimal a) { return (a < 10) ? 1 : (int)log10((double)a); }
int digits(decimal a) { return abs((int)log10((double)a)); }

// could be more efficient
NodePtr stern_brocot_tree(decimal x, const int limit = 15, const decimal accuracy = 10e-10)
{
  number a, b, n, ap0 = 1, ap1 = 0, bp0 = 0, bp1 = 1, nn = (number)x;
  x -= (number)x;
  decimal original = x;
  for (number i = 0; i < limit; ++i)
  {
    n = (number)floor((double)x);
    x = 1 / (x - n);

    a = ap0 + n * ap1;
    b = bp0 + n * bp1;
    ap0 = ap1;
    bp0 = bp1;
    ap1 = a;
    bp1 = b;

    if (abs((decimal)b / a - original) < accuracy)
      break;
  }
  return to_node(nn * a + b) / to_node(a);
  // return to_node(nn) * to_node(a) + to_node(b) / to_node(a);
}

decimal deriveAt(const NodePtr &ptr_node, vector<decimal> &vars, int varId, const decimal accuracy = 1e8)
{
  decimal num1 = eval(ptr_node, vars);
  vars[varId] += 1 / accuracy;
  return accuracy * (eval(ptr_node, vars) - num1);
}

struct range
{
  double min_val, max_val;
  range(double _min_val, double _max_val) : min_val(_min_val), max_val(_max_val){}; // l1, u1 inclusive
  friend ostream &operator<<(ostream &os, const range &r);
};

ostream &operator<<(ostream &os, const range &r)
{
  os << "[" << r.min_val << ", " << r.max_val << "]";
  return os;
}

range getLimits(const NodePtr &ptr_node)
{
  switch (ptr_node.get()->kind)
  {
  case NodeVar:
    return range(-INFINITY, INFINITY);
  case NodeSin:
  {
    range a = getLimits(ptr_node.get()->a);
    if (abs(a.max_val - a.min_val) < PI2)
    {
      int s1 = (int)(a.min_val / PI2);
      if (a.min_val < 0)
        --s1;
      a.min_val -= (double)(s1 * PI2);
      a.max_val -= (double)(s1 * PI2);

      if (a.min_val < M_PI_2)
      {
        if (a.max_val < M_PI_2)
          return range(sin(a.min_val), sin(a.max_val));
        else if (a.max_val < 1.5 * M_PI)
          return range(min(sin(a.min_val), sin(a.max_val)), 1);
      }
      else if (a.min_val < 1.5 * M_PI)
      {
        if (a.max_val < 1.5 * M_PI)
          return range(sin(a.max_val), sin(a.min_val));
        else if (a.max_val < 2.5 * M_PI)
          return range(-1, max(sin(a.max_val), sin(a.min_val)));
      }
      else
      {
        if (a.max_val < 2.5 * M_PI)
          return range(sin(a.min_val), sin(a.max_val));
        else if (a.max_val < 3.5 * M_PI)
          return range(min(sin(a.min_val), sin(a.max_val)), 1);
      }
    }
    return range(-1, 1);
  }
  case NodeCos:
  {
    range a = getLimits(ptr_node.get()->a);
    if (abs(a.max_val - a.min_val) < PI2)
    {
      int s1 = (int)(a.min_val / PI2);
      if (a.min_val < 0)
        --s1;
      a.min_val -= (double)(s1 * PI2);
      a.max_val -= (double)(s1 * PI2);

      if (a.min_val < M_PI)
      {
        if (a.max_val < M_PI)
          return range(cos(a.max_val), cos(a.min_val));
        else if (a.max_val < PI2)
          return range(-1, max(cos(a.max_val), cos(a.min_val)));
      }
      else
      {
        if (a.max_val < PI2)
          return range(cos(a.min_val), cos(a.max_val));
        else if (a.max_val < 3 * M_PI)
          return range(min(cos(a.min_val), cos(a.max_val)), 1);
      }
    }
    return range(-1, 1);
  }
  case NodeTan:
  {
    range a = getLimits(ptr_node.get()->a);
    if (abs(a.max_val - a.min_val) < M_PI)
    {
      int s1 = (int)(a.min_val / M_PI);
      if (a.min_val < 0)
        --s1;
      a.min_val -= s1 * M_PI;
      a.max_val -= s1 * M_PI;

      if (a.min_val > M_PI_2 || (a.min_val < M_PI_2 && a.max_val < M_PI_2))
        return range(atan(a.min_val), atan(a.max_val));
      else if (a.min_val < M_PI_2 && a.max_val > M_PI_2)
        return range(atan(a.max_val), atan(a.min_val));
    }
    return range(-INFINITY, INFINITY);
  }
  case NodeAsin:
  {
    range a = getLimits(ptr_node.get()->a);
    bool a1 = -1 <= a.min_val,
         b1 = a.max_val <= 1;

    if (a1 && b1)
      return range(asin(a.min_val), asin(a.max_val));
    else if (a1 && a.min_val <= 1)
      return range(asin(a.min_val), M_PI_2);
    else if (b1 && -1 <= a.max_val)
      return range(-M_PI_2, asin(a.max_val));
    else if (!(a1 || b1))
      return range(-M_PI_2, M_PI_2);
    else
      assert(0 && "Value out of domain for trigonometric function");
  }
  case NodeAcos:
  {
    range a = getLimits(ptr_node.get()->a);
    bool a1 = -1 <= a.min_val,
         b1 = a.max_val <= 1;

    if (a1 && b1)
      return range(acos(a.max_val), acos(a.min_val));
    else if (a1 && a.min_val <= 1)
      return range(0, acos(a.min_val));
    else if (b1 && -1 <= a.max_val)
      return range(acos(a.max_val), M_PI);
    else if (!(a1 || b1))
      return range(0, M_PI);
    else
      assert(0 && "Value out of domain for trigonometric function");
  }
  case NodeAtan:
  {
    range a = getLimits(ptr_node.get()->a);
    return range(atan(a.min_val), atan(a.max_val));
  }
  case NodeValue:
    return range((double)ptr_node.get()->value, (double)ptr_node.get()->value);
  case NodeCon:
    return range((double)ptr_node.get()->value, (double)ptr_node.get()->value);
  case NodeLn:
  {
    range a = getLimits(ptr_node.get()->a);
    if (a.min_val > 0)
      return range(log(a.min_val), log(a.max_val));
    else if (a.max_val > 0)
      return range(-INFINITY, log(a.max_val));
    else
      assert(0 && "Value out of domain for logarithm");
  }
  case NodeAdd:
  {
    range a = getLimits(ptr_node.get()->a);
    range b = getLimits(ptr_node.get()->b);
    return range(a.min_val + b.min_val, a.max_val + b.max_val);
  }
  case NodeSub:
  {
    range a = getLimits(ptr_node.get()->a);
    range b = getLimits(ptr_node.get()->b);
    return range(a.min_val - b.max_val, a.max_val - b.min_val);
  }
  case NodeMul:
  {
    range a = getLimits(ptr_node.get()->a);
    range b = getLimits(ptr_node.get()->b);
    return range(min(a.min_val * b.max_val, a.max_val * b.min_val), max(a.min_val * b.min_val, a.max_val * b.max_val));
  }
  case NodeLog:
    return getLimits(ln(copy(ptr_node.get()->b)) / ln(copy(ptr_node.get()->a))); // inefficient
  case NodeDiv:
    break;
  // {
  //   range a = getLimits(ptr_node.get()->a);
  //   range b = getLimits(ptr_node.get()->b);
  //   return range(min(a.min_val * b.max_val, a.max_val * b.min_val), max(a.min_val * b.min_val, a.max_val * b.max_val));
  // }
  case NodePow: // a ^ b
    break;
  }
  assert(0 && "error");
  return range(0, 0);
}

NodePtr inverse_var(const NodePtr &p, int varId)
{
  switch (p.get()->kind)
  {
  case NodeValue:
    return to_node(p.get()->value);
  case NodeAdd:
    return inverse_var(p.get()->a, varId) + inverse_var(p.get()->b, varId);
  case NodeMul:
    return inverse_var(p.get()->a, varId) * inverse_var(p.get()->b, varId);
  case NodeDiv:
    return inverse_var(p.get()->a, varId) / inverse_var(p.get()->b, varId);
  case NodeSub:
    return inverse_var(p.get()->a, varId) - inverse_var(p.get()->b, varId);
  case NodeVar:
    return to_node(-1) * variable(p.get()->varId, p.get()->varName);
  case NodePow:
    return inverse_var(p.get()->a, varId) + inverse_var(p.get()->b, varId);
  case NodeSin:
    return sin(inverse_var(p.get()->a, varId));
  case NodeCos:
    return cos(inverse_var(p.get()->a, varId));
  case NodeTan:
    return tan(inverse_var(p.get()->a, varId));
  case NodeAsin:
    return asin(inverse_var(p.get()->a, varId));
  case NodeAcos:
    return acos(inverse_var(p.get()->a, varId));
  case NodeAtan:
    return atan(inverse_var(p.get()->a, varId));
  case NodeLn:
    return ln(inverse_var(p.get()->a, varId));
  case NodeLog:
    return log(inverse_var(p.get()->a, varId), inverse_var(p.get()->b, varId));
  case NodeCon:
    return copy(p);
  }
  assert(0 && "error");
  return nullptr;
}

bool cmpr(const NodePtr &a, const NodePtr &b)
{
  // weitere Test mit bestimmten Werten?
  // extrem ineffiecient -> simplify(copy(...))
  return simplify(copy(a)) == simplify(copy(b));
}

// node_ptr should already be symplified
bool axis_symmetic(const NodePtr &node_ptr, int varId)
{ // f(x) == f(-x)
  return cmpr(node_ptr, inverse_var(node_ptr, varId));
}

// node_ptr should already be symplified
bool point_symmetic(const NodePtr &node_ptr, int varId)
{ // f(x) == f(-x)
  return cmpr(to_node(-1) * copy(node_ptr), inverse_var(node_ptr, varId));
}

bool brents_fun(std::function<double(double)> f, const double lower, const double upper, const double tol, const unsigned int max_iter, double &s)
{
  double a = lower;
  double b = upper;
  double fa = f(a); // calculated now to save function calls
  double fb = f(b); // calculated now to save function calls
  double fs = 0;    // initialize

  assert(fa * fb < 0 && "Signs of f(lower_bound) and f(upper_bound) must be opposites");

  if (abs(fa) < abs(b)) // if magnitude of f(lower_bound) is less than magnitude of f(upper_bound)
  {
    swap(a, b);
    swap(fa, fb);
  }

  double c = a;      // c now equals the largest magnitude of the lower and upper bounds
  double fc = fa;    // precompute function evalutation for point c by assigning it the same value as fa
  bool mflag = true; // boolean flag used to evaluate if statement later on
  s = 0;             // Our Root that will be returned
  double d = 0;      // Only used if mflag is unset (mflag == false)

  for (unsigned int iter = 1; iter < max_iter; ++iter)
  {
    // stop if converged on root or error is less than tolerance
    if (abs(b - a) < tol)
      return true;

    if (fa != fc && fb != fc) // use inverse quadratic interopolation
      s = (a * fb * fc / ((fa - fb) * (fa - fc))) + (b * fa * fc / ((fb - fa) * (fb - fc))) + (c * fa * fb / ((fc - fa) * (fc - fb)));
    else // secant method
      s = b - fb * (b - a) / (fb - fa);

    // checks to see whether we can use the faster converging quadratic && secant methods or if we need to use bisection
    if (((s < (3 * a + b) * 0.25) || (s > b)) ||
        (mflag && (abs(s - b) >= (abs(b - c) * 0.5))) ||
        (!mflag && (abs(s - b) >= (abs(c - d) * 0.5))) ||
        (mflag && (abs(b - c) < tol)) ||
        (!mflag && (abs(c - d) < tol)))
    {
      // bisection method
      s = (a + b) * 0.5;
      mflag = true;
    }
    else
      mflag = false;

    fs = f(s); // calculate fs
    d = c;     // first time d is being used (wasnt used on first iteration because mflag was set)
    c = b;     // set c equal to upper bound
    fc = fb;   // set f(c) = f(b)

    if (fa * fs < 0) // fa and fs have opposite signs
    {
      b = s;
      fb = fs; // set f(b) = f(s)
    }
    else
    {
      a = s;
      fa = fs; // set f(a) = f(s)
    }

    if (abs(fa) < abs(fb)) // if magnitude of fa is less than magnitude of fb
    {
      swap(a, b);   // swap a and b
      swap(fa, fb); // make sure f(a) and f(b) are correct after swap
    }
  }

  return false;
}

// returns number of roots found
int find_all_roots(std::function<double(double)> f, const double lower, const double upper, const double tol, vector<double> &roots)
{
  for (double x = lower; x <= upper; x += tol)
    if (abs(f(x) - f(x - tol)) < tol && abs(f(x) - f(x + tol)) < tol && abs(f(x)) < tol) // temp
    {
      roots.push_back(x);
      cout << f(x - tol) << " " << f(x) << " " << f(x + tol) << endl;
    }
  return (int)roots.size();
}

// template -> Complex64
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
  cout << '[';
  bool is_first = true;
  for (auto e : v)
    if (is_first)
      is_first = false, cout << e;
    else
      cout << ", " << e;
  cout << ']';
  return os;
}

template <typename T>
ostream &operator<<(ostream &os, const pair<Complex<T>, Complex<T>> &p)
{
  cout << '[' << p.first << ", " << p.second << ']';
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
#pragma endregion

#pragma region Visualisation
// #define ENABLE_PNG
#ifdef ENABLE_PNG
#ifndef _WIN32
const int len_seperation_lines = 5;

#define MAX_VAL (uint16_t)(UINT16_MAX)
#define RED png::rgb_pixel_16(MAX_VAL, 0, 0)
#define GREEN png::rgb_pixel_16(0, MAX_VAL, 0)
#define BLUE png::rgb_pixel_16(0, 0, MAX_VAL)
#define YELLOW png::rgb_pixel_16(MAX_VAL, MAX_VAL, 0)
#define CYAN png::rgb_pixel_16(0, MAX_VAL, MAX_VAL)
#define MAGENTA png::rgb_pixel_16(MAX_VAL, 0, MAX_VAL)
#define WHITE png::rgb_pixel_16(MAX_VAL, MAX_VAL, MAX_VAL)

struct GraphPNG
{
  png::image<png::rgb_pixel_16> img;
  int width, height, shift_x, shift_y;
  float ratio_x, ratio_y;

  GraphPNG(int _width, int _height, float window_left, float window_right, float window_bottom, float window_top) : img(_width, _height), width(_width), height(_height)
  {
    assert(width > 100 && "min. resolution");
    assert(height > 100 && "min. resolution");
    assert(1 < window_right - window_left && "min. window size");
    assert(1 < window_top - window_bottom && "min. window size");
    assert(window_left < 0);
    assert(window_bottom < 0);
    assert(window_top > 0);
    assert(window_right > 0);

    ratio_x = (float)width / (window_right - window_left);
    ratio_y = (float)height / (window_top - window_bottom);
    shift_x = -window_left * ratio_x;
    shift_y = -window_bottom * ratio_y;
  }

  GraphPNG(int _width, int _height, float window_left, float window_right, float window_bottom) : img(_width, _height), width(_width), height(_height)
  {
    float window_top = (float)(height * (window_right - window_left)) / (float)width + window_bottom;

    assert(width > 100 && "min. resolution");
    assert(height > 100 && "min. resolution");
    assert(1 < window_right - window_left && "min. window size");
    assert(1 < window_top - window_bottom && "min. window size");
    assert(window_left < 0);
    assert(window_bottom < 0);
    assert(window_top > 0);
    assert(window_right > 0);

    ratio_x = (float)width / (window_right - window_left);
    ratio_y = (float)height / (window_top - window_bottom);
    shift_x = -window_left * ratio_x;
    shift_y = -window_bottom * ratio_y;
  }

  GraphPNG(int _width, int _height, float window_left, float window_right) : img(_width, _height), width(_width), height(_height)
  {
    float window_top = (float)(height * (window_right - window_left)) / (float)(2 * width);

    assert(width > 100 && "min. resolution");
    assert(height > 100 && "min. resolution");
    assert(1 < window_right - window_left && "min. window size");
    assert(1 < 2 * window_top && "min. window size");
    assert(window_left < 0);
    assert(window_top > 0);
    assert(window_right > 0);

    ratio_x = (float)width / (window_right - window_left);
    ratio_y = ratio_x;
    shift_x = -window_left * ratio_x;
    shift_y = (float)height / 2;
  }

  bool plot(const png::rgb_pixel_16 &col, int x, int y, const float brightness)
  {
    if (x > 0 && y > 0 && x < width && y < height)
    {
      png::rgb_pixel_16 old = img.get_pixel(x, y);
      uint32_t r = old.red + col.red * brightness;
      if (r > UINT16_MAX)
        r = (uint16_t)(UINT16_MAX);
      old.red = (uint16_t)r;
      uint32_t g = old.green + col.green * brightness;
      if (g > UINT16_MAX)
        g = (uint16_t)(UINT16_MAX);
      old.green = (uint16_t)g;
      uint32_t b = old.blue + col.blue * brightness;
      if (b > UINT16_MAX)
        b = (uint16_t)(UINT16_MAX);
      old.blue = (uint16_t)b;
      img.set_pixel(x, y, old);
      return true;
    }
    return false;
  }

  void WuDrawLine(const png::rgb_pixel_16 &col, float x0, float y0, float x1, float y1)
  {
    x0 += shift_x;
    x1 += shift_x;
    y0 = height - (y0 + shift_y);
    y1 = height - (y1 + shift_y);

    auto ipart = [](float x) -> int { return int(std::floor(x)); };
    auto round = [](float x) -> float { return std::round(x); };
    auto fpart = [](float x) -> float { return x - std::floor(x); };
    auto rfpart = [=](float x) -> float { return 1 - fpart(x); };

    const bool steep = abs(y1 - y0) > abs(x1 - x0);
    if (steep)
    {
      std::swap(x0, y0);
      std::swap(x1, y1);
    }
    if (x0 > x1)
    {
      std::swap(x0, x1);
      std::swap(y0, y1);
    }

    const float dx = x1 - x0;
    const float dy = y1 - y0;
    const float gradient = (dx == 0) ? 1 : dy / dx;

    int xpx11;
    float intery;
    {
      const float xend = round(x0);
      const float yend = y0 + gradient * (xend - x0);
      const float xgap = rfpart(x0 + 0.5f);
      xpx11 = int(xend);
      const int ypx11 = ipart(yend);
      if (steep)
      {
        plot(col, ypx11, xpx11, rfpart(yend) * xgap);
        plot(col, ypx11 + 1, xpx11, fpart(yend) * xgap);
      }
      else
      {
        plot(col, xpx11, ypx11, rfpart(yend) * xgap);
        plot(col, xpx11, ypx11 + 1, fpart(yend) * xgap);
      }
      intery = yend + gradient;
    }

    int xpx12;
    {
      const float xend = round(x1);
      const float yend = y1 + gradient * (xend - x1);
      const float xgap = rfpart(x1 + 0.5f);
      xpx12 = int(xend);
      const int ypx12 = ipart(yend);
      if (steep)
      {
        plot(col, ypx12, xpx12, rfpart(yend) * xgap);
        plot(col, ypx12 + 1, xpx12, fpart(yend) * xgap);
      }
      else
      {
        plot(col, xpx12, ypx12, rfpart(yend) * xgap);
        plot(col, xpx12, ypx12 + 1, fpart(yend) * xgap);
      }
    }

    if (steep)
      for (int x = xpx11 + 1; x < xpx12; x++)
      {
        plot(col, ipart(intery), x, rfpart(intery));
        plot(col, ipart(intery) + 1, x, fpart(intery));
        intery += gradient;
      }
    else
      for (int x = xpx11 + 1; x < xpx12; x++)
      {
        plot(col, x, ipart(intery), rfpart(intery));
        plot(col, x, ipart(intery) + 1, fpart(intery));
        intery += gradient;
      }
  }

  void drawAxis(const png::rgb_pixel_16 &col = WHITE)
  {
    int dist_seperation_lines_x = ratio_x;
    int dist_seperation_lines_y = ratio_y;

    WuDrawLine(col, 0, -shift_y, 0, height - shift_y);
    WuDrawLine(col, -shift_x, 0, width - shift_x, 0);

    for (int i = dist_seperation_lines_y; i < height - shift_y; i += dist_seperation_lines_y)
      WuDrawLine(col, -len_seperation_lines, i, len_seperation_lines, i);
    for (int i = dist_seperation_lines_y; i >= -shift_y; i -= dist_seperation_lines_y)
      WuDrawLine(col, -len_seperation_lines, i, len_seperation_lines, i);
    for (int i = dist_seperation_lines_x; i < width - shift_x; i += dist_seperation_lines_x)
      WuDrawLine(col, i, -len_seperation_lines, i, len_seperation_lines);
    for (int i = dist_seperation_lines_x; i >= -shift_x; i -= dist_seperation_lines_x)
      WuDrawLine(col, i, -len_seperation_lines, i, len_seperation_lines);
  }

  void drawFunction(const NodePtr &f, int varId, vector<decimal> variables, const png::rgb_pixel_16 &col = WHITE)
  {
    variables[varId] = -shift_x / ratio_x;
    float r2, r1 = ratio_y * (float)eval(f, variables);
    for (int x = -shift_x + 1; x < width - shift_x; ++x)
    {
      variables[varId] = x / ratio_x;
      r2 = ratio_y * (float)eval(f, variables);
      WuDrawLine(col, (float)x, r1, (float)(x + 1), r2);
      r1 = r2;
    }
  }

  void save(string name)
  {
    img.write(name);
  }
};
#endif
#endif

enum EqualizeType
{
  EqualizeNone,
  EqualizeXAxes,
  EqualizeYAxes,
  EqualizeAuto
};

const int higlighting_step = 5;

const int perfect_val_len = 125;

const int nums_circle_radius = 10;
const int num_shift_x_axis = 20;
const int num_shift_y_axis = num_shift_x_axis;

enum Mode
{
  LIGHT,
  DARK,
  SUPER_DARK,
  PERSONALIZED
};

struct Style
{
  Mode mode;
  string color_background, color_axis, color_lines, color_lines_highlighted, color_nums;

  Style(const Mode _mode) : mode(_mode)
  {
    assert(mode != PERSONALIZED);
    if (mode == LIGHT)
    {
      color_background = "#fff";
      color_axis = "#000";
      color_lines = "#E5E5E5";
      color_lines_highlighted = "#C0C0C0";
    }
    else if (mode == DARK)
    {
      color_background = "#333333";
      color_axis = "#fff";
      color_lines = "#424242";
      color_lines_highlighted = "#5C5C5C";
    }
    else
    {
      color_background = "#000";
      color_axis = "#fff";
      color_lines = "#1A1A1A";
      color_lines_highlighted = "#3F3F3F";
    }
    color_nums = color_axis;
  };

  Style(const string _color_background, const string _color_axis, const string _color_lines, const string _color_lines_highlighted, const string _color_nums) : mode(PERSONALIZED), color_background(_color_background), color_axis(_color_axis), color_lines(_color_lines), color_lines_highlighted(_color_lines_highlighted), color_nums(_color_nums){};
};

const string url = "http://www.w3.org/2000/svg";
struct GraphSVG
{
  float dif_tick_x, dif_tick_y;
  int width, height;
  bool set_window;
  float xMin, xMax, yMin, yMax;
  Style style_mode;
  std::ofstream pic;

  // Fehler: Keine Y-Achse, X-Achse unv.
  void drawInterlines(const float min_val, const float max_val, const float tick, std::function<void(float, bool)> stn)
  {
    int counter = 0;
    if (min_val <= 0 && max_val >= 0)
    {
      for (float x = -tick; x > min_val; x -= tick)
        stn(x, ((++counter >= higlighting_step) ? counter = 0, true : false));
      counter = 0;
      for (float x = tick; x < max_val; x += tick)
        stn(x, ((++counter >= higlighting_step) ? counter = 0, true : false));
    }
    else if (max_val < 0)
      for (float x = max_val - (float)fmod(max_val, tick); x > min_val; x -= tick) // flaot inacurracy, reason for partly ugly layout (left, right)
        stn(x, ((++counter >= higlighting_step) ? counter = 0, true : false));
    else if (min_val > 0)
      for (float x = min_val + (float)fmod(min_val, tick); x < max_val; x += tick)
        stn(x, ((++counter >= higlighting_step) ? counter = 0, true : false));
  }

  template <typename T>
  void addAttribute(const string &identifier, const T value) { pic << " " << identifier << "=\"" << value << "\""; }

  void drawLine(float x1, float y1, float x2, float y2, const string stroke, const float stroke_width = 3.0)
  {
    pic << "<line";
    addAttribute("stroke", stroke);
    addAttribute("fill", "transparent");
    addAttribute("stroke-width", stroke_width);
    addAttribute("x1", x1);
    addAttribute("y1", y1);
    addAttribute("x2", x2);
    addAttribute("y2", y2);
    pic << "></line>\n";
  }

  void drawHorizontal(float x1, float y, float x2, const string stroke, const float stroke_width = 3.0) { drawLine(x1, y, x2, y, stroke, stroke_width); }

  void drawVertical(float x, float y1, float y2, const string stroke, const float stroke_width = 3.0) { drawLine(x, y1, x, y2, stroke, stroke_width); }

  float canvasXFromX(float x) { return (float)width * ((x - xMin) / (xMax - xMin)); }

  float canvasYFromY(float y) { return (float)height - (float)height * ((y - yMin) / (yMax - yMin)); }

  array<float, 2> canvasPtFromXY(float x, float y) { return array<float, 2>{(float)width * ((x - xMin) / (xMax - xMin)), (float)height - ((y - yMin) / (yMax - yMin)) * (float)height}; }

  GraphSVG(const string filename, const int _width, const int _height, float _xMin, float _xMax, float _yMin, float _yMax, EqualizeType equalizetype, Style _style_mode = Style(DARK), bool drawAxes = true) : width(_width), height(_height), set_window(false), xMin(_xMin), xMax(_xMax), yMin(_yMin), yMax(_yMax), style_mode(_style_mode)
  {
    pic.open(filename);
    pic << std::setprecision(5);

    const string style = "background-color:" + style_mode.color_background + ";shape-rendering:geometricPrecision; text-rendering:geometricPrecision; image-rendering:optimizeQuality; fill-rule:evenodd; clip-rule:evenodd;";
    pic << "<svg width=\"" << width << "\" height=\"" << height << "\" style=\"" << style << "\" xmlns=\"" << url << "\"><style>.function:hover{stroke-width: 4;transition: all ease 0.3s;}.label:hover{cursor: default;pointer-events: none;}.label{cursor: default;pointer-events: none;}</style>\n";
    pic << "<title>Calculator</title>\n";
    if (equalizetype == EqualizeXAxes)
    {
      float xMid = (xMax + xMin) / 2.0f;
      float half = (((float)width * (yMax - yMin)) / (float)height) / 2.0f;
      xMin = xMid - half;
      xMax = xMid + half;
    }
    else if (equalizetype == EqualizeYAxes)
    {
      float yMid = (yMax + yMin) / 2.0f;
      float half = (((float)height * (xMax - xMin)) / (float)width) / 2.0f;
      yMin = yMid - half;
      yMax = yMid + half;
    }
    else if (equalizetype == EqualizeAuto)
    {
      float xRatio = (xMax - xMin) / (float)width;
      float yRatio = (yMax - yMin) / (float)height;
      if (xRatio < yRatio)
      {
        float xMid = (xMax + xMin) / 2.0f;
        float half = (yRatio / xRatio) * (xMax - xMin) / 2.0f;
        xMin = xMid - half;
        xMax = xMid + half;
      }
      else
      {
        float yMid = (yMax + yMin) / 2.0f;
        float half = (xRatio / yRatio) * (yMax - yMin) / 2.0f;
        yMin = yMid - half;
        yMax = yMid + half;
      }
    }

    float tmp_x = (float)perfect_val_len * (xMax - xMin) / (float)width;
    int ceros_x = (int)floor(log10(tmp_x));
    if (ceros_x != 0)
      tmp_x /= (float)pow(10, ceros_x);

    if (tmp_x < 1.5)
      dif_tick_x = 1 * (float)pow(10, ceros_x);
    else if (tmp_x < 3.5)
      dif_tick_x = 2 * (float)pow(10, ceros_x);
    else if (tmp_x < 7.5)
      dif_tick_x = 5 * (float)pow(10, ceros_x);
    else
      dif_tick_x = (float)pow(10, ceros_x + 1);

    float tmp_y = (float)perfect_val_len * (yMax - yMin) / (float)height;
    int ceros_y = (int)floor(log10(tmp_y));
    if (ceros_y != 0)
      tmp_y /= (float)pow(10, ceros_y);

    if (tmp_y < 1.5)
      dif_tick_y = 1 * (float)pow(10, ceros_y);
    else if (tmp_y < 3.5)
      dif_tick_y = 2 * (float)pow(10, ceros_y);
    else if (tmp_y < 7.5)
      dif_tick_y = 5 * (float)pow(10, ceros_y);
    else
      dif_tick_y = (float)pow(10, ceros_y + 1);

    if (drawAxes)
    {
      array<float, 2> leftPt = canvasPtFromXY(xMin, 0);
      array<float, 2> rightPt = canvasPtFromXY(xMax, 0);
      array<float, 2> botPt = canvasPtFromXY(0, yMin);
      array<float, 2> topPt = canvasPtFromXY(0, yMax);

      drawInterlines(xMin, xMax, dif_tick_x / 5, [=](float x, bool highlighted) -> void { drawVertical(canvasXFromX(x), botPt[1], topPt[1], ((highlighted) ? style_mode.color_lines_highlighted : style_mode.color_lines), 1.5f); });
      drawInterlines(yMin, yMax, dif_tick_y / 5, [=](float y, bool highlighted) -> void { drawHorizontal(leftPt[0], canvasYFromY(y), rightPt[0], ((highlighted) ? style_mode.color_lines_highlighted : style_mode.color_lines), 1.5f); });

      if (0 <= leftPt[1] && rightPt[1] <= (float)height)
        drawLine(leftPt[0], leftPt[1], rightPt[0], rightPt[1], style_mode.color_axis, 1.5f);
      if (0 <= botPt[0] && topPt[0] <= (float)width)
        drawLine(botPt[0], botPt[1], topPt[0], topPt[1], style_mode.color_axis, 1.5f);
    }
  };

  ~GraphSVG()
  {
    if (pic.is_open())
    {
      pic.flush();
      pic.close();
    }
  }

  void drawFn(const NodePtr &f, int varId, vector<decimal> variables, const int numFnPts = 300, const string stroke = "red", const float stroke_width = 3.0)
  {
    float xDelta = (xMax - xMin) / (float)(numFnPts - 1);
    vector<pair<float, float>> pts;

    pic << "<path";
    addAttribute("class", "function");
    addAttribute("stroke", stroke);
    addAttribute("fill", "transparent");
    addAttribute("stroke-width", stroke_width);
    pic << " d=\"";

    bool first_iteration = true, first_point = true, jump = true;
    float last_x, last_y;

    for (int i = 0; i < numFnPts; ++i)
    {
      float x, xTarget = xMin + (float)i * xDelta;
      do
      {
        x = xTarget;

        variables[varId] = x;
        float y = (float)eval(f, variables);
        array<float, 2> canvasPt = canvasPtFromXY(x, y);

        // float perc = 0.5;
        // while (!first_point && abs(last_y - canvasPt[1]) > 30 && perc >= 0.5)
        // {
        //   x = (1.0f - perc) * xPrev + perc * xTarget;
        //   variables[varId] = x;
        //   float y = (float)eval(f, variables);
        //   canvasPt = canvasPtFromXY(xMin, xMax, yMin, yMax, x, y);
        //   perc /= 2.0f;
        // }

        if (0 <= canvasPt[1] && canvasPt[1] <= (float)height)
        {
          if (jump)
          {
            if (first_point)
            {
              pic << "M";
              if (!first_iteration)
                pic << last_x << " " << last_y << " L";
            }
            else
              pic << " M" << last_x << " " << last_y << " L";
            first_point = false;
            jump = false;
          }
          else
            pic << " L";

          pic << canvasPt[0] << " " << canvasPt[1];
        }
        else
        {
          if (!jump)
          {
            const int val = (canvasPt[1] < 0) ? 0 : height;
            pic << " L" << (((float)val - canvasPt[1]) * (canvasPt[0] - last_x)) / ((canvasPt[1] - last_y)) + canvasPt[0] << " " << val;
          }
          jump = true;
        }

        last_x = canvasPt[0];
        last_y = canvasPt[1];
        first_iteration = false;

      } while (x < xTarget);
    }

    pic << "\"></path>\n";
  }

  void drawCircle(float x, float y, float r, const string col)
  {
    pic << "<circle";
    addAttribute("cx", x);
    addAttribute("cy", y);
    addAttribute("r", r);
    addAttribute("fill", col);
    pic << "/>\n";
  }

  template <typename Text>
  void drawText(const float x, const float y, const string col, const Text text)
  {
    pic << "<text";
    addAttribute("fill", col);
    // addAttribute("stroke", "#333333");
    addAttribute("dominant-baseline", "central");
    addAttribute("text-anchor", "middle");
    addAttribute("x", x);
    addAttribute("y", y);
    pic << ">" << text << "</text>\n";
  }

  void drawTextCircle(float x, float y, const float num, const string col, const int rad, const int shift_x, const int shift_y)
  {
    x = (float)shift_x + canvasXFromX((float)x);
    y = (float)shift_y + canvasYFromY((float)y);
    drawCircle(x, y, (float)rad, col);
    drawText(x, y, style_mode.color_nums, num);
  }

  void save()
  {
    float axis_shift_x, axis_shift_y;
    int shift_x_axis = num_shift_x_axis, shift_y_axis = num_shift_y_axis;

    if (xMin <= 0 && xMax >= 0)
      axis_shift_x = 0;
    else if (xMax < 0)
      axis_shift_x = xMax, shift_x_axis = -shift_x_axis;
    else if (xMin > 0)
      axis_shift_x = xMin;

    if (yMin <= 0 && yMax >= 0)
      axis_shift_y = 0;
    else if (yMax < 0)
      axis_shift_y = yMax;
    else if (yMin > 0)
      axis_shift_y = yMin, shift_y_axis = -shift_y_axis;

    drawInterlines(xMin, xMax, dif_tick_x, [=](float x, bool highlighted) -> void { drawTextCircle(x, axis_shift_y, x, (highlighted) ? style_mode.color_background : style_mode.color_background, nums_circle_radius, 0, shift_y_axis); }); // change color
    drawInterlines(yMin, yMax, dif_tick_y, [=](float y, bool highlighted) -> void { drawTextCircle(axis_shift_x, y, y, (highlighted) ? style_mode.color_background : style_mode.color_background, nums_circle_radius, shift_x_axis, 0); }); // change color

    if ((yMin <= 0 && 0 <= yMax) || (xMin <= 0 && 0 <= xMax))
      drawTextCircle(axis_shift_x, axis_shift_y, 0, style_mode.color_background, nums_circle_radius, num_shift_x_axis, num_shift_y_axis);

    pic << "</svg>";
    pic.flush();
    pic.close();
  }
};
#pragma endregion

long benchmark(std::function<void()> f)
{
  auto start = std::chrono::system_clock::now();
  f();
  auto end = std::chrono::system_clock::now();
  return std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
}

// compile with ./a.out example.svg 1920 1080 -6.2 4 -8.5 10.35 auto dark "sin(x)" "#d0f" 5.0 "cos(x)" "#ff0" 2.0
int main(int argc, char *argv[])
{
  std::ios::sync_with_stdio(false);
  cin.tie(0);
  cout << std::setprecision(8);

  if (argc > 1)
  {
    const string filename = argv[1];
    const int resolution_height = std::stoi(argv[2]);
    const int resolution_width = std::stoi(argv[3]);
    const float window_left = std::stof(argv[4]);
    const float window_right = std::stof(argv[5]);
    const float window_bottom = std::stof(argv[6]);
    const float window_top = std::stof(argv[7]);
    const string str_equalize = argv[8];
    const string mode_typ = argv[9];

    Mode style_typ;
    if (mode_typ == "dark")
      style_typ = DARK;
    else if (mode_typ == "super_dark")
      style_typ = SUPER_DARK;
    else if (mode_typ == "light")
      style_typ = LIGHT;

    EqualizeType equalize;
    if (str_equalize == "x")
      equalize = EqualizeXAxes;
    else if (str_equalize == "y")
      equalize = EqualizeYAxes;
    else if (str_equalize == "auto")
      equalize = EqualizeAuto;
    else
      equalize = EqualizeNone;

    auto start = std::chrono::system_clock::now();
    GraphSVG g(filename, resolution_height, resolution_width, window_left, window_right, window_bottom, window_top, equalize, Style(style_typ));
    int i = 10;
    while (i < argc)
    {
      map<string, int> vars;
      NodePtr p = parse(string(argv[i]), vars);
      assert(vars.size() > 0);
      const string color = argv[++i];
      float stroke_width = std::stof(string(argv[++i]));
      i++;
      vector<decimal> vars2(vars.size(), 0);
      g.drawFn(p, 0, vars2, resolution_width, color, stroke_width);
    }
    g.save();
    auto end = std::chrono::system_clock::now();
    cout << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms" << endl;
  }
  else
  {
    // input 2e -> 2 * e : mal ergaenzen
    // Schreibweise "sin 4" anstatt "sin(4)" testen

    // Wann lohnt es sich eine Referenz / den Wert zu übergeben? -> double ???
    // for-schleife auto oder size_t

    // benchmark: "-(-4-5*x--2)+7*(3-2)+2*sin(0.2)*log(4,5)(x+1)" parse, simplify, derive, simplify ~ 60.000ns

    // double result;
    // if (brents_fun([](double x) { return (x * x * x - 3 * x * x + 2 * x); }, -1, 3, 1e-8, 1e4, result))
    //   cout << "success: " << result;
    // else
    //   cout << "failed";
    // cout << endl;

    // vector<double> v11;
    // cout << find_all_roots([](double x) { return (x * x * x - 3 * x * x + 2 * x); }, -1, 3, 0.01, v11) << endl;
    // for (auto &e : v11)
    //   cout << e << endl;
    // cout << endl;

    // Complex aa(5, 3); // = 5 + 3i
    // Complex b(1, -1); // = 1 - i
    // pair<Complex, Complex> z1 = sqrt(aa + Complex(1, -1));
    // cout << z1 << endl;
    // cout << ln(aa + Complex(1, -1)) << endl;
    // cout << cbrt(aa + Complex(1, -1)) << endl;
    // cout << n_root(aa + Complex(1, -1), 3) << endl;
    // cout << aa / 3 << endl;
    // for (auto it : quart(1, 2, 3, 4, 5))
    //   cout << it << " ";
    // cout << endl;

    string input;

    // input = "-2(-x+1)x(x-1)(4x)*zy+variableX7";
    // input = "-1+7*(3-2)+2*sin(0.2)*log(4,5)";
    // input = "-(-4-5*x--2)+7*(3-2)+2*sin(0.2)*log(4,5)*(x-1)"; //(x+1)";
    // input = "x^7";
    // input = "x / sin(x)";
    // input = "log(2,x)";
    // input = "ln(x) / ln(2)";
    // input = "ln(x)";
    // input = "ln(2)";
    // input = "2*e*pi";
    // input = "x^(x^2)";
    // input = "(x^2)^3)";
    // input = "3x(x+1)";
    // input = "3x";
    // input = "sin(x) * cos(x)";
    // input = "tan(x^2)";
    // input = "log(2, 1 - 3 * x)";
    // input = "sqrt(2^x)";
    // input = "atan(sqrt(e^x))";

    // input = "atan(1 / x)"; // '= -1/(1+x^2)
    // input = "asin(x - 1)"; // 1 / sqrt(2x - x^2)
    // input = "(1 / a) * atan(x / a)"; // 1 / (a^2 + x^2)
    // input = "atan((x + 1) / (x - 1))"; // -1/(1+x^2)
    // input = "atan(x - sqrt(1 + x ^ 2))"; // 1/(2(1+x^2))
    // input = "arccos(x) * arctan(x)"; // acos(x)/(1+x^2) - atan(x)/sqrt(1-x^2)
    // input = "asin(sqrt(1 - x ^ 2))"; // -x/(abs(x)sqrt(1-x^2)) -> x / abs(x) = sign(x)
    // input = "atan(sqrt(e^x))"; // sqrt(e^x)/(2(1+e^x))
    // input = "asin(1 / sqrt(x))"; // -1/(2x*sqrt(x-1))
    // input = "x * asin(x) + sqrt(1 - x^2)"; // asin(x)
    // error, should be asin(x)

    // input = "asin((1 - x^2) / (1 + x^2)";
    // input = "tan(ln(x))";
    // input = "sin(x) * sin(x)+3";
    // input = "sin(-(4+2x))";
    // input = "sin(acos(x))";
    // input = "acos(sin(x))";
    input = "sin(x)";
    // input = "(2x) / (1 + x^2)"; // 2x / (...) ???

    map<string, int> vars;
    NodePtr f = parse(input, vars);
    NodePtr fs = simplify(f);
    NodePtr d = derive(fs, vars.find("x")->second);
    NodePtr s1 = simplify(d);

    decimal val = 2.7;
    vector<decimal> vars2(vars.size(), 0);
    for (auto &item : vars2)
      item = val;
    cout << "input: '" << input << "'" << endl
         << "f(x) = " << f << endl
         << "     = " << fs << " (simplified)" << endl
         << "f'(x) = " << d << endl
         << "      = " << s1 << " (simplified)" << endl
         << "f(" << val << ") = " << eval(fs, vars2) << endl
         << "f'(" << val << ") = " << eval(d, vars2) << endl
         << "f limits: " << getLimits(f) << endl // geht noch net
         << "inverse f: " << inverse_var(f, 0) << endl
         << "Achensymmetrisch: " << ((axis_symmetic(f, 0)) ? "yes" : "no / maybe") << endl
         << "Punktsymmetrisch: " << ((point_symmetic(f, 0)) ? "yes" : "no / maybe") << endl;

    // GraphPNG g(1920, 1080, -0.5, 10, -0.5, 10);
    // GraphPNG g(1920, 1080, -3, 10, -3, 10);
    // GraphPNG g(1920, 1080, -3, 10);
    // cout << "graph 1: " << benchmark([]() -> void { // ~271 ms
    //   map<string, int> vars;
    //   vector<decimal> vars2(1, 0);

    //   GraphPNG g(1920, 1080, -3, 10, -1.5, 6);
    //   g.drawAxis();
    //   g.drawFunction(parse("x + 1", vars), 0, vars2, BLUE);
    //   g.drawFunction(parse("sin(x)", vars), 0, vars2, RED);
    //   g.drawFunction(parse("cos(x)", vars), 0, vars2, GREEN);
    //   g.drawFunction(parse("tan(x)", vars), 0, vars2, YELLOW);
    //   g.drawFunction(parse("x^2", vars), 0, vars2, CYAN);
    //   g.save("graph.png");
    // }) << "ms"
    //      << endl;

    cout << "graph 1: " << benchmark([]() -> void { // ~20 ms
      map<string, int> vars;
      vector<decimal> vars2(1, 0);

      GraphSVG g("example.svg", 1920, 1080, -6.2f, 10.0f, -8.5f, 10.35f, EqualizeYAxes, Style(SUPER_DARK));
      // GraphSVG g("example.svg", 1920, 1080, -10.2f, -6.0f, 8.5f, 10.35f, EqualizeNone);
      // GraphSVG g("example.svg", 1920, 1080, -10.2f, -6.0f, -10.5f, -8.35f, EqualizeNone);
      g.drawFn(parse("x + 1", vars), 0, vars2, 1920, "#00f", 2.0f);
      g.drawFn(parse("-x", vars), 0, vars2, 1920, "#f0f", 2.0f);
      g.drawFn(parse("sin(x)", vars), 0, vars2, 1920, "#f00", 2.0f);
      g.drawFn(parse("cos(x)", vars), 0, vars2, 1920, "#0f0", 2.0f);
      g.drawFn(parse("tan(x)", vars), 0, vars2, 1920, "#ff0", 2.0f);
      g.drawFn(parse("x^2", vars), 0, vars2, 1920, "#0ff", 2.0f);
      g.save();
    }) << "ms"
         << endl;
  }
};

// ln(|x|) -> abs, ! 1 / x
// https://www.dummies.com/wp-content/uploads/323191.image0.png
