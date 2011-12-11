#include "build.h"
#include "callback.h"

void build (gboolean run)
{
	gchar *command, *outputfile, *exefile, *tmp;
	gchar **argv;
	gchar *output = NULL, *text;
	if (!on_file_save ())
	{
		outputfile = g_strdup (wnd.filename);
		tmp = outputfile;
		tmp = g_strrstr (tmp, "asm");
		g_strlcpy (tmp, "obj", 4);

		command = g_strjoin ("*", wnd.compilerpath, wnd.filename, outputfile, NULL);
		argv = g_strsplit (command, "*", 0);
		g_spawn_sync (NULL, argv, NULL, G_SPAWN_STDERR_TO_DEV_NULL, NULL, NULL,
		              &output, NULL, NULL, NULL); 
		text = g_strdup (output);
		g_free (output);
		g_free (command);

		exefile = g_strdup (wnd.filename);
		tmp = exefile;
		tmp = g_strrstr (tmp, "asm");
		g_strlcpy (tmp, "exe", 4);
		command = g_strjoin ("&", wnd.linkerpath, outputfile, exefile, NULL);
		argv = g_strsplit (command, "&", 0);
		g_spawn_sync (NULL, argv, NULL, G_SPAWN_STDERR_TO_DEV_NULL, NULL, NULL,
		              &output, NULL, NULL, NULL); 
		text = g_strjoin ("\n", text, output, NULL);
		g_free (output);
		g_free (command);

		run_window_with_text (text);
		g_free (text);

		if (run)
			g_spawn_async (NULL, argv+2, NULL, 0, NULL, NULL, NULL, NULL); 

		g_free (outputfile);
		g_free (exefile);
		g_strfreev (argv);
	}
}