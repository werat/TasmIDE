#include "Editor.h"
#include "dialog.h"
#include "asmproccessor.h"
#include "build.h"
#include "color.h"

#include <stdio.h>
#include <string.h>

static void set_selection_bound (GtkTextBuffer *buffer, gint start, gint end)
{
	GtkTextIter start_iter, end_iter;

	gtk_text_buffer_get_iter_at_offset (buffer, &start_iter, start);
	if (end < 0)
		gtk_text_buffer_get_end_iter (buffer, &end_iter);
	else
		gtk_text_buffer_get_iter_at_offset (buffer, &end_iter, end);
	gtk_text_buffer_place_cursor (buffer, &end_iter);
	gtk_text_buffer_move_mark_by_name (buffer, "selection_bound", &start_iter);
}

gint on_file_save_as (void)
{
	GtkWidget *dialog;
	gint result = 1;
	dialog = gtk_file_chooser_dialog_new ("Save File",
													  GTK_WINDOW (wnd.window),
													  GTK_FILE_CHOOSER_ACTION_SAVE,
													  GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
													  GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,
													  NULL);
	gtk_file_chooser_set_do_overwrite_confirmation (GTK_FILE_CHOOSER (dialog),
																	TRUE);
	if (wnd.fileopened)
		gtk_file_chooser_set_current_name (GTK_FILE_CHOOSER (dialog),
													  wnd.filename);
	else
		gtk_file_chooser_set_current_name (GTK_FILE_CHOOSER (dialog),
													  "Untitled.asm");

	if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
	{
		gchar *filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));

		if (wnd.filename)
			g_free (wnd.filename);
		wnd.filename = g_strdup (filename);
		wnd.fileopened = TRUE;

		save_document ();
		result = 0;
	}
	gtk_widget_destroy (dialog);

	return result;
}

gint on_file_save (void)
{
	gint result = 1;
	if (wnd.fileopened)
	{
		save_document ();
		return 0;
	}
	else
	{
		result = on_file_save_as ();
	}
	return result;
}

void on_file_open (void)
{
	gboolean can_open = FALSE;

	if (wnd.modified)
	{
		gint res = run_dialog_question (GTK_WINDOW (wnd.window),
											     "Do you want to save current file?");
		if (res == GTK_RESPONSE_YES)
			can_open = !on_file_save ();
		else if (res == GTK_RESPONSE_NO)
			can_open = TRUE;
		else if (res == GTK_RESPONSE_CANCEL || res == GTK_RESPONSE_DELETE_EVENT)
			can_open = FALSE;
	}

	if (can_open || !wnd.modified)
	{
		GtkWidget *dialog;
		dialog = gtk_file_chooser_dialog_new ("Open File",
														  GTK_WINDOW (wnd.window),
														  GTK_FILE_CHOOSER_ACTION_OPEN,
														  GTK_STOCK_CANCEL,
														  GTK_RESPONSE_CANCEL,
														  GTK_STOCK_OPEN,
														  GTK_RESPONSE_ACCEPT,
														  NULL);
		if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
		{
			if (wnd.filename)
				g_free (wnd.filename);
			wnd.filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
			load_document ();
		}
		gtk_widget_destroy (dialog);
	}
}

void on_file_new (void)
{
	if (wnd.modified)
	{
		gint res = run_dialog_question (GTK_WINDOW (wnd.window),
												  "Do you want to save current file?");
		if (res == GTK_RESPONSE_YES)
		{
			if (!on_file_save ())
				new_document ();
		}
		else if (res == GTK_RESPONSE_NO)
			new_document ();
	}
	else
	{
		new_document ();
	}
}

void on_file_close (void)
{
}

gboolean on_file_quit (void)
{
	gboolean can_close = !wnd.modified;
	if (wnd.modified)
	{
		gint res = run_dialog_question (GTK_WINDOW (wnd.window),
											     "Do you want to save current file?");
		if (res == GTK_RESPONSE_YES)
			can_close = !on_file_save ();
		else if (res == GTK_RESPONSE_NO)
			can_close = TRUE;
		else if (res == GTK_RESPONSE_CANCEL || res == GTK_RESPONSE_DELETE_EVENT)
			can_close = FALSE;
	}
	if (can_close)
	{
		on_exit ();
	}
	return !can_close;
}

