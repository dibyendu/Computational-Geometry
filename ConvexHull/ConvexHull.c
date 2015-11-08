/*
 * File:   ConvexHull.c
 * Author: dibyendu
 *
 * Created on January 8, 2011
 */

#include "ConvexHull.h"

long
noOfPoints(PointList *list) {
    long count = 0;
    while(list) {
        count++;
        list = list->next;
    }

    return count;
}

bool
isEqualPoints(Point p1, Point p2) {

    return (p1.x == p2.x && p1.y == p2.y) ? true : false;
}

void
swapPoints(Point *p1, Point *p2) {

    Point point = *p1;
    *p1 = *p2;
    *p2 = point;

    return;
}

void
sortPoints(Point points[], long noOfPoints) {

    for(long i = 0; i < noOfPoints; i++) {
        for(long j = noOfPoints - 1; j > i; j--) {
            if(points[j - 1].x > points[j].x)
                swapPoints(&points[j], &points[j - 1]);
        }
    }

    for(long i = 0; i < noOfPoints; i++) {
        if(i != noOfPoints - 1 && points[i].x == points[i + 1].x) {
            long j = i;
            while(j != noOfPoints - 1 && points[j].x == points[j + 1].x)
                j++;
            j -= i - 1;
            for(long k = i; k < i + j; k++) {
                for(long l = i + j - 1; l > k; l--) {
                    if(points[l - 1].y > points[l].y)
                        swapPoints(&points[l], &points[l - 1]);
                }
            }
            i += j - 1;
        }
    }

    return;
}

bool
isPointOnLeft(Line line, Point point) {
    Point temp;
    LineDirection direction = bottomToTop;

    if(line.point_1.x == line.point_2.x) {
        if(line.point_1.y > line.point_2.y) {
            if(point.x < line.point_1.x)
                return true;
            else return false;
        } else {
            if(point.x > line.point_1.x)
                return true;
            else return false;
        }
    }

    if(line.point_1.y < line.point_2.y) {
        temp = line.point_1;
        line.point_1 = line.point_2;
        line.point_2 = temp;
        direction = topToBottom;
    } else if(line.point_1.y == line.point_2.y) {
        if(line.point_1.x > line.point_2.x) {
            temp = line.point_1;
            line.point_1 = line.point_2;
            line.point_2 = temp;
            direction = topToBottom;
        }
    }

    double slope = (line.point_1.y - line.point_2.y) / (line.point_2.x - line.point_1.x);
    temp.y = line.point_1.y - point.y;
    temp.x = point.x - line.point_1.x;

    double check = temp.y - slope * temp.x;

    if(slope >= 0) {
        if(direction == bottomToTop)
            return check > 0 ? true : false;
        else
            return check < 0 ? true : false;
    } else {
        if(direction == bottomToTop)
            return check < 0 ? true : false;
        else
            return check > 0 ? true : false;
    }
}

LineList *
convexHullSlow(PointList *pointList) {

    LineList *lineList = NULL, *newLineNode;
    long inputPoints = noOfPoints(pointList);

    if(inputPoints <= 1)
        return NULL;

    Point points[inputPoints];
    bool validLine;

    for(long i = 0; pointList; i++, pointList = pointList->next)
        points[i] = pointList->point;

    for(long i = 0; i < inputPoints; i++) {
        for(long j = 0; j < inputPoints; j++) {
            Line tempLine;
            validLine = true;
            if(i != j) {
                tempLine.point_1 = points[i];
                tempLine.point_2 = points[j];
                for(long k = 0; k < inputPoints; k++) {
                    if(k != i && k != j) {
                        if(isPointOnLeft(tempLine, points[k]))
                            validLine = false;
                    }
                }
                if(validLine) {
                    newLineNode = (LineList *) calloc(1, sizeof (LineList));
                    newLineNode->next = NULL;
                    newLineNode->line = tempLine;
                    if(!lineList)
                        lineList = newLineNode;
                    else {
                        LineList *head = lineList;
                        while(head->next)
                            head = head->next;
                        head->next = newLineNode;
                    }
                }
            }
        }
    }

    return lineList;
}

PointList *
convexHullFast(PointList *pointList) {

    long inputPoints = noOfPoints(pointList);

    if(inputPoints <= 1)
        return NULL;

    Point points[inputPoints];

    for(long i = 0; pointList; i++, pointList = pointList->next)
        points[i] = pointList->point;

    sortPoints(points, inputPoints);

    PointList *upperHull = (PointList *) calloc(1, sizeof (PointList));
    upperHull->next = (PointList *) calloc(1, sizeof (PointList));
    upperHull->previous = NULL;
    upperHull->next->next = NULL;
    upperHull->next->previous = upperHull;
    upperHull->point = points[0];
    upperHull->next->point = points[1];

    for(long i = 2; i < inputPoints; i++) {
        Line line;
        PointList *temp = upperHull;
        while(temp->next)
            temp = temp->next;
        temp->next = (PointList *) calloc(1, sizeof (PointList));
        temp->next->point = points[i];
        temp->next->next = NULL;
        temp->next->previous = temp;
        if(noOfPoints(upperHull) > 2) {
            line.point_1 = temp->previous->point;
            line.point_2 = temp->point;
        }
        while(noOfPoints(upperHull) > 2 && isPointOnLeft(line, points[i])) {
            temp = upperHull;
            while(temp->next->next->next)
                temp = temp->next;
            PointList *delete = temp->next;
            temp->next->next->previous = temp;
            temp->next = temp->next->next;
            free(delete);
            if(noOfPoints(upperHull) > 2) {
                line.point_1 = temp->previous->point;
                line.point_2 = temp->point;
            }
        }
    }

    PointList *lowerHull = (PointList *) calloc(1, sizeof (PointList));
    lowerHull->next = (PointList *) calloc(1, sizeof (PointList));
    lowerHull->previous = NULL;
    lowerHull->next->next = NULL;
    lowerHull->next->previous = lowerHull;
    lowerHull->point = points[inputPoints - 1];
    long index;
    for(long i = inputPoints - 2; i >= 0; i--) {
        if(!isEqualPoints(points[i], points[inputPoints - 1])) {
            lowerHull->next->point = points[i];
            index = i;
            break;
        }
    }

    for(long i = index - 1; i >= 0; i--) {
        Line line;
        PointList *temp = lowerHull;
        while(temp->next)
            temp = temp->next;
        if(isEqualPoints(points[i], temp->point) ||
           isEqualPoints(points[i], temp->previous->point))
            continue;
            temp->next = (PointList *) calloc(1, sizeof (PointList));
        temp->next->point = points[i];
        temp->next->next = NULL;
        temp->next->previous = temp;
        if(noOfPoints(lowerHull) > 2) {
            line.point_1 = temp->previous->point;
            line.point_2 = temp->point;
        }
        while(noOfPoints(lowerHull) > 2 && isPointOnLeft(line, points[i])) {
            temp = lowerHull;
            while(temp->next->next->next)
                temp = temp->next;
            PointList *delete = temp->next;
            temp->next->next->previous = temp;
            temp->next = temp->next->next;
            free(delete);
            if(noOfPoints(lowerHull) > 2) {
                line.point_1 = temp->previous->point;
                line.point_2 = temp->point;
            }
        }
    }

    lowerHull = lowerHull->next;
    free(lowerHull->previous);
    PointList *temp = upperHull;
    while(temp->next)
        temp = temp->next;
    temp->next = lowerHull;
    lowerHull->previous = temp;
    while(temp->next)
        temp = temp->next;
    temp = temp->previous;
    free(temp->next);
    temp->next = NULL;

    return upperHull;
}