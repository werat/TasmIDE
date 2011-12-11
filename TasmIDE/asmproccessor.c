#include "asmproccessor.h"
#include "undo.h"

void comment_selected_lines (GtkTextBuffer *buffer, gint mode)
{
	GtkTextIter start, end;
	gunichar ch;
	gtk_text_buffer_get_selection_bounds (buffer, &start, &end);
	gtk_text_buffer_get_iter_at_line (buffer, &start,
	                                  gtk_text_iter_get_line (&start));

	ch = gtk_text_iter_get_char (&end);
	if (ch != '\r' && ch != '\n')
		gtk_text_iter_forward_to_line_end (&end);
	comment_lines_auto (buffer, &start, &end, mode);
}

void comment_lines_auto (GtkTextBuffer *buffer, 
                         GtkTextIter *start, GtkTextIter *end,
								 gint mode)
{
	gint i, counter = 0,
		startline = gtk_text_iter_get_line (start),
		endline = gtk_text_iter_get_line (end),
		startoffset = gtk_text_iter_get_offset (start),
		endoffset = gtk_text_iter_get_offset (end);

	GtkTextIter tmp, *iter = gtk_text_iter_copy (start);
	undo_create_undo_info (buffer, 2, startoffset, endoffset);
	switch (mode)
	{
	case 0:
		for (i = startline; i < endline + 1; ++i)
		{
			gunichar c;
			while ((c = gtk_text_iter_get_char (iter)) != '\n' 
				&& g_unichar_isspace (c))
				gtk_text_iter_forward_char (iter);
			if (c == '\0')
				break;
			if (c == ';')
			{
				tmp = *iter;
				gtk_text_iter_forward_char (&tmp);
				gtk_text_buffer_delete (buffer, iter, &tmp);
				--counter;
			}
			else
			{
				if (c != '\n')
				{
					gtk_text_buffer_get_iter_at_line (buffer, &tmp, i);
					gtk_text_buffer_insert (buffer, &tmp, ";", 1);
					*iter = tmp;
					++counter;
				}
			}
			if (!gtk_text_iter_forward_line (iter))
				break;
		}
		break;
	case 1:
		for (i = startline; i < endline + 1; ++i)
		{
			gunichar ch = gtk_text_iter_get_char (iter);
			if (ch != '\r' && ch != '\n')
			{
				gtk_text_buffer_insert (buffer, iter, ";", 1);
				++counter;
			}
			if (ch == '\0')
				break;
			if (!gtk_text_iter_forward_line (iter))
				break;
		}
		break;
	case 2:
		for (i = startline; i < endline + 1; ++i)
		{
			gunichar ch;
			while ((ch = gtk_text_iter_get_char (iter)) != ';'
				&& ch != '\n' && g_unichar_isspace (ch))
				gtk_text_iter_forward_char (iter);
			if (ch == '\0')
				break;
			if (ch == ';')
			{
				tmp = *iter;
				gtk_text_iter_forward_char (&tmp);
				gtk_text_buffer_delete (buffer, iter, &tmp);
				--counter;
			}
			if (!gtk_text_iter_forward_line (iter))
				break;
		}
		break;
	}
	gtk_text_iter_free (iter);
	highlight_lines (buffer, startline, endline);
	undo_create_undo_info (buffer, 0, startoffset, endoffset + counter);
	undo_set_sequency_reserve ();
}