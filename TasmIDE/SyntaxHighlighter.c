
#define EXPAND_ARRAY2(a) (a[0]),(a[1])
#define EXPAND_ARRAY4(a) (a[0]),(a[1]),(a[2]),(a[3])
#define EXPAND_ARRAY6(a) (a[0]),(a[1]),(a[2]),(a[3]),(a[4]),(a[5])
#define EXPAND_ARRAY8(a) (a[0]),(a[1]),(a[2]),(a[3]),(a[4]),(a[5]),(a[6]),(a[7])

#include <string.h>
#include "Editor.h"

static void create_trie (Trie **trie, GKeyFile *keyfile, const gchar *type);
static void create_tag (GKeyFile *keyfile, GtkTextBuffer *buffer,
								const gchar *type);

gboolean pasted = FALSE;
Trie *keyword_trie;
Trie *command_trie;
Trie *register_trie;
Trie *jump_trie;

static gchar *trie_search (gchar *word)
{
	gchar *tag = NULL;
	// Check if keyword
	if (trie_lookup (keyword_trie, word))
	{
		tag = g_strdup ("keyword");
	}
	// Check if command
	else if (trie_lookup (command_trie, word))
	{
		tag = g_strdup ("command");
	}
	// Check if register
	else if (trie_lookup (register_trie, word))
	{
		tag = g_strdup ("register");
	}
	// Check if "jump"
	else if (trie_lookup (jump_trie, word))
	{
		tag = g_strdup ("jump");
	}
	return tag;
}

void highlight_lines (GtkTextBuffer *buffer,
                      gint startl, gint endl)
{
	GtkTextIter starti, endi; // selection starti -> endi
	gint i; gunichar ch;
	gtk_text_buffer_get_iter_at_line (buffer, &starti, startl);
	gtk_text_buffer_get_iter_at_line (buffer, &endi, endl);
	ch = gtk_text_iter_get_char (&endi);
	if (ch != '\r' && ch != '\n')
		gtk_text_iter_forward_to_line_end (&endi);
	gtk_text_buffer_remove_all_tags (buffer, &starti, &endi);
	gtk_text_buffer_get_start_iter (buffer, &starti);
	gtk_text_buffer_get_end_iter (buffer, &endi);

	for (i = startl; i < endl + 1; ++i)
	{
		gunichar c, matched = 0;
		gboolean wasstart = FALSE, waitformatch = FALSE;
		gboolean lastiterarion = FALSE;
		gtk_text_buffer_get_iter_at_line (buffer, &endi, i);
		starti = endi;

		c = gtk_text_iter_get_char (&endi);
		while (c != '\n' || lastiterarion)
		{
			// Check for words
			if (g_unichar_isspace (c) || g_unichar_ispunct (c) || c == ';' || c == '_')
			{
				if (wasstart)
				{
					gchar *cased_word = gtk_text_buffer_get_text (
						buffer, &starti, &endi, FALSE);
					gchar *word = g_utf8_strdown (cased_word, -1);

					gchar *trie_tag = trie_search (word);
					if (trie_tag)
					{
						gtk_text_buffer_apply_tag_by_name (buffer, trie_tag, &starti, &endi);
						g_free (trie_tag);
						starti = endi;
					}
					wasstart = FALSE;
					g_free (cased_word);
					g_free (word);
				}
			}
			if (g_unichar_isalpha (c) || c == '.' || c == '_')
			{
				if (!wasstart && !waitformatch)
				{
					starti = endi;
					wasstart = TRUE;
				}
			}

			// Check for marks
			if (c == ':' && !waitformatch)
			{
				GtkTextIter tmp = endi, etmp = endi;
				while (gtk_text_iter_backward_char (&tmp) 
					&& !g_unichar_isspace (gtk_text_iter_get_char (&tmp)))
					;
				if (g_unichar_isspace (gtk_text_iter_get_char (&tmp)))
					gtk_text_iter_forward_char (&tmp);
				gtk_text_iter_forward_char (&etmp);
				gtk_text_buffer_remove_all_tags (buffer, &tmp, &etmp);
				gtk_text_buffer_apply_tag_by_name (buffer, "mark", &tmp, &etmp);
				starti = endi;
			}

			// Check for strings
			if (c == '\'' || c == '"')
			{
				if (c == matched)
				{
					GtkTextIter tmp = endi;
					gtk_text_iter_forward_char (&tmp);
					gtk_text_buffer_apply_tag_by_name (buffer, "string", &starti, &tmp);
					starti = endi;
					waitformatch = FALSE;
				} 
				else if (!waitformatch)
				{
					starti = endi;
					matched = c;
					waitformatch = TRUE;
				}
			}
			// Check for comment
			else if (c == ';')
			{
				starti = endi;
				gtk_text_iter_forward_to_line_end (&endi);
				gtk_text_buffer_apply_tag_by_name (buffer, "comment", &starti, &endi);
				waitformatch = FALSE;
				lastiterarion = TRUE;
			}

			if (lastiterarion)
				break;
			if (!gtk_text_iter_forward_char (&endi)
				|| (c = gtk_text_iter_get_char (&endi)) == '\n')
			{
				if (waitformatch)
					c = matched;
				else
					c = ' ';
				lastiterarion = TRUE;
				continue;
			}
		}
	}
}

