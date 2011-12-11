#include "Editor.h"
#include <string.h>

static gint entry_len;
static gchar *string_find = NULL;
static gchar *string_replace = NULL;
static gboolean replace_all = FALSE;

static void toggle_check_all (GtkWidget *widget)
{
	replace_all = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget));
}

static void toggle_sensitivity (GtkWidget *w, gint pos1, gint pos2, gint *pos3)
{
	if (pos3)
	{
		if (! entry_len)
			gtk_dialog_set_response_sensitive (GTK_DIALOG (
														  gtk_widget_get_toplevel (w)),
														  GTK_RESPONSE_OK, TRUE);
		entry_len += pos2;
	}
	else
	{
		entry_len = entry_len + pos1 - pos2;
		if (! entry_len)
			gtk_dialog_set_response_sensitive (GTK_DIALOG (
														  gtk_widget_get_toplevel (w)),
														  GTK_RESPONSE_OK, FALSE);
	}
}

static void run_dialog_message (GtkWidget *window,
										  GtkMessageType type,
										  gchar *message, ...)
{
	va_list ap;
	GtkWidget *dialog;
	gchar *str;

	va_start (ap, message);
	str = g_strdup_vprintf (message, ap);
	va_end (ap);

	dialog = gtk_message_dialog_new (GTK_WINDOW (window),
												GTK_DIALOG_DESTROY_WITH_PARENT,
												type,
												GTK_BUTTONS_NONE,
												"%s", str);
	gtk_window_set_resizable (GTK_WINDOW (dialog), FALSE);
	gtk_dialog_add_buttons (GTK_DIALOG (dialog),
									GTK_STOCK_OK, GTK_RESPONSE_CANCEL, NULL);
	gtk_dialog_set_default_response (GTK_DIALOG (dialog), GTK_RESPONSE_CANCEL);
	g_free (str);

	gtk_dialog_run (GTK_DIALOG (dialog));
	gtk_widget_destroy (dialog);
}

gboolean document_search_real (GtkWidget *textview, gint direction)
{
	GtkTextIter iter, match_start, match_end;
	gboolean res;
	GtkTextBuffer *textbuffer =
		gtk_text_view_get_buffer (GTK_TEXT_VIEW (textview));

	if (! string_find)
		return FALSE;

	gtk_text_mark_set_visible (
		gtk_text_buffer_get_selection_bound (
		GTK_TEXT_VIEW (textview)->buffer), FALSE);

	gtk_text_buffer_get_iter_at_mark (textbuffer, &iter,
												 gtk_text_buffer_get_insert (textbuffer));
	if (direction < 0)
	{
		res = gtk_text_iter_backward_search (
			&iter, string_find, 0, &match_start, &match_end, NULL);
		if (gtk_text_iter_equal (&iter, &match_end))
		{
			res = gtk_text_iter_backward_search (
				&match_start, string_find, 0, &match_start, &match_end, NULL);
		}
	}
	else
	{
		res = gtk_text_iter_forward_search (
			&iter, string_find, 0, &match_start, &match_end, NULL);
	}

	if (! res)
	{
		if (direction < 0)
		{
			gtk_text_buffer_get_end_iter (textbuffer, &iter);
			res = gtk_text_iter_backward_search (
				&iter, string_find, 0, &match_start, &match_end, NULL);
		}
		else
		{
			gtk_text_buffer_get_start_iter (textbuffer, &iter);
			res = gtk_text_iter_forward_search (
				&iter, string_find, 0, &match_start, &match_end, NULL);
		}
	}

	if (res)
	{
		gtk_text_buffer_place_cursor (textbuffer, &match_start);
		gtk_text_buffer_move_mark_by_name (textbuffer, "insert", &match_end);
		scroll_to_cursor (textbuffer, 0.1);
		gtk_widget_grab_focus (textview);
	}
	else if (direction == 0)
		run_dialog_message (gtk_widget_get_toplevel (textview),
								  GTK_MESSAGE_WARNING,
								  "Search string not found");

	return res;
}

