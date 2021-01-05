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

  Node(NodeKind _kind, decimal _value);
  Node(int _varId, string _varName);
  Node(NodeKind _kind, NodePtr _a);
  Node(NodeKind _kind, NodePtr _a, NodePtr _b);
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

  TokenIter(vector<Token> _tokens, size_t _cur);

  bool next(const TokenKind kind);

  bool take(TokenKind kind);

  Token token();

  void back();
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
  range(double _min_val, double _max_val);
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