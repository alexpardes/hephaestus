#pragma once
#include <SFML/Graphics.hpp>

struct HslColor {
  double hue, saturation, lightness, alpha;
};

class Coloring {
  public:
    static sf::Image ColorImage(sf::Image image, const sf::Image& mask,
        const sf::Color& color);

    static double Hue(const sf::Color& color);
    static double Saturation(const sf::Color& color);
    static double Lightness(const sf::Color& color);
    static sf::Color AdjustLightness(HslColor color,
        double baseLightness);
    static HslColor ConvertToHsl(const sf::Color& color);
    static sf::Color ConvertToRgb(HslColor color);
    static double HueToRgb(double p, double q, double t);
    static double Max(double a, double b, double c);
    static double Min(double a, double b, double c);
};