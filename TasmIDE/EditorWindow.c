#include <glib.h>
#include "EditorWindow.h"
#include "callback.h"

void set_window_title ()
{
	//gchar *file = g_strrstr (wnd.filename, "\\");
	gchar *title = g_strconcat (wnd.filename, wnd.modified ? "*" : ""," - ", EDITOR_HEADER, NULL);
	gtk_window_set_title (GTK_WINDOW (wnd.window), title);
	g_free (title);
}

void on_exit ()
{
	free_textview ();
	gtk_main_quit ();
}

void create_editor_window ()
{
	GtkItemFactory *ifactory;

	wnd.filename = NULL;
	wnd.fileopened = FALSE;
	wnd.modified = FALSE;

	/* Create a Window. */
	wnd.window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title (GTK_WINDOW (wnd.window), EDITOR_HEADER);

	/* Set a decent default size for the window. */
	gtk_window_set_default_size (GTK_WINDOW (wnd.window), 800, 600);
	g_signal_connect (G_OBJECT (wnd.window), "delete-event",
							G_CALLBACK (on_file_quit), NULL);

	/* Create Vbox container */
	wnd.vbox = gtk_vbox_new (FALSE, 0);
	gtk_container_add (GTK_CONTAINER (wnd.window), wnd.vbox);

	/* Create Scrolled window */
	wnd.scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (wnd.scrolledwindow),
											  GTK_POLICY_AUTOMATIC,
											  GTK_POLICY_AUTOMATIC);

	/* Create Statusbar */
	wnd.statusbar = statusbar_new ();

	/* Create a multiline text widget. */
	wnd.textview = create_text_view ();
	gtk_container_add (GTK_CONTAINER (wnd.scrolledwindow), wnd.textview);

	/* Create Menubar */
	wnd.menubar = create_menu_bar (wnd.window);

	/* Init undo */
	ifactory = gtk_item_factory_from_widget (wnd.menubar);
	undo_init (wnd.textview,
				  gtk_item_factory_get_widget (ifactory, "/Edit/Undo"),
				  gtk_item_factory_get_widget (ifactory, "/Edit/Redo"));


	/* Packing */
	gtk_box_pack_start (GTK_BOX (wnd.vbox), wnd.menubar, FALSE, TRUE, 0);
	gtk_box_pack_start (GTK_BOX (wnd.vbox), wnd.scrolledwindow, TRUE, TRUE, 0);
	gtk_box_pack_start (GTK_BOX (wnd.vbox), wnd.statusbar, FALSE, FALSE, 0);

	gtk_widget_show_all (wnd.window);

	/* Set focus to text view */
	gtk_widget_grab_focus (GTK_WIDGET (wnd.textview));
	new_document ();
}
