/*
 * File:   LineSegmentIntersection.c
 * Author: dibyendu
 *
 * Created on January 9, 2011
 */

#include "LineSegmentIntersection.h"

long
noOfLines(LineList *list) {
    long count = 0;
    while(list) {
        count++;
        list = list->next;
    }

    return count;
}

long
noOfPoints(PointList *list) {
    long count = 0;
    while(list) {
        count++;
        list = list->next;
    }

    return count;
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

PointPosition
findPointPosition(Line line, Point point) {

    Point temp;

    if(line.point_1.x == line.point_2.x) {
        if(point.x < line.point_1.x)
            return leftOfLine;
        else if(point.x == line.point_1.x)
            return onLine;
        return rightOfLine;
    }

    double slope = (line.point_1.y - line.point_2.y) / (line.point_2.x - line.point_1.x);
    temp.y = line.point_1.y - point.y;
    temp.x = point.x - line.point_1.x;

    double check = temp.y - slope * temp.x;

    if(slope >= 0) {
        if(check > 0)
            return leftOfLine;
        else if(check < 0)
            return rightOfLine;
        return onLine;
    } else {
        if(check < 0)
            return leftOfLine;
        else if(check > 0)
            return rightOfLine;
        return onLine;
    }
}

bool
isPointWithinSegment(Line line, Point point) {
    if(line.point_1.y != line.point_2.y) {
        if(point.y <= line.point_1.y && point.y >= line.point_2.y)
            return true;
    } else {
        if(point.x >= line.point_1.x && point.x <= line.point_2.x)
            return true;
    }

    return false;
}

bool
isEqualPoints(Point p1, Point p2) {

    return (p1.x == p2.x && p1.y == p2.y) ? true : false;
}

bool
isEqualLines(Line line_1, Line line_2) {

    return (isEqualPoints(line_1.point_1, line_2.point_1) &&
            isEqualPoints(line_1.point_2, line_2.point_2)) ||
              (isEqualPoints(line_1.point_1, line_2.point_2) &&
               isEqualPoints(line_1.point_2, line_2.point_1)) ? true : false;
}

PointList *
intersectingPoint(Line line_1, Line line_2) {

    PointList *point = NULL;

    if(line_1.point_1.y < line_1.point_2.y) {
        swapPoints(&(line_1.point_1), &(line_1.point_2));
    } else if(line_1.point_1.y == line_1.point_2.y) {
        if(line_1.point_1.x > line_1.point_2.x)
            swapPoints(&(line_1.point_1), &(line_1.point_2));
    }

    if(line_2.point_1.y < line_2.point_2.y) {
        swapPoints(&(line_2.point_1), &(line_2.point_2));
    } else if(line_2.point_1.y == line_2.point_2.y) {
        if(line_2.point_1.x > line_2.point_2.x)
            swapPoints(&(line_2.point_1), &(line_2.point_2));
    }

    PointPosition pos_1 = findPointPosition(line_1, line_2.point_1),
              Pos_2 = findPointPosition(line_1, line_2.point_2);
    point = (PointList *) calloc(1, sizeof (PointList));
    point->next = NULL;

    if((pos_1 == leftOfLine && Pos_2 == leftOfLine) ||
       (pos_1 == rightOfLine && Pos_2 == rightOfLine)) {
        free(point);
        return NULL;
    } else if(pos_1 == onLine && Pos_2 != onLine) {
        if(isPointWithinSegment(line_1, line_2.point_1))
            point->point = line_2.point_1;
        else {
            free(point);
            return NULL;
        }
    } else if(pos_1 != onLine && Pos_2 == onLine) {
        if(isPointWithinSegment(line_1, line_2.point_2))
            point->point = line_2.point_2;
        else {
            free(point);
            return NULL;
        }
    } else if(pos_1 == onLine && Pos_2 == onLine) {
        if(isEqualPoints(line_1.point_1, line_2.point_2))
            point->point = line_1.point_1;
        else if(isEqualPoints(line_1.point_2, line_2.point_1))
            point->point = line_2.point_1;
        else {
            free(point);
            return NULL;
        }
    } else {
        if(line_1.point_1.x == line_1.point_2.x) {
            Point temp;
            temp.x = line_1.point_1.x;
            double slope = (line_2.point_1.y - line_2.point_2.y) /
                      (line_2.point_2.x - line_2.point_1.x);
            temp.y = line_2.point_1.y - slope * (line_1.point_1.x - line_2.point_1.x);
            if(isPointWithinSegment(line_2, temp) && isPointWithinSegment(line_1, temp))
                point->point = temp;
            else {
                free(point);
                return NULL;
            }
        } else if(line_2.point_1.x == line_2.point_2.x) {
            Point temp;
            temp.x = line_2.point_1.x;
            double slope = (line_1.point_1.y - line_1.point_2.y) /
                      (line_1.point_2.x - line_1.point_1.x);
            temp.y = line_1.point_1.y - slope * (line_2.point_1.x - line_1.point_1.x);
            if(isPointWithinSegment(line_2, temp) && isPointWithinSegment(line_1, temp))
                point->point = temp;
            else {
                free(point);
                return NULL;
            }
        } else {
            Point temp;
            double slope_1 = (line_1.point_1.y - line_1.point_2.y) /
                      (line_1.point_2.x - line_1.point_1.x);
            double slope_2 = (line_2.point_1.y - line_2.point_2.y) /
                      (line_2.point_2.x - line_2.point_1.x);
            Line tempLine = line_1;
            line_1.point_1.x -= line_2.point_1.x;
            line_1.point_1.y = line_2.point_1.y - line_1.point_1.y;
            temp.x = (line_1.point_1.y - slope_1 * line_1.point_1.x) / (slope_2 - slope_1);
            temp.y = slope_2 * temp.x;
            temp.x += line_2.point_1.x;
            temp.y = line_2.point_1.y - temp.y;
            if(isPointWithinSegment(line_2, temp) && isPointWithinSegment(tempLine, temp))
                point->point = temp;
            else {
                free(point);
                return NULL;
            }
        }
    }

    return point;
}

void
removeDuplicate(PointList *head) {
    long number = noOfPoints(head);
    Point points[number];
    PointList *temp = head;
    long i = 0;
    while(temp) {
        points[i] = temp->point;
        i++;
        temp = temp->next;
    }
    sortPoints(points, number);
    temp = head;
    PointList *list = temp;
    for(long i = 0; i < number; i += 2) {
        list->point = points[i];
        temp = list;
        list = list->next;
    }
    free(list);
    temp->next = NULL;

    return;

}

PointList *
findIntersectionList(LineList *lineList) {

    PointList *pointList = NULL;

    long inputLines = noOfLines(lineList);

    if(inputLines <= 1)
        return pointList;

    LineList *tempList = lineList;
    while(tempList) {
        LineList *tempLine = lineList;
        while(tempLine) {
            if(!isEqualLines(tempList->line, tempLine->line)) {
                if(!pointList)
                    pointList = intersectingPoint(tempList->line, tempLine->line);
                else {
                    PointList *temp = pointList;
                    while(temp->next)
                        temp = temp->next;
                    temp->next = intersectingPoint(tempList->line, tempLine->line);
                }
            }
            tempLine = tempLine->next;
        }
        tempList = tempList->next;
    }
    if(pointList)
        removeDuplicate(pointList);

    return pointList;
}
