package core;

import java.util.*;

public class GameMap {
	private final int width, height;
	private final List<List<Integer>> terrainGrid;
	private final List<Tile> tileTable = new ArrayList<Tile>();
	private final Map<String, Integer> tileDictionary
			= new HashMap<String, Integer>();
	private final List<UnitType> unitTypes = new ArrayList<UnitType>();
	private final List<GameUnit> startingUnits = new ArrayList<GameUnit>();
	private final Map<String, Integer> unitTypeDictionary
			= new HashMap<String, Integer>();
	
	public GameMap(int width, int height) {
		assert(width > 0);
		assert(height > 0);
		
		this.width = width;
		this.height = height;		
		
		terrainGrid = new ArrayList<List<Integer>>(width);
		while (terrainGrid.size() < width) {
			List<Integer> row = new ArrayList<Integer>(height);
			while (row.size() < height) row.add(0);
			terrainGrid.add(row);
		}		
	}
	
	public GameMap(Map<String, Object> jsonMap) {
		width = (int)jsonMap.get("width");
		height = (int)jsonMap.get("height");
		
		terrainGrid = (List<List<Integer>>)jsonMap.get("terrain");		
		
		List<Map<String, Object>> tiles =
				(List<Map<String, Object>>)jsonMap.get("tiles");		
		for (Map<String, Object> tile : tiles) {
			addTile(new Tile(tile));
		}
		
		List<Map<String, Object>> types =
				(List<Map<String, Object>>)jsonMap.get("types");
		for (Map<String, Object> type: types) {
			addUnitType(new UnitType(type));
		}
		
		List<Map<String, Object>> units =
				(List<Map<String, Object>>)jsonMap.get("units");
		for (Map<String, Object> unit : units) {
			placeUnit(new GameUnit(unit));
		}
	}
	
	public void addUnitType(UnitType type) {
		unitTypes.add(type);
	}
	
	public void placeUnit(String typename, int playerId, double x, double y) {
		startingUnits.add(new GameUnit(typename, playerId, x, y));
	}
	
	public void placeUnit(GameUnit unit) {
		startingUnits.add(unit);
	}
	
	public Tile getTile(int column, int row) {
		assert(row < terrainGrid.size());
		assert(column < terrainGrid.get(0).size());
		int tileId = terrainGrid.get(column).get(row);
		assert(tileId < tileTable.size());
		
		return tileTable.get(tileId);
	}
	
	public void setTile(String tileName, int column, int row) {
		assert(tileDictionary.containsKey(tileName));
		terrainGrid.get(column).set(row, tileDictionary.get(tileName));
	}
	
	public void addTile(Tile tile) {
		tileTable.add(tile);
		tileDictionary.put(tile.getName(), tileTable.size() - 1);
	}
	
	public List<Tile> getTiles() { return tileTable; }
	public List<UnitType> getUnits() { return unitTypes; }
	public List<GameUnit> getPlacedUnits() { return startingUnits; }
	
	public int getWidth() { return width; }
	public int getHeight() { return height; }
	
	public Map<String, Object> getJsonMap() {
		Map<String, Object> result = new HashMap<String, Object>();
		List<Map<String, Object>> tiles = new ArrayList<Map<String, Object>>();
		for (Tile tile : tileTable) {
			tiles.add(tile.getJsonMap());
		}
		
		List<Map<String, Object>> types = new ArrayList<Map<String, Object>>();
		for (UnitType type : unitTypes) {
			types.add(type.getJsonMap());
		}
		
		List<Map<String, Object>> units = new ArrayList<Map<String, Object>>();
		for (GameUnit unit : startingUnits) {
			units.add(unit.getJsonMap());
		}
		
		result.put("types", types);
		result.put("units", units);
		result.put("tiles", tiles);		
		result.put("width", width);
		result.put("height", height);
		result.put("terrain", terrainGrid);
		
		return result;
	}
}