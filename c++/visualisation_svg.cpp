#include "visualisation_svg.h"

Style::Style(const Mode _mode) : mode(_mode)
{
  assert(mode != PERSONALIZED);
  if (mode == LIGHT)
  {
    color_background = "#fff";
    color_axis = "#000";
    color_lines = "#E5E5E5";
    color_lines_highlighted = "#C0C0C0";
  }
  else if (mode == DARK)
  {
    color_background = "#333333";
    color_axis = "#fff";
    color_lines = "#424242";
    color_lines_highlighted = "#5C5C5C";
  }
  else
  {
    color_background = "#000";
    color_axis = "#fff";
    color_lines = "#1A1A1A";
    color_lines_highlighted = "#3F3F3F";
  }
  color_nums = color_axis;
};

Style::Style(const string _color_background, const string _color_axis, const string _color_lines, const string _color_lines_highlighted, const string _color_nums) : mode(PERSONALIZED), color_background(_color_background), color_axis(_color_axis), color_lines(_color_lines), color_lines_highlighted(_color_lines_highlighted), color_nums(_color_nums){};

void GraphSVG::drawInterlines(const float min_val, const float max_val, const float tick, std::function<void(float, bool)> stn)
{
  int counter = 0;
  if (min_val <= 0 && max_val >= 0)
  {
    for (float x = -tick; x > min_val; x -= tick)
      stn(x, ((++counter >= higlighting_step) ? counter = 0, true : false));
    counter = 0;
    for (float x = tick; x < max_val; x += tick)
      stn(x, ((++counter >= higlighting_step) ? counter = 0, true : false));
  }
  else if (max_val < 0)
    for (float x = max_val - (float)fmod(max_val, tick); x > min_val; x -= tick) // flaot inacurracy, reason for partly ugly layout (left, right)
      stn(x, ((++counter >= higlighting_step) ? counter = 0, true : false));
  else if (min_val > 0)
    for (float x = min_val + (float)fmod(min_val, tick); x < max_val; x += tick)
      stn(x, ((++counter >= higlighting_step) ? counter = 0, true : false));
}

template <typename T>
void GraphSVG::addAttribute(const string &identifier, const T value) { pic << " " << identifier << "=\"" << value << "\""; }

void GraphSVG::drawLine(float x1, float y1, float x2, float y2, const string stroke, const float stroke_width)
{
  pic << "<line";
  addAttribute("stroke", stroke);
  addAttribute("fill", "transparent");
  addAttribute("stroke-width", stroke_width);
  addAttribute("x1", x1);
  addAttribute("y1", y1);
  addAttribute("x2", x2);
  addAttribute("y2", y2);
  pic << "></line>\n";
}

void GraphSVG::drawHorizontal(float x1, float y, float x2, const string stroke, const float stroke_width) { drawLine(x1, y, x2, y, stroke, stroke_width); }

void GraphSVG::drawVertical(float x, float y1, float y2, const string stroke, const float stroke_width) { drawLine(x, y1, x, y2, stroke, stroke_width); }

float GraphSVG::canvasXFromX(float x) { return (float)width * ((x - xMin) / (xMax - xMin)); }

float GraphSVG::canvasYFromY(float y) { return (float)height - (float)height * ((y - yMin) / (yMax - yMin)); }

array<float, 2> GraphSVG::canvasPtFromXY(float x, float y) { return array<float, 2>{(float)width * ((x - xMin) / (xMax - xMin)), (float)height - ((y - yMin) / (yMax - yMin)) * (float)height}; }