void highlight_text_pos (GtkTextBuffer *buffer,
                         gint start, gint end)
{
	GtkTextIter istart, iend;
	gint sline, eline;
	gtk_text_buffer_get_iter_at_offset (buffer, &istart, start);
	gtk_text_buffer_get_iter_at_offset (buffer, &iend, end);

	sline = gtk_text_iter_get_line (&istart);
	eline = gtk_text_iter_get_line (&iend);
	highlight_lines (buffer, sline, eline);
}

void highlight_all (GtkTextBuffer *buffer)
{	
	highlight_lines (buffer, 0, gtk_text_buffer_get_line_count (buffer));
}

//============= Initialize highlighter ========================================
void initialize_highlighter (GtkTextBuffer *buffer, GKeyFile *keyfile)
{
	create_trie (&keyword_trie, keyfile, "keywords");
	create_trie (&command_trie, keyfile, "commands");
	create_trie (&register_trie, keyfile, "registers");
	create_trie (&jump_trie, keyfile, "jumps");

	create_tag (keyfile, buffer, "keyword");
	create_tag (keyfile, buffer, "command");
	create_tag (keyfile, buffer, "register");
	create_tag (keyfile, buffer, "comment");
	create_tag (keyfile, buffer, "string");
	create_tag (keyfile, buffer, "foundword");
	create_tag (keyfile, buffer, "mark");
	create_tag (keyfile, buffer, "jump");
}

void free_highlighter ()
{
	trie_free (keyword_trie);
	trie_free (command_trie);
	trie_free (register_trie);
}

static void create_tag (GKeyFile *keyfile, GtkTextBuffer *buffer,
								const gchar *name)
{
	gsize length;
	gchar **ps = g_key_file_get_string_list (
		keyfile, "colors", name, &length, NULL);
	switch (length)
	{
		case 2:
			gtk_text_buffer_create_tag (buffer, name,
												 EXPAND_ARRAY2 (ps), NULL);
			break;
		case 4:
			gtk_text_buffer_create_tag (buffer, name,
												 EXPAND_ARRAY4 (ps), NULL);
			break;
		case 6:
			gtk_text_buffer_create_tag (buffer, name,
												 EXPAND_ARRAY6 (ps), NULL);
			break;
		case 8:
			gtk_text_buffer_create_tag (buffer, name,
												 EXPAND_ARRAY8 (ps), NULL);
			break;
		default:
			break;
	}
	g_strfreev (ps);
}
static void create_trie (Trie **trie, GKeyFile *keyfile, const gchar *type)
{
	gsize i, length;
	gchar **keywords = g_key_file_get_string_list (
		keyfile, "highlight", type, &length, NULL);
	*trie = trie_new ();
	for (i = 0; i < length; ++i)
		trie_insert (*trie, keywords[i]);
	g_strfreev (keywords);
}
//=============================================================================

static gint start, end;

//=============== Events ======================================================
void on_insert_text (GtkTextBuffer *buffer, GtkTextIter *location,
							gchar *text, gint len, gpointer user_data)
{
	start = gtk_text_iter_get_offset (location);
	end = start + g_utf8_strlen (text, -1);
}
void on_after_delete (GtkTextBuffer *buffer, 
                      GtkTextIter *start_iter, GtkTextIter *end_iter)
{
	start = gtk_text_iter_get_line (start_iter);
	end = gtk_text_iter_get_line (end_iter);
}
void on_text_changed (GtkTextBuffer *buffer, gpointer data)
{
	highlight_text_pos (buffer, start, end);
	if (!wnd.modified)
	{
		wnd.modified = TRUE;
		set_window_title ();
	}
}
//=============================================================================