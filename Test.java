/**
 * @date 4.7.2020
 * 
 * Kategorie: Übung Competitive programming
 * Selbst gesetztes Ziel: 1h
 * Benötigte Zeit: 1h, 26min
 * 
 * Aufgabe:
 * Programm das aus einer Rechnung (String) ein Ergebnis ausrechnet
 * 
 * Nachtrag:
 * Hautsächlich linear runterprogrammiert, noch keine Fehler implementiert.
 */

import java.util.ArrayList;

// Tokenizer:
// ==========================================

abstract class Token {
}

class TokenValue extends Token {
  double value;

  public TokenValue(double value) {
    this.value = value;
  }

  @Override
  public String toString() {
    return Double.toString(value);
  }
}

class TokenOpen extends Token {
  @Override
  public String toString() {
    return "(";
  }
}

class TokenClose extends Token {
  @Override
  public String toString() {
    return ")";
  }
}

class TokenOperator extends Token {
  char operator;

  public TokenOperator(char operator) {
    this.operator = operator;
  }

  @Override
  public String toString() {
    return Character.toString(operator);
  }
}

class Tokenizer {
  public static ArrayList<Token> tokenize(String str) {
    ArrayList<Token> tokens = new ArrayList<>();

    String cur = "";
    for (int i = 0; i < str.length(); i++) {
      char chr = str.charAt(i);

      if (chr == '(' || chr == ')' || chr == '+' || chr == '-' || chr == '*' || chr == '/' || chr == '^'
          || chr == ' ') {
        if (cur.length() != 0) {
          if (tokens.size() > 0 && tokens.get(tokens.size() - 1).getClass() == TokenClose.class)
            tokens.add(new TokenOperator('*'));
          tokens.add(new TokenValue(Double.parseDouble(cur)));
          if (tokens.size() > 0 && chr == '(')
            tokens.add(new TokenOperator('*'));
          cur = "";
        }

        if (chr == '(') {
          tokens.add(new TokenOpen());
        } else if (chr == ')') {
          tokens.add(new TokenClose());
        } else if (chr == '-') {
          if (tokens.size() == 0)
            cur += chr;
          else
            tokens.add(new TokenOperator(chr));
        } else if (chr != ' ') {
          tokens.add(new TokenOperator(chr));
        }
      } else {
        cur += chr;
      }
    }

    if (cur.length() != 0) {
      if (tokens.size() > 0 && tokens.get(tokens.size() - 1).getClass() == TokenClose.class)
        tokens.add(new TokenOperator('*'));
      tokens.add(new TokenValue(Double.parseDouble(cur)));
    }

    return tokens;
  }
}

// Parser:
// ==========================================

abstract class Node {
}

class NodeValue extends Node {
  double value;

  public NodeValue(double value) {
    this.value = value;
  }

  @Override
  public String toString() {
    return "(" + value + ")";
  }
}

class NodeAdd extends Node {
  Node a, b;

  public NodeAdd(Node a, Node b) {
    this.a = a;
    this.b = b;
  }

  @Override
  public String toString() {
    return "(" + a + " + " + b + ")";
  }
}

class NodeSub extends Node {
  Node a, b;

  public NodeSub(Node a, Node b) {
    this.a = a;
    this.b = b;
  }

  @Override
  public String toString() {
    return "(" + a + " - " + b + ")";
  }
}

class NodeMul extends Node {
  Node a, b;

  public NodeMul(Node a, Node b) {
    this.a = a;
    this.b = b;
  }

  @Override
  public String toString() {
    return "(" + a + " * " + b + ")";
  }
}

class NodeDiv extends Node {
  Node a, b;

  public NodeDiv(Node a, Node b) {
    this.a = a;
    this.b = b;
  }

  @Override
  public String toString() {
    return "(" + a + " / " + b + ")";
  }
}

class NodePow extends Node {
  Node a, b;

  public NodePow(Node a, Node b) {
    this.a = a;
    this.b = b;
  }

  @Override
  public String toString() {
    return "(" + a + " ^ " + b + ")";
  }
}

class Parser {
  ArrayList<Token> tokens;
  int cur;

  public int OPERATORS(char chr) {
    if (chr == '+' || chr == '-') {
      return 1;
    } else if (chr == '*' || chr == '/') {
      return 2;
    }
    return 3;
  }

  public Parser(ArrayList<Token> tokens) {
    this.tokens = tokens;
    cur = 0;
  }

  public boolean next(Class<? extends Token> kind) {
    if (cur >= tokens.size())
      return false;
    return tokens.get(cur).getClass() == kind;
  }

  public boolean take(Class<? extends Token> kind) {
    if (next(kind)) {
      cur++;
      return true;
    }
    return false;
  }

  public Token token() {
    return tokens.get(cur - 1);
  }

  public void back() {
    cur--;
  }

  public Node parse(int level) {
    Node result = null;

    if (take(TokenValue.class)) {
      result = new NodeValue(((TokenValue) token()).value);
    } else if (take(TokenOpen.class)) {
      result = parse(0);
      take(TokenClose.class);
    }

    if (result == null) {
      System.out.println("error");
      return result;
    }

    while (take(TokenOperator.class)) {
      char chr = ((TokenOperator) token()).operator;

      if (OPERATORS(chr) <= level) {
        back();
        return result;
      }

      Node b = parse(OPERATORS(chr));
      if (b == null) {
        System.out.println("error");
        return result;
      }

      if (chr == '+') {
        result = new NodeAdd(result, b);
      } else if (chr == '-') {
        result = new NodeSub(result, b);
      } else if (chr == '*') {
        result = new NodeMul(result, b);
      } else if (chr == '/') {
        result = new NodeDiv(result, b);
      } else if (chr == '^') {
        result = new NodePow(result, b);
      }
    }

    return result;
  }
}

class Calculator {
  public static double calc(Node n) {
    if (n.getClass() == NodeValue.class)
      return ((NodeValue) n).value;
    if (n.getClass() == NodeAdd.class)
      return calc(((NodeAdd) n).a) + calc(((NodeAdd) n).b);
    if (n.getClass() == NodeSub.class)
      return calc(((NodeSub) n).a) - calc(((NodeSub) n).b);
    if (n.getClass() == NodeMul.class)
      return calc(((NodeMul) n).a) * calc(((NodeMul) n).b);
    if (n.getClass() == NodeDiv.class)
      return calc(((NodeDiv) n).a) / calc(((NodeDiv) n).b);
    if (n.getClass() == NodePow.class)
      return Math.pow(calc(((NodePow) n).a), calc(((NodePow) n).b));
    return 0;
  }
}

public class Test {
  public static void main(String[] args) {
    ArrayList<Token> tokens = Tokenizer.tokenize("-5(3 +6)2-3*2^5");
    System.out.println("Tokens: " + tokens);
    Node n = new Parser(tokens).parse(0);
    System.out.println("Node: " + n);
    double d = Calculator.calc(n);
    System.out.println("Ergebniss: " + d);
  }
}
