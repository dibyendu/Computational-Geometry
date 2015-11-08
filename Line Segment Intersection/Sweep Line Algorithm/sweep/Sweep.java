/**
 *
 * @author dibyendu
 */
 
import java.io.*;
import java.util.*;

class NS {

    public static final double EPSILON = 1.0e-10;

    public static double fuzzy(double alpha) {
        if ((alpha > -EPSILON) && (alpha < EPSILON)) {
            return 0.0;
        }
        return alpha;
    }
}

class Point {

    public double x;
    public double y;

    public final int compare(Point p2) {
        if ((NS.fuzzy(y - p2.y) > 0.0) || ((NS.fuzzy(y - p2.y) == 0.0) && (NS.fuzzy(x - p2.x) < 0.0))) {
            return -1;
        }
        if ((NS.fuzzy(y - p2.y) < 0.0) || ((NS.fuzzy(y - p2.y) == 0.0) && (NS.fuzzy(x - p2.x) > 0))) {
            return 1;
        }
        return 0;
    }

    public Point(double xs, double ys) {
        x = xs;
        y = ys;
    }

    @Override
    public String toString() {
        return "(" + x + ", " + y + ")";
    }
}

class Segment {
    /*
     *  Vertices within a segment are either top - down or left to right, as defined in Point.cmp
     */

    public Point p1;        // This is always an "upper" point according to Point.cmp
    public Point p2;        // This is always a "lower" point according to Point.cmp
    public boolean z_len;
    private double a, b, c; // Equation of a line the segment is on

    public Segment(double x1, double y1, double x2, double y2) {
        p1 = new Point(x1, y1);
        p2 = new Point(x2, y2);
        if (p1.compare(p2) > 0) {
            Point swap = p1;
            p1 = p2;
            p2 = swap;

            x1 = p1.x;
            y1 = p1.y;
            x2 = p2.x;
            y2 = p2.y;
        }
        if (NS.fuzzy(x1 - x2) != 0.0) {
            b = 1.0;
            a = (y2 - y1) / (x1 - x2);
            c = -a * x1 - b * y1;

            /*
             *  This is here so that ax + by + c > 0 if (x, y) is to the left
             */
            if (NS.fuzzy(x1 - x2) > 0) {
                a = -a;
                b = -b;
                c = -c;
            }
            z_len = false;
        } else if (NS.fuzzy(y1 - y2) != 0.0) {
            b = 0;
            a = 1;
            c = -x1;
        } else {
            z_len = true;
        }
    }

    @Override
    public String toString() {
        return "seg [" + p1 + " - " + p2 + "]";
    }

    public final boolean pointOn(Point p) {
        if (z_len) {
            if (p1.compare(p) == 0) {
                return true;
            }
            return false;
        }
        if (NS.fuzzy(a * p.x + b * p.y + c) == 0.0) {
            return true;
        }
        return false;
    }

    public final boolean pointToRight(Point p) {
        if (z_len) {
            return NS.fuzzy(p.x - p1.x) > 0.0;
        }
        return NS.fuzzy(a * p.x + b * p.y + c) > 0;
    }

    public final boolean pointToLeft(Point p) {
        if (z_len) {
            return NS.fuzzy(p.x - p1.x) < 0.0;
        }
        return NS.fuzzy(a * p.x + b * p.y + c) < 0;
    }

    public final int orientation(Point p) {
        double x = NS.fuzzy(a * p.x + b * p.y + c);
        if (x < 0) {
            return -1;
        } else if (x > 0) {
            return 1;
        }
        return 0;
    }

