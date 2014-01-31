package user_interface;

import java.awt.Point;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import core.GameMap;
import pathing.PathingGrid;
import pathing.SubgoalGraph;
import pathing.ObstacleCorner;

public class PathingCompiler {
	
	private static List<Integer> getCoordinates(ObstacleCorner corner) {
		List<Integer> coordinates = new ArrayList<Integer>(2);
		Point point = corner.getPoint(0);
		coordinates.add(point.x);
		coordinates.add(point.y);
		int direction;
		switch (corner.Direction()) {
			case UP_LEFT:
				direction = 0;
				break;
			case UP_RIGHT:
				direction = 1;
				break;
			case DOWN_LEFT:
				direction = 2;
				break;
			case DOWN_RIGHT:
				direction = 3;
				break;
			default:
				direction = -1;
		}
		coordinates.add(direction);
		return coordinates;
	}
	
	private static Map<String, Object> makeJsonMap(SubgoalGraph graph) {
		Map<String, Object> jsonMap = new HashMap<String, Object>();
		List<ObstacleCorner> subgoalTable = graph.getSubgoalTable();
		List<List<Integer>> jsonSubgoals =
				new ArrayList<List<Integer>>(subgoalTable.size());
		List<List<Integer>> adjacencyTable =
				new ArrayList<List<Integer>>(subgoalTable.size());
		for (int i = 0; i < subgoalTable.size(); ++i) {
			jsonSubgoals.add(getCoordinates(subgoalTable.get(i)));			
			adjacencyTable.add(graph.getAdjacencies(i));			
		}
		
		jsonMap.put("subgoals", jsonSubgoals);
		jsonMap.put("adjacencies", adjacencyTable);
		
		return jsonMap;
	}
	
	public static Map<String, Object> makePathingInfo(GameMap map) {
		PathingGrid grid = new PathingGrid(map.getWidth(), map.getHeight());
		for (int x = 0; x < map.getWidth(); ++x) {
			for (int y = 0; y < map.getHeight(); ++y) {
				grid.setBlocked(x, y, map.isBlocked(x, y));
			}
		}
		SubgoalGraph graph = new SubgoalGraph(grid);
		graph.connectSubgoals();
		return makeJsonMap(graph);
	}
}
