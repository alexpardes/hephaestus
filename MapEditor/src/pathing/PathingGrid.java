package pathing;

import java.awt.Point;
import java.util.*;

public class PathingGrid {
	private List<List<Boolean>> isBlocked;
	
	public PathingGrid(int width, int height) {
		isBlocked = new ArrayList<List<Boolean>>(width);
		while (isBlocked.size() < width) {
			List<Boolean> column = new ArrayList<Boolean>(height);
			while (column.size() < height) {
				column.add(false);
			}			
			isBlocked.add(column);
		}
	}
	
	public PathingGrid(Point tile) {
		this(tile.x, tile.y);
	}
	
	public boolean containsPoint(int x, int y) {
		return x >= 0 && x < width() && y >= 0 && y < height();
	}
	
	public int width() { return isBlocked.size(); }
	public int height() { return isBlocked.get(0).size(); }
	
	public boolean isBlocked(int x, int y) {
		
		return containsPoint(x, y) && isBlocked.get(x).get(y);
	}
	
	public boolean isUnblocked(int x, int y) {
		return containsPoint(x, y) && !isBlocked.get(x).get(y);
	}
	
	public boolean isUnblocked(Point tile) {
		return isUnblocked(tile.x, tile.y);
	}
	
	public boolean isBlocked(Point tile) {
		return isBlocked(tile.x, tile.y);
	}
	
	public void setBlocked(int x, int y, boolean isBlocked) {
		this.isBlocked.get(x).set(y, isBlocked);
	}
	
	public void setBlocked(int x, int y) {
		setBlocked(x, y, true);
	}
	
	public void setBlocked(Point tile) {
		setBlocked(tile.x, tile.y);
	}
	
	public void setBlocked(Point tile, boolean isBlocked) {
		setBlocked(tile.x, tile.y, isBlocked);
	}
	
	public List<Point> getUnblockedNeighbors(Point tile) {
		List<Point> result = new ArrayList<Point>();
		for (int xOffset = -1; xOffset <= 1; ++xOffset) {
			for (int yOffset = -1; yOffset <= 1; ++yOffset) {
				if (xOffset!= 0 || yOffset != 0) {
					Point neighbor = new Point(tile.x + xOffset,
							tile.y + yOffset);
					if (isUnblocked(neighbor)) {
						result.add(neighbor);
					}
				}
			}
		}
		return result;
	}
}