    public static Point intersect(Segment s1, Segment s2) {
        /*
         *  Degenerate cases first
         */
        if (s2.z_len) {
            if (s1.pointOn(s2.p1)) {
                return new Point(s2.p1.x, s2.p1.y);
            }
            return null;
        }

        /*
         *  Non-degenerate cases
         */
        double sign11 = NS.fuzzy(s1.a * s2.p1.x + s1.b * s2.p1.y + s1.c);
        double sign12 = NS.fuzzy(s1.a * s2.p2.x + s1.b * s2.p2.y + s1.c);
        double sign21 = NS.fuzzy(s2.a * s1.p1.x + s2.b * s1.p1.y + s2.c);
        double sign22 = NS.fuzzy(s2.a * s1.p2.x + s2.b * s1.p2.y + s2.c);

        if ((sign11 * sign12 <= 0.0) && (sign21 * sign22 <= 0.0)) {
            /*
             *  One more degenerate case
             */
            if ((NS.fuzzy(s1.a - s2.a) == 0.0) && (NS.fuzzy(s1.b - s2.b) == 0.0)) {
                if (NS.fuzzy(s1.c - s2.c) != 0.0) {
                    return null;
                }
                if (s1.p1.compare(s2.p1) < 0) {
                    return new Point((s2.p1.x + s1.p2.x) / 2.0, (s2.p1.y + s1.p2.y) / 2.0);
                } else {
                    return new Point((s1.p1.x + s2.p2.x) / 2.0, (s1.p1.y + s2.p2.y) / 2.0);
                }
            }
            double y = (s2.a * s1.c - s2.c * s1.a) / (s2.b * s1.a - s2.a * s1.b);
            double x = (NS.fuzzy(s1.a) != 0.0) ? -(s1.c + s1.b * y) / s1.a : -(s2.c + s2.b * y) / s2.a;
            return new Point(x, y);
        }
        return null;
    }
}

class SegmentList {

    public Segment seg;
    public SegmentList next;

    public SegmentList(Segment s, SegmentList n) {
        seg = s;
        next = n;
    }

    public static int length(SegmentList run) {
        int n = 0;
        while (run != null) {
            n++;
            run = run.next;
        }
        return n;
    }

    @Override
    public final String toString() {
        String x = "seglist[" + seg;
        SegmentList run = next;
        while (run != null) {
            x = x + ", " + run.seg;
            run = run.next;
        }
        return x + "]";
    }
}

class QueueEvent {

    public Point pt;
    public SegmentList U;
    public SegmentList L;
    /*
     *  This counts how many times Event was inserted in the queue
     */
    public int n_ins;

    public QueueEvent(Point p) {
        pt = p;
        U = null;
        L = null;
        n_ins = 1;
    }

    private final SegmentList addToList(Segment s, SegmentList l) {
        return new SegmentList(s, l);
    }

    public final void add(Segment s) {
        if (pt.compare(s.p1) == 0) {
            U = addToList(s, U);
        } else {
            L = addToList(s, L);
        }
    }

    @Override
    public String toString() {
        return "evt[" + pt + " / U:" + U + " / L: " + L + "]";
    }
}

class EventTreeNode {

    public EventTreeNode left;
    public EventTreeNode right;
    public EventTreeNode parent;
    public QueueEvent evt;
    public int color;
}

class EventTree {

    private final static int BLACK = 0;
    private final static int RED = 1;
    private EventTreeNode nil;
    private EventTreeNode root;

    /*
     *  Constructor
     */
    public EventTree() {
        nil = new EventTreeNode();
        nil.color = BLACK;
        root = nil;
    }

    /*
     *  True if queue is empty
     */
    public final boolean isEmpty() {
        return root == nil;
    }

    /*
     *  insert edge point (with segment s), or intersection point
     *  (in which case segment is null). Note the difference between
     *  insertion and deleting: while insert inserts segment one by one,
     *  delete destroys the entire node.
     */
    public final void insert(Point p) {
        insert(p, null);
    }

