package core;

import java.util.*;

public class UnitType {
	public String name, imageSource1, imageSource2;
	public int maxHealth;
	public double attackDamage, attackRange, attackSpeed, collisionRadius,
			selectionRadius, speed;
			
	
	public UnitType() { }
	
	public UnitType(Map<String, Object> jsonMap) {
		name = (String)jsonMap.get("name");
		imageSource1 = (String)jsonMap.get("source1");
		imageSource2 = (String)jsonMap.get("source2");
		attackDamage = (double)jsonMap.get("damage");
		attackRange = (double)jsonMap.get("range");
		attackSpeed = (double)jsonMap.get("aspeed");
		collisionRadius = (double)jsonMap.get("cradius");
		maxHealth = (int)jsonMap.get("health");
		selectionRadius = (double)jsonMap.get("sradius");
		speed = (double)jsonMap.get("mspeed");
	}
	
	public void setType(UnitType type) {
		name = type.name;
		imageSource1 = type.imageSource1;
		imageSource2 = type.imageSource2;
		attackDamage = type.attackDamage;
		attackRange = type.attackRange;
		attackSpeed = type.attackSpeed;
		collisionRadius = type.collisionRadius;
		maxHealth = type.maxHealth;
		selectionRadius = type.selectionRadius;
		speed = type.speed;
	}
	
	public Map<String, Object> getJsonMap() {
		Map<String, Object> result = new HashMap<String, Object>();
		result.put("name", name);
		result.put("source1", imageSource1);
		result.put("source2", imageSource2);
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
