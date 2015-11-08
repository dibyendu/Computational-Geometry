/*
 * File:   main.c
 * Author: dibyendu
 *
 * Created on January 9, 2011
 */

#include "LineSegmentIntersection.h"

GdkColor *color = NULL;
GtkWidget *label;
LineList *lineList = NULL;
PointList *intersectionPointList = NULL;
static unsigned short buttonPressCount = 0;
static double radius = 24;
static double alpha = 0;
gint timerTag = 0;

void
reDrawWindow(GtkWindow *window) {
    gint posx, posy;
    gtk_window_get_position(window, &posx, &posy);
    gtk_window_move(window, posx+1, posy+1);
    gtk_window_move(window, posx, posy);
    return;
}

gboolean
drawLines(GtkWidget *widget, GdkEventButton *event, gpointer data) {

    LineList *tempList = NULL;
    buttonPressCount++;
    buttonPressCount %= 2;

    if(buttonPressCount) {
        if(!lineList) {
            lineList = (LineList *) calloc(1, sizeof (LineList));
            lineList->line.point_1.x = event->x;
            lineList->line.point_1.y = event->y;
            lineList->next = NULL;
        } else {
            tempList = lineList;
            while(tempList->next)
                tempList = tempList->next;
            tempList->next = (LineList *) calloc(1, sizeof (LineList));
            tempList->next->line.point_1.x = event->x;
            tempList->next->line.point_1.y = event->y;
            tempList->next->next = NULL;
        }
    } else {
        tempList = lineList;
        while(tempList->next)
            tempList = tempList->next;
        tempList->line.point_2.x = event->x;
        tempList->line.point_2.y = event->y;
    }

    cairo_t *cr = gdk_cairo_create(widget->window);
    cairo_set_source_rgba(cr, 0, 0, 1, 0.5);
    cairo_arc(cr, event->x, event->y, 4, 0, 2 * M_PI);
    cairo_fill(cr);

    if(!buttonPressCount) {
        cairo_set_source_rgba(cr, 0, 1, 0, 0.8);
        cairo_set_line_width(cr, 1.4);
        cairo_move_to(cr, tempList->line.point_1.x, tempList->line.point_1.y);
        cairo_line_to(cr, tempList->line.point_2.x, tempList->line.point_2.y);
        cairo_stroke(cr);
    }
    reDrawWindow(GTK_WINDOW(GTK_WIDGET(widget)->parent->parent->parent));
    cairo_destroy(cr);

    return TRUE;
}

gboolean
resetScreen(GtkWidget *widget, gpointer data) {

    gtk_widget_modify_bg(GTK_WIDGET(data), GTK_STATE_NORMAL, color);
    if(lineList) {
        free(lineList);
        lineList = NULL;
    }
    if(intersectionPointList) {
        free(intersectionPointList);
        intersectionPointList = NULL;
    }
    buttonPressCount = 0;
    if(timerTag)
        g_source_remove(timerTag);
    gtk_label_set_label(GTK_LABEL(label), "");

    return TRUE;
}

gboolean
drawScreen(gpointer data) {

    if((GTK_WIDGET(data))->window == NULL)
        return FALSE;
    cairo_t * cr = gdk_cairo_create((GTK_WIDGET(data))->window);
    cairo_set_source_rgba(cr, 1, 1, 1, 1);
    cairo_rectangle(cr, 0, 0, (GTK_WIDGET(data))->allocation.width,
                    (GTK_WIDGET(data))->allocation.height);
    cairo_fill(cr);
    LineList * tempLine = lineList;
    while(tempLine) {
        cairo_set_source_rgba(cr, 0, 0, 1, 0.5);
        cairo_arc(cr, tempLine->line.point_1.x, tempLine->line.point_1.y,
                  4, 0, 2 * M_PI);
        cairo_arc(cr, tempLine->line.point_2.x, tempLine->line.point_2.y,
                  4, 0, 2 * M_PI);
        cairo_fill(cr);
        cairo_set_source_rgba(cr, 0, 1, 0, 0.8);
        cairo_set_line_width(cr, 1.4);
        cairo_move_to(cr, tempLine->line.point_1.x, tempLine->line.point_1.y);
        cairo_line_to(cr, tempLine->line.point_2.x, tempLine->line.point_2.y);
        cairo_stroke(cr);
        tempLine = tempLine->next;
    }
    alpha += 0.025;
    radius -= 1;
    cairo_set_source_rgba(cr, 1, 0, 0, alpha);
    PointList *tempPoint = intersectionPointList;
    while(tempPoint) {
        cairo_arc(cr, tempPoint->point.x, tempPoint->point.y,
                  radius, 0, 2 * M_PI);
        cairo_fill(cr);
        tempPoint = tempPoint->next;
    }
    if(radius <= 4 || alpha >= 0.5) {

        radius = 24;
        alpha = 0;
        return FALSE;
    }

    return TRUE;
}

