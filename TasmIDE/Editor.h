#ifndef EDITOR_H
#define EDITOR_H

#include <gtk/gtk.h>

#include "EditorWindow.h"
#include "dialog.h"
#include "document.h"
#include "font.h"
#include "menu.h"
#include "Search.h"
#include "insert.h"
#include "SyntaxHighlighter.h"
#include "TextView.h"
#include "statusbar.h"
#include "trie.h"
#include "undo.h"

#define EDITOR_HEADER "TasmIDE by Yankowsky"

typedef struct
{
	GtkWidget *window;
	GtkWidget *vbox;
	GtkWidget *menubar;
	GtkWidget *scrolledwindow;
	GtkWidget *textview;
	GtkWidget *statusbar;

	gchar *filename;
	gboolean fileopened;
	gboolean modified;

	gchar *compilerpath;
	gchar *linkerpath;
} EditorWindow;


EditorWindow wnd;

#endif /* EDITOR_H */
