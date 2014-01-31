package pathing;

import java.awt.Point;
import java.util.ArrayList;
import java.util.Collection;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.PriorityQueue;

import pathing.ObstacleCorner.Direction;

public class SubgoalGraph {
	private PathingGrid grid;
	private List<ObstacleCorner> corners;
	private List<Node<ObstacleCorner>> subgoalTable;
	
	// Maps from subgoal points to node indices in subgoalTable. 
	private Map<Point, Integer> subgoalMap;
	
	public SubgoalGraph(PathingGrid grid) {
		this.grid = grid;
	}
	
	private void findCorners() {
		corners = new ArrayList<ObstacleCorner>();
		for (int x = 0; x < grid.width(); ++x) {
			for (int y = 0; y < grid.height(); ++y) {
				corners.addAll(getCorners(x, y));
			}
		}
	}
	
	public List<ObstacleCorner> getCorners() {
		return corners;
	}
	
	// Returns all corners at the given location.
	private List<ObstacleCorner> getCorners(int x, int y) {
		List<ObstacleCorner> corners = new ArrayList<ObstacleCorner>();
		Point point = new Point(x, y);
		
		if (grid.isBlocked(x, y)) {
			if (isCorner(x, y, -1, -1)) {
				corners.add(new ObstacleCorner(point, Direction.UP_LEFT));
			}
			if (isCorner(x, y, 1, -1)) {
				corners.add(new ObstacleCorner(point, Direction.UP_RIGHT));
			}
			if (isCorner(x, y, -1, 1)) {
				corners.add(new ObstacleCorner(point, Direction.DOWN_LEFT));
			}
			if (isCorner(x, y, 1, 1)) {
				corners.add(new ObstacleCorner(point, Direction.DOWN_RIGHT));
			}				
		}
		return corners;
	}
	
	// xOffset, yOffset should each be 1 or -1.
	private boolean isCorner(int x, int y, int xOffset, int yOffset) {
		boolean result = false;
		if (grid.isUnblocked(x + xOffset, y)
				&& grid.isUnblocked(x, y + yOffset)
				&& grid.isUnblocked(x + xOffset, y + yOffset)) {
			result = true;
		}
		return result;
	}
		
	
	private boolean canMoveInDirection(Point point, Point direction) {
		return grid.isUnblocked(new Point(point.x + direction.x, point.y))
				&& grid.isUnblocked(new Point(point.x, point.y + direction.y))
				&& grid.isUnblocked(new Point(point.x + direction.x,
						point.y + direction.y));
	}
	
	// Tests if point1 and point2 are connected by arbitrary h-length path,
	// where h is the octile distance from point1 to point2. 
	public boolean areDirectlyConnected(Point point1, Point point2) {
		if (point1.equals(point2)) {
			return true;
		}
		
		Point direction = PointUtil.getDirection(point1, point2);
		if (!canMoveInDirection(point1, direction)) {
			return false;
		}
		Point nextPoint = PointUtil.add(point1, direction);
		return areDirectlyConnected(nextPoint, point2);
	}
	
	// Assumes find corners has been run.
	private void makeSubgoals(int size) {
		subgoalTable = new ArrayList<Node<ObstacleCorner>>(corners.size());
		subgoalMap = new HashMap<Point, Integer>(corners.size());
		for (ObstacleCorner corner : corners) {
			Point point = corner.getPoint(size);
			subgoalMap.put(point, subgoalTable.size());			
			subgoalTable.add(new Node<ObstacleCorner>(corner));
		}
	}
	
	public void connectSubgoals() {
		findCorners();			
		makeSubgoals(1);
		for (Node<ObstacleCorner> subgoal : subgoalTable) {
			connectToSubgoals(subgoal);
		}
	}
	
	// Returns the list of subgoals which can be directly reached
	// from the point.
	private void connectToSubgoals(Node<ObstacleCorner> startSubgoal) {
		Point point = startSubgoal.value().getPoint(1);
		PriorityQueue<Node<ObstacleCorner>> subgoalQueue =
				new PriorityQueue<Node<ObstacleCorner>>(corners.size(),
				new SubgoalComparator(point));
		for (Node<ObstacleCorner> subgoal : subgoalTable) {
			subgoalQueue.add(subgoal);
		}
		
		Collection<GridRegion> blockedRegions = new ArrayList<GridRegion>();
		
		while (!subgoalQueue.isEmpty()) {
			Node<ObstacleCorner> subgoal = subgoalQueue.poll();
			Point q = subgoal.value().getPoint(1);
			if (point.equals(q)) continue;
			
			boolean blocked = false;
			for (GridRegion region : blockedRegions) {
				if (region.contains(q)) {
					blocked = true;
					break;
				}
			}
			if (!blocked && areDirectlyConnected(point, q)) {
				startSubgoal.addNeighbor(subgoal);
				blockedRegions.add(GridRegion.getIndirect(point, q));
			}
		}		
	}	
	
	public List<ObstacleCorner> getSubgoalTable() {
		List<ObstacleCorner> pointTable =
				new ArrayList<ObstacleCorner>(subgoalTable.size());
		for (Node<ObstacleCorner> subgoal : subgoalTable) {
			pointTable.add(subgoal.value());
		}
		return pointTable;
	}
	
	public List<Integer> getAdjacencies(int subgoalIndex) {
		Collection<Node<ObstacleCorner>> adjacencies =
				subgoalTable.get(subgoalIndex).getNeighbors();
		List<Integer> indices = new ArrayList<Integer>(adjacencies.size());
		for (Node<ObstacleCorner> subgoal : adjacencies) {
			indices.add(subgoalMap.get(subgoal.value().getPoint(1)));
		}
		return indices;
	}
}
