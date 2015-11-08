/*
 * File:   LineSegmentIntersection.h
 * Author: dibyendu
 *
 * Created on January 9, 2011
 */

#ifndef LINESEGMENTINTERSECTION_H
    #define	LINESEGMENTINTERSECTION_H

    #include <stdbool.h>
    #include <stdlib.h>
    #include <math.h>
    #include <gtk/gtk.h>
    #include <cairo.h>

typedef struct {
    double x, y;
} Point;

typedef struct _PointList {
    Point point;
    struct _PointList *next;
} PointList;

typedef struct {
    Point point_1, point_2;
} Line;

typedef struct _LineList {
    Line line;
    struct _LineList *next;
} LineList;

typedef enum {
    leftOfLine,
    onLine,
    rightOfLine
} PointPosition;

PointList* findIntersectionList(LineList *lineList);
long noOfLines(LineList *list);

#endif	/* LINESEGMENTINTERSECTION_H */