package pathing;

import java.awt.Point;
import java.util.Comparator;

public class SubgoalComparator implements Comparator<Node<ObstacleCorner>> {
	private Point base;
	
	public SubgoalComparator(Point base) {
		this.base = base;
	}


	@Override
	public int compare(Node<ObstacleCorner> s1, Node<ObstacleCorner> s2) {
		if (s1.equals(s2)) {
			return 0;
		}
		double distance1 =
				PointUtil.distanceBetween(s1.value().getPoint(1), base);
		double distance2 =
				PointUtil.distanceBetween(s2.value().getPoint(1), base);
		return Double.compare(distance1, distance2);
	}

}