    public final void insert(Point p, Segment s) {
        EventTreeNode y = nil;
        EventTreeNode r = root;
        int c = 0;
        while (r != nil) {
            y = r;
            c = r.evt.pt.compare(p);
            if (c > 0) {
                r = r.left;
            } else if (c < 0) {
                r = r.right;
            } else {
                if (s != null) {
                    r.evt.add(s);
                }
                r.evt.n_ins++;
                return;
            }
        }
        EventTreeNode x = new EventTreeNode();
        x.left = nil;
        x.right = nil;
        x.evt = new QueueEvent(p);
        if (s != null) {
            x.evt.add(s);
        }
        x.parent = y;
        x.color = RED;
        if (y == nil) {
            root = x;
        } else {
            if (c > 0) {
                y.left = x;
            } else {
                y.right = x;
            }
        }

        /*
         *  Do RB balancing
         */
        while ((x != root) && (x.parent.color == RED)) {
            if (x.parent == x.parent.parent.left) {
                y = x.parent.parent.right;
                if (y.color == RED) {
                    x.parent.color = BLACK;
                    y.color = BLACK;
                    x = x.parent.parent;
                    x.color = RED;
                } else {
                    if (x == x.parent.right) {
                        x = x.parent;
                        leftRotate(x);
                    }
                    x.parent.color = BLACK;
                    x.parent.parent.color = RED;
                    rightRotate(x.parent.parent);
                }
            } else {
                y = x.parent.parent.left;
                if (y.color == RED) {
                    x.parent.color = BLACK;
                    y.color = BLACK;
                    x = x.parent.parent;
                    x.color = RED;
                } else {
                    if (x == x.parent.left) {
                        x = x.parent;
                        rightRotate(x);
                    }
                    x.parent.color = BLACK;
                    x.parent.parent.color = RED;
                    leftRotate(x.parent.parent);
                }
            }
        }
        root.color = BLACK;
    }

    /*
     *  Extract minimal node and delete it... Return associated event.
     */
    public final QueueEvent get() {
        EventTreeNode x = root;
        if (x == nil) {
            return null;
        }
        while (x.left != nil) {
            x = x.left;
        }
        return delete(x);
    }

    /*
     *  Private section
     *
     *  Destroy the node and return event associated with it...
     */
    private final QueueEvent delete(EventTreeNode z) {
        QueueEvent res = z.evt;
        EventTreeNode y = ((z.left == nil) || (z.right == nil)) ? z : treeSuccessor(z);
        EventTreeNode x = (y.left != nil) ? y.left : y.right;
        x.parent = y.parent;
        if (y.parent == nil) {
            root = x;
        } else {
            if (y == y.parent.left) {
                y.parent.left = x;
            } else {
                y.parent.right = x;
            }
        }
        if (y != z) {
            z.evt = y.evt;
        }
        if (y.color != BLACK) {
            return res;
        }
        while ((x != root) && (x.color == BLACK)) {
            EventTreeNode w;
            if (x == x.parent.left) {
                w = x.parent.right;
                if (w.color == RED) {
                    w.color = BLACK;
                    x.parent.color = RED;
                    leftRotate(x.parent);
                    w = x.parent.right;
                }
                if ((w.left.color == BLACK) && (w.right.color == BLACK)) {
                    w.color = RED;
                    x = x.parent;
                } else {
                    if (w.right.color == BLACK) {
                        w.left.color = BLACK;
                        w.color = RED;
                        rightRotate(w);
                        w = x.parent.right;
                    }
                    w.color = x.parent.color;
                    x.parent.color = BLACK;
                    w.right.color = BLACK;
                    leftRotate(x.parent);
                    x = root;
                }
            } else {
                w = x.parent.left;
                if (w.color == RED) {
                    w.color = BLACK;
                    x.parent.color = RED;
                    rightRotate(x.parent);
                    w = x.parent.left;
                }
                if ((w.left.color == BLACK) && (w.right.color == BLACK)) {
                    w.color = RED;
                    x = x.parent;
                } else {
                    if (w.left.color == BLACK) {
                        w.right.color = BLACK;
                        w.color = RED;
                        leftRotate(w);
                        w = x.parent.left;
                    }
                    w.color = x.parent.color;
                    x.parent.color = BLACK;
                    w.left.color = BLACK;
                    rightRotate(x.parent);
                    x = root;
                }
            }
        }
        x.color = BLACK;
        return res;
    }

    private final void leftRotate(EventTreeNode x) {
        EventTreeNode y = x.right;
        x.right = y.left;
        if (y.left != nil) {
            y.left.parent = x;
        }
        y.parent = x.parent;
        if (x.parent == nil) {
            root = y;
        } else {
            if (x == x.parent.left) {
                x.parent.left = y;
            } else {
                x.parent.right = y;
            }
        }
        y.left = x;
        x.parent = y;
    }

