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
#include "version.h"

#define xstr(s) str(s)
#define str(s) #s

static const char *aboutme() {

	const char str[] = "<html><head><title>about:fifth</title></head><bopdy><center>"
				"<h3>Fifth "
#if GIT
				GITVERSION
#else
				VERSION
#endif
				", built on " __DATE__
				"<p>"
				"Running on WebkitFLTK " xstr(WK_FLTK_MAJOR) "."
				xstr(WK_FLTK_MINOR) "." xstr(WK_FLTK_PATCH)
				"</h3>"
				"</center></body></html>";

	return strdup(str);
}

static const char *aboutconfig() {

	string s = "<html><head><title>about:config</title>"
			"<script type=\"text/javascript\"></script>"
			"<style type=\"text/css\"></style></head><body>";

	s.reserve(16384);


	s += "<input type=\"text\" size=\"80\" placeholder=\"Filter...\"><hr>";

	s += "<table>\n";
	u32 i;
	for (i = 0; i < numDefaults; i++) {
		s += "<tr class=\"";
		s += defaultSettings[i].name;
		s += "\"><td class=\"name\">";
		s += defaultSettings[i].name;
		s += "</td><td class=\"type\">";

		switch (defaultSettings[i].type) {
			case ST_CHAR:
				s += "char";
			break;
			case ST_FLOAT:
				s += "float";
			break;
			case ST_U32:
				s += "unsigned";
			break;
			case ST_COUNT:
				die("Setting corruption\n");
		}

		s += "</td><td class=\"input\">"
			"<input type=\"text\" size=\"40\" value=\"";
		const setting * const set = getSetting(defaultSettings[i].name);

		char tmp[16];
		switch (set->type) {
			case ST_CHAR:
				s += set->val.c;
			break;
			case ST_FLOAT:
				snprintf(tmp, 16, "%f", set->val.f);
				s += tmp;
			break;
			case ST_U32:
				snprintf(tmp, 16, "%u", set->val.u);
				s += tmp;
			break;
			case ST_COUNT:
				die("Setting corruption\n");
		}

		s += "\"><input type=\"button\" value=\"Save\" name=\"save\"></td></tr>\n";
	}
	s += "</table>";

	s += "</body></html>";

	return strdup(s.c_str());
}

const char *aboutpage(const char * const page) {

	#define is(a) if (!strcmp(page, a))
	is ("fifth") {
		return aboutme();
	} else is ("config") {
		return aboutconfig();
	}
	#undef is

	return NULL;
}