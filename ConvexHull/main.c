/*
 * File:   main.c
 * Author: dibyendu
 *
 * Created on January 6, 2011
 */

#include "ConvexHull.h"

#define MAX_POINTS 8000


PointList *pointList = NULL;
static long long noOfPoints = 0;

GdkColor *color = NULL;
GtkWidget *label, *radioButtonSlow, *radioButtonFast, *radioUserInput,
*radioRandomInput, *eventBox, *textEntry;
gulong buttonPressTag;
static double radius = 4;

void
reDrawWindow(GtkWindow *window) {
    gint posx, posy;
    gtk_window_get_position(window, &posx, &posy);
    gtk_window_move(window, posx+1, posy+1);
    gtk_window_move(window, posx, posy);
    return;
}

void
drawPoints(GtkWidget *widget, GdkEventButton *event,
           gpointer data) {
    cairo_t *cr = gdk_cairo_create(widget->window);
    cairo_set_source_rgba(cr, 0, 0, 1, 0.5);

    PointList *node = (PointList *) calloc(1, sizeof (PointList));
    noOfPoints++;
    node->point.x = event->x;
    node->point.y = event->y;
    node->next = NULL;
    if(!pointList)
        pointList = node;
    else {
        PointList *temp = pointList;
        for(int i = 2; i < noOfPoints; i++)
            temp = temp->next;
        temp->next = node;
    }
    cairo_arc(cr, event->x, event->y, radius, 0, 2 * M_PI);
    cairo_fill(cr);
    cairo_destroy(cr);

    return;
}

void
resetScreen(GtkWidget *widget, gpointer data) {

    gtk_widget_modify_bg(GTK_WIDGET(data), GTK_STATE_NORMAL, color);
    if(pointList) {
        free(pointList);
        pointList = NULL;
    }
    noOfPoints = 0;
    radius = 4;
    gtk_label_set_label(GTK_LABEL(label), "");

    return;
}

void
drawHull(GtkWidget *widget, gpointer data) {

    LineList *lineList = NULL, *tempLineList = NULL;
    PointList *hulltList = NULL, *tempHullList = NULL;
    GTimer *timer = g_timer_new();

    if(GTK_TOGGLE_BUTTON(radioButtonSlow)->active)
        tempLineList = lineList = convexHullSlow(pointList);
    else
        tempHullList = hulltList = convexHullFast(pointList);
    gdouble elaspedTime = g_timer_elapsed(timer, NULL);
    g_timer_destroy(timer);
    char str[80];
    sprintf(str, "Time Taken  :  %lf ms    [ for %lld point(s) ]", elaspedTime * 1000, noOfPoints);
    cairo_t *cr = gdk_cairo_create((GTK_WIDGET(data))->window);
    if(GTK_TOGGLE_BUTTON(radioButtonSlow)->active) {
        while(lineList) {
            cairo_set_source_rgba(cr, 1, 1, 1, 1);
            cairo_arc(cr, lineList->line.point_1.x, lineList->line.point_1.y,
                      radius, 0, 2 * M_PI);
            cairo_arc(cr, lineList->line.point_2.x, lineList->line.point_2.y,
                      radius, 0, 2 * M_PI);
            cairo_fill_preserve(cr);
            cairo_set_source_rgba(cr, 1, 0, 0, 0.4);
            cairo_fill(cr);
            lineList = lineList->next;
        }
    } else {
        if(hulltList) {
            while(hulltList) {
                cairo_set_source_rgba(cr, 1, 1, 1, 1);
                cairo_arc(cr, hulltList->point.x, hulltList->point.y, radius, 0, 2 * M_PI);
                cairo_fill_preserve(cr);
                cairo_set_source_rgba(cr, 0, 1, 0, 1);
                cairo_fill(cr);
                hulltList = hulltList->next;
            }
        }
    }
    cairo_set_line_width(cr, 1.2);
    if(GTK_TOGGLE_BUTTON(radioButtonSlow)->active) {
        cairo_set_source_rgba(cr, 1, 0, 0, 1);
        lineList = tempLineList;
        while(lineList) {
            cairo_move_to(cr, lineList->line.point_1.x, lineList->line.point_1.y);
            cairo_line_to(cr, lineList->line.point_2.x, lineList->line.point_2.y);
            cairo_stroke(cr);
            lineList = lineList->next;
        }
    } else {
        cairo_set_source_rgba(cr, 0, 0, 0, 0.6);
        hulltList = tempHullList;
        if(hulltList) {
            while(hulltList->next) {
                cairo_move_to(cr, hulltList->point.x, hulltList->point.y);
                cairo_line_to(cr, hulltList->next->point.x, hulltList->next->point.y);
                cairo_stroke(cr);
                hulltList = hulltList->next;
            }
            cairo_move_to(cr, hulltList->point.x, hulltList->point.y);
            cairo_line_to(cr, tempHullList->point.x, tempHullList->point.y);
            cairo_stroke(cr);
        }
    }
    gtk_label_set_label(GTK_LABEL(label), str);
    reDrawWindow(GTK_WINDOW(GTK_WIDGET(data)->parent->parent->parent));
    cairo_destroy(cr);
    if(tempLineList)
        free(tempLineList);
    if(tempHullList)
        free(tempHullList);

    return;
}