    private final void rightRotate(EventTreeNode y) {
        EventTreeNode x = y.left;
        y.left = x.right;
        if (x.right != nil) {
            x.right.parent = y;
        }
        x.parent = y.parent;
        if (x.parent == nil) {
            root = x;
        } else if (y == y.parent.right) {
            y.parent.right = x;
        } else {
            y.parent.left = x;
        }
        x.right = y;
        y.parent = x;
    }

    private final EventTreeNode treeSuccessor(EventTreeNode x) {
        if (x.right != nil) {
            x = x.right;
            while (x.left != nil) {
                x = x.left;
            }
            return x;
        }
        EventTreeNode y = x.parent;
        while ((y != nil) && (x == y.right)) {
            x = y;
            y = y.parent;
        }
        return y;
    }
}

class IntersectionPoint {

    public Point point;
    private SegmentList segmentList;
    IntersectionPoint next;

    public IntersectionPoint(Point p, IntersectionPoint n) {
        point = p;
        next = n;
    }

    public final void addSegmentList(SegmentList s) {
        while (s != null) {
            segmentList = new SegmentList(s.seg, segmentList);
            s = s.next;
        }
    }

    public final void addSegment(Segment s) {
        segmentList = new SegmentList(s, segmentList);
    }
}

class StatusTreeNode {

    public StatusTreeNode parent;
    public StatusTreeNode left;
    public StatusTreeNode right;
    public Segment seg_stored;
    public Segment seg_max;
    public Segment seg_min;
    public int color;
    public int id;
    private static int id_counter;

    public StatusTreeNode() {
        id = ++id_counter;
    }

    @Override
    public String toString() {
        return "Node " + id;
    }
}

class StatusTree {

    private final static int BLACK = 0;
    private final static int RED = 1;
    private StatusTreeNode root;
    public Point p_sweep;

    /*
     *  This returns the successor to given node; or minimum
     *  element if passed null; can return null.
     */
    public final StatusTreeNode successor(StatusTreeNode n) {
        if (n == null) {
            if (root == null) {
                return null;
            }
            n = root;
            while (n.seg_stored == null) {
                n = n.left;
            }
            return n;
        }
        while ((n != root) && (n == n.parent.right)) {
            n = n.parent;
        }
        if (n == root) {
            return null;
        }
        n = n.parent.right;
        while (n.left != null) {
            n = n.left;
        }
        return n;
    }

    /*
     *  This returns the predecessor to given node, or maximum
     *  element if passed null; can return null!
     */
    public final StatusTreeNode predecessor(StatusTreeNode n) {
        if (n == null) {
            if (root == null) {
                return null;
            }
            n = root;
            while (n.seg_stored == null) {
                n = n.right;
            }
            return n;
        }
        while ((n != root) && (n == n.parent.left)) {
            n = n.parent;
        }
        if (n == root) {
            return null;
        }
        n = n.parent.left;
        while (n.right != null) {
            n = n.right;
        }
        return n;
    }

    /*
     *  Delete the node; rebalance the tree...
     */
    public final void deleteNode(StatusTreeNode n) {
        if (n == root) {
            root = null;
            return;
        }
        StatusTreeNode x = (n == n.parent.left) ? n.parent.right : n.parent.left;
        if (n.parent == root) {
            root = x;
            root.color = BLACK;
            root.parent = null;
            return;
        }
        StatusTreeNode y = n.parent.parent;
        x.parent = y;
        if (n.parent == y.left) {
            y.left = x;
        } else {
            y.right = x;
        }

        /*
         *  Propagate up...
         */
        while (y != null) {
            boolean changed = false;
            if (y.seg_min != y.left.seg_min) {
                changed = true;
                y.seg_min = y.left.seg_min;
            }
            if (y.seg_max != y.right.seg_max) {
                changed = true;
                y.seg_max = y.right.seg_max;
            }
            if (!changed) {
                break;
            }
            y = y.parent;
        }

        /*
         *  Rebalance
         */
        y = n.parent;
        if (y.color == BLACK) {
            while ((x != root) && (x.color == BLACK)) {
                if (x == x.parent.left) {
                    StatusTreeNode w = x.parent.right;
                    if (w.color == RED) {
                        w.color = BLACK;
                        x.parent.color = RED;

                        leftRotate(x.parent);
                        w = x.parent.right;
                    }
                    if ((w.left.color == BLACK) && (w.right.color == BLACK)) {
                        w.color = RED;
                        x = x.parent;
                    } else {
                        if (w.right.color == BLACK) {
                            w.left.color = BLACK;
                            w.color = RED;
                            rightRotate(w);
                            w = x.parent.right;
                        }
                        w.color = x.parent.color;
                        x.parent.color = BLACK;
                        w.right.color = BLACK;
                        leftRotate(x.parent);
                        x = root;
                    }
                } else {
                    StatusTreeNode w = x.parent.left;
                    if (w.color == RED) {
                        w.color = BLACK;
                        x.parent.color = RED;
                        rightRotate(x.parent);
                        w = x.parent.left;
                    }
                    if ((w.left.color == BLACK) && (w.right.color == BLACK)) {
                        w.color = RED;
                        x = x.parent;
                    } else {
                        if (w.left.color == BLACK) {
                            w.right.color = BLACK;
                            w.color = RED;
                            leftRotate(w);
                            w = x.parent.left;
                        }
                        w.color = x.parent.color;
                        x.parent.color = BLACK;
                        w.left.color = BLACK;
                        rightRotate(x.parent);
                        x = root;
                    }
                }
            }
            x.color = BLACK;
        }
    }

