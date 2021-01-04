import tables, sequtils, math
import parser

# ln(8x) = ln(8) + ln(x)
# KONSTANTEN
# 0 - vermeiden

converter to_node*(x: float64): Node = Node(kind: NodeValue, value: x)
converter to_node*(x: int): Node = Node(kind: NodeValue, value: toFloat x)

proc `+`*(a, b: Node): Node = Node(kind: NodeAdd, a: a, b: b)
proc `*`*(a, b: Node): Node = Node(kind: NodeMul, a: a, b: b)
proc `/`*(a, b: Node): Node = Node(kind: NodeDiv, a: a, b: b)
proc `-`*(a, b: Node): Node = Node(kind: NodeSub, a: a, b: b)
proc `^`*(a, b: Node): Node = Node(kind: NodePow, a: a, b: b)

proc `==`(a, b: Node): bool =
    if a.kind != b.kind:
        return false

    case a.kind:
        of NodeValue: return a.value == b.value
        of NodeAdd, NodeMul, NodeDiv, NodeSub, NodePow:
            return a.a == b.a and a.b == b.b
        of NodeSin, NodeCos:
            return a.val == b.val
        of NodeLog: return a.x == b.x and a.base == b.base
        of NodeVar: return a.varId == b.varId

proc value*(a: float64): Node = Node(kind: NodeValue, value: a)
proc variable*(a: int, b: string): Node = Node(kind: NodeVar, varId: a, varName: b)
proc sin*(a: Node): Node = Node(kind: NodeSin, val: a)
proc cos*(a: Node): Node = Node(kind: NodeCos, val: a)
proc tan*(a: Node): Node = sin(a) / cos(a)
proc log*(a, b: Node): Node = Node(kind: NodeLog, x: a, base: b)
proc log*(a: Node): Node = Node(kind: NodeLog, x: a, base: 10)
proc ln*(a: Node): Node = Node(kind: NodeLog, x: a, base: to_node E)
proc sqrt*(a: Node): Node = Node(kind: NodePow, a: a, b: 0.5)
proc rad*(a: float64): float64 = a * PI / 180
proc deg*(a: float64): float64 = a * 180 / PI
# let i = sqrt(-1)

proc flatten(node: Node, node_kind: NodeKind): seq[Node] =
    case node.kind:
        of NodeValue, NodeCos, NodeSin, NodeLog: return @[node]
        else:
            if node.kind == node_kind:
                return node.a.flatten(node_kind).concat(node.b.flatten(node_kind))
            return @[node]

proc flatten(node: Node): seq[Node] = node.flatten(node.kind)

proc unflatten(terms: seq[Node], node_kind: NodeKind): Node =
    var cur = terms[0]

    for it in 1..<terms.len:
        case node_kind:
            of NodeAdd, NodeSub, NodeMul, NodeDiv, NodePow:
                cur = Node(kind: node_kind, a: cur, b: terms[it])
            else: discard
    return cur

proc eval*(node: Node, bindings: Table[int, float64]): float64 =
    case node.kind:
        of NodeValue: return node.value
        of NodeAdd: return node.a.eval(bindings) + node.b.eval(bindings)
        of NodeMul: return node.a.eval(bindings) * node.b.eval(bindings)
        of NodeDiv: return node.a.eval(bindings) / node.b.eval(bindings)
        of NodeSub: return node.a.eval(bindings) - node.b.eval(bindings)
        of NodePow:
            let n1 = node.b.eval(bindings)
            if n1 == 0.5:
                return sqrt(node.a.eval(bindings))
            return pow(node.a.eval(bindings), n1)
        of NodeSin: return sin(node.val.eval(bindings))
        of NodeCos: return cos(node.val.eval(bindings))
        of NodeLog:
            let n = node.base.eval(bindings)
            if n == E:
                return ln(node.x.eval(bindings))
            return log(node.x.eval(bindings), n)
        of NodeVar:
            if node.varName == "e":
                return E
            if node.varName == "pi":
                return PI
            return bindings[node.varId]

