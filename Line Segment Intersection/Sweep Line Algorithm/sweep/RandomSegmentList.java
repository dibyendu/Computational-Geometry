/**
 *
 * @author dibyendu
 */

public class RandomSegmentList {

    public RandomSegmentList next;
    public double x1, y1;
    public double x2, y2;

    public RandomSegmentList(double a_x1, double a_y1, double a_x2, double a_y2,
            RandomSegmentList n) {
        next = n;
        x1 = a_x1;
        x2 = a_x2;
        y1 = a_y1;
        y2 = a_y2;
    }
}