    private final int compareSegment(Segment s1, Segment s2) {
        Point p = Segment.intersect(s1, s2);
        if (p != null) {
            if (p_sweep.compare(p) < 0) {
                return -s1.orientation(s2.p1);
            } else {
                return -s1.orientation(s2.p2);
            }
        }

        /*
         *  p_sweep is known to be on s2
         */
        return -s1.orientation(p_sweep);
    }

    /*
     *  Insert segment in the tree
     */
    public final void insertSegment(Segment s) {
        StatusTreeNode m = new StatusTreeNode();
        m.seg_stored = s;
        m.seg_max = s;
        m.seg_min = s;
        m.color = BLACK;
        if (root == null) {
            root = m;
            return;
        }
        StatusTreeNode n = root;
        while (n.seg_stored == null) {
            if (compareSegment(n.right.seg_min, s) > 0) {
                n = n.left;
            } else {
                n = n.right;
            }
        }
        StatusTreeNode np = new StatusTreeNode();
        np.parent = n.parent;
        n.parent = np;
        m.parent = np;
        if (n == root) {
            root = np;
        } else {
            if (np.parent.left == n) {
                np.parent.left = np;
            } else {
                np.parent.right = np;
            }
        }
        if (compareSegment(n.seg_stored, s) > 0) {
            np.left = m;
            np.right = n;
        } else {
            np.left = n;
            np.right = m;
        }
        np.seg_min = np.left.seg_min;
        np.seg_max = np.right.seg_max;
        np = np.parent;

        /*
         *  Propagate the change up...
         */
        while (np != null) {
            boolean changed = false;
            if (np.seg_min != np.left.seg_min) {
                changed = true;
                np.seg_min = np.left.seg_min;
            }
            if (np.seg_max != np.right.seg_max) {
                changed = true;
                np.seg_max = np.right.seg_max;
            }
            if (!changed) {
                break;
            }
            np = np.parent;
        }

        /*
         *  Rebalance the tree
         */
        StatusTreeNode x = m.parent;
        x.color = RED;
        while ((x != root) && (x.parent.color == RED)) {
            if (x.parent == x.parent.parent.left) {
                StatusTreeNode y = x.parent.parent.right;
                if (y.color == RED) {
                    x.parent.color = BLACK;
                    y.color = BLACK;
                    x.parent.parent.color = RED;
                    x = x.parent.parent;
                } else {
                    if (x == x.parent.right) {
                        x = x.parent;
                        leftRotate(x);
                    }
                    x.parent.color = BLACK;
                    x.parent.parent.color = RED;
                    rightRotate(x.parent.parent);
                }
            } else {
                StatusTreeNode y = x.parent.parent.left;
                if (y.color == RED) {
                    x.parent.color = BLACK;
                    y.color = BLACK;
                    x.parent.parent.color = RED;
                    x = x.parent.parent;
                } else {
                    if (x == x.parent.left) {
                        x = x.parent;
                        rightRotate(x);
                    }
                    x.parent.color = BLACK;
                    x.parent.parent.color = RED;
                    leftRotate(x.parent.parent);
                }
            }
        }
        root.color = BLACK;
    }

