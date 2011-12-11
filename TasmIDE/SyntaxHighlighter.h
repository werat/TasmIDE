#ifndef SYNTAXHIGHLIGHTER_H
#define SYNTAXHIGHLIGHTER_H

#include <gtk/gtk.h>

void highlight_all (GtkTextBuffer *buffer);
void highlight_lines (GtkTextBuffer *buffer,
                      gint startl, gint endl);
void highlight_text_pos (GtkTextBuffer *buffer,
                         gint start, gint end);

void initialize_highlighter (GtkTextBuffer *buffer, GKeyFile *keyfile);
void free_highlighter (void);

void on_insert_text (GtkTextBuffer *buffer, GtkTextIter *location,
							gchar *text, gint len, gpointer user_data);

void on_text_changed (GtkTextBuffer *buffer, gpointer data);
void on_after_delete (GtkTextBuffer *buffer, 
                      GtkTextIter *start_iter, GtkTextIter *end_iter);
#endif /* SYNTAXHIGHLIGHTER_H */
