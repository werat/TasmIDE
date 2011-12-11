#include "Editor.h"
#include <string.h>

void insert_insert_proc (GtkWidget *textview)
{
	gchar *procedure = run_dialog_insert_proc (textview,
	                                           "Insert Procedure", 
															 "Procedure name");
	if (procedure && *procedure)
	{
		gchar *text;
		GtkTextBuffer *buffer = GTK_TEXT_VIEW (textview)->buffer;
		GtkTextMark *mark;
		GtkTextIter iter;
		gint start, end;
		gtk_text_buffer_get_iter_at_mark (buffer, &iter,
	                                     gtk_text_buffer_get_insert (buffer));
		start = gtk_text_iter_get_offset (&iter);

		text = g_strdup_printf ("%s proc\n", procedure);
		gtk_text_buffer_insert (buffer, &iter, text, -1);
		indent_line (buffer, &iter);
		mark = gtk_text_buffer_create_mark (buffer, "cursor_place", &iter, TRUE);
		gtk_text_buffer_insert (buffer, &iter, "\n", 1);
		indent_line (buffer, &iter);
		gtk_text_buffer_insert (buffer, &iter, "ret\n", 4);
		indent_line (buffer, &iter);
		g_free (text);
		text = g_strdup_printf ("%s endp", procedure);
		gtk_text_buffer_insert (buffer, &iter, text, -1);
		end = gtk_text_iter_get_offset (&iter);
		g_free (text);

		undo_create_undo_info (buffer, 0, start, end);
		undo_set_sequency_reserve ();

		gtk_text_buffer_get_iter_at_mark (buffer, &iter, mark);
		gtk_text_buffer_place_cursor (buffer, &iter);
		gtk_text_buffer_delete_mark_by_name (buffer, "cursor_place");
		scroll_to_cursor (buffer, 0.1);
		gtk_widget_grab_focus (textview);
	}
}

void insert_insert_push_pop (GtkWidget *textview)
{
	gchar *registers = run_dialog_insert_proc (textview,
	                                           "Insert push/pop", 
															 "Registers");
	if (registers && *registers)
	{
		GtkTextBuffer *buffer = GTK_TEXT_VIEW (textview)->buffer;
		gchar **regs = g_strsplit (registers, " ", -1);
		gint count = 0, start, end;
		GtkTextMark *mark;
		GtkTextIter iter;
		gtk_text_buffer_get_iter_at_mark (buffer, &iter,
	                                     gtk_text_buffer_get_insert (buffer));
		start = gtk_text_iter_get_offset (&iter);
		while (*regs)
		{
			if (**regs)
			{
				gchar *text = g_strdup_printf ("push %s\n", *regs);
				gtk_text_buffer_insert (buffer, &iter, text, -1);
				indent_line (buffer, &iter);
				g_free (text);
			}
			++count;	++regs;
		}
		mark = gtk_text_buffer_create_mark (buffer, "cursor_place", &iter, TRUE);
		gtk_text_buffer_insert (buffer, &iter, "\n", 1);
		indent_line (buffer, &iter);

		while (count - 1 > 0)
		{
			--regs;
			if (**regs)
			{
				gchar *text = g_strdup_printf ("pop %s\n", *regs);
				gtk_text_buffer_insert (buffer, &iter, text, -1);
				indent_line (buffer, &iter);
				g_free (text);
			}
			--count;
		}
		--regs;
		if (**regs)
		{
			gchar *text = g_strdup_printf ("pop %s", *regs);
			gtk_text_buffer_insert (buffer, &iter, text, -1);
			indent_line (buffer, &iter);
			g_free (text);
		}
		end = gtk_text_iter_get_offset (&iter);

		undo_create_undo_info (buffer, 0, start, end);
		undo_set_sequency_reserve ();

		gtk_text_buffer_get_iter_at_mark (buffer, &iter, mark);
		gtk_text_buffer_place_cursor (buffer, &iter);
		gtk_text_buffer_delete_mark_by_name (buffer, "cursor_place");
		scroll_to_cursor (buffer, 0.1);
		gtk_widget_grab_focus (textview);

		g_strfreev (regs);
	}
}

