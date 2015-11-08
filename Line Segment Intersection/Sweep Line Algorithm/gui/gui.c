#include "gui.h"

PointList *lines[2000], *points;
int lists;
double radius = 4;
GdkColor *color = NULL;
GtkWidget *eventBox;

void
draw(IO status) {
    cairo_t *cr = gdk_cairo_create(eventBox->window);
    if (status == in)
        cairo_set_source_rgba(cr, 0, 1, 0, 1);
    else
        cairo_set_source_rgba(cr, 1, 0, 0, 1);
    while (points) {
        cairo_arc(cr, points->point.x, points->point.y, radius, 0, 2 * M_PI);
        cairo_fill(cr);
        if (points->next) {
            points = points->next;
            free(points->previous);
        } else {
            free(points);
            points = NULL;
        }
    }
    if (status == in)
        cairo_set_source_rgba(cr, 0, 0, 1, 1);
    else
        cairo_set_source_rgba(cr, 0, 0, 0, 1);
    cairo_set_line_width(cr, 1);
    for (int i = 0; i < lists; i++) {
        if (!lines[i]->next->next) {
            cairo_move_to(cr, lines[i]->point.x, lines[i]->point.y);
            cairo_line_to(cr, lines[i]->next->point.x, lines[i]->next->point.y);
            cairo_stroke(cr);
            free(lines[i]);
            free(lines[i]->next);
        } else {
            PointList *temp = lines[i];
            Point first = lines[i]->point;
            while (temp->next) {
                cairo_move_to(cr, temp->point.x, temp->point.y);
                cairo_line_to(cr, temp->next->point.x, temp->next->point.y);
                cairo_stroke(cr);
                temp = temp->next;
                free(temp->previous);
            }
            cairo_move_to(cr, temp->point.x, temp->point.y);
            cairo_line_to(cr, first.x, first.y);
            cairo_stroke(cr);
            free(temp);
        }
    }
    cairo_destroy(cr);
    return;
}

void
resetScreen(GtkWidget *widget, gpointer data) {
    gtk_widget_modify_bg(GTK_WIDGET(data), GTK_STATE_NORMAL, color);
    return;
}

void
readFile(GtkWidget *widget, gpointer data) {
    points = NULL;
    memset(lines, 0, 1000 * sizeof (PointList *));
    lists = 0;
    Io_Status *io = (Io_Status *) data;
    PointList *curPoint, *curLine;
    FILE *fp = fopen(io->path, "r");
    char buffer[2000];
    while (fscanf(fp, "%s", buffer) != EOF) {
        if (buffer[1] != '(') {
            if (!points) {
                points = (PointList *) calloc(1, sizeof (PointList));
                points->previous = points->next = NULL;
                curPoint = points;
            } else {
                curPoint->next = (PointList *) calloc(1, sizeof (PointList));
                curPoint = curPoint->next;
                curPoint->next = NULL;
            }
            sscanf(buffer, "(%f,%f)", &(curPoint->point.x), &(curPoint->point.y));
            curPoint->point.x *= eventBox->allocation.width / 1064.0;
            curPoint->point.y *= eventBox->allocation.height / 768.0;
            curPoint->point.y = eventBox->allocation.height - curPoint->point.y;
        } else {
            int count = 1, index = 1;
            lists++;
            while (count) {
                if (buffer[index] == '(') {
                    if (!lines[lists - 1]) {
                        lines[lists - 1] = (PointList *) calloc(1, sizeof (PointList));
                        lines[lists - 1]->previous = lines[lists - 1]->next = NULL;
                        curLine = lines[lists - 1];
                    } else {
                        curLine->next = (PointList *) calloc(1, sizeof (PointList));
                        curLine = curLine->next;
                        curLine->next = NULL;
                    }
                    sscanf(buffer + index, "(%f,%f)", &(curLine->point.x), &(curLine->point.y));
                    curLine->point.x *= eventBox->allocation.width / 1064.0;
                    curLine->point.y *= eventBox->allocation.height / 768.0;
                    curLine->point.y = eventBox->allocation.height - curLine->point.y;
                    while (buffer[index] != ')')
                        index++;
                    index++;
                }
                if (buffer[index] == ')')
                    count--;
            }
        }
    }
    fclose(fp);
    draw(io->flag);
    return;
}

bool
createGui(int argc, char **argv, char *inputPath, char *outputPath) {
    GtkWidget *window, *vBox, *hBox, *button, *separator;
    color = (GdkColor *) calloc(1, sizeof (GdkColor));
    Io_Status input, output;
    input.flag = in;
    output.flag = out;
    strcpy(input.path, inputPath);
    strcpy(output.path, outputPath);

    gtk_init(&argc, &argv);

    gdk_color_parse("#FFFFFF", color);
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    g_signal_connect(window, "delete-event", G_CALLBACK(gtk_main_quit), NULL);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_window_set_resizable(GTK_WINDOW(window), TRUE);
    gtk_window_resize(GTK_WINDOW(window), 1000, 700);

    eventBox = gtk_event_box_new();
    gtk_widget_modify_bg(GTK_WIDGET(eventBox), GTK_STATE_NORMAL, color);

    hBox = gtk_hbox_new(FALSE, 4);

    button = gtk_button_new_with_label("Input");
    g_signal_connect(button, "clicked", G_CALLBACK(readFile), (gpointer) & input);
    gtk_box_pack_start(GTK_BOX(hBox), button, TRUE, FALSE, 10);
    button = gtk_button_new_with_label("Reset");
    g_signal_connect(button, "clicked", G_CALLBACK(resetScreen), (gpointer) eventBox);
    gtk_box_pack_start(GTK_BOX(hBox), button, TRUE, FALSE, 10);
    button = gtk_button_new_with_label("Output");
    g_signal_connect(button, "clicked", G_CALLBACK(readFile), (gpointer) & output);
    gtk_box_pack_start(GTK_BOX(hBox), button, TRUE, FALSE, 10);

    vBox = gtk_vbox_new(FALSE, 4);
    gtk_box_pack_start(GTK_BOX(vBox), eventBox, TRUE, TRUE, 4);
    separator = gtk_hseparator_new();
    gtk_box_pack_start(GTK_BOX(vBox), separator, FALSE, FALSE, 4);
    gtk_box_pack_start(GTK_BOX(vBox), hBox, FALSE, FALSE, 4);

    gtk_container_add(GTK_CONTAINER(window), vBox);
    gtk_widget_show_all(window);
    gtk_main();

    free(color);
    return true;
}
