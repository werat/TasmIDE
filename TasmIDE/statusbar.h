#ifndef STATUSBAR_H
#define STATUSBAR_H

#include <gtk/gtk.h>

void mark_set_callback (GtkTextBuffer *buffer,
                        const GtkTextIter *new_location, GtkTextMark *mark,
                        gpointer data);

GtkWidget *statusbar_new ();

#endif