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

		/// virtual method to print the value of the data, useful for debugging
		virtual void printValue(ostream& out) { out<<"[cannot_print_data_value]"; };

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

		virtual void packDataForMPI(char*& buffer,int& size) {buffer = NULL; size = 0;};
		virtual void unpackDataForMPI(char* buffer,int size) {};

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




		/**
		 * This method is used by the dataParser to allow a string to set the value of this data item.  The format of
		 * the string is specific to the data item (as initialization depends on the type of data it is), but generally
		 * it just indicates a number.  See basic implementations of this method for the primitive data items.  This
		 * method should throw an exception if the string cannot be parsed.
		 * @param string string containing the information which should allow parsing this data item
		 * @see core/input/dataParser/dataParser.hh
		 * @see core/agent/data/primitive/primitiveData.hh
		 */
		virtual void initializeFromString(const string s) {
			throw(HiveException("Cannot initializeFromString(string) for this data item (name:"+this->getName()+"' type: '"+this->getType()+"'), because initializeFromString is not implemented for this Data",
							"Data::initializeFromString(Data *)"));
		}

	protected:
		/// name of type of data
		string dataName;
		string datatype;
	};
}

#endif /* DATA_HPP_ */
