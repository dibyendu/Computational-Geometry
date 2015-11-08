/* 
 * File:   kd_tree.h
 * Author: dibyendu
 *
 * Created on June 4, 2011, 12:10 AM
 */

#ifndef KD_TREE_H
#define	KD_TREE_H
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <cairo.h>
#include <math.h>

#define MAX_POINTS 10000

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

typedef struct {
  float x, y;
} Point;

typedef struct _PointList {
  Point point;
  struct _PointList *next, *previous;
} PointList;

typedef struct _BST {
    Point point;
    int depth;
    struct _BST *parent, *left, *right;
} KdTree;

KdTree *buildKdTree(Point *Xlist, int low, int high, int depth);
KdTree *realize(KdTree *root);
void searchKdTree(KdTree *node, PointList **list, Point *range);

#endif	/* KD_TREE_H */