static gint document_replace_real(GtkWidget *textview)
{
	GtkTextIter iter, match_start, match_end, rep_start;
	GtkTextMark *mark_init = NULL;
	gboolean res;
	gint num = 0, offset;
	GtkTextBuffer *textbuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview));

	if (replace_all) 
	{
		gtk_text_buffer_get_iter_at_mark (textbuffer,
			&iter, gtk_text_buffer_get_insert (textbuffer));
		mark_init = gtk_text_buffer_create_mark (textbuffer, NULL, &iter, FALSE);
		gtk_text_buffer_get_start_iter (textbuffer, &iter);

		gtk_text_buffer_get_end_iter (textbuffer, &match_end);
		gtk_text_buffer_remove_all_tags (textbuffer,
			&iter, &match_end);
	}

	do 
	{
		if (replace_all) 
		{
			res = gtk_text_iter_forward_search (
				&iter, string_find, 0, &match_start, &match_end, NULL);
			if (res) 
			{
				gtk_text_buffer_place_cursor (textbuffer, &match_start);
				gtk_text_buffer_move_mark_by_name (textbuffer, "insert", &match_end);
				gtk_text_buffer_get_iter_at_mark (
					textbuffer, &iter, gtk_text_buffer_get_insert (textbuffer));
			}
		}
		else
			res = document_search_real(textview, 2);
		
		if (res) 
		{
			if (!replace_all) 
			{
				GtkTextIter ins,bou;
				gtk_text_buffer_get_selection_bounds(textbuffer, &ins, &bou);
				gtk_text_buffer_apply_tag_by_name (textbuffer, "foundword",
					&ins, &bou);
				switch (run_dialog_question (GTK_WINDOW (wnd.window), "Replace?"))
				{
				case GTK_RESPONSE_YES:
					gtk_text_buffer_select_range(textbuffer, &ins, &bou);
					break;
				case GTK_RESPONSE_NO:
					gtk_text_buffer_remove_tag_by_name (textbuffer, "foundword",
						&ins, &bou);
					continue;
				default:
					gtk_text_buffer_remove_tag_by_name (textbuffer, "foundword",
						&ins, &bou);
					res = 0;
					if (num == 0)
						num = -1;
					continue;
				}
			}
			gtk_text_buffer_delete_selection (textbuffer, TRUE, TRUE);
			if (strlen (string_replace)) 
			{
				gtk_text_buffer_get_iter_at_mark (
					textbuffer, &rep_start,
					gtk_text_buffer_get_insert (textbuffer));
				offset = gtk_text_iter_get_offset (&rep_start);
				undo_set_sequency (TRUE);
				g_signal_emit_by_name (G_OBJECT (textbuffer),
					"begin-user-action");
				gtk_text_buffer_insert_at_cursor (textbuffer,
					string_replace, strlen (string_replace));
				g_signal_emit_by_name (G_OBJECT (textbuffer),
					"end-user-action");
				gtk_text_buffer_get_iter_at_mark (
					textbuffer, &iter,
					gtk_text_buffer_get_insert (textbuffer));
				gtk_text_buffer_get_iter_at_offset (textbuffer,
					&rep_start, offset);
			} 
			else
				gtk_text_buffer_get_iter_at_mark (
					textbuffer, &iter,
					gtk_text_buffer_get_insert (textbuffer));
			
			num++;
			undo_set_sequency (replace_all);
		}
	} while (res);
	
	if (replace_all) 
	{
		gtk_text_buffer_get_iter_at_mark (textbuffer, &iter, mark_init);
		gtk_text_buffer_place_cursor (textbuffer, &iter);
		run_dialog_message (gtk_widget_get_toplevel (textview), GTK_MESSAGE_INFO,
			"%d strings replaced", num);
		undo_set_sequency (FALSE);
	}
	
	return num;
}


