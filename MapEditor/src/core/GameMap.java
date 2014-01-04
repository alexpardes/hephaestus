package core;

import java.util.*;

public class GameMap {
	private final int width, height;
	private final List<List<Integer>> terrainGrid;
	private final List<Tile> tileTable = new ArrayList<Tile>();
	private final Map<String, Integer> tileDictionary
			= new HashMap<String, Integer>();
	private final List<GameUnit> placedUnits = new ArrayList<GameUnit>();
	private final Map<String, UnitType> unitTypeDictionary
			= new HashMap<String, UnitType>();
	
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
	
	public UnitType getUnitType(String name) {
		assert unitTypeDictionary.containsKey(name);
		return unitTypeDictionary.get(name);
	}
	
	public void addUnitType(UnitType type) {
		unitTypeDictionary.put(type.name, type);
	}
	
	public void editUnitType(String name, UnitType newType) {
		UnitType type = getUnitType(name);
		type.setType(newType);
		
		if (!name.equals(newType.name)) {
			unitTypeDictionary.put(newType.name, newType);			
			unitTypeDictionary.remove(name);
			
			for (GameUnit unit : placedUnits) {
				if (unit.typeName.equals(name)) {
					unit.typeName = newType.name;
				}
			}
		}
	}
	
	public void placeUnit(String typename, int playerId, double x, double y) {
		placedUnits.add(new GameUnit(typename, playerId, x, y));
	}
	
	public void placeUnit(GameUnit unit) {
		placedUnits.add(unit);
	}
	
	public Tile getTile(String name) {
		assert tileDictionary.containsKey(name);
		return tileTable.get(tileDictionary.get(name));
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
	
	public boolean hasTile(String tileName) {
		return tileDictionary.containsKey(tileName);
	}
	
	public boolean hasUnitType(String typeName) {
		return unitTypeDictionary.containsKey(typeName);
	}
	
	public void editTile(String tileName, Tile newTile) {
		Tile tile = getTile(tileName);
		tile.setName(newTile.getName());
		tile.setImageSource(newTile.getImageSource());
		tile.setTraversable(newTile.isTraversable());
		
		boolean hasNewName = !tileName.equals(newTile.getName());
		if (hasNewName) {
			tileDictionary.put(newTile.getName(), tileDictionary.get(tileName));
			tileDictionary.remove(tileName);
		}
	}
	
	public GameUnit getUnit(double x, double y) {
		GameUnit result = null;
		for (int i = 0; i < placedUnits.size(); ++i) {
			GameUnit unit = placedUnits.get(i);
			double distanceSquared = getDistanceSquared(x, y, unit.x, unit.y);
			double radius = getType(unit).selectionRadius;
			double radiusSquared = radius*radius;
			if (distanceSquared < radiusSquared) {
				result = unit;
				break;
			}
		}
		return result;
	}	
	
	private UnitType getType(GameUnit unit) {
		return getUnitType(unit.typeName);
	}
	
	private double getDistanceSquared(double x1, double y1, double x2,
			double y2) {
		return (x1 - x2)*(x1 - x2) + (y1 - y2)*(y1 - y2);
	}
	
	public List<Tile> getTiles() { return tileTable; }
	
	public Collection<UnitType> getUnitTypes() {
			return unitTypeDictionary.values();
		}
	
	public List<GameUnit> getPlacedUnits() { return placedUnits; }
	
	public int getWidth() { return width; }
	public int getHeight() { return height; }
	
	public Map<String, Object> getJsonMap() {
		Map<String, Object> result = new HashMap<String, Object>();
		List<Map<String, Object>> tiles = new ArrayList<Map<String, Object>>();
		for (Tile tile : tileTable) {
			tiles.add(tile.getJsonMap());
		}
		
		List<Map<String, Object>> types = new ArrayList<Map<String, Object>>();
		for (UnitType type : unitTypeDictionary.values()) {
			types.add(type.getJsonMap());
		}
		
		List<Map<String, Object>> units = new ArrayList<Map<String, Object>>();
		for (GameUnit unit : placedUnits) {
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