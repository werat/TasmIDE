#include "Editor.h"
#include <gdk/gdkkeysyms.h>

static gint keyval;
static gboolean auto_indent = TRUE;

void set_auto_indent (gboolean autoindent)
{
	auto_indent = autoindent;
}

gint get_current_keyval (void)
{
	return keyval;
}
void clear_current_keyval (void)
{
	keyval = 0;
}

void scroll_to_cursor (GtkTextBuffer *buffer, gdouble within_margin)
{
	gtk_text_view_scroll_to_mark (GTK_TEXT_VIEW (wnd.textview),
											gtk_text_buffer_get_insert (buffer),
											within_margin, FALSE, 0, 0);
}

static void draw_line_numbers (GtkWidget *widget,
										 GdkWindow *window, gint start_y)
{
	GtkTextView *textview = GTK_TEXT_VIEW (widget);
	cairo_t *cr;
	GtkTextIter iter, end_iter;
	gint layout_width, col = 5, margin = 5, y_out;
	gint yrange, line_height;
	gboolean should_draw = TRUE;

	gchar *empty_str = g_strnfill (col, 0x30);
	PangoLayout *layout = gtk_widget_create_pango_layout (widget, empty_str);
	g_free (empty_str);

	pango_layout_get_pixel_size (layout, &layout_width, NULL);
	gtk_text_view_set_border_window_size (GTK_TEXT_VIEW (textview),
													  GTK_TEXT_WINDOW_LEFT,
													  layout_width + margin);

	cr = gdk_cairo_create (window);

	// draw left rectangle for line numbers 
	cairo_set_source_rgb (cr, 0.90, 0.90, 1);
	//cairo_set_line_width (cr, 1);
	cairo_rectangle (cr, 0, start_y, layout_width + margin,
						  widget->allocation.height);
	cairo_fill_preserve (cr);
	// draw line numbers 
	cairo_set_source_rgb (cr, 0.5, 0.5, 0.5);
	gtk_text_view_window_to_buffer_coords (
		textview, GTK_TEXT_WINDOW_TEXT, 0, 0, NULL, &y_out);
	gtk_text_view_get_line_at_y (textview, &iter, y_out, NULL);
	gtk_text_view_window_to_buffer_coords (
		textview, GTK_TEXT_WINDOW_TEXT, 0, widget->allocation.height,
		NULL, &y_out);
	gtk_text_view_get_line_at_y (textview, &end_iter, y_out, NULL);
	gtk_text_iter_forward_line (&end_iter);

	do
	{
		gint line_number, ph, pw;
		gchar *line_num_str;
		gtk_text_view_get_line_yrange (textview, &iter, &yrange, &line_height);
		gtk_text_view_buffer_to_window_coords (
			textview, GTK_TEXT_WINDOW_TEXT, 0, yrange, NULL, &yrange);
		line_number = gtk_text_iter_get_line (&iter);
		line_num_str = g_strdup_printf ("%d", line_number + 1);
		pango_layout_set_text (layout, line_num_str, -1);
		ph = pango_layout_get_height (layout);
		pw = pango_layout_get_width (layout);
		pango_layout_get_pixel_size (layout, &pw, NULL);
		cairo_move_to (cr, layout_width - pw, yrange - ph);
		pango_cairo_show_layout (cr, layout);
		g_free (line_num_str);

		if (! should_draw)
			break;
		should_draw = gtk_text_iter_forward_line (&iter);
	} while (! (gtk_text_iter_equal (&iter, &end_iter) && should_draw));

	g_object_unref (layout);
	cairo_destroy (cr);
}

static void load_general_settings (GtkWidget *textview, GKeyFile *keyfile)
{
	GdkColor color;
	gchar **values;
	gint fsize;
	PangoFontDescription *font = pango_font_description_new ();

	values = g_key_file_get_string_list (
		keyfile, "general", "background", NULL, NULL);
	if (gdk_color_parse (values[0], &color))
		gtk_widget_modify_base (textview, GTK_STATE_NORMAL, &color);
	g_strfreev (values);

	values = g_key_file_get_string_list (
		keyfile, "general", "font", NULL, NULL);
	pango_font_description_set_family (font, values[0]);
	fsize = (gint)g_ascii_strtoll (values[1], NULL, 10);
	pango_font_description_set_size (font, fsize * PANGO_SCALE);
	gtk_widget_modify_font (textview, font);
	g_strfreev (values);
	pango_font_description_free (font);
}

