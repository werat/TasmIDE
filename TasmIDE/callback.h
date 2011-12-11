#ifndef CALLBACK_H
#define CALLBACK_H

#include <gtk/gtk.h>

void on_file_new (void);
void on_file_open (void);
gint on_file_save (void);
gint on_file_save_as (void);
void on_file_close (void);
gboolean on_file_quit (void);
void on_edit_undo (void);
void on_edit_redo (void);
void on_edit_cut (void);
void on_edit_copy (void);
void on_edit_paste (void);
void on_edit_delete (void);
void on_edit_select_all (void);
void on_search_find (void);
void on_search_find_next (void);
void on_search_find_previous (void);
void on_search_replace (void);
void on_search_jump_to (void);
void on_asm_comment_lines_auto (void);
void on_asm_comment_lines (void);
void on_asm_uncomment_lines (void);
void on_insert_insert_proc (void);
void on_insert_insert_push_pop (void);
void on_insert_insert_model_small (void);
void on_build_build (void);
void on_build_build_run (void);

void on_option_color_command (void);
void on_option_color_keyword (void);
void on_option_color_register (void);
void on_option_color_jump (void);
void on_option_color_comment (void);
void on_option_color_string (void);
void on_option_color_foundword (void);
void on_option_color_mark (void);
void on_option_font (void);
void on_option_auto_indent (void);
void on_help_about (void);

#endif /* CALLBACK_H */
