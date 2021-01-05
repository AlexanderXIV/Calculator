#include "defs.h"
// #include "mathematics.h"
#include "parser.h"
// #include "visualisation_png.h"
#include "visualisation_svg.h"

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <chrono>
#include <map>
#include <assert.h>

#include <math.h>
#include <algorithm>
#include <functional>
#include <fstream>

using std::abs;
using std::cin;
using std::cout;
using std::endl;
using std::map;
using std::pair;
using std::string;
using std::vector;

// hint: use exceptions instead of assert()
// hint: Performance over code redundancy

long benchmark(std::function<void()> f)
{
  auto start = std::chrono::system_clock::now();
  f();
  auto end = std::chrono::system_clock::now();
  return std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
}

// compile with ./a.out example.svg 1920 1080 -6.2 4 -8.5 10.35 auto dark "sin(x)" "#d0f" 5.0 "cos(x)" "#ff0" 2.0
int main(int argc, char *argv[])
{
  std::ios::sync_with_stdio(false);
  cin.tie(0);
  cout << std::setprecision(8);

  if (argc > 1)
  {
    const string filename = argv[1];
    const int resolution_height = std::stoi(argv[2]);
    const int resolution_width = std::stoi(argv[3]);
    const float window_left = std::stof(argv[4]);
    const float window_right = std::stof(argv[5]);
    const float window_bottom = std::stof(argv[6]);
    const float window_top = std::stof(argv[7]);
    const string str_equalize = argv[8];
    const string mode_typ = argv[9];

    Mode style_typ;
    if (mode_typ == "dark")
      style_typ = DARK;
    else if (mode_typ == "super_dark")
      style_typ = SUPER_DARK;
    else if (mode_typ == "light")
      style_typ = LIGHT;

    EqualizeType equalize;
    if (str_equalize == "x")
      equalize = EqualizeXAxes;
    else if (str_equalize == "y")
      equalize = EqualizeYAxes;
    else if (str_equalize == "auto")
      equalize = EqualizeAuto;
    else
      equalize = EqualizeNone;

    auto start = std::chrono::system_clock::now();
    GraphSVG g(filename, resolution_height, resolution_width, window_left, window_right, window_bottom, window_top, equalize, Style(style_typ));
    int i = 10;
    while (i < argc)
    {
      map<string, int> vars;
      NodePtr p = parse(string(argv[i]), vars);
      assert(vars.size() > 0);
      const string color = argv[++i];
      float stroke_width = std::stof(string(argv[++i]));
      i++;
      vector<decimal> vars2(vars.size(), 0);
      g.drawFn(p, 0, vars2, resolution_width, color, stroke_width);
    }
    g.save();
    auto end = std::chrono::system_clock::now();
    cout << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms" << endl;
  }
  else
  {
    // input 2e -> 2 * e : mal ergaenzen
    // Schreibweise "sin 4" anstatt "sin(4)" testen

    // Wann lohnt es sich eine Referenz / den Wert zu übergeben? -> double ???
    // for-schleife auto oder size_t

    // benchmark: "-(-4-5*x--2)+7*(3-2)+2*sin(0.2)*log(4,5)(x+1)" parse, simplify, derive, simplify ~ 60.000ns

    // double result;
    // if (brents_fun([](double x) { return (x * x * x - 3 * x * x + 2 * x); }, -1, 3, 1e-8, 1e4, result))
    //   cout << "success: " << result;
    // else
    //   cout << "failed";
    // cout << endl;

    // vector<double> v11;
    // cout << find_all_roots([](double x) { return (x * x * x - 3 * x * x + 2 * x); }, -1, 3, 0.01, v11) << endl;
    // for (auto &e : v11)
    //   cout << e << endl;
    // cout << endl;

    // Complex aa(5, 3); // = 5 + 3i
    // Complex b(1, -1); // = 1 - i
    // pair<Complex, Complex> z1 = sqrt(aa + Complex(1, -1));
    // cout << z1 << endl;
    // cout << ln(aa + Complex(1, -1)) << endl;
    // cout << cbrt(aa + Complex(1, -1)) << endl;
    // cout << n_root(aa + Complex(1, -1), 3) << endl;
    // cout << aa / 3 << endl;
    // for (auto it : quart(1, 2, 3, 4, 5))
    //   cout << it << " ";
    // cout << endl;

    string input;

    // input = "-2(-x+1)x(x-1)(4x)*zy+variableX7";
    // input = "-1+7*(3-2)+2*sin(0.2)*log(4,5)";
    // input = "-(-4-5*x--2)+7*(3-2)+2*sin(0.2)*log(4,5)*(x-1)"; //(x+1)";
    // input = "x^7";
    // input = "x / sin(x)";
    // input = "log(2,x)";
    // input = "ln(x) / ln(2)";
    // input = "ln(x)";
    // input = "ln(2)";
    // input = "2*e*pi";
    // input = "x^(x^2)";
    // input = "(x^2)^3)";
    // input = "3x(x+1)";
    // input = "3x";
    // input = "sin(x) * cos(x)";
    // input = "tan(x^2)";
    // input = "log(2, 1 - 3 * x)";
    // input = "sqrt(2^x)";
    // input = "atan(sqrt(e^x))";

    // input = "atan(1 / x)"; // '= -1/(1+x^2)
    // input = "asin(x - 1)"; // 1 / sqrt(2x - x^2)
    // input = "(1 / a) * atan(x / a)"; // 1 / (a^2 + x^2)
    // input = "atan((x + 1) / (x - 1))"; // -1/(1+x^2)
    // input = "atan(x - sqrt(1 + x ^ 2))"; // 1/(2(1+x^2))
    // input = "arccos(x) * arctan(x)"; // acos(x)/(1+x^2) - atan(x)/sqrt(1-x^2)
    // input = "asin(sqrt(1 - x ^ 2))"; // -x/(abs(x)sqrt(1-x^2)) -> x / abs(x) = sign(x)
    // input = "atan(sqrt(e^x))"; // sqrt(e^x)/(2(1+e^x))
    // input = "asin(1 / sqrt(x))"; // -1/(2x*sqrt(x-1))
    // input = "x * asin(x) + sqrt(1 - x^2)"; // asin(x)
    // error, should be asin(x)

    // input = "asin((1 - x^2) / (1 + x^2)";
    // input = "tan(ln(x))";
    // input = "sin(x) * sin(x)+3";
    // input = "sin(-(4+2x))";
    // input = "sin(acos(x))";
    // input = "acos(sin(x))";
    input = "sin(x)";
    // input = "(2x) / (1 + x^2)"; // 2x / (...) ???

    map<string, int> vars;
    NodePtr f = parse(input, vars);
    NodePtr fs = simplify(f);
    NodePtr d = derive(fs, vars.find("x")->second);
    NodePtr s1 = simplify(d);

    decimal val = 2.7;
    vector<decimal> vars2(vars.size(), 0);
    for (auto &item : vars2)
      item = val;
    cout << "input: '" << input << "'" << endl
         << "f(x) = " << f << endl
         << "     = " << fs << " (simplified)" << endl
         << "f'(x) = " << d << endl
         << "      = " << s1 << " (simplified)" << endl
         << "f(" << val << ") = " << eval(fs, vars2) << endl
         << "f'(" << val << ") = " << eval(d, vars2) << endl
         << "f limits: " << getLimits(f) << endl // geht noch net
         << "inverse f: " << inverse_var(f, 0) << endl
         << "Achensymmetrisch: " << ((axis_symmetic(f, 0)) ? "yes" : "no / maybe") << endl
         << "Punktsymmetrisch: " << ((point_symmetic(f, 0)) ? "yes" : "no / maybe") << endl;

    // GraphPNG g(1920, 1080, -0.5, 10, -0.5, 10);
    // GraphPNG g(1920, 1080, -3, 10, -3, 10);
    // GraphPNG g(1920, 1080, -3, 10);
    // cout << "graph 1: " << benchmark([]() -> void { // ~271 ms
    //   map<string, int> vars;
    //   vector<decimal> vars2(1, 0);

    //   GraphPNG g(1920, 1080, -3, 10, -1.5, 6);
    //   g.drawAxis();
    //   g.drawFunction(parse("x + 1", vars), 0, vars2, BLUE);
    //   g.drawFunction(parse("sin(x)", vars), 0, vars2, RED);
    //   g.drawFunction(parse("cos(x)", vars), 0, vars2, GREEN);
    //   g.drawFunction(parse("tan(x)", vars), 0, vars2, YELLOW);
    //   g.drawFunction(parse("x^2", vars), 0, vars2, CYAN);
    //   g.save("graph.png");
    // }) << "ms"
    //      << endl;

    cout << "graph 1: " << benchmark([]() -> void { // ~20 ms
      map<string, int> vars;
      vector<decimal> vars2(1, 0);

      GraphSVG g("example.svg", 1920, 1080, -6.2f, 10.0f, -8.5f, 10.35f, EqualizeYAxes, Style(SUPER_DARK));
      // GraphSVG g("example.svg", 1920, 1080, -10.2f, -6.0f, 8.5f, 10.35f, EqualizeNone);
      // GraphSVG g("example.svg", 1920, 1080, -10.2f, -6.0f, -10.5f, -8.35f, EqualizeNone);
      g.drawFn(parse("x + 1", vars), 0, vars2, 1920, "#00f", 2.0f);
      g.drawFn(parse("-x", vars), 0, vars2, 1920, "#f0f", 2.0f);
      g.drawFn(parse("sin(x)", vars), 0, vars2, 1920, "#f00", 2.0f);
      g.drawFn(parse("cos(x)", vars), 0, vars2, 1920, "#0f0", 2.0f);
      g.drawFn(parse("tan(x)", vars), 0, vars2, 1920, "#ff0", 2.0f);
      g.drawFn(parse("x^2", vars), 0, vars2, 1920, "#0ff", 2.0f);
      g.save();
    }) << "ms"
         << endl;
  }
};

// ln(|x|) -> abs, ! 1 / x
// https://www.dummies.com/wp-content/uploads/323191.image0.png