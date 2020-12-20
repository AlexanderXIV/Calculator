#include <bits/stdc++.h>
using namespace std;

#define watch(x) cout << #x << " is " << x << endl

typedef long double decimal;
typedef long long int number;
typedef unsigned long long int u_number;
const decimal decimal_epsilon = numeric_limits<decimal>::epsilon();

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
  TokenLog,
  TokenLn,
  TokenVar,
  TokenOpen,
  TokenClose,
  TokenValue,
  TokenComma
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

  Token(const Token &t)
  {
    kind = t.kind;
    if (t.kind == TokenValue)
      value = t.value;
    else if (t.kind == TokenVar)
      name = t.name;
  };
  ~Token(){};
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
  if (str == "sin")
    return Token(TokenSin);
  if (str == "cos")
    return Token(TokenCos);
  if (str == "tan")
    return Token(TokenTan);
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
  NodeLog,
  NodeCon
};

struct Node
{
  NodeKind kind;
  // Optimierung: union
  decimal value;

  int varId;
  string varName;

  // log(a, b) -> ln(4) == log(e, 4)
  unique_ptr<Node> a /* base */, b /* value */;
  bool a_var, b_var; // true if in sub-tree a / b variable is used

  friend ostream &operator<<(ostream &os, const Node &t);

  Node(NodeKind kind1, decimal val) : kind(kind1), value(val), a_var(false), b_var(false){};
  Node(int vardId1, string varName1) : kind(NodeVar), varId(vardId1), varName(varName1), a_var(false), b_var(false){};
  Node(NodeKind kind1, unique_ptr<Node> a1) : kind(kind1), a(move(a1)), a_var(false), b_var(false){};
  Node(NodeKind kind1, unique_ptr<Node> a1, unique_ptr<Node> b1) : kind(kind1), a(move(a1)), b(move(b1)), a_var(false), b_var(false){};
};

typedef unique_ptr<Node> NodePtr;

inline decimal rad(decimal val) { return val * M_PI / 180; }
inline decimal deg(decimal val) { return val * 180 / M_PI; }

NodePtr to_node(decimal value) { return make_unique<Node>(NodeValue, value); }
NodePtr const_e() { return make_unique<Node>(NodeCon, M_E); }
NodePtr const_pi() { return make_unique<Node>(NodeCon, M_PI); }

NodePtr operator+(NodePtr a, NodePtr b) { return make_unique<Node>(Node(NodeAdd, move(a), move(b))); }
NodePtr operator-(NodePtr a, NodePtr b) { return make_unique<Node>(Node(NodeSub, move(a), move(b))); }
NodePtr operator*(NodePtr a, NodePtr b) { return make_unique<Node>(Node(NodeMul, move(a), move(b))); }
NodePtr operator/(NodePtr a, NodePtr b) { return make_unique<Node>(Node(NodeDiv, move(a), move(b))); }
NodePtr operator^(NodePtr a, NodePtr b) { return make_unique<Node>(Node(NodePow, move(a), move(b))); }
NodePtr variable(int varId, string varName) { return make_unique<Node>(varId, varName); }
NodePtr sin(NodePtr a) { return make_unique<Node>(NodeSin, move(a)); }
NodePtr cos(NodePtr a) { return make_unique<Node>(NodeCos, move(a)); }
NodePtr tan(NodePtr a) { return make_unique<Node>(NodeTan, move(a)); }
NodePtr log(NodePtr a, NodePtr b) { return make_unique<Node>(NodeLog, move(a), move(b)); }
// NodePtr log(NodePtr a) { return make_unique<Node>(NodeLog, a, 10); }
NodePtr ln(NodePtr a) { return make_unique<Node>(NodeLog, const_e(), move(a)); }
// NodePtr sqrt(NodePtr a) { return make_unique<Node>(NodePow, a, 0.5); }

bool operator==(const NodePtr &a, const decimal &f) { return a.get()->kind == NodeValue && a.get()->value == f; }

bool operator==(const decimal &f, const NodePtr &a) { return a.get()->kind == NodeValue && a.get()->value == f; }

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
    return a.get()->a == b.get()->a;
  }
  assert(0 && "error");
  return false;
}

inline bool is_const_e(decimal val) { return fabs(val - M_E) <= decimal_epsilon; }
inline bool is_const_pi(decimal val) { return fabs(val - M_PI) <= decimal_epsilon; }

