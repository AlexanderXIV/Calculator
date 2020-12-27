#include <bits/stdc++.h>
#include <png++/png.hpp>
using namespace std;

#define watch(x) cout << #x << " is " << x << endl

#if defined __linux__ || defined __APPLE__
#else
#define M_PI 3.141592653589793
#define INFINITY 1e8
#endif

typedef long double decimal;
typedef long long int number;
typedef unsigned long long int u_number;
const decimal decimal_epsilon = numeric_limits<decimal>::epsilon();

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
  // union
  // {
  decimal value;
  string name;
  // };

  Token(TokenKind k) : kind(k){};
  Token(TokenKind k, decimal val) : kind(TokenValue), value(val){};
  Token(TokenKind k, string str) : kind(TokenVar), name(str){};

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
    return Token(TokenVar, stof(str));
  return Token(TokenValue, str);
}

vector<Token> tokenize(string str)
{
  vector<Token> result;
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
  return result;
}
#pragma endregion

#pragma region Calculator
#define ACCURACY_STANDARD false

const decimal standard_accuracy = 10e-10;
const int sin_lookup_max = 10;
const decimal pi_half = M_PI / 2;
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
  decimal v = 2 * my_faculty(2 * n) / (decimal)pow(2 * M_PI, 2 * n);
  if (!(n & 1))
    v = -v;
  decimal res = 0;
  for (auto i = 1; i < 30; ++i)
    res += 1 / (decimal)pow(i, 2 * n);
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
  return pi_half - my_arcsin(x);
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

typedef unique_ptr<Node> NodePtr;

struct Node
{
  NodeKind kind;
  // Optimierung: union
  decimal value;

  int varId;
  string varName;

  // log(a, b) -> ln(4) == log(e, 4)
  NodePtr a /* base */, b /* value */;
  bool a_var, b_var; // true if in sub-tree a or b variable is used

  friend ostream &operator<<(ostream &os, const Node &t);

  Node(NodeKind kind1, decimal val) : kind(kind1), value(val), a_var(false), b_var(false){};
  Node(int vardId1, string varName1) : kind(NodeVar), varId(vardId1), varName(varName1), a_var(true), b_var(true){};
  Node(NodeKind kind1, NodePtr a1) : kind(kind1), a(move(a1)), a_var(a.get()->a_var || a.get()->b_var), b_var(false){};
  Node(NodeKind kind1, NodePtr a1, NodePtr b1) : kind(kind1), a(move(a1)), b(move(b1)), a_var(a.get()->a_var || a.get()->b_var), b_var(b.get()->a_var || b.get()->b_var){};
};

inline decimal rad(decimal val) { return val * M_PI / 180; }
inline decimal deg(decimal val) { return val * 180 / M_PI; }

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

inline bool is_const_e(decimal val) { return fabs(val - M_E) <= decimal_epsilon; }

inline bool is_const_pi(decimal val) { return fabs(val - M_PI) <= decimal_epsilon; }

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

  TokenIter(vector<Token> tokens1, size_t cur1) : cur(cur1), tokens(tokens1){};

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
        result = variable(index, name);
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
  TokenIter iter = TokenIter(tokenize(str), 0);
  return parse(iter, vars, false);
}

