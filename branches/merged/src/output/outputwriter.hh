/**
 * @file outputwriter.hh
 *
 * this file contains the interface for the outputwriter.
 *
 * @author: Garrit Jentsch
 *
 * @date Oct 13, 2009 last edited Oct 13, 2009 by Garrit
 *
 */

#ifndef OUTPUTWRITER_HH_
#define OUTPUTWRITER_HH_

#include "../agent/database/database.hh"

#include <iostream>
#include <fstream>

using namespace std;


namespace Hive
{

	/**
	 * @brief abstract class outputwriter
	 *
	 * an outputwriter is responsible for storing a database on disk
	 *
	 * @todo Everything
	 *
	 * @bug it is not a bug it's a feature.
	 *
	 * @author Garrit Jentsch
	 *
	 * @date Oct 13, 2009 last edited: Oct 13, 2009
	 */

	class Database;

	class OutputWriter {
	public:
		/// constructor
		OutputWriter();

		/// destructor
		~OutputWriter();

		/**
		 * dumb database
		 */
		void dumbDatabase();

		/**
		 * give database to outputwriter
		 * @param db pointer to database
		 */
		void setDatabase(Database *db);

		/**
		 * give outputfile to outputwriter
		 * @param output output filestream
		 */
		void setOutputFileStream(ofstream& output);

	protected:
		/**
		 * outputs items of the database
		 */
		void outputNextItem();

	private:
		/// database which should be dumbed
		Database *db;

		/// output filestream
		ofstream& output;
	};
}

#endif /* OUTPUTWRITER_HH_ */
