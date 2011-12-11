#ifndef ASMPROCCESSOR_H
#define ASMPROCCESSOR_H

#include "Editor.h"

// 0 - for autocomment
// 1 - comment
// 2 - uncomment
void comment_selected_lines (GtkTextBuffer *buffer, gint mode);
void comment_lines_auto (GtkTextBuffer *buffer, 
                         GtkTextIter *start, GtkTextIter *end,
								 gint mode);
#endif // ASMPROCCESSOR_H