decimal eval(const NodePtr &ptr_node, const vector<decimal> &vars)
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
    return pow(eval(node->a, vars), eval(node->b, vars));
  case NodeSin:
    return sin(eval(node->a, vars));
  case NodeCos:
    return cos(eval(node->a, vars));
  case NodeTan:
    return tan(eval(node->a, vars));
  case NodeAsin:
    return asin(eval(node->a, vars));
  case NodeAcos:
    return acos(eval(node->a, vars));
  case NodeAtan:
    return atan(eval(node->a, vars));
  case NodeLn:
    return log(eval(node->a, vars));
  case NodeLog:
    return log(eval(node->b, vars)) / log(eval(node->a, vars));
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
NodePtr derive(const NodePtr &ptr_node, int varID) // varID according to which is derived
{
  Node *node = ptr_node.get();
  switch (node->kind)
  {
  case NodeValue:
  case NodeCon:
    return to_node(0);
  case NodeAdd:
    if (node->a_var)
      return (node->b_var) ? derive(node->a, varID) + derive(node->b, varID) : derive(node->a, varID);
    else
      return (node->b_var) ? derive(node->b, varID) : to_node(0);
  case NodeSub:
    if (node->a_var)
      return (node->b_var) ? derive(node->a, varID) - derive(node->b, varID) : derive(node->a, varID);
    else
      return (node->b_var) ? to_node(-1) * derive(node->b, varID) : to_node(0);
  case NodeMul:
    if (node->a_var)
      return (node->b_var) ? copy(node->a) * derive(node->b, varID) + derive(node->a, varID) * copy(node->b) : derive(node->a, varID) * copy(node->b);
    else
      return (node->b_var) ? copy(node->a) * derive(node->b, varID) : to_node(0);
  case NodeDiv:
    if (node->a_var)
      return (node->b_var) ? (copy(node->b) * derive(node->a, varID) - derive(node->b, varID) * copy(node->a)) / (copy(node->b) ^ to_node(2)) : derive(node->a, varID) / copy(node->b);
    else
      return (node->b_var) ? (to_node(-1) * derive(node->b, varID) * copy(node->a)) / (copy(node->b) ^ to_node(2)) : to_node(0);
  case NodePow:
    if (node->a_var)
      return (node->b_var) ? (ln(copy(node->a)) * derive(node->b, varID) + copy(node->b) / copy(node->a) * derive(node->a, varID)) * copy(ptr_node) : copy(node->b) * (copy(node->a) ^ (copy(node->b) - to_node(1)));
    else
      return (node->b_var) ? ln(copy(node->a)) * derive(node->b, varID) * copy(ptr_node) : to_node(0);
  case NodeSin:
    return (node->a_var) ? cos(copy(node->a)) * derive(node->a, varID) : to_node(0);
  case NodeCos:
    return (node->a_var) ? to_node(-1) * sin(copy(node->a)) * derive(node->a, varID) : to_node(0);
  case NodeTan:
    return (node->a_var) ? derive(node->a, varID) * ((tan(copy(node->a)) ^ to_node(2)) + to_node(1)) : to_node(0);
  case NodeAsin:
    return derive(node->a, varID) / sqrt(to_node(1) - (copy(node->a) ^ to_node(2)));
  case NodeAcos:
    return (to_node(-1) * derive(node->a, varID)) / sqrt(to_node(1) - (copy(node->a) ^ to_node(2)));
  case NodeAtan:
    return derive(node->a, varID) / (to_node(1) + (copy(node->a) ^ to_node(2)));
  case NodeLog: // Annahme : log a(b) == log(a, b)
  {
    if (node->a_var)
    {
      if (node->b_var)
        return (ln(copy(node->a)) * (derive(node->b, varID) / copy(node->b)) - (derive(node->a, varID) / copy(node->a)) * ln(copy(node->b))) / (ln(copy(node->a)) ^ to_node(2));
      else
        return (to_node(-1) * (derive(node->a, varID) / copy(node->a)) * ln(copy(node->b))) / (ln(copy(node->a)) ^ to_node(2));
    }
    else
      return (node->b_var) ? derive(node->b, varID) / (copy(node->b) * ln(copy(node->a))) : to_node(0);
  }
  case NodeLn:
    return (node->a_var) ? derive(node->a, varID) / copy(node->a) : to_node(0);
  case NodeVar:
    return to_node(node->varId == varID);
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

NodePtr unflatten(vector<NodePtr> terms, NodeKind node_kind)
{
  NodePtr cur = move(terms[0]);

  for (size_t i = 1; i < terms.size(); ++i)
    if (node_kind == NodeAdd || node_kind == NodeSub || node_kind == NodeMul || node_kind == NodeDiv || node_kind == NodePow)
      cur = make_unique<Node>(node_kind, move(cur), move(terms[i]));
  return cur;
}

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
    // if (a == NodeAsin)
    //   return move(a.get()->a);
    return sin(move(a));
  }
  case NodeCos:
    return cos(simplify(node->a));
  case NodeTan:
    return tan(simplify(node->a));
  case NodeAsin:
    return asin(simplify(node->a));
  case NodeAcos:
  {
    NodePtr a = simplify(node->a);
    // if (a == NodeCos)
    //   return move(a.get()->a);
    return acos(move(a));
  }
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

int digits_before_dot(decimal a) { return (a < 10) ? 1 : (int)log10(a); }
int digits(decimal a) { return abs((int)log10(a)); }

// could be more efficient
NodePtr stern_brocot_tree(decimal x, const int limit = 15, const decimal accuracy = 10e-10)
{
  number a, b, n, ap0 = 1, ap1 = 0, bp0 = 0, bp1 = 1, nn = (number)x;
  x -= (number)x;
  decimal original = x;
  for (number i = 0; i < limit; ++i)
  {
    n = (number)floor(x);
    x = 1 / (x - n);

    a = ap0 + n * ap1;
    b = bp0 + n * bp1;
    ap0 = ap1;
    bp0 = bp1;
    ap1 = a;
    bp1 = b;

    if (fabs((decimal)b / a - original) < accuracy)
      break;
  }
  return to_node(nn * a + b) / to_node(a);
  // return to_node(nn) * to_node(a) + to_node(b) / to_node(a);
}

decimal deriveAt(const NodePtr &ptr_node, vector<decimal> &vars, int varId, const int accuracy = 8)
{
  decimal num1 = eval(ptr_node, vars);
  vars[varId] += 1e-8;
  return 1e8 * (eval(ptr_node, vars) - num1);
}

struct range
{
  decimal min_val, max_val;
  range(decimal l1, decimal u1) : min_val(l1), max_val(u1){}; // l1, u1 inclusive
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
    if (abs(a.max_val - a.min_val) < 2 * M_PI)
    {
      int s1 = (int)(a.min_val / (2 * M_PI));
      if (a.min_val < 0)
        --s1;
      a.min_val -= s1 * 2 * M_PI;
      a.max_val -= s1 * 2 * M_PI;

      if (a.min_val < 0.5 * M_PI)
      {
        if (a.max_val < 0.5 * M_PI)
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
    if (abs(a.max_val - a.min_val) < 2 * M_PI)
    {
      int s1 = (int)(a.min_val / (2 * M_PI));
      if (a.min_val < 0)
        --s1;
      a.min_val -= s1 * 2 * M_PI;
      a.max_val -= s1 * 2 * M_PI;

      if (a.min_val < M_PI)
      {
        if (a.max_val < M_PI)
          return range(cos(a.max_val), cos(a.min_val));
        else if (a.max_val < 2 * M_PI)
          return range(-1, max(cos(a.max_val), cos(a.min_val)));
      }
      else
      {
        if (a.max_val < 2 * M_PI)
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

      if (a.min_val > 0.5 * M_PI || (a.min_val < 0.5 * M_PI && a.max_val < 0.5 * M_PI))
        return range(atan(a.min_val), atan(a.max_val));
      else if (a.min_val < 0.5 * M_PI && a.max_val > 0.5 * M_PI)
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
      return range(asin(a.min_val), M_PI / 2);
    else if (b1 && -1 <= a.max_val)
      return range(-M_PI / 2, asin(a.max_val));
    else if (!(a1 || b1))
      return range(-M_PI / 2, M_PI / 2);
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
    return range(ptr_node.get()->value, ptr_node.get()->value);
  case NodeCon:
    return range(ptr_node.get()->value, ptr_node.get()->value);
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
#pragma endregion

// input 2e -> 2 * e : mal ergaenzen
// Schreibweise "sin 4" anstatt "sin(4)" testen

#pragma region Visualisation
void WuDrawLine(png::image<png::rgb_pixel_16> &img, const png::rgb_pixel_16 &col, int center_x, int center_y, float x0, float y0, float x1, float y1)
{
  assert(x0 <= x1);
  assert(y0 <= y1);

  int w = 1910, h = 1070;
  y0 = img.get_height() - y0;
  y1 = img.get_height() - y1;

  center_x = 500;
  center_y = 500;

  // cout << x0 << " " << y0 << "\t\t" << x1 << " " << y1 << endl;

  x0 = x0 + center_x;
  x1 = x1 + center_x;

  y0 = y0 - center_y;
  y1 = y1 - center_y;

  if (x0 == x1 && (x0 < 0 || x0 > w))
    return;

  // cout << x0 << " " << y0 << "\t\t" << x1 << " " << y1 << endl;

  if (x0 < 0)
  {
    y0 = y0 - x0 * ((y1 - y0) / (x1 - x0));
    x0 = 0;
  }
  else if (x0 > w)
    return;

  // cout << x0 << " " << y0 << "\t\t" << x1 << " " << y1 << endl;

  if (x1 < 0)
    return;
  else if (x1 > w)
  {
    float a = (y1 - y0) / (x1 - x0);
    y1 = a * w + y0 - x0 * a;
    x1 = w;
  }

  if (y0 < 0)
    return;
  else if (x1 > w)
  {
    float a = (y1 - y0) / (x1 - x0);
    y1 = a * w + y0 - x0 * a;
    x1 = w;
  }

  // cout << x0 << " " << y0 << "\t\t" << x1 << " " << y1 << endl;

  auto plot = [](png::image<png::rgb_pixel_16> &img, const png::rgb_pixel_16 &col, int x, int y, float brightness) -> void { img.set_pixel(x, y, png::rgb_pixel_16(col.red * brightness, col.green * brightness, col.blue * brightness)); };
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
      plot(img, col, ypx11, xpx11, rfpart(yend) * xgap);
      plot(img, col, ypx11 + 1, xpx11, fpart(yend) * xgap);
    }
    else
    {
      plot(img, col, xpx11, ypx11, rfpart(yend) * xgap);
      plot(img, col, xpx11, ypx11 + 1, fpart(yend) * xgap);
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
      plot(img, col, ypx12, xpx12, rfpart(yend) * xgap);
      plot(img, col, ypx12 + 1, xpx12, fpart(yend) * xgap);
    }
    else
    {
      plot(img, col, xpx12, ypx12, rfpart(yend) * xgap);
      plot(img, col, xpx12, ypx12 + 1, fpart(yend) * xgap);
    }
  }

  if (steep)
    for (int x = xpx11 + 1; x < xpx12; x++)
    {
      plot(img, col, ipart(intery), x, rfpart(intery));
      plot(img, col, ipart(intery) + 1, x, fpart(intery));
      intery += gradient;
    }
  else
    for (int x = xpx11 + 1; x < xpx12; x++)
    {
      plot(img, col, x, ipart(intery), rfpart(intery));
      plot(img, col, x, ipart(intery) + 1, fpart(intery));
      intery += gradient;
    }
}

struct Graph
{
  int width, height;
  float window_left, window_right, window_top, window_bottom;
  png::image<png::rgb_pixel_16> img;

  Graph(int width1, int height1, float window_left1, float window_right1, float window_bottom1, float window_top1) : img(width, height), width(width1), height(height1), window_left(window_left1),
                                                                                                                     window_right(window_right1), window_top(window_top1), window_bottom(window_bottom1)
  {
    assert(width > 100 && "min. resolution");
    assert(height > 100 && "min. resolution");
    assert(1 < window_right - window_left && "min. window size");
    assert(1 < window_top - window_bottom && "min. window size");
  }

  void drawFunction(const NodePtr &f, const png::rgb_pixel_16 &col = png::rgb_pixel_16((uint16_t)(UINT16_MAX), (uint16_t)(UINT16_MAX), (uint16_t)(UINT16_MAX)))
  {
    float w = (float)width, h = (float)height;

    float scale_tb = window_top - window_bottom;
    float scale_lr = window_right - window_left;
    float coor_x = w - (w - 1) * window_top / scale_tb;
    float coor_y = (h - 1) * window_right / scale_lr;

    WuDrawLine(img, col, 0, 0, 0, -100, 0, 100);
    WuDrawLine(img, col, 0, 0, -100, 0, 100, 0);
    // if (window_left > 0)
    //   WuDrawLine(img, col, coor_x, 0, coor_x, h - 1);
    // else if (window_right < 0)
    //   WuDrawLine(img, col, coor_x, 0, coor_x, h - 1);
    // else
    //   WuDrawLine(img, col, coor_x, 0, coor_x, h - 1);
    // WuDrawLine(img, col, 0, coor_y, w - 1, coor_y);
    // for (size_t i = 0; i < scale_lr - 1; i++)
    // {
    //   float h = 90 * i;
    //   WuDrawLine(img, col, coor_x - 10, h, coor_x + 10, h);
    // }
    // for (size_t i = 0; i < scale_tb - 1; i++)
    // {
    //   float h = 180 * i;
    //   WuDrawLine(img, col, h, coor_y - 10, h, coor_y + 10);
    // }

    // vector<decimal> vars22(1, 0);
    // vars22[0] = window_left;
    // float fac1 = height / (1.2 * scale_tb);
    // float r1 = coor_y - fac1 * eval(f, vars22), r2;
    // for (int x = 1; x < width - 1; ++x)
    // {
    //   vars22[0] = window_left + (scale_tb * (float)x) / (float)width;
    //   r2 = coor_y - fac1 * eval(f, vars22);
    //   if (0 < (int)r1 && (int)r1 < height - 1 && 0 < r2 && (int)r2 < height - 1)
    //     WuDrawLine(img, col, (float)x, r1, (float)(x + 1), r2);
    //   r1 = r2;
    // }
  }

  void save(string name)
  {
    img.write(name);
  }
};
// void drawGraph(png::image<png::rgb_pixel_16> &img, png::rgb_pixel_16 col, NodePtr &f, float window_left, float window_right, float window_bottom, float window_top) {
//   int widht = img.get_width();
//   png::rgb_pixel_16 white((uint16_t)(UINT16_MAX), (uint16_t)(UINT16_MAX), (uint16_t)(UINT16_MAX));
//   png::rgb_pixel_16 red((uint16_t)(UINT16_MAX), 0, (uint16_t)(UINT16_MAX / 2));

//   float scale_tb = window_top - window_bottom;
//   float scale_lr = window_right - window_left;
//   float coor_x = (float)width - (float)(width - 1) * window_top / scale_tb;
//   float coor_y = (float)(height - 1) * window_right / scale_lr;
//   WuDrawLine(img, white, coor_x, 0, coor_x, height - 1);
//   WuDrawLine(img, white, 0, coor_y, width - 1, coor_y);
//   for (size_t i = 0; i < scale_lr - 1; i++)
//   {
//     float h = 90 * i;
//     WuDrawLine(img, white, coor_x - 10, h, coor_x + 10, h);
//   }
//   for (size_t i = 0; i < scale_tb - 1; i++)
//   {
//     float h = 180 * i;
//     WuDrawLine(img, white, h, coor_y - 10, h, coor_y + 10);
//   }

//   vector<decimal> vars22(1, 0);
//   vars22[0] = window_left;
//   float fac1 = height / (1.2 * scale_tb);
//   float r1 = coor_y - fac1 * eval(f, vars22), r2;
//   for (int x = 1; x < width - 1; ++x)
//   {
//     vars22[0] = window_left + (scale_tb * (float)x) / (float)width;
//     r2 = coor_y - fac1 * eval(f, vars22);
//     if (0 < (int)r1 && (int)r1 < height - 1 && 0 < r2 && (int)r2 < height - 1)
//       WuDrawLine(img, white, (float)x, r1, (float)(x + 1), r2);
//     r1 = r2;
//   }

//   img.write(name);
// }
#pragma endregion

int main()
{
  ios::sync_with_stdio(false);
  cin.tie(0);
  cout << setprecision(8);

  // Wann lohnt es sich eine Referenz / den Wert zu übergeben?
  // for-schleife auto oder size_t

  // benchmark: "-(-4-5*x--2)+7*(3-2)+2*sin(0.2)*log(4,5)(x+1)" parse, simplify, derive, simplify ~ 60.000ns
  // auto start = chrono::system_clock::now();
  // auto end = chrono::system_clock::now();
  // auto t = chrono::duration_cast<chrono::nanoseconds>(end - start);
  // cout << t.count() << "ns" << endl;

  // NodePtr p1 = to_node(7) + to_node(11);
  // vector<NodePtr> vc;
  // let terms = node.flatten().map(simplify)

  // var
  //     value_sum: float64 = 0
  //     rest: seq[Node] = @[]
  // for term in terms:
  //     case term.kind:
  //         of NodeValue:
  //             value_sum += term.value
  //         else:
  //             rest.add(term)
  // if value_sum != 0 or rest.len == 0:
  //     rest.add(value_sum.value())

  // return rest.unflatten(NodeAdd)
  // for (auto &item : vc)
  //   cout << item << endl;

  decimal val = 2.7;
  string input;
  map<string, int> vars;

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
  input = "sin(x) * sin(x)+3";
  // input = "sin(x)";
  // input = "(2x) / (1 + x^2)"; // 2x / (...) ???

  auto start = chrono::system_clock::now();
  NodePtr f = parse(input, vars);
  NodePtr fs = simplify(f);
  NodePtr d = derive(fs, vars.find("x")->second);
  NodePtr s1 = simplify(d);
  auto end = chrono::system_clock::now();
  auto t = chrono::duration_cast<chrono::microseconds>(end - start);
  cout << (double)t.count() / 1000 << " ms" << endl;

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
       << "f limits: " << getLimits(f) << endl;

  ios::sync_with_stdio(false);
  cin.tie(0);
  cout << setprecision(8);

  // Graph g(1920, 1080, -0.5, 10, -0.5, 10);
  Graph g(1920, 1080, 3, 10, 3, 10);
  g.drawFunction(f);
  g.save("graph.png");
};

// ln(|x|) -> abs, ! 1 / x
