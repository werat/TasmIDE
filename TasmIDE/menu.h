#ifndef MENU_H
#define MENU_H

#include <gtk/gtk.h>

void menu_sensitivity_from_modified_flag (gboolean is_text_modified);
void menu_sensitivity_from_selection_bound (gboolean is_bound_exist);
void menu_sensitivity_from_clipboard (void);
GtkWidget *create_menu_bar (GtkWidget *window);

#endif /* MENU_H */
