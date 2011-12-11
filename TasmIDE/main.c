#include "Editor.h"

#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")

int main (int argc, char *argv[])
{
	gtk_init (&argc, &argv);
	
	create_editor_window ();

	if (argc == 2)
	{
		wnd.filename = *(argv + 1);
		load_document ();
	}

	gtk_main ();
	return 0;
}