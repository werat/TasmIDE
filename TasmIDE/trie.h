#ifndef TRIE_H
#define TRIE_H

#include <gtk/gtk.h>

typedef struct _Trie Trie;

Trie *trie_new (void);

void trie_free (Trie *trie);

void trie_insert (Trie *trie, gchar *key);

gboolean trie_lookup (Trie *trie, gchar *key);

#endif // #ifndef TRIE_H