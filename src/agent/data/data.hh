/**
 * @file data.hh
 *
 * this file contains the interfaces to the classes Data, ...
 *
 *
 * @date Oct 7, 2009  Last edited: Oct 14, 2009 by Garrit and Michael
 *
 * @author Garrit Jentsch
 */

#ifndef DATA_HPP_
#define DATA_HPP_

#include <string>

using namespace std;


namespace Hive
{

	/**
	 * @brief individual element of the database
	 *
	 *  abstract class data. all data items stored in the database will have to comply with
	 *  this interface.
	 *
	 *  @todo
	 *
	 *  @date Oct 7, 2009 Last edited: Oct 14, 2009 by Garrit and Michael
	 *
	 *  @author Garrit Jentsch
	 */
	class Data {
	public:

		/**
		 * constructor
		 * @param s typename
		 */
		Data(string name, string type) {this->dataName=name; this->datatype=type;};

		/// Destructor
		virtual ~Data() {};

		/// virtual method for print the content of a data item on sterr.
		virtual void printContent(ostream& out) = 0;

		/**
		 * returns type name of data
		 * @return typename
		 */
		string getType() const { return datatype; };

		/**
		* returns type name of data
		* @return typename
		*/
		string getName() const { return dataName; };

	protected:
		/// name of type of data
		string dataName;
		string datatype;
	};
}

#endif /* DATA_HPP_ */
