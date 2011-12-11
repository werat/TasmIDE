#include "Editor.h"
#include "color.h"

static gchar *get_color_by_selector (GtkWidget *window)
{
	GtkWidget *dialog;
	gchar *color;

	dialog = gtk_color_selection_dialog_new ("Choose color");
	gtk_window_set_transient_for (GTK_WINDOW (dialog), GTK_WINDOW (window));
	if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_OK)
	{
		GtkWidget *colorsel;
		GdkColor col;
		colorsel 
			= gtk_color_selection_dialog_get_color_selection (
			GTK_COLOR_SELECTION_DIALOG (dialog));
		gtk_color_selection_get_current_color (GTK_COLOR_SELECTION (colorsel), &col);
		color = gdk_color_to_string (&col);
	}
	else
		color = NULL;
	gtk_widget_destroy (dialog);

	return color;
}

void change_color_by_selector (GtkWidget *widget, const gchar *type)
{
	gchar *color;

	color = get_color_by_selector (
		gtk_widget_get_toplevel (widget));
	if (color)
	{
		gchar *text;
		GtkTextBuffer *buffer;
		GKeyFile *keyfile;
		GError *error = NULL;
		keyfile = g_key_file_new ();
		if (! g_key_file_load_from_file (keyfile, "config.ini", 0, &error))
		{
			g_error ("%s", error->message);
		}
		else
		{
			gchar **args;
			gchar *data;
			text = g_strdup_printf ("foreground %s", color);
			args = g_strsplit (text, " ", -1);
			g_key_file_set_string_list (keyfile, "colors", type, args, 2);
			data = g_key_file_to_data (keyfile, NULL, NULL);
			g_file_set_contents ("config.ini", data, -1, NULL);
			g_free (data);
			g_strfreev (args);
		}

		if (error)
			g_error_free (error);
		g_key_file_free (keyfile);
		g_free (color);
	}
}