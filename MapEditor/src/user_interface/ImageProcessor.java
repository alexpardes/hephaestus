package user_interface;

import java.util.ArrayList;
import java.util.List;

import javafx.scene.image.Image;
import javafx.scene.image.PixelReader;
import javafx.scene.image.PixelWriter;
import javafx.scene.image.WritableImage;
import javafx.scene.paint.Color;

public class ImageProcessor {

	public static Image createPlayerImage(Image base, Image mask, int playerNum) {
		Color playerColor = MapEditor.instance.getPlayerColor(playerNum);

		PixelReader maskReader = mask.getPixelReader();
		PixelReader baseReader = base.getPixelReader();
		
		WritableImage playerImage = new WritableImage((int)base.getWidth(),
				(int)base.getHeight());
		PixelWriter writer = playerImage.getPixelWriter();

		assert mask.getWidth() == base.getWidth();
		assert mask.getHeight() == base.getHeight();

		for (int x = 0; x < base.getWidth(); ++x) {
			for (int y = 0; y < base.getHeight(); ++y) {
				Color maskColor = maskReader.getColor(x, y);
				Color color = baseReader.getColor(x, y);
				if (maskColor.getOpacity() > 0.0) {
					color = adjustLightness(playerColor, color);
				}
				writer.setColor(x, y, color);
			}
		}

		return playerImage;
	}
	
	private static Color adjustLightness(Color base, Color mask) {
		double maskLightness = getLightness(mask);
		double newLightness = scaleLightness(getLightness(base), maskLightness);
		double hslSaturation = getHslSaturation(base);
		
		return getColor(base.getHue(), hslSaturation, newLightness);
	}
	
	// A mask of 0 returns 0, a mask of 1 returns 1, and a mask of .5 returns
	// the base. Intermediate masks interpolate.
	private static double scaleLightness(double base, double mask) {
		double maskWeight = 2*Math.abs(mask - 0.5);
		return (1 - maskWeight)*base + maskWeight*mask;
	}
	
	private static Color getColor(double hue, double saturation, double lightness) {
		return Color.hsb(hue, getSaturation(saturation, lightness),
				getBrightness(saturation, lightness));
	}
	
	private static double getLightness(Color color) {
		return (2 - color.getSaturation()) * color.getBrightness() / 2;
	}
	
	private static double getHslSaturation(Color color) {
		double result = color.getSaturation();
		if (result > 0.0 && result < 1.0) {
			result = color.getSaturation()*color.getBrightness();
			result /= 1 - Math.abs(1
					- (2 - color.getSaturation())*color.getBrightness());
			if (result < 0.0) result = 0.0;
			if (result > 1.0) result = 1.0;
		}
		return result;
	}
	
	private static double getSaturation(double hslSaturation, double lightness) {
		double result = 0.0;
		if (lightness > 0.0) {
			double temp = 1 - Math.abs(1 - 2*lightness);
			result = 2*hslSaturation*temp;
			result /= (2*lightness + temp);
			if (result < 0.0) result = 0.0;
			if (result > 1.0) result = 1.0;
		}
		return result;
	}
	
	private static double getBrightness(double hslSaturation, double lightness) {
		double temp = 1.0 - Math.abs(1.0 - 2.0*lightness);
		double result = (2.0*lightness + hslSaturation*temp) / 2.0;
		if (result < 0.0) result = 0.0;
		if (result > 1.0) result = 1.0;
		return result;
	}
}
