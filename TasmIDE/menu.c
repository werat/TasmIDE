#include <gdk/gdkkeysyms.h>
#include "callback.h"

static GtkWidget *menu_item_save;
static GtkWidget *menu_item_cut;
static GtkWidget *menu_item_copy;
static GtkWidget *menu_item_paste;
static GtkWidget *menu_item_delete;

static GtkItemFactoryEntry menu_items[] =
{
	{ "/_File", NULL,
		NULL, 0, "<Branch>" },
	{ "/File/_New", "<control>N",
		G_CALLBACK (on_file_new), 0, "<StockItem>", GTK_STOCK_NEW },
	{ "/File/_Open...", "<control>O",
		G_CALLBACK (on_file_open), 0, "<StockItem>", GTK_STOCK_OPEN },
	{ "/File/_Save", "<control>S",
		G_CALLBACK (on_file_save), 0, "<StockItem>", GTK_STOCK_SAVE },
	{ "/File/Save _As...", "<shift><control>S",
		G_CALLBACK (on_file_save_as), 0, "<StockItem>", GTK_STOCK_SAVE_AS },
	{ "/File/---", NULL,
		NULL, 0, "<Separator>" },
	{ "/File/_Quit", "<control>Q",
		G_CALLBACK (on_file_quit), 0, "<StockItem>", GTK_STOCK_QUIT },
	{ "/_Edit",	 NULL,
		NULL, 0, "<Branch>" },
	{ "/Edit/_Undo", "<control>Z",
		G_CALLBACK (on_edit_undo), 0, "<StockItem>", GTK_STOCK_UNDO },
	{ "/Edit/_Redo", "<shift><control>Z",
		G_CALLBACK (on_edit_redo), 0, "<StockItem>", GTK_STOCK_REDO },
	{ "/Edit/---", NULL,
		NULL, 0, "<Separator>" },
	{ "/Edit/Cu_t", "<control>X",
		G_CALLBACK (on_edit_cut), 0, "<StockItem>", GTK_STOCK_CUT },
	{ "/Edit/_Copy", "<control>C",
		G_CALLBACK (on_edit_copy), 0, "<StockItem>", GTK_STOCK_COPY },
	{ "/Edit/_Paste", "<control>V",
		G_CALLBACK (on_edit_paste), 0, "<StockItem>", GTK_STOCK_PASTE },
	{ "/Edit/_Delete", NULL,
		G_CALLBACK (on_edit_delete), 0, "<StockItem>", GTK_STOCK_DELETE },
	{ "/Edit/---", NULL,
		NULL, 0, "<Separator>" },
	{ "/Edit/Select _All", "<control>A",
		G_CALLBACK (on_edit_select_all), 0 },
	{ "/Edit/---", NULL,
		NULL, 0, "<Separator>" },
	{ "/Edit/Autocomment lines", "<control>semicolon",
		G_CALLBACK (on_asm_comment_lines_auto), 0 },
	{ "/Edit/Comment lines", "<control>K",
		G_CALLBACK (on_asm_comment_lines), 0 },
	{ "/Edit/Uncomment lines", "<control>U",
		G_CALLBACK (on_asm_uncomment_lines), 0 },
	{ "/_Search",	 NULL,
		NULL, 0, "<Branch>" },
	{ "/Search/_Find...", "<control>F",
		G_CALLBACK (on_search_find), 0, "<StockItem>", GTK_STOCK_FIND },
	{ "/Search/Find _Next", "<control>G",
		G_CALLBACK (on_search_find_next), 0 },
	{ "/Search/Find _Previous", "<shift><control>G",
		G_CALLBACK (on_search_find_previous), 0 },
	{ "/Search/_Replace...", "<control>H",
		G_CALLBACK (on_search_replace), 0, "<StockItem>",
		GTK_STOCK_FIND_AND_REPLACE },
	{ "/Search/---", NULL,
		NULL, 0, "<Separator>" },
	{ "/Search/_Jump To...", "<control>J",
		G_CALLBACK (on_search_jump_to), 0, "<StockItem>", GTK_STOCK_JUMP_TO },
	{ "/_Insert", NULL,
		NULL, 0, "<Branch>" },
	{ "/Insert/Insert proc", "<shift><control>P",
		G_CALLBACK (on_insert_insert_proc), 0 },
	{ "/Insert/Insert push-pop", "<shift><control>R",
		G_CALLBACK (on_insert_insert_push_pop), 0 },
	{ "/Insert/Insert model small", "<shift><control>M",
		G_CALLBACK (on_insert_insert_model_small), 0 },
	{ "/_Build",	NULL,
		NULL, 0, "<Branch>" },
	{ "/Build/_Build", "<control>B",
		G_CALLBACK (on_build_build), 0 },
	{ "/Build/Builnd and run", "F5",
		G_CALLBACK (on_build_build_run), 0 },
	{ "/_Options", NULL,
		NULL, 0, "<Branch>" },
	{ "/Options/Colors", NULL,
		NULL, 0, "<Branch>" },
	{ "/Options/Colors/Command", 0,
		G_CALLBACK (on_option_color_command), 0 },
	{ "/Options/Colors/Keyword", 0,
		G_CALLBACK (on_option_color_keyword), 0 },
	{ "/Options/Colors/Register", 0,
		G_CALLBACK (on_option_color_register), 0 },
	{ "/Options/Colors/Jump", 0,
		G_CALLBACK (on_option_color_jump), 0 },
	{ "/Options/Colors/Comment", 0,
		G_CALLBACK (on_option_color_comment), 0 },
	{ "/Options/Colors/String", 0,
		G_CALLBACK (on_option_color_string), 0 },
	{ "/Options/Colors/Foundword", 0,
		G_CALLBACK (on_option_color_foundword), 0 },
	{ "/Options/Colors/Mark", 0,
		G_CALLBACK (on_option_color_mark), 0 },
	{ "/Options/_Font...", NULL,
		G_CALLBACK (on_option_font), 0, "<StockItem>", GTK_STOCK_SELECT_FONT },
	{ "/Options/---", NULL,
		NULL, 0, "<Separator>" },
	{ "/Options/_Auto Indent", NULL,
		G_CALLBACK (on_option_auto_indent), 0, "<CheckItem>" },
	{ "/_Help", NULL,
		NULL, 0, "<Branch>" },
	{ "/Help/_About", NULL,
		G_CALLBACK (on_help_about), 0, "<StockItem>", GTK_STOCK_ABOUT },
};

