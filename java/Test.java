
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
import java.util.Scanner;

// Tokenizer:
// ==========================================

abstract class Token {
}

class TokenValue extends Token {
  double value;

  public TokenValue(double value) {
    this.value = value;
  }
}

class TokenOperator extends Token {
  char operator;

  public TokenOperator(char operator) {
    this.operator = operator;
  }
}

class TokenOpen extends Token {
}

class TokenClose extends Token {
}

class TokenFunction extends Token {
  String str;

  public TokenFunction(String str) {
    this.str = str;
  }
}

class Tokenizer {
  public static boolean isDouble(String str) {
    if (str.length() == 0)
      return false;
    for (char c : str.toCharArray()) {
      if ((Character.isDigit(c) || c == '.') == false) return false;
    }
    try {
      Double.parseDouble(str);
      return true;
    } catch (Exception e) {
      return false;
    }
  }

  public static Token make_token(String str) {
    if (str.equals("sin") || str.equals("cos") || str.equals("tan") || str.equals("asin") || str.equals("acos")
        || str.equals("atan") || str.equals("exp") || str.equals("sqrt") || str.equals("abs"))
      return new TokenFunction(str);
    if (str.equals("e"))
      return new TokenValue(Math.E);
    if (str.equals("pi"))
      return new TokenValue(Math.PI);
    if (isDouble(str))
      return new TokenValue(Double.parseDouble(str));
    if (isDouble(str.substring(0, str.length() - 1)) && str.charAt(str.length() - 1) == 'd')
      return new TokenValue(Math.toRadians(Double.parseDouble(str)));
    System.out.println("Unexpected Token: " + str);
    System.exit(1);
    return null;
  }

  public static ArrayList<Token> tokenize(String str) {
    String cur = "";
    ArrayList<Token> tokens = new ArrayList<>();

    for (int i = 0; i < str.length(); i++) {
      char chr = str.charAt(i);
      if (chr == '(' || chr == ')' || chr == '+' || chr == '-' || chr == '*' || chr == '/' || chr == '^'
          || chr == ' ') {
        if (cur.length() > 0) {
          tokens.add(make_token(cur));
          cur = "";
        }

        if (chr == '(') {
          tokens.add(new TokenOpen());
        } else if (chr == ')') {
          tokens.add(new TokenClose());
        } else if (chr == '-') {
          if (tokens.size() == 0)
            tokens.add(new TokenValue(0));
          tokens.add(new TokenOperator(chr));
        } else if (chr != ' ') {
          tokens.add(new TokenOperator(chr));
        }
      } else {
        if (isDouble(cur) && isDouble(cur + chr) == false && chr == 'd') {
          tokens.add(make_token(cur + chr));
          cur = "";
        } else {
          if (isDouble(cur) && isDouble(cur + chr) == false) {
            tokens.add(make_token(cur));
            cur = "";
          }
          cur += chr;
        }
      }
    }

    if (cur.length() > 0)
      tokens.add(make_token(cur));

    return tokens;
  }
}

// Parser:
// ==========================================
class Parser {
  ArrayList<Token> tokens;
  int cur;

  // Potenz vor Punkt vor Strich
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

  public double parse(int level) {
    double result = 0;

    if (take(TokenValue.class)) {
      result = ((TokenValue) token()).value;
    } else if (take(TokenOpen.class)) {
      result = parse(0);
      if (!take(TokenClose.class)) {
        System.out.println("Fehler: Klammern");
        System.exit(1);
      }
    } else if (take(TokenFunction.class)) {
      String str = ((TokenFunction) token()).str;
      result = parse(0);

      if (str.equals("abs"))
        result = Math.abs(result);
      else if (str.equals("sqrt"))
        result = Math.sqrt(result);
      else if (str.equals("sin"))
        result = Math.sin(result);
      else if (str.equals("cos"))
        result = Math.cos(result);
      else if (str.equals("tan"))
        result = Math.tan(result);
      else if (str.equals("asin"))
        result = Math.asin(result);
      else if (str.equals("acos"))
        result = Math.acos(result);
      else if (str.equals("atan"))
        result = Math.atan(result);
      else if (str.equals("exp"))
        result = Math.exp(result);
    }

    while (take(TokenOperator.class)) {
      char chr = ((TokenOperator) token()).operator;

      if (OPERATORS(chr) <= level) {
        back();
        return result;
      }

      double b = parse(OPERATORS(chr));

      if (chr == '+') {
        result = result + b;
      } else if (chr == '-') {
        result = result - b;
      } else if (chr == '*') {
        result = result * b;
      } else if (chr == '/') {
        result = result / b;
      } else if (chr == '^') {
        result = Math.pow(result, b);
      }
    }

    return result;
  }
}

class Calculator {
  public static double calculate(String str) {
    return new Parser(Tokenizer.tokenize(str.toLowerCase())).parse(0);
  }
}

public class Test {
  public static void main(String[] args) {
    Scanner eingabe = new Scanner(System.in);
    String str;

    System.out.println("Taschenrechner");
    System.out
        .println("Operationen: '+', '-', '*', '/', '^', 'sin', 'asin, 'cos', 'acos', 'tan', 'atan', 'exp', 'abs'");
    System.out.println("Konstanten: 'e', 'pi'");
    System.out.println(
        "Hinweis: Alles wird in Bogenmaß gerechnet. Um in Grad zu rechnen hinter der gewünschten Zahl 'd' (degree) platzieren.");
    System.out.println("Syntax: 'sin cos 20' entspricht 'sin(cos(20))'");

    boolean b = true;
    while (b) {
      System.out.print("> ");
      str = eingabe.nextLine();
      System.out.println("Ergebnis: " + Calculator.calculate(str));
    }

    eingabe.close();
  }
}
