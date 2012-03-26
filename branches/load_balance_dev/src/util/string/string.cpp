/**
 * @file string.cpp
 *
 * Implementation of the string functions.
 *
 * @date Oct 14th, 2009   last edited: Oct 14th, 2009
 *
 * @author Michael Sneddon
 */

#include "string.hh"

using namespace std;
using namespace Util;

void Util::trim(std::string& str) {
	size_t startpos = str.find_first_not_of(" \t");
	size_t endpos = str.find_last_not_of(" \t");
	if(( string::npos == startpos ) || (string::npos == endpos)) {
		str="";
	} else {
		str = str.substr(startpos,endpos-startpos+1);
	}
}

void Util::StringToStrings(string FullString, const char* Delim, vector <string> &NewVect, bool TreatConsecutiveDelimAsOne) {

    string Buff(FullString);

    int Location;
    do {
	Location = int(Buff.find_first_of(Delim));
	if (Location != -1) {
	    if (Location == 0) {
		if (!TreatConsecutiveDelimAsOne) {
		    string NewString;
		    NewVect.push_back(NewString);
		}
		Buff = Buff.substr(Location+1, Buff.length()-(Location+1));
	    } else {
		string NewString = Buff.substr(0, Location);
		NewVect.push_back(NewString);
		Buff = Buff.substr(Location+1, Buff.length()-(Location+1));
	    }
	}
    } while(Location != -1);
	
    if (Buff.length() != 0 || !TreatConsecutiveDelimAsOne) {
		NewVect.push_back(Buff);
	}
	
}
