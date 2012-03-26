/**
 * @file inputdatareader.hh
 *
 * this file contains the interface for the class inputreader
 *
 * @author Garrit Jentsch
 *
 * @date Oct 13, 2009 last edited: Oct 13, 2009 by Garrit
 *
 */

#ifndef INPUTDATAREADER_HH_
#define INPUTDATAREADER_HH_

#include <fstream>
#include <iostream>

#include "../../agent/data/data.hh"

using namespace std;

namespace Hive
{

	/**
	 *  @brief reads input from filestream
	 *
	 *  by introducing an input data reader class we separate the storage of a database from
	 *  its use.
	 *
	 *  @todo Everything
	 *
	 *  @bug unknown as of yet
	 *
	 *  @author Garrit Jentsch
	 *
	 *  @date Oct 13, 2009 last edited: Oct 13, 2009 by Garrit
	 *
	 */

	class InputDataReader {
	public:
		/// constructor
		InputDataReader();

		/// destructor
		~InputDataReader();

		/**
		 * read data from file
		 * @param input input file stream
		 */
		void read(ifstream& input);

	protected:
		/**
		 * reads the next element from the input file-stream
		 * @return pointer to data element
		 */
		Data* getNextDataItem();

	private:
		/// input file stream
		ifstream& input;
	};

}

#endif /* INPUTDATAREADER_HH_ */
