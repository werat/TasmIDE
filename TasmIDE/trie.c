#include <string.h>
#include "trie.h"

typedef struct _TrieNode TrieNode;

struct _TrieNode
{
	gboolean used;
	TrieNode *next[256];
};

struct _Trie
{
	TrieNode *root_node;
};

static void trie_free_node (TrieNode *node)
{
	gint i;
	if (node == NULL)
		return;
	for (i = 0; i < 256; ++i)
		trie_free_node (node->next[i]);
	g_free (node);
}

void trie_free (Trie *trie)
{
	trie_free_node (trie->root_node);
	g_free (trie);
}


Trie *trie_new (void)
{
	Trie *new_trie;

	new_trie = (Trie *) g_malloc (sizeof(Trie));
	new_trie->root_node = NULL;

	return new_trie;
}

void trie_insert (Trie *trie, gchar *key)
{
	TrieNode **rover;
	TrieNode *node;
	gchar *p;
	gint c;

	rover = &trie->root_node;
	p = key;

	while (TRUE)
	{
		node = *rover;
		if (node == NULL)
		{
			// Node does not exist, so create it
			node = (TrieNode *) g_malloc (sizeof(TrieNode));
			memset (node, 0, sizeof(TrieNode));
			*rover = node;
		}
		c = *p;
		// Reached the end of string?  If so, we're finished.
		if (c == '\0')
		{
			node->used = TRUE;
			break;
		}
		// Advance to the next node in the chain
		rover = &node->next[c];
		++p;
	}
}

gboolean trie_lookup (Trie *trie, gchar *key)
{
	TrieNode *node;
	gchar *p;
	gint c;

	node = trie->root_node;
	p = key;

	while (*p != '\0')
	{
		if (node == NULL)
			return FALSE;

		// Advance to the next node in the chain, next character 
		c = *p;
		if (c < 0) return FALSE;
		node = node->next[c];
		++p;
	}
	if (node == NULL)
		return FALSE;
	return node->used;
}

