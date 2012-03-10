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
#include "../../exception/exception.hh"

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
	 *    LOOK CHRIS LOOK CHRIS LOOK CHRIS!
	 *    we would have to add a method for casting this into a char vector
	 *    also a method for extracting the data from a char vector
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
		virtual void printContent(ostream& out)=0; //  { out << this->dataName << " " << this->datatype << endl; } ;

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


		// we would have to add a method for casting this into a char vector

		// also a method for extracting the data from a char vector


		/**
		 * Given the destination, this data item will take its contents and copy it into
		 * the destination.  This requires the Data items to be of the same type.  If this
		 * function is not implemented for a particular data item, it will fail.  Note also
		 * that for template objects, this may fail if the objects in the vector are
		 * pointers as this method will copy the pointer, but not necessarily the underlying
		 * object that is being pointed to!
		 * @param destination Data to copy the contents onto
		 */
		virtual void copyContentsInto(Data *destination) {
			throw(HiveException("Cannot copy data item, because copyContents is not implemented for this Data",
				"Data::copyContents(Data *)"));
		}

	protected:
		/// name of type of data
		string dataName;
		string datatype;
	};
}

#endif /* DATA_HPP_ */
