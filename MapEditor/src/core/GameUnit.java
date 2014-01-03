package core;

import java.util.*;

public class GameUnit {
	public String typeName;	
	public int ownerId;
	public double x, y;
	
	public GameUnit(String typeName, int ownerId, double x, double y) {
		this.typeName = typeName;
		this.ownerId = ownerId;
		this.x = x;
		this.y = y;
	}
	
	public GameUnit(Map<String, Object> jsonMap) {
		typeName = (String)jsonMap.get("type");
		ownerId = (int)jsonMap.get("owner");
		x = (double)jsonMap.get("x");
		y = (double)jsonMap.get("y");
	}
	
	public Map<String, Object> getJsonMap() {
		Map<String, Object> result = new HashMap<String, Object>();
		result.put("type", typeName);
		result.put("owner", ownerId);
		result.put("x", x);
		result.put("y", y);		
		return result;
	}
}