static void load_editor_settings (GKeyFile *keyfile)
{
	wnd.compilerpath = g_key_file_get_string (
		keyfile, "settings", "compilerpath", NULL);
	wnd.linkerpath = g_key_file_get_string (
		keyfile, "settings", "linkerpath", NULL);
}
static void fetch_settings_from_ini (GtkWidget *textview)
{
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
		buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (textview));
		initialize_highlighter (buffer, keyfile);
		load_general_settings (textview, keyfile);
		load_editor_settings (keyfile);
	}

	if (error)
		g_error_free (error);
	g_key_file_free (keyfile);
}
static void initialize_textview (GtkWidget *widget)
{
	GtkTextView *textview = GTK_TEXT_VIEW (widget);
	gint layout_width, col = 5, margin = 5;
	gchar *empty_str = g_strnfill (col, 0x30);
	PangoLayout *layout = gtk_widget_create_pango_layout (widget, empty_str);
	g_free (empty_str);
	pango_layout_get_pixel_size (layout, &layout_width, NULL);
	gtk_text_view_set_border_window_size (GTK_TEXT_VIEW (textview),
													  GTK_TEXT_WINDOW_LEFT,
													  layout_width + margin);

	fetch_settings_from_ini (widget);

	g_object_unref (layout);
}
void free_textview ()
{
	free_highlighter ();
}

//---------------------- EVENTS ---------------------------------------------//
static gboolean on_textview_expose (GtkWidget *widget,
												GdkEventExpose *event, gpointer data)
{
	GtkTextView *textview = GTK_TEXT_VIEW (widget);
	if (gtk_text_view_get_window_type (textview, event->window)
		 != GTK_TEXT_WINDOW_LEFT)
		return FALSE;
	else
		draw_line_numbers (widget, event->window, event->area.y);
	return FALSE;
}

static void on_mark_changed (GtkTextBuffer *buffer)
{
	menu_sensitivity_from_selection_bound (
		gtk_text_buffer_get_selection_bounds (buffer, NULL, NULL));
}

static gboolean on_key_pressed (GtkWidget *view, GdkEventKey *event)
{
	gint cursor_pos;

	gtk_im_context_get_preedit_string (
		GTK_TEXT_VIEW (view)->im_context, NULL, NULL, &cursor_pos);
	if (cursor_pos)
		return FALSE;

	keyval = event->keyval;
	if (  (event->state & GDK_CONTROL_MASK)
			|| (event->keyval == GDK_Control_L)
			|| (event->keyval == GDK_Control_R))
	{
		keyval = keyval + 0x10000;
	}
	return FALSE;
}
//---------------------------------------------------------------------------//

//---------------------- PUBLIC FUNCTIONS -----------------------------------//
void indent_line (GtkTextBuffer *buffer, GtkTextIter *location)
{
	if (auto_indent)
	{
		GtkTextIter *iter = gtk_text_iter_copy (location);
		GtkTextIter *start;
		gchar ch;
		gint len = 0;
		gtk_text_iter_backward_line (iter);
		start = gtk_text_iter_copy (iter);
		ch = gtk_text_iter_get_char (iter);
		while (ch == ' ' || ch == '\t')
		{
			gtk_text_iter_forward_char (iter);
			ch = gtk_text_iter_get_char (iter);
			++len;
		}
		if (! gtk_text_iter_equal (iter, location))
		{
			gchar *text = gtk_text_iter_get_slice (start, iter);
			gtk_text_buffer_insert (buffer, location, text, len);
		}
		gtk_text_iter_free (iter);
		gtk_text_iter_free (start);
	}
}
//---------------------------------------------------------------------------//

GtkWidget *create_text_view ()
{
	GtkWidget *textview;
	GtkTextBuffer *buffer;

	textview = gtk_text_view_new ();
	gtk_text_view_set_left_margin (GTK_TEXT_VIEW (textview), 10);
	gtk_text_view_set_right_margin (GTK_TEXT_VIEW (textview), 10);

	g_signal_connect (textview, "key-press-event",
							G_CALLBACK (on_key_pressed), NULL);

	g_signal_connect (textview, "expose-event",
							G_CALLBACK (on_textview_expose), NULL);

	buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (textview));
	g_signal_connect (buffer, "mark-set",
							G_CALLBACK (on_mark_changed), NULL);
	g_signal_connect (buffer, "mark-deleted",
							G_CALLBACK (on_mark_changed), NULL);

	g_signal_connect_after (buffer, "changed",
							G_CALLBACK (on_text_changed), NULL);
	g_signal_connect (buffer, "insert-text",
							G_CALLBACK (on_insert_text), NULL);
	g_signal_connect_after (buffer, "delete_range",
	                        G_CALLBACK (on_after_delete), NULL);
	g_signal_connect (buffer, "mark_set", 
	                  G_CALLBACK (mark_set_callback), wnd.statusbar);

	initialize_textview (textview);

	return textview;
}
