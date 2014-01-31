package unit_tests;

import java.awt.Point;
import java.util.Collection;
import java.util.List;

import org.junit.Assert;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.JUnit4;

import pathing.Node;
import pathing.SubgoalComparator;
import pathing.GridRegion;
import pathing.ObstacleCorner;
import pathing.ObstacleCorner.Direction;
import pathing.PathingGrid;
import pathing.PointUtil;
import pathing.SearchNode;
import pathing.SubgoalGraph;

@RunWith(JUnit4.class)
public class TestPathing {	
	@Test
	public void setupPathingGrid() {
		PathingGrid grid = new PathingGrid(3, 3);		
		Assert.assertFalse(grid.isBlocked(2, 2));
		grid.setBlocked(2, 2);
		Assert.assertTrue(grid.isBlocked(2, 2));
		List<Point> neighbors = grid.getUnblockedNeighbors(new Point(1, 1));
		Assert.assertEquals(7, neighbors.size());
	}
	
//	@Test
//	public void findCornersSmall() {
//		PathingGrid grid = new PathingGrid(3, 3);
//		grid.setBlocked(1, 1);
//		SubgoalGraph processor = new SubgoalGraph(grid);
//		processor.findCorners();
//		
//		List<ObstacleCorner> subgoals = processor.getCorners();
//		Point center = new Point(1, 1);
//		Assert.assertEquals(subgoals.size(), 4);
//		Assert.assertTrue(subgoals.contains(
//				new ObstacleCorner(center, Direction.UP_LEFT)));
//		Assert.assertTrue(subgoals.contains(
//				new ObstacleCorner(center, Direction.UP_RIGHT)));
//		Assert.assertTrue(subgoals.contains(
//				new ObstacleCorner(center, Direction.DOWN_LEFT)));
//		Assert.assertTrue(subgoals.contains(
//				new ObstacleCorner(center, Direction.DOWN_RIGHT)));
//	}
	
//	@Test
//	public void findCorners() {
//		PathingGrid grid = new PathingGrid(6, 5);
//		grid.setBlocked(0, 4);
//		grid.setBlocked(1, 1);
//		grid.setBlocked(1, 2);
//		grid.setBlocked(1, 3);
//		grid.setBlocked(2, 1);
//		grid.setBlocked(2, 3);
//		grid.setBlocked(3, 1);
//		grid.setBlocked(3, 2);
//		grid.setBlocked(3, 3);
//		grid.setBlocked(4, 2);
//		
//		SubgoalGraph processor = new SubgoalGraph(grid);
//		processor.findCorners();
//		List<ObstacleCorner> subgoals = processor.getCorners();
//		
//		Assert.assertEquals(5, subgoals.size());
//		List<ObstacleCorner> corners = new ArrayList<ObstacleCorner>(5);
//		corners.add(new ObstacleCorner(1, 1, Direction.UP_LEFT));
//		corners.add(new ObstacleCorner(3, 1, Direction.UP_RIGHT));
//		corners.add(new ObstacleCorner(3, 3, Direction.DOWN_RIGHT));
//		corners.add(new ObstacleCorner(4, 2, Direction.UP_RIGHT));
//		corners.add(new ObstacleCorner(4, 2, Direction.DOWN_RIGHT));
//		Assert.assertTrue(subgoals.containsAll(corners));
//	}
	
	@Test
	public void searchNodeDistance() {
		PathingGrid grid = new PathingGrid(3, 3);
		SearchNode node = new SearchNode(grid, new Point(0, 0), 1);
		List<SearchNode> neighbors = node.getNeighbors();
		Assert.assertEquals(3, neighbors.size());
		int neighborsFound = 0;
		for (SearchNode neighbor : neighbors) {
			if (neighbor.point().equals(new Point(1, 0))) {
				Assert.assertEquals(2, neighbor.cost(), 0.1);
				++neighborsFound;
			} else if (neighbor.point().equals(new Point(1, 1))) {
				Assert.assertEquals(2.4, neighbor.cost(), 0.1);
				++neighborsFound;
			}
		}
		Assert.assertEquals(2, neighborsFound);
	}
	
	@Test
	public void areDirectlyConnected() {
		PathingGrid grid = new PathingGrid(3, 3);
		SubgoalGraph processor = new SubgoalGraph(grid);
		Assert.assertTrue(processor.areDirectlyConnected(new Point(0, 0),
				new Point(2, 1)));
		grid.setBlocked(1, 0);
		grid.setBlocked(1, 1);
		Assert.assertFalse(processor.areDirectlyConnected(new Point(0, 0),
				new Point(2, 1)));
	}
	