GraphSVG::GraphSVG(const string filename, const int _width, const int _height, float _xMin, float _xMax, float _yMin, float _yMax, EqualizeType equalizetype, Style _style_mode, bool drawAxes) : width(_width), height(_height), set_window(false), xMin(_xMin), xMax(_xMax), yMin(_yMin), yMax(_yMax), style_mode(_style_mode)
{
  pic.open(filename);
  pic << std::setprecision(5);

  const string style = "background-color:" + style_mode.color_background + ";shape-rendering:geometricPrecision; text-rendering:geometricPrecision; image-rendering:optimizeQuality; fill-rule:evenodd; clip-rule:evenodd;";
  pic << "<svg width=\"" << width << "\" height=\"" << height << "\" style=\"" << style << "\" xmlns=\"" << url << "\"><style>.function:hover{stroke-width: 4;transition: all ease 0.3s;}.label:hover{cursor: default;pointer-events: none;}.label{cursor: default;pointer-events: none;}</style>\n";
  pic << "<title>Calculator</title>\n";
  if (equalizetype == EqualizeXAxes)
  {
    float xMid = (xMax + xMin) / 2.0f;
    float half = (((float)width * (yMax - yMin)) / (float)height) / 2.0f;
    xMin = xMid - half;
    xMax = xMid + half;
  }
  else if (equalizetype == EqualizeYAxes)
  {
    float yMid = (yMax + yMin) / 2.0f;
    float half = (((float)height * (xMax - xMin)) / (float)width) / 2.0f;
    yMin = yMid - half;
    yMax = yMid + half;
  }
  else if (equalizetype == EqualizeAuto)
  {
    float xRatio = (xMax - xMin) / (float)width;
    float yRatio = (yMax - yMin) / (float)height;
    if (xRatio < yRatio)
    {
      float xMid = (xMax + xMin) / 2.0f;
      float half = (yRatio / xRatio) * (xMax - xMin) / 2.0f;
      xMin = xMid - half;
      xMax = xMid + half;
    }
    else
    {
      float yMid = (yMax + yMin) / 2.0f;
      float half = (xRatio / yRatio) * (yMax - yMin) / 2.0f;
      yMin = yMid - half;
      yMax = yMid + half;
    }
  }

  float tmp_x = (float)perfect_val_len * (xMax - xMin) / (float)width;
  int ceros_x = (int)floor(log10(tmp_x));
  if (ceros_x != 0)
    tmp_x /= (float)pow(10, ceros_x);

  if (tmp_x < 1.5)
    dif_tick_x = 1 * (float)pow(10, ceros_x);
  else if (tmp_x < 3.5)
    dif_tick_x = 2 * (float)pow(10, ceros_x);
  else if (tmp_x < 7.5)
    dif_tick_x = 5 * (float)pow(10, ceros_x);
  else
    dif_tick_x = (float)pow(10, ceros_x + 1);

  float tmp_y = (float)perfect_val_len * (yMax - yMin) / (float)height;
  int ceros_y = (int)floor(log10(tmp_y));
  if (ceros_y != 0)
    tmp_y /= (float)pow(10, ceros_y);

  if (tmp_y < 1.5)
    dif_tick_y = 1 * (float)pow(10, ceros_y);
  else if (tmp_y < 3.5)
    dif_tick_y = 2 * (float)pow(10, ceros_y);
  else if (tmp_y < 7.5)
    dif_tick_y = 5 * (float)pow(10, ceros_y);
  else
    dif_tick_y = (float)pow(10, ceros_y + 1);

  if (drawAxes)
  {
    array<float, 2> leftPt = canvasPtFromXY(xMin, 0);
    array<float, 2> rightPt = canvasPtFromXY(xMax, 0);
    array<float, 2> botPt = canvasPtFromXY(0, yMin);
    array<float, 2> topPt = canvasPtFromXY(0, yMax);

    drawInterlines(xMin, xMax, dif_tick_x / 5, [=](float x, bool highlighted) -> void { drawVertical(canvasXFromX(x), botPt[1], topPt[1], ((highlighted) ? style_mode.color_lines_highlighted : style_mode.color_lines), 1.5f); });
    drawInterlines(yMin, yMax, dif_tick_y / 5, [=](float y, bool highlighted) -> void { drawHorizontal(leftPt[0], canvasYFromY(y), rightPt[0], ((highlighted) ? style_mode.color_lines_highlighted : style_mode.color_lines), 1.5f); });

    if (0 <= leftPt[1] && rightPt[1] <= (float)height)
      drawLine(leftPt[0], leftPt[1], rightPt[0], rightPt[1], style_mode.color_axis, 1.5f);
    if (0 <= botPt[0] && topPt[0] <= (float)width)
      drawLine(botPt[0], botPt[1], topPt[0], topPt[1], style_mode.color_axis, 1.5f);
  }
};

GraphSVG::~GraphSVG()
{
  if (pic.is_open())
  {
    pic.flush();
    pic.close();
  }
}

