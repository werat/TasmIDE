#include "Editor.h"

void set_text_font_by_name (GtkWidget *widget, gchar *fontname)
{
	PangoFontDescription *font_desc;

	font_desc = pango_font_description_from_string (fontname);
	gtk_widget_modify_font (widget, font_desc);
	pango_font_description_free (font_desc);
}

gchar *get_font_name_from_widget (GtkWidget *widget) /* MUST BE FREED */
{
	GtkStyle *style;

	style = gtk_widget_get_style (widget);
	return pango_font_description_to_string (style->font_desc);
}

static gchar *get_font_name_by_selector (GtkWidget *window,
													  gchar *current_fontname)
{
	GtkWidget *dialog;
	gchar *fontname;

	dialog = gtk_font_selection_dialog_new ("Font");
	gtk_window_set_transient_for (GTK_WINDOW (dialog), GTK_WINDOW (window));
	gtk_font_selection_dialog_set_font_name (GTK_FONT_SELECTION_DIALOG (dialog),
														  current_fontname);
	if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_OK)
		fontname =
			gtk_font_selection_dialog_get_font_name (GTK_FONT_SELECTION_DIALOG (
																  dialog));
	else
		fontname = NULL;
	gtk_widget_destroy (dialog);

	return fontname;
}

void change_text_font_by_selector (GtkWidget *widget)
{
	gchar *current_fontname, *fontname;

	current_fontname = get_font_name_from_widget (widget);
	fontname = get_font_name_by_selector (
		gtk_widget_get_toplevel (widget), current_fontname);
	if (fontname)
		set_text_font_by_name (widget, fontname);

	g_free (fontname);
	g_free (current_fontname);
}