void
drawIntersections(GtkWidget *widget, gpointer data) {

    cairo_t *cr = gdk_cairo_create((GTK_WIDGET(data))->window);
    if(buttonPressCount) {
        if(lineList->next) {
            LineList *tempList = lineList;
            while(tempList->next->next)
                tempList = tempList->next;
            cairo_set_source_rgba(cr, 1, 1, 1, 1);
            cairo_arc(cr, tempList->next->line.point_1.x, tempList->next->line.point_1.y,
                      4, 0, 2 * M_PI);
            cairo_fill(cr);
            free(tempList->next);
            tempList->next = NULL;
        } else {
            gtk_widget_modify_bg(GTK_WIDGET(data), GTK_STATE_NORMAL, color);
            free(lineList);
            lineList = NULL;
            buttonPressCount = 0;
        }
    }

    long lines = noOfLines(lineList);
    GTimer *timer = g_timer_new();
    intersectionPointList = findIntersectionList(lineList);
    gdouble elaspedTime = g_timer_elapsed(timer, NULL);
    g_timer_destroy(timer);

    if(intersectionPointList)
        timerTag = g_timeout_add(100, (GSourceFunc) drawScreen, data);
    char str[80];
    sprintf(str, "Time Taken  :  %lf ms    [ for %ld line(s) ]", elaspedTime * 1000, lines);
    gtk_label_set_label(GTK_LABEL(label), str);
    cairo_destroy(cr);

    return;
}

int
main(int argc, char** argv) {

    GtkWidget *window, *vBox, *hBox, *eventBox, *button, *separator;
    color = (GdkColor *) calloc(1, sizeof (GdkColor));

    gtk_init(&argc, &argv);

    gdk_color_parse("#FFFFFF", color);
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    g_signal_connect(window, "delete-event", G_CALLBACK(gtk_main_quit), NULL);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_window_set_title(GTK_WINDOW(window), "Line Segment Intersection");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);
    gtk_window_set_resizable(GTK_WINDOW(window), TRUE);

    eventBox = gtk_event_box_new();
    g_signal_connect(eventBox, "button-press-event", G_CALLBACK(drawLines), NULL);
    gtk_widget_modify_bg(eventBox, GTK_STATE_NORMAL, color);
    vBox = gtk_vbox_new(TRUE, 10);
    button = gtk_button_new_with_label("        Find\nIntersection");
    g_signal_connect(button, "clicked", G_CALLBACK(drawIntersections), (gpointer) eventBox);
    gtk_box_pack_start(GTK_BOX(vBox), button, TRUE, FALSE, 4);
    button = gtk_button_new_with_label("Reset");
    g_signal_connect(button, "clicked", G_CALLBACK(resetScreen), (gpointer) eventBox);
    gtk_box_pack_start(GTK_BOX(vBox), button, TRUE, FALSE, 4);

    hBox = gtk_hbox_new(FALSE, 4);
    gtk_box_pack_start(GTK_BOX(hBox), eventBox, TRUE, TRUE, 4);
    separator = gtk_vseparator_new();
    gtk_box_pack_start(GTK_BOX(hBox), separator, FALSE, FALSE, 4);
    gtk_box_pack_start(GTK_BOX(hBox), vBox, FALSE, FALSE, 4);

    label = gtk_label_new("");

    vBox = gtk_vbox_new(FALSE, 4);
    gtk_box_pack_end(GTK_BOX(vBox), label, FALSE, FALSE, 2);
    separator = gtk_hseparator_new();
    gtk_box_pack_end(GTK_BOX(vBox), separator, FALSE, FALSE, 4);
    gtk_box_pack_start(GTK_BOX(vBox), hBox, TRUE, TRUE, 2);

    gtk_container_add(GTK_CONTAINER(window), vBox);
    gtk_widget_show_all(window);
    gtk_main();
    free(color);

    return (EXIT_SUCCESS);
}