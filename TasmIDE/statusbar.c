#include "Editor.h"

static void update_statusbar (GtkTextBuffer *buffer,
                              GtkStatusbar  *statusbar)
{
	gchar *msg;
	gint row, col;
	GtkTextIter iter;
	gtk_statusbar_pop (statusbar, 0); 

	gtk_text_buffer_get_iter_at_mark (buffer,
		                               &iter, 
		                               gtk_text_buffer_get_insert (buffer));

	row = gtk_text_iter_get_line (&iter);
	col = gtk_text_iter_get_line_offset (&iter);

	msg = g_strdup_printf ("Col %d Ln %d", col+1, row+1);
	gtk_statusbar_push (statusbar, 0, msg);
	g_free (msg);
}

void mark_set_callback (GtkTextBuffer *buffer,
                        const GtkTextIter *new_location, GtkTextMark *mark,
                        gpointer data)
{
	update_statusbar (buffer, GTK_STATUSBAR (data));
}

GtkWidget *statusbar_new ()
{
	GtkWidget* statusbar = gtk_statusbar_new ();
	return statusbar;
}