#include "parser.h"

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

NodePtr parse(TokenIter &iter, map<string, int> &vars, bool next_negated, int level)
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
NodePtr stern_brocot_tree(decimal x, const int limit, const decimal accuracy)
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

decimal deriveAt(const NodePtr &ptr_node, vector<decimal> &vars, int varId, const decimal accuracy)
{
  decimal num1 = eval(ptr_node, vars);
  vars[varId] += 1 / accuracy;
  return accuracy * (eval(ptr_node, vars) - num1);
}

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
      // cout << f(x - tol) << " " << f(x) << " " << f(x + tol) << endl;
    }
  return (int)roots.size();
}

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