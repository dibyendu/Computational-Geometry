/* 
 * File:   gui.h
 * Author: dibyendu
 *
 * Created on June 2, 2011, 8:27 PM
 */

#ifndef GUI_H
#define	GUI_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <gtk/gtk.h>
#include <cairo.h>

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

    typedef enum {
        in = 1,
        out
    } IO;

    typedef struct {
        char path[80];
        IO flag;
    } Io_Status;

    /*
     * The main API for the GUI interface
     */
    bool createGui(int argc, char **argv, char *inputPath, char *outputPath);

#ifdef	__cplusplus
}
#endif

#endif	/* GUI_H */