/*
 *@file inputsystemreader.hh
 *
 * this file contains the interface to the inputsystemreader class
 *
 * @author Garrit Jentsch
 *
 * @date Oct 13th, 2009 last edited 10-14-2009 by Garrit and Michael
 *
 */

#ifndef INPUTSYSTEMREADER_HH_
#define INPUTSYSTEMREADER_HH_

#include <fstream>
#include <iostream>

#include "../system/system.hh"

using namespace std;

namespace Hive
	{

	/**
	 * @brief abstract class for reading a system
	 *
	 * this class reads a system for agent setup
	 *
	 * @todo everything
	 *
	 * @bug
	 *
	 * @author Garrit Jentsch and Michael Sneddon
	 *
	 * @date Oct 13th, 2009 last edited: 12-15-2009 by garrit
	 *
	 *
	 */
	class InputSystemReader {
	public:
		/// default constructor
		InputSystemReader() {};

		/// destructor
		virtual ~InputSystemReader() {};

		/**
		 * parse inputfilestream and return the system
		 * @return parsed system
		 */
		virtual System* parseSystem(ifstream& in) {return sys;};

	protected:
		/// pointer to system
		System *sys;

	};
}
#endif /* INPUTSYSTEMREADER_HH_ */