proc eval*(node: Node): float64 =
    case node.kind:
        of NodeValue: return node.value
        of NodeAdd: return node.a.eval() + node.b.eval()
        of NodeMul: return node.a.eval() * node.b.eval()
        of NodeDiv: return node.a.eval() / node.b.eval()
        of NodeSub: return node.a.eval() - node.b.eval()
        of NodePow:
            let n1 = node.b.eval()
            if n1 == 0.5:
                return sqrt(node.a.eval())
            return pow(node.a.eval(), n1)
        of NodeSin: return sin(node.val.eval())
        of NodeCos: return cos(node.val.eval())
        of NodeLog:
            let n = node.base.eval()
            if n == E:
                return ln(node.x.eval())
            return log(node.x.eval(), n)
        of NodeVar:
            if node.varName == "e":
                return E
            if node.varName == "pi":
                return PI
            quit "not defined"

proc derive*(node: Node): Node =
    case node.kind:
        of NodeValue: return 0
        of NodeSin: return cos(node.val) * node.val.derive()
        of NodeCos: return (-1) * sin(node.val) * node.val.derive()
        of NodeLog:
            if (node.base.kind == NodeVar and node.base.varName == "e" or node.base == E):
                return node.x.derive() / node.x
            return (ln(node.x) / ln(node.base)).derive()
        of NodeAdd: return node.a.derive() + node.b.derive()
        of NodeSub: return node.a.derive() - node.b.derive()
        of NodeMul: return node.a * node.b.derive() + node.a.derive() * node.b
        of NodeDiv: return (node.b * node.a.derive() - node.b.derive() * node.a) / (node.b ^ 2)
        of NodePow: return node * (node.b * ln(node.a)).derive()
        of NodeVar:
            if node.varName == "e": return 0
            return 1

proc digits(a: float64): int =
    var n = a
    while n != n.floor:
        result += 1
        n *= 10

proc simplify(val_a, val_b: float64): (int, int) =
    var a1, b1: int

    if (val_a != int val_a) or (val_b != int val_b):
        var l = toFloat (10 ^ max(digits(val_a), digits(val_b)))
        a1 = int (val_a * l)
        b1 = int (val_b * l)
    else:
        a1 = int val_a
        b1 = int val_b

    var g = a1.gcd(b1)
    return (a1 div g, b1 div g)

proc take_value(val_a: var seq[Node]): Node =
    for it, val in val_a:
        if val.kind == NodeValue:
            var temp = val
            val_a.delete(it)
            return temp
    return 1

