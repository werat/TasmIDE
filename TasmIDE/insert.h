#ifndef INSERT_H
#define INSERT_H

#include <gtk/gtk.h>
void insert_insert_proc (GtkWidget *textview);
void insert_insert_push_pop (GtkWidget *textview);
void insert_insert_model_small (GtkWidget *textview);

gchar *run_dialog_insert_proc (GtkWidget *textview,
                             const gchar *title, const gchar *text);

#endif