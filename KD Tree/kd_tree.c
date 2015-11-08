#include "kd_tree.h"

void
swap(Point *a, Point *b) {
    Point temp = *a;
    *a = *b;
    *b = temp;
}

void
quickSort(Point *list, int low, int high, bool Xsort) {
    if (low < high) {
        float pivot = (Xsort ? list[high].x : list[high].y);
        int i = low - 1;
        for (int j = low; j < high; j++) {
            if (Xsort ? list[j].x <= pivot : list[j].y <= pivot) {
                i++;
                swap(&list[i], &list[j]);
            }
        }
        swap(&list[i + 1], &list[high]);
        quickSort(list, low, i, Xsort);
        quickSort(list, i + 2, high, Xsort);
    }
}

KdTree *
buildKdTree(Point *list, int low, int high, int depth) {
    if (high == low) {
        KdTree *leaf = (KdTree *) calloc(1, sizeof (KdTree));
        leaf->point = list[low];
        leaf->depth = depth;
        leaf->left = leaf->right = NULL;
        return leaf;
    }
    KdTree *node = (KdTree *) calloc(1, sizeof (KdTree));
    int median;
    node->depth = depth;
    if (!(depth % 2)) {
        quickSort(list, low, high, true);
        median = low + (!((high - low + 1) % 2) ? (high - low) / 2 : (high - low + 1) / 2);
        node->point.x = list[median].x;
    } else {
        quickSort(list, low, high, false);
        median = low + (!((high - low + 1) % 2) ? (high - low) / 2 : (high - low + 1) / 2);
        node->point.y = list[median].y;
    }
    node->left = buildKdTree(list, low, median, depth + 1);
    node->right = buildKdTree(list, median + 1, high, depth + 1);
    return node;
}

KdTree *
realize(KdTree *node) {
    if (!node->depth)
        node->parent = NULL;
    if (!node->left && !node->right)
        return node;
    node->left->parent = node->right->parent = node;
    realize(node->left);
    realize(node->right);
    return node;
}

Point *
getRegion(KdTree *node) {
    if (!node->right && !node->left)
        return NULL;
    Point *bounds = (Point *) calloc(2, sizeof (Point));
    float xmin, xmax, ymin, ymax;
    xmin = xmax = ymin = ymax = -1;
    if (node->depth % 2) {
        if (node->parent) {
            if (node == node->parent->left)
                xmax = node->parent->point.x;
            else
                xmin = node->parent->point.x;
            node = node->parent;
        }
        if (node->parent) {
            if (node == node->parent->left)
                ymax = node->parent->point.y;
            else
                ymin = node->parent->point.y;
            node = node->parent;
        }
        if (node->parent) {
            if (node == node->parent->left) {
                if (xmax == -1)
                    xmax = node->parent->point.x;
                else xmin = 0;
            } else {
                if (xmin == -1)
                    xmin = node->parent->point.x;
                else xmax = 1024;
            }
            node = node->parent;
        }
        if (node->parent) {
            if (node == node->parent->left) {
                if (ymax == -1)
                    ymax = node->parent->point.y;
                else ymin = 0;
            } else {
                if (ymin == -1)
                    ymin = node->parent->point.y;
                else ymax = 768;
            }
        }
    } else {
        if (node->parent) {
            if (node == node->parent->left)
                ymax = node->parent->point.y;
            else
                ymin = node->parent->point.y;
            node = node->parent;
        }
        if (node->parent) {
            if (node == node->parent->left)
                xmax = node->parent->point.x;
            else
                xmin = node->parent->point.x;
            node = node->parent;
        }
        if (node->parent) {
            if (node == node->parent->left) {
                if (ymax == -1)
                    ymax = node->parent->point.y;
                else ymin = 0;
            } else {
                if (ymin == -1)
                    ymin = node->parent->point.y;
                else ymax = 768;
            }
            node = node->parent;
        }
        if (node->parent) {
            if (node == node->parent->left) {
                if (xmax == -1)
                    xmax = node->parent->point.x;
                else xmin = 0;
            } else {
                if (xmin == -1)
                    xmin = node->parent->point.x;
                else xmax = 1024;
            }
        }
    }
    xmax = (xmax == -1) ? 1024 : xmax;
    ymax = (ymax == -1) ? 768 : ymax;
    xmin = (xmin == -1) ? 0 : xmin;
    ymin = (ymin == -1) ? 0 : ymin;
    bounds[0].x = xmin;
    bounds[0].y = ymin;
    bounds[1].x = xmax;
    bounds[1].y = ymax;
    return bounds;
}

bool
isPointInsideRegion(Point p, Point *region) {
    return (region[0].x <= p.x && region[1].x >= p.x && region[0].y <= p.y && region[1].y >= p.y) ? true : false;
}

bool
isFullyInside(Point *region, Point *range) {
    return (isPointInsideRegion(region[0], range) && isPointInsideRegion(region[1], range)) ? true : false;
}

void
reportSubTree(PointList **list, KdTree *node) {
    if (!node->left && !node->right) {
        if (!(*list)) {
            *list = (PointList *) calloc(1, sizeof (PointList));
            (*list)->point = node->point;
            (*list)->previous = (*list)->next = NULL;
        } else {
            PointList *temp = *list;
            while (temp->next)
                temp = temp->next;
            temp->next = (PointList *) calloc(1, sizeof (PointList));
            temp->next->point = node->point;
            temp->next->previous = temp;
            temp->next->next = NULL;
        }
        return;
    }
    reportSubTree(list, node->left);
    reportSubTree(list, node->right);
    return;
}

void
searchKdTree(KdTree *node, PointList **list, Point *range) {
    if (!node->left && !node->right) {
        if (isPointInsideRegion(node->point, range)) {
            if (!(*list)) {
                *list = (PointList *) calloc(1, sizeof (PointList));
                (*list)->point = node->point;
                (*list)->previous = (*list)->next = NULL;
            } else {
                PointList *temp = *list;
                while (temp->next)
                    temp = temp->next;
                temp->next = (PointList *) calloc(1, sizeof (PointList));
                temp->next->point = node->point;
                temp->next->previous = temp;
                temp->next->next = NULL;
            }
        }
    } else {
        Point *region = getRegion(node->left);
        if (region && isFullyInside(region, range))
            reportSubTree(list, node->left);
        else
            searchKdTree(node->left, list, range);
        region = getRegion(node->right);
        if (region && isFullyInside(region, range))
            reportSubTree(list, node->right);
        else
            searchKdTree(node->right, list, range);
    }
    return;
}