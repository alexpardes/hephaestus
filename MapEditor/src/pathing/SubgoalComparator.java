package pathing;

import java.awt.Point;
import java.util.Comparator;

public class SubgoalComparator implements Comparator<Node<Point>> {
	private Point base;
	
	public SubgoalComparator(Point base) {
		this.base = base;
	}


	@Override
	public int compare(Node<Point> s1, Node<Point> s2) {
		if (s1.equals(s2)) {
			return 0;
		}
		double distance1 = PointUtil.distanceBetween(s1.value(), base);
		double distance2 = PointUtil.distanceBetween(s2.value(), base);
		return Double.compare(distance1, distance2);
	}

}
