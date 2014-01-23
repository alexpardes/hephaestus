package pathing;

import java.awt.Point;
import java.util.ArrayList;
import java.util.List;

public class SearchNode implements Comparable<SearchNode>{
	private Point location;
	private double cost;
	private PathingGrid grid;
	
	public SearchNode(PathingGrid grid, Point point, double cost) {
		this.grid = grid;
		this.location = point;
		this.cost = cost;
	}

	@Override
	public int compareTo(SearchNode other) { 
		return Double.compare(cost, other.cost);
	}
	
	public List<SearchNode> getNeighbors() {
		List<SearchNode> result = new ArrayList<SearchNode>();
		for (Point point : grid.getUnblockedNeighbors(location)) {
			double newCost = location.distance(point) + cost;
			result.add(new SearchNode(grid, point, newCost));
		}
		return result;
	}
	
	public double cost() { return cost; }
	public Point point() { return location; }

}