gint run_dialog_search (GtkWidget *textview, gint mode)
{
	GtkWidget *dialog;
	GtkWidget *table;
	GtkWidget *label_find, *label_replace;
	GtkWidget *entry_find, *entry_replace = NULL;
	GtkWidget *check_all;
	gint res;

	if (mode)
		dialog = gtk_dialog_new_with_buttons ("Replace",
														  GTK_WINDOW (
														  gtk_widget_get_toplevel (
														  textview)),
														  GTK_DIALOG_DESTROY_WITH_PARENT,
														  GTK_STOCK_CANCEL,
														  GTK_RESPONSE_CANCEL,
														  GTK_STOCK_FIND_AND_REPLACE,
														  GTK_RESPONSE_OK,
														  NULL);
	else
		dialog = gtk_dialog_new_with_buttons ("Find",
														  GTK_WINDOW (
														  gtk_widget_get_toplevel (
														  textview)),
														  GTK_DIALOG_DESTROY_WITH_PARENT,
														  GTK_STOCK_CANCEL,
														  GTK_RESPONSE_CANCEL,
														  GTK_STOCK_FIND, GTK_RESPONSE_OK,
														  NULL);
	gtk_dialog_set_has_separator (GTK_DIALOG (dialog), FALSE);

	table = gtk_table_new (mode + 1, 2, FALSE);
	gtk_table_set_row_spacings (GTK_TABLE (table), 8);
	gtk_table_set_col_spacings (GTK_TABLE (table), 8);
	gtk_container_set_border_width (GTK_CONTAINER (table), 8);
	gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialog)->vbox), table, FALSE,
							  FALSE, 0);
	label_find = gtk_label_new_with_mnemonic ("Fi_nd what:");
	gtk_misc_set_alignment (GTK_MISC (label_find), 0, 0.5);
	gtk_table_attach_defaults (GTK_TABLE (table), label_find, 0, 1, 0, 1);
	entry_find = gtk_entry_new ();
	gtk_table_attach_defaults (GTK_TABLE (table), entry_find, 1, 2, 0, 1);
	gtk_label_set_mnemonic_widget (GTK_LABEL (label_find), entry_find);
	gtk_dialog_set_response_sensitive (GTK_DIALOG (dialog),
												  GTK_RESPONSE_OK, FALSE);
	entry_len = 0;
	g_signal_connect (G_OBJECT (entry_find), "insert-text",
							G_CALLBACK (toggle_sensitivity), NULL);
	g_signal_connect (G_OBJECT (entry_find), "delete-text",
							G_CALLBACK (toggle_sensitivity), NULL);
	if (string_find)
	{
		gtk_entry_set_text (GTK_ENTRY (entry_find), string_find);
		gtk_dialog_set_response_sensitive (GTK_DIALOG (dialog),
													  GTK_RESPONSE_OK, TRUE);
	}
	if (mode)
	{
		label_replace = gtk_label_new_with_mnemonic ("Re_place with:");
		gtk_misc_set_alignment (GTK_MISC (label_replace), 0, 0.5);
		gtk_table_attach_defaults (GTK_TABLE (table), label_replace, 0, 1, 1, 2);
		entry_replace = gtk_entry_new ();
		gtk_table_attach_defaults (GTK_TABLE (table), entry_replace, 1, 2, 1, 2);
		gtk_label_set_mnemonic_widget (GTK_LABEL (label_replace), entry_replace);
		if (string_replace)
			gtk_entry_set_text (GTK_ENTRY (entry_replace), string_replace);
	}
	gtk_dialog_set_default_response (GTK_DIALOG (dialog), GTK_RESPONSE_OK);
	gtk_entry_set_activates_default (GTK_ENTRY (entry_find), TRUE);
	if (mode)
		gtk_entry_set_activates_default (GTK_ENTRY (entry_replace), TRUE);

	if (mode)
	{
		check_all = gtk_check_button_new_with_mnemonic ("Replace _all at once");
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (check_all),
												replace_all);
		g_signal_connect (GTK_OBJECT (check_all), "toggled",
								G_CALLBACK (toggle_check_all), NULL);
		gtk_table_attach_defaults (GTK_TABLE (table), check_all, 0, 2, 2 + mode,
											3 + mode);
	}
	gtk_window_set_resizable (GTK_WINDOW (dialog), FALSE);
	gtk_widget_show_all (table);

	res = gtk_dialog_run (GTK_DIALOG (dialog));
	if (res == GTK_RESPONSE_OK)
	{
		g_free (string_find);
		string_find = g_strdup (gtk_entry_get_text (GTK_ENTRY (entry_find)));
		if (mode)
		{
			g_free (string_replace);
			string_replace =
				g_strdup (gtk_entry_get_text (GTK_ENTRY (entry_replace)));
		}
	}

	gtk_widget_destroy (dialog);

	if (res == GTK_RESPONSE_OK)
	{
		if (strlen (string_find))
		{
			if (mode)
				document_replace_real(textview);
			else
				document_search_real (textview, 0);
		}
	}

	return res;
}


