#include "Editor.h"

void new_document ()
{
	undo_reset_modified_step (GTK_TEXT_VIEW (wnd.textview)->buffer);
	undo_clear_all (GTK_TEXT_VIEW (wnd.textview)->buffer);
	wnd.fileopened = FALSE;
	wnd.filename = g_strdup ("Untitled.asm");
	gtk_text_buffer_set_text (
		GTK_TEXT_VIEW (wnd.textview)->buffer, "", 0);
	wnd.modified = FALSE;
	set_window_title ();
}

void save_document ()
{
	gchar *contents;
	GtkTextIter start, end;
	GtkTextBuffer *buffer = GTK_TEXT_VIEW (wnd.textview)->buffer;
	gtk_text_buffer_get_start_iter (buffer, &start);
	gtk_text_buffer_get_end_iter (buffer, &end);
	contents = gtk_text_buffer_get_text (buffer, &start, &end, TRUE);
	g_file_set_contents (wnd.filename, contents, -1, NULL);

	wnd.modified = FALSE;
	set_window_title ();

	g_free (contents);
}

void load_document ()
{
	gchar *contents;
	GtkTextBuffer *buffer = GTK_TEXT_VIEW (wnd.textview)->buffer;
	GtkTextIter iter;

	g_file_get_contents (wnd.filename, &contents, NULL, NULL);
	if (g_utf8_validate (contents, -1, NULL))
	{
		undo_reset_modified_step (buffer);
		undo_clear_all (buffer);
		gtk_text_buffer_set_text (buffer, contents, -1);
		gtk_text_buffer_get_start_iter (buffer, &iter);
		gtk_text_buffer_place_cursor (buffer, &iter);
		wnd.fileopened = TRUE;
		highlight_all (buffer);

		wnd.modified = FALSE;
		set_window_title ();
	}
	else
	{
		run_dialog_warning (GTK_WINDOW (wnd.window), 
		                    "Target file doesn't contain valid UTF8 text");
	}
}
