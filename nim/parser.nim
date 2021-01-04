import tables, math
import lexer

const OPERATORS = {'+': 1, '-': 1, '*': 2, '/': 2, '^': 3}.to_table

type
  NodeKind* = enum NodeValue, NodeAdd, NodeMul, NodeDiv, NodeSub, NodeVar,
      NodePow, NodeSin, NodeCos, NodeLog

  Node* = ref object
    case kind*: NodeKind:
      of NodeValue: value*: float64
      of NodeVar:
        varId*: int
        varName*: string
      of NodeAdd, NodeSub, NodeMul, NodeDiv, NodePow:
        a*, b*: Node
      of NodeSin, NodeCos:
        val*: Node
      of NodeLog:
        x*, base*: Node

var e* = Node(kind: NodeValue, value: E)
var pi* = Node(kind: NodeValue, value: E)

proc stringify(node: Node, level: int): string =
  case node.kind:
    of NodeValue:
      if node.value == E: return "e"
      if node.value == PI: return "pi"
      if node.value == floor(node.value):
        result = $(int node.value)
      else:
        result = $node.value
      if node.value < 0:
        result = "(" & result & ")"
    of NodeAdd:
      result = node.a.stringify(1) & " + " & node.b.stringify(1)
      if level > 1:
        result = "(" & result & ")"
    of NodeSub:
      result = node.a.stringify(1) & " - " & node.b.stringify(2)
      if level > 1:
        result = "(" & result & ")"
    of NodeMul:
      if node.a.kind == NodeValue and node.a.value == -1:
        result = "-" & node.b.stringify(2)
      elif node.b.kind == NodeValue and node.b.value == -1:
        result = "-" & node.a.stringify(2)
      elif node.b.kind == NodeVar and node.a.kind != NodeVar:
        result = node.a.stringify(1) & node.b.stringify(1)
      elif node.a.kind == NodeVar and node.b.kind != NodeVar:
        result = node.b.stringify(1) & node.a.stringify(1)
      else:
        result = node.a.stringify(2) & " * " & node.b.stringify(2)

      if level > 1:
        result = "(" & result & ")"
    of NodeDiv:
      result = node.a.stringify(2) & " / " & node.b.stringify(3)
      if level > 2:
        result = "(" & result & ")"
    of NodeSin:
      result = "sin(" & node.val.stringify(1) & ")"
    of NodeCos:
      result = "cos(" & node.val.stringify(1) & ")"
    of NodeLog:
      if node.base.value == E:
        result = "ln(" & node.x.stringify(1) & ")"
      else:
        result = "log(" & node.x.stringify(1) & ", base: " &
            node.base.stringify(1) & ")"

      if level > 2:
        result = "(" & result & ")"
    of NodePow:
      result = node.a.stringify(3) & " ^ " & node.b.stringify(3)

      if level > 2:
        result = "(" & result & ")"
    of NodeVar:
      return node.varName

proc `$`*(node: Node): string = node.stringify(0)

type
  TokenIter = object
    cur: int
    tokens: seq[Token]

proc next(iter: TokenIter, kind: TokenKind): bool =
  if iter.cur >= iter.tokens.len:
    return false
  return iter.tokens[iter.cur].kind == kind

proc take(iter: var TokenIter, kind: TokenKind): bool =
  if iter.next(kind):
    iter.cur += 1
    return true
  return false

proc token(iter: TokenIter): Token = iter.tokens[iter.cur - 1]

proc back(iter: var TokenIter) = iter.cur -= 1

proc parse(iter: var TokenIter, vars: var ref Table[string, int],
    level: int = 0): Node =
  if iter.take(TokenValue):
    result = Node(kind: NodeValue, value: iter.token.value)

  elif iter.take(TokenLn):
    doAssert true == iter.take(TokenOpen)
    result = Node(kind: NodeLog, x: iter.parse(vars), base: e)
    doAssert true == iter.take(TokenClose)
  elif iter.take(TokenSin):
    doAssert true == iter.take(TokenOpen)
    result = Node(kind: NodeSin, val: iter.parse(vars))
    doAssert true == iter.take(TokenClose)
  elif iter.take(TokenCos):
    doAssert true == iter.take(TokenOpen)
    result = Node(kind: NodeCos, val: iter.parse(vars))
    doAssert true == iter.take(TokenClose)
  elif iter.take(TokenTan):
    doAssert true == iter.take(TokenOpen)
    result = Node(kind: NodeSin, val: iter.parse(vars))
    doAssert true == iter.take(TokenClose)
  elif iter.take(TokenLog):
    doAssert true == iter.take(TokenOpen)
    var n1 = iter.parse(vars)
    doAssert true == iter.take(TokenDot)
    result = Node(kind: NodeLog, x: n1, base: iter.parse(vars))
    doAssert true == iter.take(TokenClose)

  elif iter.take(TokenVar):
    var name = iter.token.name
    var index = -1

    if name != "e" and name != "pi":
      if vars.hasKey(name):
        index = vars.getOrDefault(name)
      else:
        index = vars.len
        vars.add(name, index)

    result = Node(kind: NodeVar, varid: index, varName: name)

  elif iter.take(TokenOpen):
    result = iter.parse(vars)
    discard iter.take(TokenClose)

  if result == nil:
    return

  while iter.take(TokenOperator):
    let chr = iter.token().chr
    if OPERATORS[chr] <= level:
      iter.back()
      return

    let b = iter.parse(vars, OPERATORS[chr])
    if b == nil:
      return nil

    case chr:
      of '+': result = Node(kind: NodeAdd, a: result, b: b)
      of '-': result = Node(kind: NodeSub, a: result, b: b)
      of '*': result = Node(kind: NodeMul, a: result, b: b)
      of '/': result = Node(kind: NodeDiv, a: result, b: b)
      of '^': result = Node(kind: NodePow, a: result, b: b)
      else: quit "Unreachable"

proc parse*(str: string): Node =
  var
    tokens = str.tokenize
    iter = TokenIter(tokens: tokens, cur: 0)
    vars = newTable[string, int]()
  echo tokens
  return iter.parse(vars)
