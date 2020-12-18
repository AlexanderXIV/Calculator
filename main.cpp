#include <bits/stdc++.h>
using namespace std;

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
  float value;
  string name;
  // };

  Token(TokenKind k) : kind(k){};
  Token(TokenKind k, float val) : kind(TokenValue), value(val){};
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
    os << " , ";
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

bool is_float(const string &str)
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
  if (is_float(str))
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
      if (is_float(cur) && !is_float(cur + chr))
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

const double standard_accuracy = 10e-10;
const int sin_lookup_max = 10;
const double pi_half = M_PI / 2;
const long long int sin_lookup[sin_lookup_max] = {
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
const long long int cos_lookup[cos_lookup_max] = {
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
const long long int exp_lookup[exp_lookup_max] = {
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

long long int my_faculty(const long long int value)
{
  long long int result = 1;
  for (auto i = 1; i <= value; ++i)
    result *= i;
  return result;
}

double my_sin(double x, const double accuracy = standard_accuracy)
{
  double res = 0, x_sqr = x * x, new_val;
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

double my_cos(double x, const double accuracy = standard_accuracy)
{
  double res = 0, x_sqr = x * x, new_val;
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

int my_gcd(int a, int b)
{
  int t;
  while (b)
  {
    t = b;
    b = a % b;
    a = t;
  }

  return a;
}

double two_even_Bernoulli_number(int n, const double accuracy = standard_accuracy)
{
  double v = 2 * my_faculty(2 * n) / pow(2 * M_PI, 2 * n);
  if (!(n & 1))
    v = -v;
  double res = 0;
  for (auto i = 1; i < 30; ++i)
    res += 1 / pow(i, 2 * n);
  return v * res;
}

int n_k(int n, int k)
{
  assert(0 <= k && k <= n && "range");
  long long int a = 1, b = 1;

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

  return a / b;
}

int my_lcm(int a, int b)
{
  return (a * b) / my_gcd(a, b);
}

unsigned int my_fibonacci(unsigned int x)
{
  if (x == 0)
    return 0;
  if (x == 1)
    return 1;

  unsigned int res, h1 = 0, h2 = 1;
  for (unsigned int i = 1; i < x; ++i)
  {
    res = h1 + h2;
    h1 = h2;
    h2 = res;
  }

  return res;
}

double my_pow(double x, int n)
{
  double res = 1;
  while (n > 0)
  {
    if (n & 1)
      res *= x;
    n >>= 1;
    x *= x;
  }
  return res;
}

int my_pow(int x, int n)
{
  int res = 1;
  while (n > 0)
  {
    if (n & 1)
      res *= x;
    n >>= 1;
    x *= x;
  }
  return res;
}

const int asin_lookup_max = 17;
const double asin_lookup[asin_lookup_max]{
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

double my_arcsin_slow(double x, const double accuracy = standard_accuracy)
{
  double res = 0, powers2 = x, x_sqr = x * x;
  long long int powers = 1, aa = 1, bb = 1;

  for (unsigned int i = 0; i < 30; ++i)
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

double my_arcsin(double x, const double accuracy = standard_accuracy)
{
  double res = 0, powers2 = x, x_sqr = x * x;
  for (unsigned int i = 0; i < asin_lookup_max; ++i)
  {
    res += asin_lookup[i] * powers2 / (2 * i + 1);
    powers2 *= x_sqr;
  }
  return res;
}

double my_arccos(double x, const double accuracy = standard_accuracy)
{
  return pi_half - my_arcsin(x, accuracy);
}

double my_tan(double x, const double accuracy = standard_accuracy)
{
  return my_sin(x, accuracy) / (my_cos(x, accuracy));
}
// 1 / (1 / x - 1)

// double Euler_numbers(double x, const double accuracy = standard_accuracy) // Eulerschen Zahlen
// {
//   double res = 0;
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

// double my_tan(double x, const double accuracy = standard_accuracy)
// {
//   double res = 0;
//   return res;
// }

double my_exp(double x, const double accuracy = standard_accuracy)
{
  double res = 0, x_tmp = 1, new_val;
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

double my_ln(double x, const double accuracy = standard_accuracy)
{
  assert(x > 0 && "ln <= 0 not defined");
  x = (x - 1) / (x + 1);
  double res = 0, x_sqr = x * x, tmp = 1, new_val;
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
  NodeLog,
  NodeCon
};

struct Node
{
  NodeKind kind;
  // Optimierung: union
  float value;

  int varId;
  string varName;

  unique_ptr<Node> a, b;

  friend ostream &operator<<(ostream &os, const Node &t);

  Node(NodeKind kind1, float val) : kind(kind1), value(val){};
  Node(int vardId1, string varName1) : kind(NodeVar), varId(vardId1), varName(varName1){};
  Node(NodeKind kind1, unique_ptr<Node> a1) : kind(kind1), a(move(a1)){};
  Node(NodeKind kind1, unique_ptr<Node> a1, unique_ptr<Node> b1) : kind(kind1), a(move(a1)), b(move(b1)){};
};

typedef unique_ptr<Node> NodePtr;

NodePtr to_node(float value)
{
  return make_unique<Node>(Node(NodeValue, value));
}

NodePtr operator+(NodePtr &a, NodePtr &b)
{
  return make_unique<Node>(Node(NodeAdd, move(a), move(b)));
}

NodePtr operator-(NodePtr &a, NodePtr &b)
{
  return make_unique<Node>(Node(NodeSub, move(a), move(b)));
}

NodePtr operator*(NodePtr &a, NodePtr &b)
{
  return make_unique<Node>(Node(NodeMul, move(a), move(b)));
}

NodePtr operator/(NodePtr &a, NodePtr &b)
{
  return make_unique<Node>(Node(NodeDiv, move(a), move(b)));
}

NodePtr operator^(NodePtr &a, NodePtr &b)
{
  return make_unique<Node>(Node(NodePow, move(a), move(b)));
}

bool operator==(const NodePtr &a, const NodePtr &b)
{
  if (a.get()->kind != b.get()->kind)
    return false;

  switch (a.get()->kind)
  {
  case NodeValue:
    return a.get()->value == b.get()->value;
  case NodeCon:
    return a.get()->value == b.get()->value;
  case NodeVar:
    return a.get()->varId == b.get()->varId;
  case NodeAdd:
    return a.get()->a == b.get()->a && a.get()->b == b.get()->b; // Vertauschungen hinzufügen? -> Problem: (4 + 3 == 3 + 4) -> false
  case NodeMul:
    return a.get()->a == b.get()->a && a.get()->b == b.get()->b;
  case NodeDiv:
    return a.get()->a == b.get()->a && a.get()->b == b.get()->b;
  case NodeSub:
    return a.get()->a == b.get()->a && a.get()->b == b.get()->b;
  case NodePow:
    return a.get()->a == b.get()->a && a.get()->b == b.get()->b;
  case NodeLog:
    return a.get()->a == b.get()->a && a.get()->b == b.get()->b;
  case NodeSin:
    return a.get()->a == b.get()->a;
  case NodeCos:
    return a.get()->a == b.get()->a;
  }
  assert(0 && "error");
  return false;
}

ostream &stringify(ostream &os, const Node &t, int level)
{
  switch (t.kind)
  {
  case NodeCon:
    if (t.value == M_E)
      os << "e";
    else if (t.value == M_PI)
      os << "pi";
    else
      assert(0 && "unknown Constant");
    break;
  case NodeValue:
    if (t.value < 0)
      os << "(";

    if (t.value == floor(t.value))
      os << (int)(t.value);
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
    if (t.a.get()->kind == NodeValue && t.a.get()->value == -1)
    {
      os << "-";
      stringify(os, *t.b.get(), 2);
    }
    else if (t.b.get()->kind == NodeValue && t.b.get()->value == -1)
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
  case NodeLog:
    if (level > 2)
      os << "(";

    if (t.a.get()->value == M_E)
    {
      os << "ln(";
      stringify(os, *t.b.get(), 1);
      os << ")";
    }
    else
    {
      os << "log(";
      stringify(os, *t.b.get(), 1);
      os << ", ";
      stringify(os, *t.a.get(), 1);
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

  bool next(const TokenKind kind)
  {
    return cur < tokens.size() && tokens[cur].kind == kind;
  }

  bool take(TokenKind kind)
  {
    if (next(kind))
    {
      cur++;
      return true;
    }
    return false;
  }

  Token token()
  {
    return tokens[cur - 1];
  }

  void back()
  {
    cur--;
  }
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
  }
  assert(0 && "error parsing");
  return 0;
}

NodePtr parse(TokenIter &iter, map<string, int> &vars, bool next_negated, int level = 0)
{
  NodePtr result = nullptr;
  if (iter.take(TokenValue))
    result = to_node(iter.token().value);
  else if (iter.take(TokenLn))
  {
    assert(iter.take(TokenOpen) && "failed parsing");
    result = make_unique<Node>(Node(NodeLog, make_unique<Node>(Node(NodeCon, M_E)), parse(iter, vars, false)));
    assert(iter.take(TokenClose) && "failed parsing");
  }
  else if (iter.take(TokenSin))
  {
    assert(iter.take(TokenOpen) && "failed parsing");
    result = make_unique<Node>(Node(NodeSin, parse(iter, vars, false)));
    assert(iter.take(TokenClose) && "failed parsing");
  }
  else if (iter.take(TokenCos))
  {
    assert(iter.take(TokenOpen) && "failed parsing");
    result = make_unique<Node>(Node(NodeCos, parse(iter, vars, false)));
    assert(iter.take(TokenClose) && "failed parsing");
  }
  else if (iter.take(TokenTan))
  {
    assert(iter.take(TokenOpen) && "failed parsing");
    result = make_unique<Node>(Node(NodeSin, parse(iter, vars, false)));
    assert(iter.take(TokenClose) && "failed parsing");
  }
  else if (iter.take(TokenLog))
  {
    assert(iter.take(TokenOpen) && "failed parsing");
    NodePtr temp = parse(iter, vars, false);
    assert(iter.take(TokenComma) && "failed parsing");
    result = make_unique<Node>(Node(NodeLog, parse(iter, vars, false), move(temp)));
    assert(iter.take(TokenClose) && "failed parsing");
  }
  else if (iter.take(TokenVar))
  {
    string name = iter.token().name;

    if (name == "e")
      result = make_unique<Node>(Node(NodeCon, M_E));
    else if (name == "pi")
      result = make_unique<Node>(Node(NodeCon, M_PI));
    else
    {
      map<string, int>::iterator it = vars.find(name);
      if (it != vars.end())
        result = make_unique<Node>(Node(it->second, name));
      else
      {
        int index = vars.size();
        vars.emplace(name, index);
        result = make_unique<Node>(Node(index, name));
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

  if (!result)
    return result;

  if (next_negated)
    result = make_unique<Node>(Node(NodeMul, to_node(-1), move(result)));

  while (iter.take(TokenAdd) || iter.take(TokenSub) || iter.take(TokenMul) || iter.take(TokenDiv) || iter.take(TokenPot))
  {
    TokenKind kind = iter.token().kind;
    int lv = operators(kind);
    if (lv <= level)
    {
      iter.back();
      return result;
    }

    NodePtr b = parse(iter, vars, false, lv);
    if (!b)
      return nullptr;

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

    result = make_unique<Node>(Node(nk, move(result), move(b)));
  }

  return result;
}

NodePtr parse(string str, map<string, int> &vars)
{
  TokenIter iter = TokenIter(tokenize(str), 0);
  return parse(iter, vars, false);
}
#pragma endregion

double eval(const NodePtr &ptr_node, const map<string, int> &vars)
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
  case NodeLog:
    return log10(eval(node->a, vars)) / log10(eval(node->b, vars));
  case NodeVar:
    auto it = vars.find(node->varName);
    assert(it != vars.end() && "");
    return it->second;
  }
  assert(0 && "error");
  return 0;
}

// NodePtr derive(NodePtr &ptr_node, map<string, int> &vars)
// {
//   Node *node = ptr_node.get();
//   switch (node->kind)
//   {
//   case NodeValue:
//     return node->value;
//   case NodeCon:
//     return node->value;
//   case NodeAdd:
//     return eval(node->a, vars) + eval(node->b, vars);
//   case NodeSub:
//     return eval(node->a, vars) - eval(node->b, vars);
//   case NodeMul:
//     return eval(node->a, vars) * eval(node->b, vars);
//   case NodeDiv:
//     return eval(node->a, vars) / eval(node->b, vars);
//   case NodePow:
//     return pow(eval(node->a, vars), eval(node->b, vars));
//   case NodeSin:
//     return sin(eval(node->a, vars));
//   case NodeCos:
//     return cos(eval(node->a, vars));
//   case NodeLog:
//     return log10(eval(node->a, vars)) / log10(eval(node->b, vars));
//   case NodeVar:
//     auto it = vars.find(node->varName);
//     assert(it != vars.end() && "");
//     return it->second;
//   }
//   assert(0 && "error");
//   return 0;
// }

// to_string

int main()
{
  ios::sync_with_stdio(false);
  cin.tie(0);

  // auto a = Node(NodeValue, 2);
  // make_unique<Node>(a);
  // auto b = Node(NodeValue, 5);
  // string input = "-2(-x+1)x(x-1)(4x)*zy+variableX7";
  // string input = "-1+7*(3-2)+2*sin(0.2)*log(4,5)";
  string input = "-(-4-+2)+7*(3-2)+2*sin(0.2)*log(4,5)";
  // x - +5;

  map<string, int> vars;
  NodePtr n = parse(input, vars);
  cout << *n.get() << " = " << eval(n, vars) << endl;

  NodePtr a = to_node(4.1);
  NodePtr b = to_node(4.0);
  NodePtr c = a + b;
  cout << c << endl;

  NodePtr a1 = to_node(4);
  NodePtr b1 = to_node(4.1);
  NodePtr c1 = a1 + b1;
  cout << c1 << endl;

  cout << (c == c1) << endl;
  // NodePtr c = make_unique<Node>(Node(NodeAdd, make_unique<Node>(Node(NodeValue, 2)), make_unique<Node>(Node(NodeValue, 7))));
  // Node c = make_unique<Node>(Node(NodeValue, 2)) + make_unique<Node>(Node(NodeValue, 25));
  // Node c = make_unique<Node>(Node(NodeValue, 2)) + make_unique<Node>(Node(NodeValue, 25));
  // // NodePtr a = make_unique<Node>(Node(NodeValue, 3));

  // cout << c << endl;

  // bool test(false);

  // for (auto i = 0; i <= 12; i++)
  // {
  //   cout << n_k(10, i) << endl;
  // }

  // cout << my_faculty(8, 4) << "\t" << my_faculty(8, 8) << endl;
  // cout << my_pow(2.1, 0) << endl;
  // cout << my_pow(2.1, 1) << endl;
  // cout << my_pow(2.1, 2) << endl;

  // for (size_t i = 1; i < 10; i++)
  // {
  //   cout << setprecision(35) << (double)i / 10 << ": " << my_tan((double)i / 10, 10e-10) << "\t" << tan((double)i / 10) << "\t" << tan((double)i / 10) - my_tan((double)i / 10, 10e-10) << endl;
  // }
  // int x = 6;
  // cout << (x ^ 1) << endl;

  // for (size_t i = 0; i < 24; i++)
  // {
  // }

  // string input = "2*(x+1)*x*(x-1)*(4*x)*zy+variableX7";
  //   cout << input << " <=> " << tokenize(input) << endl;

  //   NodePtr n1 = parse(input);
  //   if (n1)
  //     cout << *n1.get() << endl;
  //   else
  //     cout << "error" << endl;
};

//   NodeValue,
//   NodeAdd, NodeSub, NodeMul, NodeDiv,
//   NodePow,
//   NodeSin, NodeCos, NodeLog,
//   ...

// NodeValue - NodeValue: 1 // 2 * 2 -> 22
// NodeValue - NodePow: 1 // 2 * 5^2 -> 52^2
// NodeValue - NodeAdd, NodeSub, NodeMul, NodeDiv: 1
// NodeValue - NodeSin, NodeCos, NodeLog: 0
// NodeValue - NodeVar, NodeCon: 0

// NodeAdd, NodeSub, NodeMul, NodeDiv - NodeAdd, NodeSub, NodeMul, NodeDiv: 1 // ...
// NodeAdd, NodeSub, NodeMul, NodeDiv - NodePow: 1 // ...
// NodeAdd, NodeSub, NodeMul, NodeDiv - NodeSin, NodeCos, NodeLog: 1
// NodeAdd, NodeSub, NodeMul, NodeDiv - NodeVar, NodeCon: 1

// NodePow - NodePow: 0
// NodePow - NodeSin, NodeCos, NodeLog:
// NodePow - NodeVar, NodeCon:

// NodeSin, NodeCos, NodeLog - NodeSin, NodeCos, NodeLog:
// NodeSin, NodeCos, NodeLog - NodeVar, NodeCon:

// NodeVar, NodeCon - NodeVar, NodeCon: 1 // pi * e -> pie