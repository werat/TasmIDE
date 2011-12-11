#ifndef UNDO_H
#define UNDO_H

#include <gtk/gtk.h>

void undo_reset_modified_step (GtkTextBuffer *buffer);
void undo_clear_all (GtkTextBuffer *buffer);
void undo_init (GtkWidget *view, GtkWidget *undo_button,
					 GtkWidget *redo_button);
void undo_set_sequency (gboolean seq);
void undo_set_sequency_reserve (void);
void undo_undo (GtkTextBuffer *buffer);
void undo_redo (GtkTextBuffer *buffer);

void undo_create_undo_info (GtkTextBuffer *buffer, gchar command,
                            gint start, gint end);

void cb_insert_text (GtkTextBuffer *buffer,
							GtkTextIter *iter, gchar *str, gint len);

#endif
