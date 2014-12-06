#include "stdafx.h"
#include "Coloring.h"

sf::Image Coloring::ColorImage(sf::Image image,
                               const sf::Image& mask,
                               const sf::Color& color) {
  HslColor hslColor = ConvertToHsl(color);
  for (unsigned int x = 0; x < image.getSize().x; ++x) {
    for (unsigned int y = 0; y < image.getSize().y; ++y) {
      sf::Color baseColor = image.getPixel(x, y);
      if (mask.getPixel(x, y).a) {
        sf::Color newColor = AdjustLightness(hslColor, Lightness(baseColor));
        newColor.a = baseColor.a;
        image.setPixel(x, y, newColor);
      }
    }
  }
  return image;
}

double Coloring::Lightness(const sf::Color& color) {
  double red = color.r / 255.0;
  double green = color.g / 255.0;
  double blue = color.b / 255.0;

  double max = Max(red, green, blue);
  double min = Min(red, green, blue);

  double lightness = (max + min) / 2;
  return lightness;
}

double Coloring::Saturation(const sf::Color& color) {
  double red = color.r / 255.0;
  double green = color.g / 255.0;
  double blue = color.b / 255.0;

  double max = Max(red, green, blue);
  double min = Min(red, green, blue);

  double saturation;
  if (max == min) {
    saturation = 0.0;
  } else {
    double difference = max - min;
    saturation = Lightness(color) > 0.5 ? (2 - max - min) : difference / (max + min);
  }

  return saturation;
}

double Coloring::Hue(const sf::Color& color) {
  double red = color.r / 255.0;
  double green = color.g / 255.0;
  double blue = color.b / 255.0;

  double max = Max(red, green, blue);
  double min = Min(red, green, blue);

  double hue;
  if (max == min) {
    hue = 0.0;
  } else {
      double difference = max - min;
      if (max == red) {
        hue = (green - blue) / difference + (green < blue ? 6 : 0);
      } else if (max == green) {
        hue = (blue - red) / difference + 2;
      } else {
        hue = (red - green) / difference + 4;
      }
      hue /= 6.0;
  }
  
  return hue;
}

sf::Color Coloring::AdjustLightness(HslColor newColor,
                                    double baseLightness) {
   double weight = newColor.lightness > 0.5 ? 
      (1 - baseLightness) : baseLightness;
   newColor.lightness = baseLightness + weight * (2*newColor.lightness - 1);
   return ConvertToRgb(newColor);
}

HslColor Coloring::ConvertToHsl(const sf::Color& color) {
  HslColor result;
  result.hue = Hue(color);
  result.saturation = Saturation(color);
  result.lightness = Lightness(color);
  result.alpha = color.a / 255.0;
  return result;
}

double Coloring::HueToRgb(double p, double q, double t) {
  double result = 0.0;
  if (t < 0) {
    t += 1;
  } else if (t > 1) {
    t -= 1;
  }
  if (t < 1.0 / 6) {
    result = p + (q - p) * 6 * t;
  } else if (t < 0.5) {
    result = q;
  } else if (t < 2.0 / 3) {
    result = p + (q - p) * (2.0 / 3 - t) * 6;
  } else {
    result = p;
  }
  return result;
}

sf::Color Coloring::ConvertToRgb(HslColor color) {
  double red, green, blue;
  if (color.saturation == 0) {
    red = 0;
    green = 0;
    blue = 0;
  } else {
    double q = color.lightness < 0.5 ?
        color.lightness * (1 + color.saturation) :
        color.lightness + color.saturation - color.lightness*color.saturation;
    double p = 2 * color.lightness - q;
    red = HueToRgb(p, q, color.hue + 1.0 / 3);
    green = HueToRgb(p, q, color.hue);
    blue = HueToRgb(p, q, color.hue - 1.0 / 3);
  }

  int redInt = int(255 * red + 0.5);
  int greenInt = int(255 * green + 0.5);
  int blueInt = int(255 * blue + 0.5);
  int alphaInt = int(255 * color.alpha + 0.5);
  return sf::Color(redInt, greenInt, blueInt, alphaInt);
}

double Coloring::Max(double a, double b, double c) {
  double result = DBL_MIN;
  if (a > result) result = a;
  if (b > result) result = b;
  if (c > result) result = c;
  return result;
}

double Coloring::Min(double a, double b, double c) {
  double result = DBL_MAX;
  if (a < result) result = a;
  if (b < result) result = b;
  if (c < result) result = c;
  return result;
}