package core;

import java.util.HashMap;
import java.util.Map;

public class Tile {
	private boolean isTraversable;
	private String name;
	private String imageSource;
	
	public Tile(String name, String imageSource, boolean isTraversable) {
		this.name = name;
		this.imageSource = imageSource;
		this.isTraversable = isTraversable;
	}
	
	public Tile(Map<String, Object> jsonMap) {
		name = (String)jsonMap.get("name");
		imageSource = (String)jsonMap.get("source");
		isTraversable = (boolean)jsonMap.get("traversable");
	}
	
	public String getName() { return name; }
	public String getImageSource() { return imageSource; }
	
	public Map<String, Object> getJsonMap() {
		Map<String, Object> result = new HashMap<String, Object>();
		result.put("name", name);
		result.put("source", imageSource);
		result.put("traversable", isTraversable);
		return result;
	}
}