ostream &stringify(ostream &os, const Node &t, int level)
{
  level = 10;
  switch (t.kind)
  {
  case NodeCon:
    if (is_const_e(t.value))
      os << "e";
    else if (is_const_pi(t.value))
      os << "pi";
    else
      assert(0 && "unknown Constant");
    break;
  case NodeValue:
    if (t.value < 0)
      os << "(";

    if (t.value == floor(t.value))
      os << floor(t.value);
    else
      os << t.value;

    if (t.value < 0)
      os << ")";
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
  case NodeMul:
    if (level > 2)
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
    else if (t.b.get()->kind == NodeVar && t.a.get()->kind != NodeVar)
    {
      stringify(os, *t.a.get(), 1);
      stringify(os, *t.b.get(), 1);
    }
    else if (t.a.get()->kind == NodeVar && t.b.get()->kind != NodeVar)
    {
      stringify(os, *t.b.get(), 1);
      stringify(os, *t.a.get(), 1);
    }
    else if (t.a.get()->kind == NodeValue && (t.b.get()->kind == NodeCon || t.b.get()->kind == NodeSin || t.b.get()->kind == NodeCos || t.b.get()->kind == NodeLog))
    {
      stringify(os, *t.a.get(), 1);
      stringify(os, *t.b.get(), 1);
    }
    else if (t.b.get()->kind == NodeValue && (t.a.get()->kind == NodeCon || t.a.get()->kind == NodeSin || t.a.get()->kind == NodeCos || t.a.get()->kind == NodeLog))
    {
      stringify(os, *t.b.get(), 1);
      stringify(os, *t.a.get(), 1);
    }
    else if ((t.a.get()->kind == NodeCon || t.a.get()->kind == NodeSin || t.a.get()->kind == NodeCos || t.a.get()->kind == NodeLog) && (t.b.get()->kind == NodeCon || t.b.get()->kind == NodeSin || t.b.get()->kind == NodeCos || t.b.get()->kind == NodeLog))
    {
      stringify(os, *t.a.get(), 1);
      stringify(os, *t.b.get(), 1);
    }
    else
    {
      stringify(os, *t.a.get(), 2);
      os << " * ";
      stringify(os, *t.b.get(), 2);
    }

    if (level > 2)
      os << ")";
    break;
  case NodeDiv:
    if (level > 2)
      os << "(";

    stringify(os, *t.a.get(), 2);
    os << " / ";
    stringify(os, *t.b.get(), 2);

    if (level > 2)
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
  case NodeLog:
    if (level > 2)
      os << "(";

    if (is_const_e(t.a.get()->value))
    {
      os << "ln(";
      stringify(os, *t.b.get(), 1);
      os << ")";
    }
    else
    {
      os << "log(";
      stringify(os, *t.a.get(), 1);
      os << ", ";
      stringify(os, *t.b.get(), 1);
      os << ")";
    }

    if (level > 2)
      os << ")";

    break;
  case NodePow:
    if (level > 2)
      os << "(";

    stringify(os, *t.a.get(), 3);
    os << " ^ ";
    stringify(os, *t.b.get(), 3);

    if (level > 2)
      os << ")";
    break;
  case NodeVar:
    os << t.varName;
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
    result = make_unique<Node>(NodeLog, const_e(), parse(iter, vars, false));
    assert(iter.take(TokenClose) && "failed parsing");
  }
  else if (iter.take(TokenSin))
  {
    assert(iter.take(TokenOpen) && "failed parsing");
    result = make_unique<Node>(NodeSin, parse(iter, vars, false));
    assert(iter.take(TokenClose) && "failed parsing");
  }
  else if (iter.take(TokenCos))
  {
    assert(iter.take(TokenOpen) && "failed parsing");
    result = make_unique<Node>(NodeCos, parse(iter, vars, false));
    assert(iter.take(TokenClose) && "failed parsing");
  }
  else if (iter.take(TokenTan))
  {
    assert(iter.take(TokenOpen) && "failed parsing");
    result = make_unique<Node>(NodeTan, parse(iter, vars, false));
    assert(iter.take(TokenClose) && "failed parsing");
  }
  else if (iter.take(TokenLog))
  {
    assert(iter.take(TokenOpen) && "failed parsing");
    NodePtr tmp = parse(iter, vars, false);
    assert(iter.take(TokenComma) && "failed parsing");
    result = make_unique<Node>(NodeLog, move(tmp), parse(iter, vars, false));
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
        result = make_unique<Node>(it->second, name);
      else
      {
        size_t index = vars.size();
        vars.emplace(name, index);
        result = make_unique<Node>(index, name);
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
    result = make_unique<Node>(NodeMul, to_node(-1), move(result));

  while (iter.take(TokenAdd) || iter.take(TokenSub) || iter.take(TokenMul) || iter.take(TokenDiv) || iter.take(TokenPot))
  {
    TokenKind kind = iter.token().kind;
    int lv = operators(kind);
    if (lv <= level)
    {
      iter.back();
      return result;
    }

    NodeKind nk;
    if (kind == TokenAdd)
      nk = NodeAdd;
    else if (kind == TokenSub)
      nk = NodeSub;
    else if (kind == TokenMul)
      nk = NodeMul;
    else if (kind == TokenDiv)
      nk = NodeDiv;
    else
      nk = NodePow;

    result = make_unique<Node>(nk, move(result), parse(iter, vars, false, lv));
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

bool preprocessing(const NodePtr &ptr)
{
  switch (ptr.get()->kind)
  {
  case NodeVar:
    return true;
  case NodeValue:
  case NodeCon:
    return false;
  case NodeAdd:
  case NodeSub:
  case NodeMul:
  case NodeDiv:
  case NodePow:
  case NodeLog:
    ptr.get()->a_var = preprocessing(ptr.get()->a);
    ptr.get()->b_var = preprocessing(ptr.get()->b);
    return ptr.get()->a_var || ptr.get()->b_var;
  case NodeSin:
  case NodeCos:
  case NodeTan:
    ptr.get()->a_var = preprocessing(ptr.get()->a);
    return ptr.get()->a_var;
  }
  assert(0 && "error");
  return false;
}

// derive and (partly) simplify a function
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
    {
      if (node->b_var)
        return (node->a.get()->kind == NodeCon && is_const_e(node->a.get()->value)) ? derive(node->b, varID) / copy(node->b) : derive(node->b, varID) / (copy(node->b) * ln(copy(node->a)));
      else
        return to_node(0);
    }
  }
  case NodeVar:
    return to_node(node->varId == varID);
  }
  assert(0 && "error");
  return nullptr;
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
    return sin(simplify(node->a));
  case NodeCos:
    return cos(simplify(node->a));
  case NodeTan:
    return tan(simplify(node->a));
  case NodeLog:
  {
    NodePtr a = simplify(node->a); // base
    NodePtr b = simplify(node->b); // value
    if (b == 0)
      assert(0 && "log undefines for this value");
    else if (b == 1)
      return to_node(0);
    else if (a == b)
      return to_node(1);
    return log(move(a), move(b));
  }
  case NodeMul:
  {
    NodePtr a = simplify(node->a);
    NodePtr b = simplify(node->b);

    if ((a == 0) || (b == 0))
      return to_node(0);
    else if (a == 1)
      return b;
    else if (b == 1)
      return a;
    else if (a.get()->kind == NodeValue && b.get()->kind == NodeValue)
      return to_node(a.get()->value * b.get()->value);
    return move(a) * move(b);
  }
  case NodePow:
  {
    NodePtr a = simplify(node->a);
    NodePtr b = simplify(node->b);
    if (a.get()->kind == NodeValue)
    {
      if (a.get()->value == 0)
        return to_node(0);
      else if (a.get()->value == 1)
        return to_node(1);
    }
    if (b.get()->kind == NodeValue)
    {
      if (b.get()->value == 0)
        return to_node(1);
      else if (b.get()->value == 1)
        return a;
    }
    if (a.get()->kind == NodePow)
      return simplify(move(a.get()->a) ^ move(a.get()->b) * move(b));
    return move(a) ^ move(b);
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
    if (a.get()->kind == NodeValue && b.get()->kind == NodeValue)
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
#pragma endregion

// input 2e -> 2 * e : mal ergaenzen
// Schreibweise "sin 4" anstatt "sin(4)" testen
// Operatorenschreibweise

int main()
{
  ios::sync_with_stdio(false);
  cin.tie(0);
  cout << setprecision(15);

  // Wann lohnt es sich eine Referenz / den Wert zu übergeben?
  // for-schleife auto oder size_t

  // benchmark: "-(-4-5*x--2)+7*(3-2)+2*sin(0.2)*log(4,5)(x+1)" parse, simplify, derive, simplify ~ 60.000ns
  // auto start = chrono::system_clock::now();
  // auto end = chrono::system_clock::now();
  // auto t = chrono::duration_cast<chrono::nanoseconds>(end - start);
  // cout << t.count() << "ns" << endl;

  decimal val = 2.7;
  string input;
  map<string, int> vars;

  // input = "-2(-x+1)x(x-1)(4x)*zy+variableX7";
  // input = "-1+7*(3-2)+2*sin(0.2)*log(4,5)";
  // input = "-(-4-5*x--2)+7*(3-2)+2*sin(0.2)*log(4,5)"; //(x+1)";
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
  input = "log(2, (1 - 3 * x))";

  NodePtr f = parse(input, vars);
  NodePtr fs = simplify(f);
  preprocessing(fs);
  NodePtr d = derive(fs, vars.find("x")->second);
  NodePtr s1 = simplify(d);

  vector<decimal> vars2(vars.size(), 0);
  for (auto &item : vars2)
    item = val;
  cout << "input: '" << input << "'" << endl
       << "f(x) = " << f << endl
       << "     = " << fs << " (simplified)" << endl
       << "f'(x) = " << d << endl
       << "      = " << s1 << " (simplified)" << endl
       << "f(" << val << ") = " << eval(fs, vars2) << endl
       << "f'(" << val << ") = " << eval(d, vars2) << endl;
};
