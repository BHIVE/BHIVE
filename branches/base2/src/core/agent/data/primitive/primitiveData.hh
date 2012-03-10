/**
 * @file primitiveData.hh
 *
 * this file contains the interfaces to the primitive Data classes, such as
 * Integer, Double, boolean, etc.
 *
 *
 * @date Oct 14, 2009  Last edited: Oct 23, 2009 Michael and Garrit
 *
 * @author Michael Sneddon
 */

#ifndef PRIMITIVEDATA_HPP_
#define PRIMITIVEDATA_HPP_


#include "../data.hh"

#include <iostream>
#include <vector>

using namespace std;


namespace Hive
{


	/**
	 * @brief A data object to store integer values
	 * @date Oct 14, 2009 Last edited: Oct 14, 2009 by Garrit and Michael
	 * @author Michael Sneddon
	 */
	class IntegerData : public Data {
		public:
			IntegerData(string dataName, int value);
			virtual ~IntegerData() {};

			void printContent(ostream &out);

			int getInteger() const { return integerValue; };
			void setInteger(int newValue) { integerValue = newValue; };
			void copyContentsInto(Data *destination) {
				((IntegerData *)destination)->setInteger(integerValue);
			}

		private:
			int integerValue;
	};


	/**
	 * @brief A data object to store double values
	 * @date Oct 14, 2009 Last edited: Oct 14, 2009 by Garrit and Michael
	 * @author Garrit Jentsch
	 */
	class DoubleData : public Data {
		public:
			/// default-constructor
			DoubleData(string dataName, double value);

			virtual ~DoubleData() {};

			/// virtual method for print the content of a data item on sterr.
			void printContent(ostream &out);

			/// adding to the stored value
			void addToValue(double d) { doubleValue += d; };

			/// subtracting from the stored value
			void subtractFromValue(double d) { doubleValue -= d; };

			double getDouble() const { return doubleValue; };
			void setDouble(double newValue) { doubleValue = newValue; };


			void copyContentsInto(Data *destination) {
				((DoubleData *)destination)->setDouble(doubleValue);
			}

		private:
				double doubleValue;

	};


	/**
	 * @brief A data object to store bool values
	 * @date Oct 14, 2009 Last edited: Oct 14, 2009 by Garrit and Michael
	 * @author Michael Sneddon
	 */
	class BoolData : public Data {
		public:
			BoolData(string dataName="b", bool value=false);
			virtual ~BoolData() {};

			void printContent(ostream &out);


			bool getBool() const { return boolValue; };
			void setBool(bool newValue) { boolValue = newValue; };

			void copyContentsInto(Data *destination) {
				((BoolData *)destination)->setBool(boolValue);
			}

		private:
			bool boolValue;
	};


	class StringData : public Data {
	public:
		StringData(string dataName, string value);
		virtual ~StringData() { };

		void printContent(ostream& out);

		string getString() const { return stringValue; };
		void   setString(string newString) { stringValue = newString; };
		void copyContentsInto(Data *destination) {
			((StringData*)destination)->setString(stringValue);
		};
		private:
			string stringValue;
	};



#endif /* PRIMITIVEDATA_HPP_ */
