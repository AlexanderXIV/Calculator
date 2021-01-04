import strutils

type
  TokenKind* = enum TokenValue, TokenOpen, TokenClose, TokenString, TokenVar,
    TokenSin, TokenCos, TokenTan, TokenLog, TokenLn, TokenOperator, TokenDot

  Token* = object
    case kind*: TokenKind
      of TokenValue:
        value*: float64
      of TokenString, TokenVar:
        name*: string
      of TokenOperator:
        chr*: char
      else: discard

proc is_float(str: string): bool =
  if str.len == 0: return false

  var point = false
  for it, chr in str:
    if chr.is_digit:
      continue
    if it == 0 and (chr == '+' or chr == '-') and str.len > 1:
      continue
    if chr == '.' and not point:
      point = true
      continue
    return false
  return true

proc make_token(str: string): Token =
  case str:
    of "sin": return Token(kind: TokenSin)
    of "cos": return Token(kind: TokenCos)
    of "tan": return Token(kind: TokenTan)
    of "log": return Token(kind: TokenLog)
    of "ln": return Token(kind: TokenLn)
    else:
      if str.is_float:
        return Token(kind: TokenValue, value: str.parseFloat)
      return Token(kind: TokenVar, name: str)

proc tokenize*(str: string): seq[Token] =
  var cur = ""
  for chr in str:
    case chr:
      of '(', ')', '+', '-', '*', '/', '^', ' ', '\n', ',':
        if cur != "":
          result.add(make_token(cur))
          cur = ""

        case chr:
          of '(': result.add(Token(kind: TokenOpen))
          of ')': result.add(Token(kind: TokenClose))
          of ',': result.add(Token(kind: TokenDot))
          of '-':
            if result.len == 0 or result[result.len - 1].kind == TokenOpen:
              cur &= chr
            else:
              result.add(Token(kind: TokenOperator, chr: chr))
          of '+', '*', '/', '^':
            result.add(Token(kind: TokenOperator, chr: chr))
          else: discard
      else:
        if is_float(cur) and is_float(cur & chr) == false:
          result.add(make_token(cur))
          result.add(Token(kind: TokenOperator, chr: '*'))
          cur = ""
        cur &= chr

  if cur != "":
    result.add(make_token(cur))
