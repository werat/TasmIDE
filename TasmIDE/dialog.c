#include "dialog.h"

gint run_dialog_question (GtkWindow *parent, const gchar *text)
{
	GtkWidget *dialog;
	gint res = GTK_RESPONSE_YES;
	dialog = gtk_message_dialog_new (parent,
												GTK_DIALOG_DESTROY_WITH_PARENT,
												GTK_MESSAGE_QUESTION,
												GTK_BUTTONS_NONE,
												"%s", text);
	gtk_window_set_resizable (GTK_WINDOW (dialog), FALSE);
	gtk_dialog_add_buttons (GTK_DIALOG (dialog),
									GTK_STOCK_NO, GTK_RESPONSE_NO,
									GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
									GTK_STOCK_YES, GTK_RESPONSE_YES,
									NULL);
	gtk_dialog_set_default_response (GTK_DIALOG (dialog), GTK_RESPONSE_YES);
	res = gtk_dialog_run (GTK_DIALOG (dialog));
	gtk_widget_destroy (dialog);
	return res;
}

void run_dialog_warning (GtkWindow *parent, const gchar *text)
{
	GtkWidget *dialog;
	dialog = gtk_message_dialog_new (parent,
												GTK_DIALOG_DESTROY_WITH_PARENT,
												GTK_MESSAGE_ERROR,
												GTK_BUTTONS_OK,
												"%s", text);
	gtk_window_set_resizable (GTK_WINDOW (dialog), FALSE);
	gtk_dialog_set_default_response (GTK_DIALOG (dialog), GTK_RESPONSE_YES);
	gtk_dialog_run (GTK_DIALOG (dialog));
	gtk_widget_destroy (dialog);
}

void run_window_with_text (const gchar *text)
{
	GtkWidget *window;
	GtkWidget *scrolled, *view;

	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_default_size (GTK_WINDOW (window), 400, 400);
	gtk_window_set_title (GTK_WINDOW (window), "Build log");

	scrolled = gtk_scrolled_window_new (NULL, NULL);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled),
											  GTK_POLICY_AUTOMATIC,
											  GTK_POLICY_AUTOMATIC);
	gtk_container_add (GTK_CONTAINER (window), scrolled);

	view = gtk_text_view_new ();
	gtk_text_view_set_editable (GTK_TEXT_VIEW (view), FALSE);
	gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (view), GTK_WRAP_WORD);
	gtk_container_add (GTK_CONTAINER (scrolled), view);
	gtk_text_buffer_set_text (
		GTK_TEXT_VIEW (view)->buffer, text, -1);
	gtk_widget_show_all (window);
}