gboolean
hideWidget(GtkWidget *widget, gpointer data) {

    GdkColor *red = (GdkColor *) calloc(1, sizeof (GdkColor));
    gdk_color_parse("#DE7B8E", red);
    if(!GTK_TOGGLE_BUTTON(widget)->active) {
        g_signal_handler_unblock(eventBox, buttonPressTag);
        gtk_widget_hide(GTK_WIDGET(data));
        gtk_entry_set_editable(GTK_ENTRY(textEntry), FALSE);
        gtk_widget_modify_base(textEntry, GTK_STATE_NORMAL, red);
    } else {
        g_signal_handler_block(eventBox, buttonPressTag);
        gtk_widget_show(GTK_WIDGET(data));
        gtk_entry_set_editable(GTK_ENTRY(textEntry), TRUE);
        gtk_widget_modify_base(textEntry, GTK_STATE_NORMAL, color);
    }
    free(red);

    return TRUE;
}

int
genRand(int randMin, int randMax) {

    int number = (((int) ((double) sqrt((rand())) * 10000)) % randMax) + 1;
    while(number < randMin)
        number = (((int) ((double) sqrt((rand())) * 10000)) % randMax) + 1;
    return number;
}

gboolean
generateRandom(GtkWidget *widget, gpointer data) {

    long long randomPoints = atoll((char *) gtk_entry_get_text(GTK_ENTRY(textEntry)));
    if(!randomPoints)
        return TRUE;
    else if(randomPoints > MAX_POINTS)
        randomPoints = MAX_POINTS;
    if(randomPoints > 1000 && randomPoints <= 2000)
        radius = 3.5;
    else if(randomPoints > 2000 && randomPoints <= 3000)
        radius = 3;
    else if(randomPoints > 3000 && randomPoints <= 5000)
        radius = 2.5;
    else if(randomPoints > 5000 && randomPoints <= 6000)
        radius = 2;
    else if(randomPoints > 6000 && randomPoints <= 7000)
        radius = 1.5;
    else if(randomPoints > 7000 && randomPoints <= 8000)
        radius = 1;
    if(pointList) {
        free(pointList);
        pointList = NULL;
    }
    noOfPoints = 0;
    gtk_label_set_label(GTK_LABEL(label), "");

    cairo_t *cr = gdk_cairo_create(eventBox->window);
    cairo_set_source_rgba(cr, 1, 1, 1, 1);
    cairo_rectangle(cr, 0, 0, eventBox->allocation.width, eventBox->allocation.height);
    cairo_fill(cr);
    cairo_set_source_rgba(cr, 0, 0, 1, 0.5);

    pointList = (PointList *) calloc(1, sizeof (PointList));
    PointList *temp = pointList, *delete = NULL;
    for(long long i = 0; i < randomPoints; i++) {
        noOfPoints++;
        temp->point.x = genRand(eventBox->allocation.width / 32, eventBox->allocation.width -
                                eventBox->allocation.width / 32);
        temp->point.y = genRand(eventBox->allocation.height / 32, eventBox->allocation.height -
                                eventBox->allocation.height / 32);
        temp->next = (PointList *) calloc(1, sizeof (PointList));
        cairo_arc(cr, temp->point.x, temp->point.y, radius, 0, 2 * M_PI);
        cairo_fill(cr);
        delete = temp;
        temp = temp->next;
    }
    free(temp);
    if(delete)
        delete->next = NULL;
    cairo_destroy(cr);

    return TRUE;
}

