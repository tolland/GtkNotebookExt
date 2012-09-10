# GtkMultilineTabNotebook

GtkMultilineTabNotebook is a gtk widget which is supposed to replace the standard GtkNotebook widget with one that supports multiple rows of Tabs for selecting the active sheet with tabs.

The primary motivation is that I find it annoying to use any of the gtk based text editors such as gedit, geany, bluefish and their derivatives, because you either have to give up some content area real estate to have a file selector pane, or you have to scroll horizontally the tab display area.

A gallery of some of the initial prototypes is available here;
https://www.dropbox.com/sh/owe7z9pe5rq4jcs/X5t7ZKeFp0

![prototype image](https://www.dropbox.com/sh/owe7z9pe5rq4jcs/V7fR_gL4xL/IMG_04092012_130424.png)


This is an example of something like what I am trying to achieve;

![Example multitabs](http://i.imgur.com/8rxLA.png)


![jedit tabbed editor](http://www.xhbml.com/grafikk/jedittasklistscreenshot.jpg)

jEdit uses the buffer tabs plugin;

http://sourceforge.net/projects/jedit-plugins/files/BufferTabs/1.2.2/BufferTabs-1.2.2.zip/download




## Status

Nothing that is useful at the moment. The code in repo is just that which is being used for prototyping. If you download and try to run it, and your house falls down, don't blame me. I told you not to run it ;-)

    yum install fortune-mod





## Platform support

is being tested on a fedora 15 desktop, but should run on anything with gtk2 libs




@Todo - look at;

[tomh@workstation001 ~]$ rpm -qil gtkparasite
Name        : gtkparasite
URL         : http://chipx86.github.com/gtkparasite/
Summary     : A GUI debugging tool for GTK+ applications
Description :
Parasite is a debugging and development tool that runs inside your GTK+




