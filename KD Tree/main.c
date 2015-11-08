/* 
 * File:   main.c
 * Author: dibyendu
 *
 * Created on June 2, 2011, 9:19 PM
 */

#include "kd_tree.h"

KdTree *kd_tree_root;
PointList *pointList = NULL;
Point range[2];
bool strart_ranging = false;
static long long noOfPoints = 0;

GdkColor *color = NULL;
GtkWidget *label, *radioButtonBuild, *radioButtonSearch, *radioUserInput,
*radioRandomInput, *eventBox, *textEntry, *searchButton;
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

int
genRand(int randMin, int randMax) {
    int number;
    do {
        number = (((int) ((double) sqrt(rand()) * 10000)) % randMax) + 1;
    } while (number < randMin);
    return number;
}

KdTree *
greatGrandParent(KdTree *node) {
    int count = 0;
    while (node && count < 3) {
        node = node->parent;
        count++;
    }
    return (node && count == 3) ? node : NULL;
}

void
traverse(KdTree *node, int maxx, int maxy, cairo_t *cr) {
    if (node->left)
        traverse(node->left, maxx, maxy, cr);
    if (node->left || node->right) {
        if (!node->parent) {
            cairo_move_to(cr, node->point.x, 0.0);
            cairo_line_to(cr, node->point.x, maxy);
            cairo_stroke(cr);
        }
        else {
            bool leftOfParent = (node == node->parent->left) ? true : false;
            KdTree *ggp = greatGrandParent(node);
            if (node->depth % 2) {
                if (ggp) {
                    bool leftOfGgp = (ggp->left == node->parent->parent) ? true : false;
                    if (leftOfGgp && leftOfParent) {
                        cairo_move_to(cr, 0.0, node->point.y);
                        cairo_line_to(cr, node->parent->point.x, node->point.y);
                        cairo_stroke(cr);
                    }
                    else if ((leftOfGgp && !leftOfParent) || (!leftOfGgp && leftOfParent)) {
                        cairo_move_to(cr, ggp->point.x, node->point.y);
                        cairo_line_to(cr, node->parent->point.x, node->point.y);
                        cairo_stroke(cr);
                    }
                    else {
                        cairo_move_to(cr, maxx, node->point.y);
                        cairo_line_to(cr, node->parent->point.x, node->point.y);
                        cairo_stroke(cr);
                    }
                } else {
                    if (leftOfParent) {
                        cairo_move_to(cr, 0.0, node->point.y);
                        cairo_line_to(cr, node->parent->point.x, node->point.y);
                        cairo_stroke(cr);
                    }
                    else {
                        cairo_move_to(cr, maxx, node->point.y);
                        cairo_line_to(cr, node->parent->point.x, node->point.y);
                        cairo_stroke(cr);
                    }
                }
            } else {
                if (ggp) {
                    bool leftOfGgp = (ggp->left == node->parent->parent) ? true : false;
                    if (leftOfGgp && leftOfParent) {
                        cairo_move_to(cr, node->point.x, 0.0);
                        cairo_line_to(cr, node->point.x, node->parent->point.y);
                        cairo_stroke(cr);
                    }
                    else if ((leftOfGgp && !leftOfParent) || (!leftOfGgp && leftOfParent)) {
                        cairo_move_to(cr, node->point.x, ggp->point.y);
                        cairo_line_to(cr, node->point.x, node->parent->point.y);
                        cairo_stroke(cr);
                    }
                    else {
                        cairo_move_to(cr, node->point.x, maxy);
                        cairo_line_to(cr, node->point.x, node->parent->point.y);
                        cairo_stroke(cr);
                    }
                } else {
                    if (leftOfParent) {
                        cairo_move_to(cr, node->point.x, 0.0);
                        cairo_line_to(cr, node->point.x, node->parent->point.y);
                        cairo_stroke(cr);
                    }
                    else {
                        cairo_move_to(cr, node->point.x, maxy);
                        cairo_line_to(cr, node->point.x, node->parent->point.y);
                        cairo_stroke(cr);
                    }
                }
            }
        }
    }
    if (node->right)
        traverse(node->right, maxx, maxy, cr);
    return;
}