int
main(int argc, char** argv) {

    GtkWidget *window, *vBox, *hBox, *button, *separator, *hBoxNew;
    color = (GdkColor *) calloc(1, sizeof (GdkColor));

    gtk_init(&argc, &argv);

    gdk_color_parse("#FFFFFF", color);
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    g_signal_connect(window, "delete-event", G_CALLBACK(gtk_main_quit), NULL);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_window_set_title(GTK_WINDOW(window), "Convex Hull");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);
    gtk_window_set_resizable(GTK_WINDOW(window), TRUE);

    eventBox = gtk_event_box_new();
    buttonPressTag = g_signal_connect(eventBox, "button-press-event", G_CALLBACK(drawPoints), NULL);
    gtk_widget_modify_bg(eventBox, GTK_STATE_NORMAL, color);
    vBox = gtk_vbox_new(FALSE, 10);

    button = gtk_button_new_with_label("Draw Hull");
    g_signal_connect(button, "clicked", G_CALLBACK(drawHull), (gpointer) eventBox);
    gtk_box_pack_start(GTK_BOX(vBox), button, TRUE, FALSE, 4);
    button = gtk_button_new_with_label("Reset");
    g_signal_connect(button, "clicked", G_CALLBACK(resetScreen), (gpointer) eventBox);
    gtk_box_pack_start(GTK_BOX(vBox), button, TRUE, FALSE, 4);
    button = gtk_button_new_with_label("Generate\n random\n  points");
    g_signal_connect(button, "clicked", G_CALLBACK(generateRandom), NULL);
    gtk_box_pack_start(GTK_BOX(vBox), button, TRUE, FALSE, 4);
    radioUserInput = gtk_radio_button_new_with_label(NULL, "User");
    radioRandomInput = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(radioUserInput), "Random");
    g_signal_connect(radioRandomInput, "toggled", G_CALLBACK(hideWidget), (gpointer) button);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radioRandomInput), TRUE);
    gtk_box_pack_start(GTK_BOX(vBox), radioUserInput, FALSE, FALSE, 4);
    gtk_box_pack_start(GTK_BOX(vBox), radioRandomInput, FALSE, FALSE, 4);

    hBox = gtk_hbox_new(FALSE, 4);
    gtk_box_pack_start(GTK_BOX(hBox), eventBox, TRUE, TRUE, 4);
    separator = gtk_vseparator_new();
    gtk_box_pack_start(GTK_BOX(hBox), separator, FALSE, FALSE, 4);
    gtk_box_pack_start(GTK_BOX(hBox), vBox, FALSE, FALSE, 4);

    hBoxNew = gtk_hbox_new(FALSE, 10);
    radioButtonSlow = gtk_radio_button_new_with_label(NULL, "Slow");
    radioButtonFast = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(radioButtonSlow), "Fast");
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radioButtonFast), TRUE);
    gtk_box_pack_start(GTK_BOX(hBoxNew), radioButtonSlow, FALSE, FALSE, 8);
    gtk_box_pack_start(GTK_BOX(hBoxNew), radioButtonFast, FALSE, FALSE, 8);
    label = gtk_label_new("");
    gtk_box_pack_start(GTK_BOX(hBoxNew), label, TRUE, TRUE, 10);
    textEntry = gtk_entry_new();
    gtk_entry_set_alignment(GTK_ENTRY(textEntry), 0.5);
    gtk_box_pack_end(GTK_BOX(hBoxNew), textEntry, FALSE, FALSE, 10);

    vBox = gtk_vbox_new(FALSE, 4);
    gtk_box_pack_end(GTK_BOX(vBox), hBoxNew, FALSE, FALSE, 2);
    separator = gtk_hseparator_new();
    gtk_box_pack_end(GTK_BOX(vBox), separator, FALSE, FALSE, 4);
    gtk_box_pack_start(GTK_BOX(vBox), hBox, TRUE, TRUE, 2);

    gtk_container_add(GTK_CONTAINER(window), vBox);

    gtk_widget_show_all(window);

    gtk_main();

    free(color);
    return (EXIT_SUCCESS);
}