	@Test
	public void directlyConnectedDiagonal() {
		PathingGrid grid = new PathingGrid(3, 3);
		grid.setBlocked(1, 0);
		SubgoalGraph processor = new SubgoalGraph(grid);
		Assert.assertFalse(processor.areDirectlyConnected(new Point(0, 0),
				new Point(2, 2)));
		grid.setBlocked(1, 0, false);
		grid.setBlocked(1, 1);
		Assert.assertFalse(processor.areDirectlyConnected(new Point(0, 0),
				new Point(2, 2)));
	}
	
	@Test
	public void getSubgoalFromCorner() {
		ObstacleCorner corner = new ObstacleCorner(3, 3, Direction.UP_LEFT);
		Assert.assertEquals(new Point(1, 1), corner.getPoint(2));
		ObstacleCorner corner2 = new ObstacleCorner(1, 2, Direction.UP_RIGHT);
		Assert.assertEquals(new Point(2, 0), corner2.getPoint(2));
		Assert.assertEquals(new Point(2, 1), corner2.getPoint(1));
		ObstacleCorner corner3 = new ObstacleCorner(2, 1, Direction.DOWN_LEFT);
		Assert.assertEquals(new Point(0, 2), corner3.getPoint(2));
	}
	
	@Test
	public void distanceBetween() {
		Assert.assertEquals(3.8,
				PointUtil.distanceBetween(new Point(0, 0), new Point(3, 2)),
				0.1);
	}
	
	@Test
	public void distanceComparator() {
		SubgoalComparator comparator = new SubgoalComparator(new Point(3, 3));
		Node<ObstacleCorner> s1 = new Node<ObstacleCorner>(
				new ObstacleCorner(new Point(10, 10), Direction.UP_LEFT));
		Node<ObstacleCorner> s2 = new Node<ObstacleCorner>(
				new ObstacleCorner(new Point(3, 10), Direction.UP_LEFT));
		Assert.assertEquals(1, comparator.compare(s1, s2));
	}
	
	@Test
	public void gridRegion() {
		GridRegion region = new GridRegion(new Point(5, 5),
				new Point(1, 0), new Point(1, 1));
		Assert.assertTrue(region.contains(new Point(6, 6)));
		Assert.assertFalse(region.contains(new Point(4, 4)));
	}
	
	@Test
	public void getGridRegion() {
		GridRegion region = GridRegion.getIndirect(new Point(0, 0),
				new Point(1, 2));
		Assert.assertTrue(region.contains(new Point(2, 3)));
		Assert.assertFalse(region.contains(new Point(2, 2)));
		
		region = GridRegion.getIndirect(new Point(0, 0), new Point(-1, 2));
		Assert.assertTrue(region.contains(new Point(-2, 3)));
		
		region = GridRegion.getIndirect(new Point(0, 0), new Point(-1, -2));
		Assert.assertTrue(region.contains(new Point(-2, -3)));
		
		region = GridRegion.getIndirect(new Point(0, 0), new Point(1, -2));
		Assert.assertTrue(region.contains(new Point(2, -3)));
		
		region = GridRegion.getIndirect(new Point(4, 22), new Point(6, 19));
		Assert.assertTrue(region.contains(new Point(8, 15)));
	}
	
	@Test
	public void connectSubgoals() {
		PathingGrid grid = new PathingGrid(10, 10);
		grid.setBlocked(0, 7);
		grid.setBlocked(1, 2);
		grid.setBlocked(1, 3);
		grid.setBlocked(1, 7);
		grid.setBlocked(2, 2);
		grid.setBlocked(2, 4);
		grid.setBlocked(2, 6);
		grid.setBlocked(2, 7);
		grid.setBlocked(3, 1);
		grid.setBlocked(3, 2);
		grid.setBlocked(3, 4);
		grid.setBlocked(3, 7);
		grid.setBlocked(4, 2);
		grid.setBlocked(4, 4);
		grid.setBlocked(5, 2);
		grid.setBlocked(5, 4);
		grid.setBlocked(6, 3);
		grid.setBlocked(6, 8);
		grid.setBlocked(6, 9);
		grid.setBlocked(7, 7);
		
		SubgoalGraph processor = new SubgoalGraph(grid);
		processor.connectSubgoals();
//		Collection<Node<Point>> subgoals = processor.getSubgoals();
		List<ObstacleCorner> subgoals = processor.getSubgoalTable();
		
		Assert.assertEquals(17, subgoals.size());
		int goalIndex = -1;
		for (int i = 0; i < subgoals.size(); ++i) {
			Point subgoal = subgoals.get(i).getPoint(1);
			if (subgoal.equals(new Point(4, 6))) {
				goalIndex = i;
				break;
			}
		}
		Assert.assertTrue(goalIndex >= 0);
		
		Collection<Integer> neighbors = processor.getAdjacencies(goalIndex); 		
		Assert.assertEquals(5, neighbors.size());
	}		
}
