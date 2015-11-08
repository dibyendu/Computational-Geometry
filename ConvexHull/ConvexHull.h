/*
 * File:   ConvexHull.h
 * Author: dibyendu
 *
 * Created on January 7, 2011
 */

#ifndef CONVEXHULL_H
#define	CONVEXHULL_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <cairo.h>
#include <math.h>

typedef struct {
    double x, y;
} Point;

typedef struct _PointList {
    Point point;
    struct _PointList *next, *previous;
} PointList;

typedef struct {
    Point point_1, point_2;
} Line;

typedef struct _LineList {
    Line line;
    struct _LineList *next;
} LineList;

typedef enum {
    topToBottom,
    bottomToTop
} LineDirection;

LineList* convexHullSlow(PointList *pointList);

PointList* convexHullFast(PointList *pointList);

#endif	/* CONVEXHULL_H */