static gchar *msmall[] = 
{
	".model small\n",
	".stack 100h\n",
	".data\n",
	"\n",
	".code\n",
	"\n",
	"main:\n",
	"\n",
	"%cursor%",
	"\n",
	"\n",
	"mov ah, 4ch\n",
	"int 21h\n",
	"end main",
	NULL
};

void insert_insert_model_small (GtkWidget *textview)
{
	gchar **text;
	GtkTextBuffer *buffer = GTK_TEXT_VIEW (textview)->buffer;
	GtkTextMark *mark = NULL;
	GtkTextIter iter;
	gint start, end;
	gtk_text_buffer_get_iter_at_mark (buffer, &iter,
	                                  gtk_text_buffer_get_insert (buffer));
	start = gtk_text_iter_get_offset (&iter);

	text = msmall;
	while (*text != NULL)
	{
		if (g_strcmp0 (*text, "%cursor%") == 0)
			mark = gtk_text_buffer_create_mark (buffer, "cursor_place", &iter, TRUE);
		else
		{
			gtk_text_buffer_insert (buffer, &iter, *text, -1);
			indent_line (buffer, &iter);
		}
		++text;
	}
	end = gtk_text_iter_get_offset (&iter);
	undo_create_undo_info (buffer, 0, start, end);
	undo_set_sequency_reserve ();

	if (mark != NULL)
		gtk_text_buffer_get_iter_at_mark (buffer, &iter, mark);
	gtk_text_buffer_place_cursor (buffer, &iter);
	gtk_text_buffer_delete_mark_by_name (buffer, "cursor_place");
	scroll_to_cursor (buffer, 0.1);
	gtk_widget_grab_focus (textview);
}

gchar *run_dialog_insert_proc (GtkWidget *textview, 
                               const gchar *title, const gchar *text)
{
	GtkWidget *dialog;
	GtkWidget *button;
	GtkWidget *table;
	GtkWidget *label;
	GtkWidget *entry;
	GtkTextIter iter;
	gchar *result = NULL;
	GtkTextBuffer *buffer = GTK_TEXT_VIEW (textview)->buffer;
	
	gtk_text_buffer_get_iter_at_mark (buffer, &iter,
	                                  gtk_text_buffer_get_insert (buffer));
	dialog = gtk_dialog_new_with_buttons (title,
	                                      GTK_WINDOW (gtk_widget_get_toplevel (
													              textview)),
													  GTK_DIALOG_DESTROY_WITH_PARENT,
													  GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
													  NULL);
	gtk_dialog_set_has_separator (GTK_DIALOG (dialog), FALSE);
	button = gtk_button_new_from_stock (GTK_STOCK_OK);
	GTK_WIDGET_SET_FLAGS (button, GTK_CAN_DEFAULT);
	gtk_dialog_add_action_widget (GTK_DIALOG (dialog), button, GTK_RESPONSE_OK);
	table = gtk_table_new (1, 2, FALSE);
	gtk_table_set_col_spacings (GTK_TABLE (table), 8);
	gtk_container_set_border_width (GTK_CONTAINER (table), 8);
	gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialog)->vbox), table, FALSE,
							  FALSE, 0);
	label = gtk_label_new_with_mnemonic (text);
	entry = gtk_entry_new ();
	gtk_entry_set_width_chars (GTK_ENTRY (entry), 8);
	gtk_label_set_mnemonic_widget (GTK_LABEL (label), entry);
	gtk_entry_set_activates_default (GTK_ENTRY (entry), TRUE);
	gtk_table_attach_defaults (GTK_TABLE (table), label, 0, 1, 0, 1);
	gtk_table_attach_defaults (GTK_TABLE (table), entry, 1, 2, 0, 1);

	gtk_dialog_set_default_response (GTK_DIALOG (dialog), GTK_RESPONSE_OK);
	gtk_window_set_resizable (GTK_WINDOW (dialog), FALSE);
	gtk_widget_show_all (dialog);

	if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_OK)
		result = g_strdup (gtk_entry_get_text (GTK_ENTRY (entry)));

	gtk_widget_destroy (dialog);
	return result;
}