void GraphSVG::drawFn(const NodePtr &f, int varId, vector<decimal> variables, const int numFnPts, const string stroke, const float stroke_width)
{
  float xDelta = (xMax - xMin) / (float)(numFnPts - 1);
  vector<pair<float, float>> pts;

  pic << "<path";
  addAttribute("class", "function");
  addAttribute("stroke", stroke);
  addAttribute("fill", "transparent");
  addAttribute("stroke-width", stroke_width);
  pic << " d=\"";

  bool first_iteration = true, first_point = true, jump = true;
  float last_x, last_y;

  for (int i = 0; i < numFnPts; ++i)
  {
    float x, xTarget = xMin + (float)i * xDelta;
    do
    {
      x = xTarget;

      variables[varId] = x;
      float y = (float)eval(f, variables);
      array<float, 2> canvasPt = canvasPtFromXY(x, y);

      // float perc = 0.5;
      // while (!first_point && abs(last_y - canvasPt[1]) > 30 && perc >= 0.5)
      // {
      //   x = (1.0f - perc) * xPrev + perc * xTarget;
      //   variables[varId] = x;
      //   float y = (float)eval(f, variables);
      //   canvasPt = canvasPtFromXY(xMin, xMax, yMin, yMax, x, y);
      //   perc /= 2.0f;
      // }

      if (0 <= canvasPt[1] && canvasPt[1] <= (float)height)
      {
        if (jump)
        {
          if (first_point)
          {
            pic << "M";
            if (!first_iteration)
              pic << last_x << " " << last_y << " L";
          }
          else
            pic << " M" << last_x << " " << last_y << " L";
          first_point = false;
          jump = false;
        }
        else
          pic << " L";

        pic << canvasPt[0] << " " << canvasPt[1];
      }
      else
      {
        if (!jump)
        {
          const int val = (canvasPt[1] < 0) ? 0 : height;
          pic << " L" << (((float)val - canvasPt[1]) * (canvasPt[0] - last_x)) / ((canvasPt[1] - last_y)) + canvasPt[0] << " " << val;
        }
        jump = true;
      }

      last_x = canvasPt[0];
      last_y = canvasPt[1];
      first_iteration = false;

    } while (x < xTarget);
  }

  pic << "\"></path>\n";
}

void GraphSVG::drawCircle(float x, float y, float r, const string col)
{
  pic << "<circle";
  addAttribute("cx", x);
  addAttribute("cy", y);
  addAttribute("r", r);
  addAttribute("fill", col);
  pic << "/>\n";
}

template <typename Text>
void GraphSVG::drawText(const float x, const float y, const string col, const Text text)
{
  pic << "<text";
  addAttribute("fill", col);
  // addAttribute("stroke", "#333333");
  addAttribute("dominant-baseline", "central");
  addAttribute("text-anchor", "middle");
  addAttribute("x", x);
  addAttribute("y", y);
  pic << ">" << text << "</text>\n";
}

void GraphSVG::drawTextCircle(float x, float y, const float num, const string col, const int rad, const int shift_x, const int shift_y)
{
  x = (float)shift_x + canvasXFromX((float)x);
  y = (float)shift_y + canvasYFromY((float)y);
  drawCircle(x, y, (float)rad, col);
  drawText(x, y, style_mode.color_nums, num);
}

void GraphSVG::save()
{
  float axis_shift_x, axis_shift_y;
  int shift_x_axis = num_shift_x_axis, shift_y_axis = num_shift_y_axis;

  if (xMin <= 0 && xMax >= 0)
    axis_shift_x = 0;
  else if (xMax < 0)
    axis_shift_x = xMax, shift_x_axis = -shift_x_axis;
  else if (xMin > 0)
    axis_shift_x = xMin;

  if (yMin <= 0 && yMax >= 0)
    axis_shift_y = 0;
  else if (yMax < 0)
    axis_shift_y = yMax;
  else if (yMin > 0)
    axis_shift_y = yMin, shift_y_axis = -shift_y_axis;

  drawInterlines(xMin, xMax, dif_tick_x, [=](float x, bool highlighted) -> void { drawTextCircle(x, axis_shift_y, x, (highlighted) ? style_mode.color_background : style_mode.color_background, nums_circle_radius, 0, shift_y_axis); }); // change color
  drawInterlines(yMin, yMax, dif_tick_y, [=](float y, bool highlighted) -> void { drawTextCircle(axis_shift_x, y, y, (highlighted) ? style_mode.color_background : style_mode.color_background, nums_circle_radius, shift_x_axis, 0); }); // change color

  if ((yMin <= 0 && 0 <= yMax) || (xMin <= 0 && 0 <= xMax))
    drawTextCircle(axis_shift_x, axis_shift_y, 0, style_mode.color_background, nums_circle_radius, num_shift_x_axis, num_shift_y_axis);

  pic << "</svg>";
  pic.flush();
  pic.close();
}