void run_dialog_jump_to (GtkWidget *textview)
{
	GtkWidget *dialog;
	GtkWidget *button;
	GtkWidget *table;
	GtkWidget *label;
	GtkWidget *spinner;
	GtkAdjustment *spinner_adj;
	GtkTextIter iter;
	gint num, max_num;

	GtkTextBuffer *textbuffer =
		gtk_text_view_get_buffer (GTK_TEXT_VIEW (textview));

	gtk_text_buffer_get_iter_at_mark (textbuffer, &iter,
												 gtk_text_buffer_get_insert (textbuffer));
	num = gtk_text_iter_get_line (&iter) + 1;
	gtk_text_buffer_get_end_iter (textbuffer, &iter);
	max_num = gtk_text_iter_get_line (&iter) + 1;

	dialog = gtk_dialog_new_with_buttons ("Jump To",
													  GTK_WINDOW (gtk_widget_get_toplevel (
																	  textview)),
													  GTK_DIALOG_DESTROY_WITH_PARENT,
													  GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
													  NULL);
	gtk_dialog_set_has_separator (GTK_DIALOG (dialog), FALSE);
	button = gtk_button_new_from_stock (GTK_STOCK_JUMP_TO);
	GTK_WIDGET_SET_FLAGS (button, GTK_CAN_DEFAULT);
	gtk_dialog_add_action_widget (GTK_DIALOG (dialog), button, GTK_RESPONSE_OK);
	table = gtk_table_new (1, 2, FALSE);
	gtk_table_set_col_spacings (GTK_TABLE (table), 8);
	gtk_container_set_border_width (GTK_CONTAINER (table), 8);
	gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialog)->vbox), table, FALSE,
							  FALSE, 0);
	label = gtk_label_new_with_mnemonic ("_Line number:");
	spinner_adj = (GtkAdjustment *) gtk_adjustment_new (num, 1, max_num, 1, 1,
																		 0);
	spinner = gtk_spin_button_new (spinner_adj, 1, 0);
	gtk_entry_set_width_chars (GTK_ENTRY (spinner), 8);
	gtk_label_set_mnemonic_widget (GTK_LABEL (label), spinner);
	gtk_entry_set_activates_default (GTK_ENTRY (spinner), TRUE);
	gtk_table_attach_defaults (GTK_TABLE (table), label, 0, 1, 0, 1);
	gtk_table_attach_defaults (GTK_TABLE (table), spinner, 1, 2, 0, 1);

	gtk_dialog_set_default_response (GTK_DIALOG (dialog), GTK_RESPONSE_OK);
	gtk_window_set_resizable (GTK_WINDOW (dialog), FALSE);
	gtk_widget_show_all (dialog);

	if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_OK)
	{
		gtk_text_buffer_get_iter_at_line (textbuffer, &iter,
													 gtk_spin_button_get_value_as_int (
													 GTK_SPIN_BUTTON (spinner)) - 1);
		gtk_text_buffer_place_cursor (textbuffer, &iter);
		scroll_to_cursor (textbuffer, 0.1);
		gtk_widget_grab_focus (textview);
	}

	gtk_widget_destroy (dialog);
}
