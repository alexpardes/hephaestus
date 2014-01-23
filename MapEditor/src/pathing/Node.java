package pathing;

import java.util.ArrayList;
import java.util.Collection;

public class Node<T> {
	private T value;
	private Collection<Node<T>> neighbors = new ArrayList<Node<T>>();
	
	public Node(T t) {
		value = t;
	}
	
	public void addNeighbor(Node<T> neighbor) {
		neighbors.add(neighbor);
	}
	
	public Collection<Node<T>> getNeighbors() {
		return neighbors;
	}
	
	public T value() { return value; }
}
