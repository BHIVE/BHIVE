/**
 * @file string.hh
 *
 * A collection of string functions that have been written for the hive.
 *
 * @date Oct 14th, 2009   last edited: Oct 14th, 2009
 *
 * @author Michael Sneddon
 */

#ifndef STRING_HH_
#define STRING_HH_

#include <string>

using namespace std;

namespace Util {


	//! Removes leading and trailing whitespace (including tabs) from the string
	/*!
	 * Note that this function takes arguements by reference, so that it trims
	 * the string you give it.  If you want to trim a copy of your string, you
	 * will have to make the copy yourself.
	 * @param str the string to act on (pass by reference)
	 * @author Michael Sneddon
	 */
	void trim(std::string& str);


};


#endif /* STRING_HH_ */