void on_edit_undo (void)
{
	undo_undo (gtk_text_view_get_buffer (GTK_TEXT_VIEW (wnd.textview)));
}

void on_edit_redo (void)
{
	undo_redo (gtk_text_view_get_buffer (GTK_TEXT_VIEW (wnd.textview)));
}

void on_edit_cut (void)
{
	g_signal_emit_by_name (G_OBJECT (wnd.textview), "cut-clipboard");
}

void on_edit_copy (void)
{
	g_signal_emit_by_name (G_OBJECT (wnd.textview), "copy-clipboard");
}

void on_edit_paste (void)
{
	g_signal_emit_by_name (G_OBJECT (wnd.textview), "paste-clipboard");
}

void on_edit_delete (void)
{
	GtkTextBuffer *buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (wnd.
																						  textview));
	gtk_text_buffer_delete_selection (buffer, TRUE, TRUE);
}

void on_edit_select_all (void)
{
	GtkTextBuffer *buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (wnd.
																						  textview));
	set_selection_bound (buffer, 0, -1);
}

static void activate_quick_find (void)
{
	GtkItemFactory *ifactory;
	static gboolean flag = FALSE;

	if (! flag)
	{
		ifactory = gtk_item_factory_from_widget (wnd.menubar);
		gtk_widget_set_sensitive (
			gtk_item_factory_get_widget (ifactory, "/Search/Find Next"),
			TRUE);
		gtk_widget_set_sensitive (
			gtk_item_factory_get_widget (ifactory, "/Search/Find Previous"),
			TRUE);
		flag = TRUE;
	}
}

void on_search_find (void)
{
	if (run_dialog_search (wnd.textview, 0) == GTK_RESPONSE_OK)
		activate_quick_find ();
}

void on_search_find_next (void)
{
	document_search_real (wnd.textview, 1);
}

void on_search_find_previous (void)
{
	document_search_real (wnd.textview, -1);
}

void on_search_replace (void)
{
	if (run_dialog_search(wnd.textview, 1) == GTK_RESPONSE_OK)
		activate_quick_find();
}

void on_search_jump_to (void)
{
	run_dialog_jump_to (wnd.textview);
}

void on_asm_comment_lines_auto (void)
{
	comment_selected_lines (GTK_TEXT_VIEW (wnd.textview)->buffer, 0);
}
void on_asm_comment_lines (void)
{
	comment_selected_lines (GTK_TEXT_VIEW (wnd.textview)->buffer, 1);
}
void on_asm_uncomment_lines (void)
{
	comment_selected_lines (GTK_TEXT_VIEW (wnd.textview)->buffer, 2);
}

void on_insert_insert_proc (void)
{
	insert_insert_proc (wnd.textview);
}

void on_insert_insert_push_pop (void)
{
	insert_insert_push_pop (wnd.textview);
}

void on_insert_insert_model_small (void)
{
	insert_insert_model_small (wnd.textview);
}

void on_build_build (void)
{
	build (FALSE);
}

void on_build_build_run (void)
{
	build (TRUE);
}

void on_option_color_command (void)
{
	change_color_by_selector (wnd.textview, "command");
}
void on_option_color_keyword (void)
{
	change_color_by_selector (wnd.textview, "keyword");
}
void on_option_color_register (void)
{
	change_color_by_selector (wnd.textview, "register");
}
void on_option_color_jump (void)
{
	change_color_by_selector (wnd.textview, "jump");
}
void on_option_color_comment (void)
{
	change_color_by_selector (wnd.textview, "comment");
}
void on_option_color_string (void)
{
	change_color_by_selector (wnd.textview, "string");
}
void on_option_color_foundword (void)
{
	change_color_by_selector (wnd.textview, "foundword");
}

void on_option_color_mark (void)
{
	change_color_by_selector (wnd.textview, "mark");
}

void on_option_font (void)
{
	change_text_font_by_selector (wnd.textview);
}

void on_option_auto_indent (void)
{
	static gboolean state = TRUE;
	set_auto_indent (state);
	state = !state;	
}

void on_help_about (void)
{
	const gchar *copyright = "by Andrew Yankowsky";
	const gchar *comments = "TasmIDE";

	gtk_show_about_dialog (GTK_WINDOW (wnd.window),
								  "copyright", copyright,
								  "comments", comments,
								  NULL);
}
