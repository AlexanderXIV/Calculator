#include "tokenizer.h"

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