    /*
     *  This returns the node immediately to the left of p,
     *  but not including p. can be null!
     */
    public final StatusTreeNode leftNodeForPoint(Point p) {
        StatusTreeNode n = root;
        if (n == null) {
            return null;
        }
        while (n.seg_stored == null) {
            if (!n.right.seg_min.pointToRight(p)) {
                n = n.left;
            } else {
                n = n.right;
            }
        }
        if (n.seg_stored.pointToRight(p)) {
            return n;
        }
        return null;
    }

    /*
     *  This returns the node immediately to the right of p,
     *  but not including p; can be null.
     */
    public final StatusTreeNode rightNodeForPoint(Point p) {
        StatusTreeNode n = root;
        if (n == null) {
            return null;
        }
        while (n.seg_stored == null) {
            if (!n.left.seg_max.pointToLeft(p)) {
                n = n.right;
            } else {
                n = n.left;
            }
        }
        if (n.seg_stored.pointToLeft(p)) {
            return n;
        }
        return null;
    }

    /*
     *  Private section
     */
    private final void leftRotate(StatusTreeNode x) {
        StatusTreeNode y = x.right;
        x.right = y.left;
        if (y.left != null) {
            y.left.parent = x;
        }
        y.parent = x.parent;
        if (x.parent == null) {
            root = y;
        } else {
            if (x == x.parent.left) {
                x.parent.left = y;
            } else {
                x.parent.right = y;
            }
        }
        y.left = x;
        x.parent = y;

        /*
         *  Restore augmentation (note sequence of restoration)
         */
        x.seg_max = x.right.seg_max;
        x.seg_min = x.left.seg_min;
        y.seg_max = y.right.seg_max;
        y.seg_min = y.left.seg_min;
    }

    private final void rightRotate(StatusTreeNode y) {
        StatusTreeNode x = y.left;
        y.left = x.right;
        if (x.right != null) {
            x.right.parent = y;
        }
        x.parent = y.parent;
        if (x.parent == null) {
            root = x;
        } else if (y == y.parent.right) {
            y.parent.right = x;
        } else {
            y.parent.left = x;
        }
        x.right = y;
        y.parent = x;

        /*
         *  Restore augmentation (note sequence of restoration)
         */
        y.seg_max = y.right.seg_max;
        y.seg_min = y.left.seg_min;
        x.seg_max = x.right.seg_max;
        x.seg_min = x.left.seg_min;
    }
}

public class Sweep {

    private static EventTree event_queue;
    private static StatusTree status_tree;
    private static IntersectionPoint intersectionPoint;
    private static int numberOfSegment;

    /*
     *   The Main Method
     */
    public static void main(String[] args) {
        if (args.length < 2) {
            System.out.println("Usage:\n\tjava sweep <infile> <outfile>");
            return;
        }
        event_queue = new EventTree();
        numberOfSegment = 0;
        generateInput(args[0]);
        doProcessing();
        generateOutput(args[1]);
        return;
    }

    /*
     *  Private section
     */
    private static void doProcessing() {
        status_tree = new StatusTree();
        while (!event_queue.isEmpty()) {
            QueueEvent e = event_queue.get();
            handleEventPoint(e);
        }
    }

    private static boolean generateInput(String name) {
        try {
            PrintWriter out = new PrintWriter(new File(name));
            Random r = new Random();
            System.out.print("Enter the number of segments :  ");
            Scanner in = new Scanner(System.in);
            for (int count = in.nextInt(); count > 0; count--) {
                int x1 = r.nextInt(1025), y1 = r.nextInt(769),
                        x2 = r.nextInt(1025), y2 = r.nextInt(769);
                out.printf("((%d,%d)(%d,%d))\n", x1, y1, x2, y2);
                Segment ns = new Segment(x1, y1, x2, y2);
                ++numberOfSegment;
                event_queue.insert(ns.p1, ns);
                event_queue.insert(ns.p2, ns);
            }
            out.close();
        } catch (FileNotFoundException unUsed) {
            System.out.println("File " + name + " does not exist...");
            return false;
        }
        return true;
    }

