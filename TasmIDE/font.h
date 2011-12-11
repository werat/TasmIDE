#ifndef FONT_H
#define FONT_H

#include <gtk/gtk.h>

void set_text_font_by_name (GtkWidget *widget, gchar *fontname);
gchar *get_font_name_from_widget (GtkWidget *widget);  /* MUST BE FREED */
void change_text_font_by_selector (GtkWidget *widget);

#endif /* FONT_H */
