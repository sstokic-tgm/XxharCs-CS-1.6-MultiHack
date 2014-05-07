// 
// Online Game Cheats Client.dll hook
// Copyright (c) system   2001-2002
// Copyright (c) bunny771 2001-2002
// 
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
// 
// NOTE:
// GNU license doesn't cover Engine directory.
// Content of Engine directory is copyrighted (c) 1999, 2000, by Valve LLC
// and it is licensed under Valve's proprietary license (see original HL SDK).
//
#include <map>
#include <string.h>
#include <string>
#include <assert.h>
#include <list>
#include "stringfinder.h"

using namespace std;

//========================================================================================
typedef map<string, int, less<string> > MapStringInt;
struct Private_Data
{
	MapStringInt theMap;
	MapStringInt::iterator pos;

};
#define THE_MAP ( (*((Private_Data*)data)).theMap)
#define THE_POS ( (*((Private_Data*)data)).pos)


//========================================================================================
StringFinder::StringFinder()
{
	data = new Private_Data;
}

//========================================================================================
StringFinder::~StringFinder()
{
	delete (Private_Data*)data;
}

//========================================================================================
void StringFinder::clear()
{
	THE_MAP.clear();
}

//========================================================================================
void StringFinder::add(const char* str, int num)
{
	if(!str || !*str){ return;}
    typedef MapStringInt::value_type Entry;
    THE_MAP.insert(Entry(str,num));
}

//========================================================================================
void StringFinder::erase( const char* str )
{
	MapStringInt::iterator foundPos = THE_MAP.find( std::string(str) );
	if(foundPos!=THE_MAP.end()) 
	{ 
	    THE_MAP.erase(foundPos);
	}
}

//========================================================================================
bool StringFinder::find(const char* str)
{
	if(!str || !*str){ return false; }

	MapStringInt::iterator foundPos = THE_MAP.find( std::string(str) );
	
	if(foundPos==THE_MAP.end()) 
	{ 
		return false;  
	}
	else 
	{
		num = (*foundPos).second;
		return true; 
	}
}

//========================================================================================
void StringFinder::it_start()
{
	THE_POS = THE_MAP.begin();
	num = THE_POS->second;
	str = THE_POS->first.c_str();
}

//========================================================================================
bool StringFinder::it_running()
{
	return (THE_POS!=THE_MAP.end());
}

//========================================================================================
void StringFinder::it_next()
{
	++THE_POS;
	num = THE_POS->second;
	str = THE_POS->first.c_str();
}