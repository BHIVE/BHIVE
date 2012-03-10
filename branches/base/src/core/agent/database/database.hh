/**
 * @file  database.hh
 *
 * This file contains the specification for the Database class.
 *
 *
 * @author Garrit Jentsch
 *
 * @date Started: Oct 7th, 2009   Last edited: Oct 14th, 2009 by Garrit and Michael
 */

#ifndef _database_hh
#define _database_hh

#include <map>
#include <vector>

#include "../data/data.hh"
// #include "../../input/dataParser/inputdatareader.hh"

using namespace std;


namespace Hive
{

	/**
	 * @brief Database of an agent
	 *
	 * The database of the agent class stores all of the agent's data.
	 * This is the most radical way for a generic database in that a
	 * database is a vector of void pointers.
	 *
	 * @todo Implementation, deleting items from database
	 *
	 *
	 * @date Started: Oct 7th, 2009 Last edited: Oct 14th, 2009 by Garrit and Michael
	 */
	class Database {
		public:

			/// Constructor
			Database();

			/// Destructor
			~Database();

			/** outputs the entire database
			 * @param ow Pointer to the OutputWriter that outputs the database
			 */
			void setup();

			/**
			 * removes all data from the database. after calling real_database has a size of zero.
			 */
			void clearData();

			/** print database contents to the ostream
			* @param ostream & reference to the outputstream
			*/
			void printDataBase(ostream &o);


			/** adds data to the database
			 * @param name Name of the variable to be added
			 * @param ptr Pointer to the data to be added
			 */
			void addData(string name, Data *data);

			void addData(Data *d);

			/** method for checking whether a data item exists in the
			 *  database
			 *  @param name Name of the variable to check
			*/
			bool doesDataItemExist(string name);


			/** removes data item form the database
			 * @param name Name of data item to be removed.
			 */
			void eraseDataItem(string name);

			/**
			 * returns a data item
			 * @param name identifyer of data item
			 * @return pointer to data item
			 */
			Data* getDataItem(string name);

			/** set up a database by using an input reader
			 * @param ir Pointer to an InputReader
			 */
//			void readDataBase(InputDataReader *ir);

			/** returns pointer to data item
			 *  @param name Name of variable that one wishes to obtain a pointer to.
			 *  @param ptr Pointer to the desired variable.
			 */
			void returnPointerToData(string name, Data *&ptr);

			/** ask for existence of a data_item
			 * @param name name of dataitem
			 * @return bool existence
			 */
			bool existsDataItem(string name);

			/** ask for existence of a data type
			 * @param type type of dataitem
			 * @return bool existence
			 */
			bool existsDataType(string type);


			/** Get the number of Data items inserted into the database.
			 *
			 * @return unsigned int number of elements
			 */
			unsigned int size() const { return real_database.size(); };


			/** Get the Data item by index
			 *
			 * @param i index of data item
			 * @return Data * pointer to data object at the position
			 */
			Data * at(int i) { return real_database.at(i); };




		protected:
			/// the database is really nothing else but a vector of pointers to data objects
			vector <Data *> real_database;

			/// map to give a name to the data items stored in the database
			map <string, int> data_position_identifyer;
	};
}

#endif // _database_hh

