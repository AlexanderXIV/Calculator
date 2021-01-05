#pragma once
#include "defs.h"
#include "parser.h"

#include <math.h>
#include <string>
#include <vector>
#include <fstream>
#include <assert.h>
#include <array>

using std::abs;
using std::array;
using std::max;
using std::min;
using std::pair;
using std::string;
using std::vector;

enum EqualizeType
{
  EqualizeNone,
  EqualizeXAxes,
  EqualizeYAxes,
  EqualizeAuto
};

const int higlighting_step = 5;

const int perfect_val_len = 125;

const int nums_circle_radius = 10;
const int num_shift_x_axis = 20;
const int num_shift_y_axis = num_shift_x_axis;

enum Mode
{
  LIGHT,
  DARK,
  SUPER_DARK,
  PERSONALIZED
};

struct Style
{
  Mode mode;
  string color_background, color_axis, color_lines, color_lines_highlighted, color_nums;

  Style(const Mode _mode);

  Style(const string _color_background, const string _color_axis, const string _color_lines, const string _color_lines_highlighted, const string _color_nums);
};

const string url = "http://www.w3.org/2000/svg";
struct GraphSVG
{
  float dif_tick_x, dif_tick_y;
  int width, height;
  bool set_window;
  float xMin, xMax, yMin, yMax;
  Style style_mode;
  std::ofstream pic;

  void drawInterlines(const float min_val, const float max_val, const float tick, std::function<void(float, bool)> stn);

  template <typename T>
  void addAttribute(const string &identifier, const T value);

  void drawLine(float x1, float y1, float x2, float y2, const string stroke, const float stroke_width = 3.0);

  void drawHorizontal(float x1, float y, float x2, const string stroke, const float stroke_width = 3.0);

  void drawVertical(float x, float y1, float y2, const string stroke, const float stroke_width = 3.0);

  float canvasXFromX(float x);

  float canvasYFromY(float y);

  array<float, 2> canvasPtFromXY(float x, float y);

  GraphSVG(const string filename, const int _width, const int _height, float _xMin, float _xMax, float _yMin, float _yMax, EqualizeType equalizetype, Style _style_mode = Style(DARK), bool drawAxes = true);

  ~GraphSVG();

  void drawFn(const NodePtr &f, int varId, vector<decimal> variables, const int numFnPts = 300, const string stroke = "red", const float stroke_width = 3.0);

  void drawCircle(float x, float y, float r, const string col);

  template <typename Text>
  void drawText(const float x, const float y, const string col, const Text text);

  void drawTextCircle(float x, float y, const float num, const string col, const int rad, const int shift_x, const int shift_y);

  void save();
};