void
drawPoints(GtkWidget *widget, GdkEventButton *event, gpointer data) {
    cairo_t *cr = gdk_cairo_create(widget->window);
    if(GTK_TOGGLE_BUTTON(radioButtonSearch)->active) {
        if (!strart_ranging) {
            range[0].x = event->x;
            range[0].y = event->y;
            strart_ranging = true;
            cairo_set_source_rgba(cr, 1, 0, 0, 0.5);
            cairo_arc(cr, range[0].x, range[0].y, 2, 0, 2 * M_PI);
            cairo_fill(cr);
            cairo_destroy(cr);
            return;
        } else {
            range[1].x = event->x;
            range[1].y = event->y;
            strart_ranging = false;
            cairo_set_source_rgba(cr, 1, 0, 0, 0.5);
            cairo_arc(cr, range[1].x, range[1].y, 2, 0, 2 * M_PI);
            cairo_fill(cr);
            cairo_set_source_rgba(cr, 0, 0, 0, 1);
            cairo_set_line_width(cr, 0.5);
            cairo_move_to(cr, range[0].x, range[0].y);
            cairo_line_to(cr, range[1].x, range[0].y);
            cairo_line_to(cr, range[1].x, range[1].y);
            cairo_line_to(cr, range[0].x, range[1].y);
            cairo_line_to(cr, range[0].x, range[0].y);
            cairo_stroke(cr);
            reDrawWindow(GTK_WINDOW(GTK_WIDGET(widget)->parent->parent->parent));
            cairo_destroy(cr);
            return;
        }
    }
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
buildTree(GtkWidget *widget, gpointer data) {
    PointList *list = pointList;
    int count = 0, index = 0;
    while (list) {
        count += 1;
        list = list->next;
    }
    Point points[count];
    list = pointList;
    while (list) {
        points[index].x = list->point.x;
        points[index].y = list->point.y;
        list = list->next;
        index++;
    }
    kd_tree_root = realize(buildKdTree(points, 0, count - 1, 0));
    int maxx = (GTK_WIDGET(data))->allocation.width, maxy = (GTK_WIDGET(data))->allocation.height;
    cairo_t *cr = gdk_cairo_create((GTK_WIDGET(data))->window);
    cairo_set_source_rgba(cr, 0, 1, 0, 1);
    cairo_set_line_width(cr, 0.8);
    traverse(kd_tree_root, maxx, maxy, cr);
    reDrawWindow(GTK_WINDOW(GTK_WIDGET(data)->parent->parent->parent));
    cairo_destroy(cr);
    return;
}

void
searchTree(GtkWidget *widget, gpointer data) {
    PointList *list = NULL;
    searchKdTree(kd_tree_root, &list, range);
    cairo_t *cr = gdk_cairo_create((GTK_WIDGET(data))->window);
    cairo_set_source_rgba(cr, 1, 0, 0, 1);
    while (list) {
        cairo_arc(cr, list->point.x, list->point.y, radius, 0, 2 * M_PI);
        cairo_fill(cr);    
        if (!list->next) {
            free(list);
            list = NULL;
            continue;
        }
        list = list->next;
        free(list->previous);
    }
    reDrawWindow(GTK_WINDOW(GTK_WIDGET(data)->parent->parent->parent));
    cairo_destroy(cr);
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

gboolean
activateSearchButton(GtkWidget *widget, gpointer data) {
    GdkColor *red = (GdkColor *) calloc(1, sizeof (GdkColor));
    gdk_color_parse("#DE7B8E", red);
    if(GTK_TOGGLE_BUTTON(widget)->active) {
        GList *children = gtk_container_get_children (GTK_CONTAINER(data));
        children = children->next;
        while (children) {
            gtk_widget_set_sensitive(GTK_WIDGET(children->data), false);
            children = children->next;
        }
        gtk_widget_set_sensitive(GTK_WIDGET(searchButton), true);
        g_signal_handler_unblock(eventBox, buttonPressTag);
        gtk_entry_set_editable(GTK_ENTRY(textEntry), FALSE);
        gtk_widget_modify_base(textEntry, GTK_STATE_NORMAL, red);
    } else {
        gtk_widget_set_sensitive(GTK_WIDGET(searchButton), false);
        GList *children = gtk_container_get_children (GTK_CONTAINER(data));
        children = children->next;
        while (children) {
            gtk_widget_set_sensitive(GTK_WIDGET(children->data), true);
            children = children->next;
        }
        gtk_entry_set_editable(GTK_ENTRY(textEntry), TRUE);
        gtk_widget_modify_base(textEntry, GTK_STATE_NORMAL, color);
    }
    free(red);
    return TRUE;
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
    else if (randomPoints > 7000)
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
    gtk_window_set_title(GTK_WINDOW(window), "KD Tree");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);
    gtk_window_set_resizable(GTK_WINDOW(window), TRUE);

    eventBox = gtk_event_box_new();
    buttonPressTag = g_signal_connect(eventBox, "button-press-event", G_CALLBACK(drawPoints), NULL);
    gtk_widget_modify_bg(eventBox, GTK_STATE_NORMAL, color);
    vBox = gtk_vbox_new(FALSE, 10);

    searchButton = gtk_button_new_with_label("Search Tree");
    g_signal_connect(searchButton, "clicked", G_CALLBACK(searchTree), (gpointer) eventBox);    
    gtk_box_pack_start(GTK_BOX(vBox), searchButton, TRUE, FALSE, 4);
    button = gtk_button_new_with_label("Build Tree");
    g_signal_connect(button, "clicked", G_CALLBACK(buildTree), (gpointer) eventBox);    
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
    radioButtonBuild = gtk_radio_button_new_with_label(NULL, "Build Tree");
    radioButtonSearch = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(radioButtonBuild), "Search Tree");
    g_signal_connect(radioButtonSearch, "toggled", G_CALLBACK(activateSearchButton), (gpointer) vBox);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radioButtonBuild), TRUE);
    gtk_box_pack_start(GTK_BOX(hBoxNew), radioButtonBuild, FALSE, FALSE, 8);
    gtk_box_pack_start(GTK_BOX(hBoxNew), radioButtonSearch, FALSE, FALSE, 8);
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
    gtk_widget_set_sensitive(GTK_WIDGET(searchButton), false);

    gtk_main();

    free(color);
    return (EXIT_SUCCESS);
}