proc simplify*(node: Node): Node =
    case node.kind
        of NodeValue, NodeVar: return node
        of NodeSin: return sin(simplify node.val)
        of NodeCos: return cos(simplify node.val)
        of NodeLog:
            let
                a = simplify node.x
                b = simplify node.base

            if a == b or (a.kind == NodeVar and a.varName == "e" and b == E) or (b.kind == NodeVar and b.varName == "e" and a == E) : return 1

            return log(a, b)

        of NodeAdd:
            let
                a = simplify node.a
                b = simplify node.b


            if a == b: return simplify(2 * a)

            let terms = node.flatten().map(simplify)

            var
                value_sum: float64 = 0
                rest: seq[Node] = @[]
            for term in terms:
                case term.kind:
                    of NodeValue:
                        value_sum += term.value
                    else:
                        rest.add(term)
            if value_sum != 0 or rest.len == 0:
                rest.add(value_sum.value())

            return rest.unflatten(NodeAdd)

        of NodeSub:
            let
                a = simplify node.a
                b = simplify node.b

            if b == 0: return a
            if a == 0: return ((-1) * b).simplify

            return (a + (-1) * b).simplify()

        of NodeMul:
            let
                a = simplify node.a
                b = simplify node.b

            if a == b: return simplify(a ^ 2)
            if b == 1: return a
            if a == 1: return b
            if (a == 0) or (b == 0): return 0

            if a.kind == NodeValue and b.kind == NodeValue:
                return a.value * b.value
            if a.kind == NodeDiv: # (a.a / a.b) * b = (a.a * ...) / a.b
                return simplify((a.a * b) / a.b)
            if b.kind == NodeDiv: # ... * (a / b) = (b.a * ...) / b.b
                return simplify((b.a * a) / b.b)

            let terms = node.flatten().map(simplify)
            var
                product: float64 = 1
                rest: seq[Node] = @[]
            for term in terms:
                case term.kind:
                    of NodeValue:
                        product *= term.value
                    else:
                        rest.add(term)

            if product != 1:
                var by = false
                for i in 0..<rest.len:
                    if rest[i].kind == NodeDiv:
                        rest[i] = ((rest[i].a * product) / (rest[i].b *
                                product)).simplify()
                        by = true
                        break;

                if by == false:
                    rest.add(product.value())

            return rest.unflatten(NodeMul)

        of NodeDiv:
            var
                a = simplify node.a
                b = simplify node.b

            if b == -1: return simplify((-1) * a)
            if b == 0:
                echo node
                quit "division by 0"
            if b == 1: return a
            if a == 0: return 0

            if a.kind != NodeMul:
                a = a * (value 1)
            if b.kind != NodeMul:
                b = b * (value 1)

            if a.kind == NodeMul and b.kind == NodeMul:
                var
                    terms_a = a.flatten()
                    terms_b = b.flatten()
                    terms_a_new: seq[Node] = @[]

                let n = simplify(terms_a.take_value().value, terms_b.take_value().value)
                terms_a.add(n[0])
                terms_b.add(n[1])

                for term_a in terms_a:
                    var found = -1
                    for it, term_b in terms_b:
                        if term_a == term_b:
                            found = it
                            break
                    if found != -1:
                        terms_b.delete(found)
                    else:
                        terms_a_new.add(term_a)

                return (terms_a_new.unflatten(NodeMul)).simplify() /
                        terms_b.unflatten(NodeMul)

            return a / b

        of NodePow:
            let
                a = simplify node.a
                b = simplify node.b

            if b == -1: return 1 / a
            if b == 0: return 1
            if b == 1: return a

            # if a == -1: return -1 * b#-1^2 -1^n
            if a == 0: return 0
            if a == 1: return 1

            if b.kind == NodePow: return simplify(a ^ (b.a * b.b))
            if a.kind == NodePow: return simplify(a.a ^ (a.b * b))

            return a ^ b

proc newton*(node: Node, id: int, bindings: Table[int, float64]): float64 =
    const limit = 100
    const acc = 1.0e-10

    let simply = simplify node
    let s_divided = simplify (simply / simply.derive.simplify)
    var
        b = bindings
        val = b[id]
        val2 = 0.0

    for iteration in countup(0, limit):
        b[id] = val
        val2 = s_divided.eval(b)
        val -= val2

        if val2 == Inf or val2 == -Inf: return Inf
        if val == b[id] or abs(val2) <= acc: return val
    return Inf

proc cardan*(a1, b1, c1, d1: float64): float64 =
    var a, b, c, n, d: float64

    a = b1 / a1
    b = c1 / a1
    c = d1 / a1

    n = a * b / 6 - a ^ 3 / 27 - c / 2
    d = n ^ 2 + (b / 3 - a ^ 2 / 9) ^ 3

    if d > 0:
        d = sqrt(d)
        return cbrt(n - d) + cbrt(n + d) - a / 3
    if d == 0:
        return 2 * cbrt(n) - a / 3
    return 2 / 3 * sqrt(a ^ 2 - b * 3) * cos(arctan(sqrt(-d) / n) / 3) - a / 3

proc div_poly(poly_1: Table[int, float64], p: float64): Table[int, float64] =
    for i in 0..<poly_1.len:
        if i == 0:
            result[0] = poly1[0]
        elif i == poly_1.len-1:
            if poly1[i] - result[i-1] * p != 0:
                var n: Table[int, float64]
                return n
        else:
            result[i] = poly1[i] - result[i-1] * p


# echo cardan(1.0, 3.0, -54.0, -112.0)
# echo cardan(1 / 100, - 3 / 50, - 63 / 100, 2598 / 25)

# let x = variable(0)
# echo (ln(2 * x) / (E ^ (2 * x) + 2)).derive()
# echo (ln(2 * x) / (E ^ (2 * x) + 2)).derive()#.simplify()
# echo (2 * x ^ 2).derive()#.simplify()
# echo "3*hallo+2-x1*x2".parse

#
