package pathing;

import java.awt.Point;

// A region is the set of grid points defined by a base point and two directions,
// where the directions are not more than two apart. The base point and all points
// in both given directions are contained in the region, as well as all points
// in between.
public class GridRegion {
	private Point offset, direction1, direction2;
	private int containedSide1, containedSide2;
	private boolean isEmpty;
	
	public GridRegion(Point point, Point direction1, Point direction2) {
		offset = new Point(-point.x, -point.y);
		this.direction1 = direction1;
		this.direction2 = direction2;
		
		containedSide1 = compare(direction1, direction2);
		containedSide2 = compare(direction2, direction1);
	}
	
	protected GridRegion() { isEmpty = true; }
	
	// Creates the region of points r such that there is an h-path
	// from p to r passing through q. 
	public static GridRegion getIndirect(Point p, Point q) {
		if (p.equals(q)) {
			return new GridRegion();
		}
		
		Point dir1 = null;
		Point dir2 = null;
		
		Point point = new Point(q.x - p.x, q.y - p.y);		
		int a = compare(new Point(1, 0), point);
		int b = compare(new Point(0, -1), point);
		if (a == 0) {
			dir1 = new Point(b, -1);
			dir2 = new Point(b, 1);
		} else if (b == 0) {
			dir1 = new Point(-1, a);
			dir2 = new Point(1, a);
		} else {
			int c = compare(new Point(b, a), point);
			if (c == 0) {
				dir1 = new Point(0, a);
				dir2 = new Point(b, 0);
			} else {
				dir1 = new Point(b, a);
				if (a*b*c == 1) {
					dir2 = new Point(0, a);
				} else {
					dir2 = new Point(b, 0);
				}
			}
		}
		
		return new GridRegion(q, dir1, dir2);
	}
	
	// Returns 0 if p is on the line from the origin in input direction.
	// Returns 1 for points on one side of this line, and -1 for points
	// on the other side.
	private static int compare(Point direction, Point p) {
		return Integer.compare(direction.x * p.y, direction.y * p.x);
	}
	
	public boolean contains(Point p) {
		if (isEmpty) {
			return false;
		}
		
		// Brings p to the base of the region.
		Point point = PointUtil.add(p, offset);
		int side1 = compare(direction1, point);
		int side2 = compare(direction2, point);
		
		boolean sameSide1 = side1 == 0 || side1 == containedSide1;		
		boolean sameSide2 = side2 == 0 || side2 == containedSide2;
		
		return sameSide1 && sameSide2;
	}
}
