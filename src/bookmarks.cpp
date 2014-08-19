/*
Copyright (C) 2014 Lauri Kasanen

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, version 3 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "main.h"
#include <FL/Fl_Return_Button.H>
#include <FL/Fl_Choice.H>

#define BOOKMARKFILE "bookmarks"

void loadbookmarks() {

	g->bookmarks.clear();

	const int fd = openat(g->profilefd, BOOKMARKFILE, O_RDONLY);
	if (fd < 0)
		return;
	FILE * const f = fdopen(fd, "r");
	if (!f)
		return;

	enum {
		BUFSIZE = 640
	};

	char tmp[BUFSIZE];
	bookmark mark = { NULL, NULL };
	while (fgets(tmp, BUFSIZE, f)) {
		nukenewline(tmp);

		if (strlen(tmp) < 2)
			continue;

		if (!strncmp(tmp, "name", 4)) {
			if (mark.name || mark.url)
				die("Corrupted bookmark file, line '%s'\n", tmp);

			char *ptr = tmp + 5;
			mark.name = NULL;
			if (strlen(ptr) > 1)
				mark.name = strdup(ptr);
		} else if (!strncmp(tmp, "url", 3)) {
			char *ptr = tmp + 4;
			mark.url = NULL;
			if (strlen(ptr) > 1)
				mark.url = strdup(ptr);

			g->bookmarks.push_back(mark);

			mark.name = NULL;
			mark.url = NULL;
		} else {
			die("Unknown bookmark line '%s'\n", tmp);
		}
	}

	fclose(f);
}

void savebookmarks() {

	// Backups
	renameat(g->profilefd, BOOKMARKFILE ".bak2",
			g->profilefd, BOOKMARKFILE ".bak3");
	renameat(g->profilefd, BOOKMARKFILE ".bak",
			g->profilefd, BOOKMARKFILE ".bak2");
	renameat(g->profilefd, BOOKMARKFILE,
			g->profilefd, BOOKMARKFILE ".bak");

	const int fd = openat(g->profilefd, BOOKMARKFILE, O_WRONLY);
	if (fd < 0)
		die("Failed saving bookmarks\n");
	FILE * const f = fdopen(fd, "w");
	if (!f)
		die("Failed saving bookmarks\n");

	u32 i;
	const u32 max = g->bookmarks.size();
	for (i = 0; i < max; i++) {
		fprintf(f, "name %s\n", g->bookmarks[i].name ? g->bookmarks[i].name : "");
		fprintf(f, "url %s\n\n", g->bookmarks[i].url ? g->bookmarks[i].url : "");
	}

	fflush(f);
	fsync(fd);
	fclose(f);

}

// Autogenerated by fluid.
static Fl_Double_Window *addwin=(Fl_Double_Window *)0;

static void cb_Cancel(Fl_Button*, void*) {
	addwin->hide();
}

static Fl_Input *name=(Fl_Input *)0;

static Fl_Input *url=(Fl_Input *)0;

static Fl_Choice *dir=(Fl_Choice *)0;

static void doadd() {
	if (url->size() < 2 || name->size() < 2)
		return;

	bookmark b;
	b.name = strdup(name->value());
	b.url = strdup(url->value());

	// Which dir did they want?
	const u32 pos = (u32) (uintptr_t) dir->menu()[dir->value()].user_data();
	if (!pos) {
		// Just add to the end.
		g->bookmarks.push_back(b);
	} else {
		g->bookmarks.insert(g->bookmarks.begin() + pos, b);
	}

	generatemenu();
	savebookmarks();
}

static void cb_OK(Fl_Return_Button*, void*) {
	doadd();
	addwin->hide();
}

void addbookmark() {
	const tab * const cur = &g->tabs[g->curtab];
	if (cur->state != TS_WEB)
		return;

	if (!addwin) {
		addwin = new Fl_Double_Window(480, 190, _("Add bookmark"));
		{ Fl_Button* o = new Fl_Button(270, 145, 85, 25, _("Cancel"));
			o->callback((Fl_Callback*)cb_Cancel);
		} // Fl_Button* o
		{ name = new Fl_Input(125, 20, 330, 25, _("Name:"));
		} // Fl_Input* name
		{ url = new Fl_Input(125, 55, 330, 25, _("Address:"));
		} // Fl_Input* url
		{ dir = new Fl_Choice(125, 90, 330, 25, _("Into directory:"));
			dir->down_box(FL_BORDER_BOX);
		} // Fl_Choice* dir
		{ Fl_Return_Button* o = new Fl_Return_Button(145, 145, 85, 25, _("OK"));
			o->callback((Fl_Callback*)cb_OK);
		} // Fl_Return_Button* o
		addwin->end();
	} // Fl_Double_Window* addwin

	// Set things
	name->value(cur->title());
	url->value(cur->url);
	name->maximum_size(630);
	url->maximum_size(630);

	// Fill out the directories
	dir->clear();
	dir->add(_("Bookmarks"), 0, 0);

	u32 i;
	const u32 max = g->bookmarks.size();
	u32 depth = 0;
	for (i = 0; i < max; i++) {
		const bookmark &cur = g->bookmarks[i];
		if (!cur.name) {
			depth--;
		} else if (!cur.url) {
			depth++;

			u32 pos = i + 1;
			while (g->bookmarks[pos].name || g->bookmarks[pos].url)
				pos++;

			string tmp;
			u32 d;
			for (d = 0; d < depth; d++) {
				tmp += "    ";
			}
			tmp += cur.name;
			dir->add(tmp.c_str(), 0, 0, (void *) (uintptr_t) pos);
		}
	}
	dir->value(0);

	addwin->show();
	name->take_focus();
	name->position(0, name->size());
}
