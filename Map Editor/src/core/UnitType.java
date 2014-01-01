package core;

import java.util.*;

public class UnitType {
	public String name, imageSource;
	public double attackDamage, attackRange, attackSpeed,
			collisionRadius, maxHealth, selectionRadius, speed;
			
	
	public UnitType() { }
	
	public UnitType(Map<String, Object> jsonMap) {
		name = (String)jsonMap.get("name");
		imageSource = (String)jsonMap.get("source");
		attackDamage = (int)jsonMap.get("damage");
		attackRange = (int)jsonMap.get("range");
		attackSpeed = (int)jsonMap.get("aspeed");
		collisionRadius = (int)jsonMap.get("cradius");
		maxHealth = (int)jsonMap.get("health");
		selectionRadius = (int)jsonMap.get("sradius");
		speed = (int)jsonMap.get("mspeed");
	}
	
	public Map<String, Object> getJsonMap() {
		Map<String, Object> result = new HashMap<String, Object>();
		result.put("name", name);
		result.put("source", imageSource);
		result.put("damage", attackDamage);
		result.put("range", attackRange);
		result.put("aspeed", attackSpeed);
		result.put("cradius", collisionRadius);
		result.put("health", maxHealth);
		result.put("sradius", selectionRadius);
		result.put("mspeed", speed);
		return result;
	}
}
