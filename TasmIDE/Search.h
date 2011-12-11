#ifndef SEARCH_H
#define SEARCH_H

#include <gtk/gtk.h>

gint run_dialog_search (GtkWidget *textview, gint mode);
void run_dialog_jump_to (GtkWidget *textview);

gboolean document_search_real (GtkWidget *textview, gint direction);
#endif /* SEARCH_H */