static gint nmenu_items = sizeof(menu_items) / sizeof(GtkItemFactoryEntry);

static gchar *menu_translate (const gchar *path, gpointer data)
{
	return (gchar *)(path);
}

void menu_sensitivity_from_modified_flag (gboolean is_text_modified)
{
	gtk_widget_set_sensitive (menu_item_save, is_text_modified);
}

void menu_sensitivity_from_selection_bound (gboolean is_bound_exist)
{
	gtk_widget_set_sensitive (menu_item_cut, is_bound_exist);
	gtk_widget_set_sensitive (menu_item_copy, is_bound_exist);
	gtk_widget_set_sensitive (menu_item_delete, is_bound_exist);
}

void menu_sensitivity_from_clipboard (void)
{
	gtk_widget_set_sensitive (menu_item_paste,
									  gtk_clipboard_wait_is_text_available (
									  gtk_clipboard_get (GDK_SELECTION_CLIPBOARD)));
}

GtkWidget *create_menu_bar (GtkWidget *window)
{
	GtkAccelGroup *accel_group;
	GtkItemFactory *ifactory;

	accel_group = gtk_accel_group_new ();
	ifactory = gtk_item_factory_new (GTK_TYPE_MENU_BAR, "<main>", accel_group);
	gtk_item_factory_set_translate_func (ifactory, menu_translate, NULL, NULL);
	gtk_item_factory_create_items (ifactory, nmenu_items, menu_items, NULL);
	gtk_window_add_accel_group (GTK_WINDOW (window), accel_group);

	/* hidden keybinds */
	gtk_accel_group_connect (
		accel_group, GDK_W, GDK_CONTROL_MASK, 0,
		g_cclosure_new_swap (G_CALLBACK (on_file_close), NULL, NULL));
	gtk_widget_add_accelerator (
		gtk_item_factory_get_widget (ifactory, "/Edit/Redo"),
		"activate", accel_group, GDK_Y, GDK_CONTROL_MASK, 0);
	gtk_widget_add_accelerator (
		gtk_item_factory_get_widget (ifactory, "/Search/Find Next"),
		"activate", accel_group, GDK_F3, 0, 0);
	gtk_widget_add_accelerator (
		gtk_item_factory_get_widget (ifactory, "/Search/Find Previous"),
		"activate", accel_group, GDK_F3, GDK_SHIFT_MASK, 0);
	gtk_widget_add_accelerator (
		gtk_item_factory_get_widget (ifactory, "/Search/Replace..."),
		"activate", accel_group, GDK_R, GDK_CONTROL_MASK, 0);

	/* initialize sensitivities */;
	gtk_widget_set_sensitive (
		gtk_item_factory_get_widget (ifactory, "/Search/Find Next"),
		FALSE);
	gtk_widget_set_sensitive (
		gtk_item_factory_get_widget (ifactory, "/Search/Find Previous"),
		FALSE);
	gtk_check_menu_item_set_active (
		GTK_CHECK_MENU_ITEM (gtk_item_factory_get_item (ifactory, "/Options/Auto Indent")), 
		TRUE);

	menu_item_save = gtk_item_factory_get_widget (ifactory, "/File/Save");
	menu_item_cut = gtk_item_factory_get_widget (ifactory, "/Edit/Cut");
	menu_item_copy = gtk_item_factory_get_widget (ifactory, "/Edit/Copy");
	menu_item_paste = gtk_item_factory_get_widget (ifactory, "/Edit/Paste");
	menu_item_delete = gtk_item_factory_get_widget (ifactory, "/Edit/Delete");
	menu_sensitivity_from_selection_bound (FALSE);

	return gtk_item_factory_get_widget (ifactory, "<main>");
}
