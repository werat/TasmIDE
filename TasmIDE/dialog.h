#ifndef DIALOG_H
#define DIALOG_H

#include <gtk/gtk.h>

gint run_dialog_question (GtkWindow *parent, const gchar *text);
void run_dialog_warning (GtkWindow *parent, const gchar *text);
void run_window_with_text (const gchar *text);
#endif