    private static boolean generateOutput(String name) {
        PrintWriter out;
        try {
            out = new PrintWriter(new File(name));
        } catch (FileNotFoundException unUsed) {
            System.out.println("File " + name + " does not exist...");
            return false;
        }
        if (intersectionPoint != null) {
            IntersectionPoint ipt = intersectionPoint;
            while (ipt != null) {
                out.printf("(%g,%g)\n", ipt.point.x, ipt.point.y);
                ipt = ipt.next;
            }
        }
        out.close();
        return true;
    }

    /*
     *  Everything in between ln and rn contains point. We need to sort
     *  out segments which have point as one of their ends...
     */
    private static SegmentList calculateCrosses(QueueEvent e, StatusTreeNode ln,
            StatusTreeNode rn) {
        StatusTreeNode run = status_tree.successor(ln);
        SegmentList cross = null;
        while (run != rn) {
            if ((e.pt.compare(run.seg_stored.p1) != 0) && (e.pt.compare(run.seg_stored.p2) != 0)) {
                cross = new SegmentList(run.seg_stored, cross);
            }
            run = status_tree.successor(run);
        }
        return cross;
    }

    /*
     *  This checks and records intersection
     */
    private static void checkIntersection(Point p, SegmentList upper,
            SegmentList crosses, SegmentList lower) {
        int num_inter = SegmentList.length(upper)
                + SegmentList.length(crosses) + SegmentList.length(lower);
        if (num_inter <= 1) {
            return;
        }
        intersectionPoint = new IntersectionPoint(p, intersectionPoint);
        intersectionPoint.addSegmentList(upper);
        intersectionPoint.addSegmentList(crosses);
        intersectionPoint.addSegmentList(lower);
    }

    /*
     *  Delete Nodes
     */
    private static void deleteInternalNodes(StatusTreeNode leftNode, StatusTreeNode rightNode) {
        StatusTreeNode run = status_tree.successor(leftNode);

        /*
         *  list always starts at leftNode
         */
        while (run != rightNode) {
            status_tree.deleteNode(run);
            run = status_tree.successor(leftNode);
        }
    }

    /*
     *  Main engine of this program
     */
    private static void handleEventPoint(QueueEvent event) {
        StatusTreeNode leftNode = status_tree.leftNodeForPoint(event.pt);
        StatusTreeNode rightNode = status_tree.rightNodeForPoint(event.pt);

        /*
         *  Cross list...
         */
        SegmentList C = calculateCrosses(event, leftNode, rightNode);

        /*
         *  is this an intersection
         */
        checkIntersection(event.pt, event.U, C, event.L);

        /*
         *  Delete everything that crosses the point from the tree
         */
        deleteInternalNodes(leftNode, rightNode);

        /*
         *  Set new order
         */
        status_tree.p_sweep = event.pt;

        /*
         *  If no crossing, the point is bottom end. Check intersection of new neighbors and
         *  just return
         */
        if ((event.U == null) && (C == null)) {
            if ((leftNode != null) && (rightNode != null)) {
                findNewEvent(leftNode.seg_stored, rightNode.seg_stored, event.pt);
            }
            return;
        }

        /*
         *  Insert these from event.U and reinsert ones from C
         */
        SegmentList run = event.U;
        while (run != null) {
            status_tree.insertSegment(run.seg);
            run = run.next;
        }
        run = C;
        while (run != null) {
            status_tree.insertSegment(run.seg);
            run = run.next;
        }

        /*
         * rightNode and leftNode do not contain the point, and therefore did not change.
         * The rest of points all does intersect in current point
         */
        if (leftNode != null) {
            findNewEvent(leftNode.seg_stored, status_tree.successor(leftNode).seg_stored, event.pt);
        }
        if (rightNode != null) {
            findNewEvent(status_tree.predecessor(rightNode).seg_stored, rightNode.seg_stored, event.pt);
        }
    }

    private static void findNewEvent(Segment left, Segment right, Point p) {
        Point interSectionPoint = Segment.intersect(left, right);
        if ((interSectionPoint != null) && (p.compare(interSectionPoint) < 0)) {
            event_queue.insert(interSectionPoint);
        }
    }
}