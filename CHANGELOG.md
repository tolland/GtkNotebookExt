
v6-mar-11

Here's another version of gtkmultilinetab.c with (hopefully) free of bugs drawing procedure. Now
the tabs look more closely to what you would expect them to look. I've tested this version with a couple of themes and it seems to work fine.

gtkmultilinetab.c

v5-Mar-11

	I've fixed all found bugs so far. Geany seems to work OK.
The source files of GtkMultilineTab widget were updated to enable cooperation with a particular application (Geany in this case).

Patching procedure:
1) Download Geany's source (0.21), unpack the tarball
2) copy 6 attached files (gtkmultilinetab.c gtkmultilinetabnotebook.c gtkmultilinetab.h gtkmultilinetabnotebook.h main.patch interface.patch) into 'src' subdirectory
3) two files need to be patched (main.c and interface.c), from within 'src' directory do:
patch < main.patch
patch < interface.patch
4) and build as always (from the main directory of the source directory tree):
./configure
make
make install (with root privileges)

Let me know what you think about how it works for you.
main.patch
interface.patch
gtkmultilinetab.c
gtkmultilinetab.h
gtkmultilinetabnotebook.c
gtkmultilinetabnotebook.h


I have supplied patches (instead of simply new versions of the files) just to make clear what was actually changed in the Geany source files. These patches as well as GtkMultilineTab widget implementation are valid only for the current (source) version of the Geany. As with any patches, If something changes, these files will probably require modification.




v4-Mar-11

Here's the complex extended widget source code and a simple program which I've used for testing. You can now add the widget to a container just like any other.

Compiling the test program (along with the widget):
gcc -g -o notebook `pkg-config --cflags --libs gtk+-2.0` gtkmultilinetabnotebook.h gtkmultilinetabnotebook.c notebook_test.c
You can of course compile the widget source once and then link it to an app (while building an executable).

gtkmultilinetab.h
gtkmultilinetab.c
gtkmultilinetabnotebook.h
gtkmultilinetabnotebook.c
notebook_test.c

v3-Feb-11

	
I'm sorry, there is a solution to the problem with the child widget accommodating its size in a container (I've searched some useful Internet resources like Stackoverflow...). The remedy is quite simple (but not obvious to me): connect to the "size-allocate" signal after the default handler and just request a new size for the child widget (after its parent was resized).
The source code is attached.
update to gtkmultilinetab.c


v2-feb-11

Here is the implementation of my very first idea (simple yet functional):
1. First create a widget (derived from just GtkWidget) which takes a notebook object in the "constructor" and display the associated label in a way you require. There is a limitation however: the label can be only GtkLabel (and created manually, not by passing NULL to gtk_notebook_add_page() )
2. Wrap that widget with a second extended widget (derived from GtkVBox). This container would be a composite widget (but registered and used as any other) and contain the widget with labels and the original GtkNotebook

I can't fully implement the step (1) because of reason I've described before. Can't find a way for a child widget to determine its' parent dimension in the time the child size is requested. I tried connecting to the parent's "size-allocate" signal, but with no luck.

Anyway, you can now test it. Don't analyze the chaotic notebook2.c file, it's purpose was just ad hoc testing.

Compilation:

gcc -g -o notebook `pkg-config --cflags --libs gtk+-2.0` gtkmultilinetab.h gtkmultilinetab.c notebook2.c

I didn't implement step (2), but it should be relatively simple.
You can also just place the first (extended) widget in such container without the second extended widget.
gtkmultilinetab.h
gtkmultilinetab.c
notebook2.c





v1-Feb-11

The final idea was:
1. Create a container widget (derived from GtkContainer) that would be able to position the tab labels properly inside the container.
2. Override the expose method of the container to paint GTK+ style box extensions (the "tabs") around the child widgets in the container.
Note that in order to deal with complex label widgets it would also be needed to reparent a label widget (don't confuse with GtkLabel, because it may also not be that case) from the original GtkNotebook to our container. The container should also contain the original GtkNotebook (just as one of its children).
Currently I can't reparent a widget from GtkNotebook to my container widget (I called this GtkMultilineTab), still don't know why. Another issue: in present version of code I cannot obtain the actual size requisitions from the child widgets of the container (labels), though the widget s are realized.
gtkmultilinetab.h
gtkmultilinetab.c

