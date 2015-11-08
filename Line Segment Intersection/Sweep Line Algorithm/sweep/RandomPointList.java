/**
 *
 * @author dibyendu
 */

public class RandomPointList {

    public RandomPointList next;
    public double x, y;

    public RandomPointList(double x1, double y1, RandomPointList n) {
        x = x1;
        y = y1;
        next = n;
    }
}
