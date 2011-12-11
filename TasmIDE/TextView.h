#ifndef TEXTVIEW_H
#define TEXTVIEW_H

#include <gtk/gtk.h>

void set_auto_indent (gboolean autoindent);

gint get_current_keyval (void);
void clear_current_keyval (void);
void scroll_to_cursor (GtkTextBuffer *buffer, gdouble within_margin);

void indent_line (GtkTextBuffer *buffer, GtkTextIter *location);

GtkWidget *create_text_view (void);
void free_textview (void);

#endif /* TEXTVIEW_H */
