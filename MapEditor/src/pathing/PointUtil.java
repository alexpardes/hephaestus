package pathing;

import java.awt.Point;

public class PointUtil {
	public static Point add(Point p1, Point p2) {
		return new Point(p1.x + p2.x, p1.y + p2.y);
	}
	
	// Returns a point giving the direction from p1 to p2.
	// (1, -1) means p2 has greater y and lesser x than p1.
	public static Point getDirection(Point p1, Point p2) {
		int x, y;
		int dx = p2.x - p1.x;
		if (dx == 0) {
			x = 0;
		} else {
			x = dx > 0 ? 1 : -1;
		}
		
		int dy = p2.y - p1.y;
		if (dy == 0) {
			y = 0;
		} else {
			y = dy > 0 ? 1 : -1;
		}
		
		return new Point(x, y);
	}
	
	// Returns the octile distance between p1 and p2.
	public static double distanceBetween(Point p1, Point p2) {
		int dx = Math.abs(p1.x - p2.x);
		int dy = Math.abs(p1.y - p2.y);
		return Math.max(dx, dy) + (Math.sqrt(2) - 1) * Math.min(dx, dy);
	}
}
