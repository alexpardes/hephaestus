package pathing;

import java.awt.Point;

public class ObstacleCorner {
	public enum Direction {
		UP_LEFT, UP_RIGHT, DOWN_LEFT, DOWN_RIGHT
	}
	
	private Point location;
	private Direction direction;
	
	public ObstacleCorner(Point location, Direction direction) {
		this.location = location;
		this.direction = direction;
	}
	
	public ObstacleCorner(int x, int y, Direction direction) {
		this(new Point(x, y), direction);
	}
	
	public boolean equals(Object obj) {
		boolean result = false;
		if (obj instanceof ObstacleCorner) {
			ObstacleCorner that = (ObstacleCorner)obj;
			result = location.equals(that.location)
					&& direction.equals(that.direction);
		}
		return result;
	}
	
	public Point getPoint(int size) {
		if (size == 0) return location;
		Point offset;
		switch (direction) {
			case UP_LEFT:
				offset = new Point(-size, -size);
				break;				
				
			case UP_RIGHT:
				offset = new Point(1, -size);
				break;
				
			case DOWN_LEFT:
				offset = new Point(-size, 1);
				break;
				
			case DOWN_RIGHT:
				offset = new Point(1, 1);
				break;
				
			default:
				offset = null;
		}
		return PointUtil.add(location, offset);
	}
	
	public Direction Direction() {
		return direction;
	}
}
