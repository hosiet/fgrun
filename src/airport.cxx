// airport.cxx -- Airport search and update functions.
//
// Written by Bernie Bright, started Dec 2002.
//
// Copyright (c) 2002  Bernie Bright - bbright@bigpond.net.au
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation; either version 2 of the
// License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
// $Id$

#ifdef _MSC_VER
# pragma warning(disable: 4786)
#endif

#include <deque>
#include <vector>
#include <string>
#include <algorithm>

#include <FL/Fl.h>
#include <FL/filename.h>

#include "UserInterface.h"

using std::deque;
using std::string;
using std::vector;

/**
 * FIFO queue of directory names in which to search for airport files.
 */
static deque< string > apt_dirs;

/**
 * Array of (sorted) airport names.
 */
static vector< string > airports;

/**
 * 
 */
void
search_for_airports_cb( void* v )
{
    string dir = apt_dirs.front();
    dirent** files;
    int n = fl_filename_list( dir.c_str(), &files, fl_numericsort );
    if (n > 0)
    {
	for (int i = 0; i < n; ++i)
	{
	    if (fl_filename_match( files[i]->d_name,
				   "[ew][0-9][0-9][0-9][ns][0-9][0-9]*")) {
		// Found a scenery sub-directory.
		string d = dir;
		d += "/";
		d += files[i]->d_name;
		if (fl_filename_isdir( d.c_str() )) {
		    apt_dirs.push_back( d );
		}
	    }
	    else if (fl_filename_match( files[i]->d_name, "???.btg.gz") ||
		     fl_filename_match( files[i]->d_name, "????.btg.gz"))
	    {
		char* p = strstr( files[i]->d_name, ".btg" );
		if (p != 0)
		    *p = 0;
		airports.push_back( string( files[i]->d_name ) );
	    }

	    free( files[i] );
	}

	free( files );
    }

    apt_dirs.pop_front();
    if (apt_dirs.empty())
    {
	Fl::remove_idle( search_for_airports_cb, v );
	if (!airports.empty())
	{
	    UserInterface* ui = (UserInterface*)v;
	    ui->airport->clear();
	    std::sort( airports.begin(), airports.end() );
	    typedef vector<string> StringVec;
	    StringVec::iterator i = airports.begin();
	    StringVec::iterator end = airports.end();
	    int index = 0;
	    for (; i != end; ++i)
	    {
		index = ui->airport->add( i->c_str() );
		if (ui->default_airport == *i)
		    ui->airport->value(index);
	    }
	    ui->airport->activate();
	}
    }
}

/**
 * 
 */
void
UserInterface::update_airports_cb()
{
    if (fg_scenery->size() > 0
	&& fl_filename_isdir(fg_scenery->value()))
    {
	airports.clear();
	string dir = fg_scenery->value();
	dir += "/";
	apt_dirs.push_back( dir );
	Fl::add_idle( search_for_airports_cb, this );
    }
}