#pragma once
#include "defs.h"
#include <iostream>
#include <string>
#include <vector>
#include <assert.h>

using std::ostream;
using std::string;
using std::vector;

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

ostream &operator<<(ostream &os, const Token &t);

ostream &operator<<(ostream &os, const vector<Token> &tokens);

bool is_decimal(const string &str);

Token make_token(const string &str);

void tokenize(string str, vector<Token> &result);