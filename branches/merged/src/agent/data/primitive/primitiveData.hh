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

	/// bullshit!
	class DoublePtrData : public Data {
	public:
		/// default-constructor
		DoublePtrData(string dataName, double *ptr);

		virtual ~DoublePtrData() {};

		void printContent(ostream& out);

		double *getDoublePtr() { return double_ptr; };
		void setDoublePtr(double *new_ptr) { double_ptr = new_ptr; };

		void copyContentsInto(Data *destination) {
			throw(HiveException("You are trying to use DoublePtrData? why? ",
						"DoublePtrData::copyContents(Data *)"));
		}

	private:
		double *double_ptr;
	};

	/**
	 * @brief A data object to store bool values
	 * @date Oct 14, 2009 Last edited: Oct 14, 2009 by Garrit and Michael
	 * @author Michael Sneddon
	 */
	class BoolData : public Data {
		public:
			BoolData(string dataName, bool value);
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

	/**
	 * @brief A data object to store a simple int vector
	 * @date Oct, 21, 2009
	 * @author garrit jentsch
	 */
	class IntVectorData : public Data {
	public:
		/// default constructor
		IntVectorData(string dataName, int number_of_elements);

		virtual ~IntVectorData() {};

		void printContent(ostream &out);

		int getInt(int pos) const { return vec[pos]; };
		void setInt(int new_value, int pos) { vec[pos] = new_value; };
		void setNumElements(int num_ele) { vec.resize(num_ele); };
		int getNumElements() { return int(vec.size()); };
		void addElementToEnd(int new_value) { vec.push_back(new_value); };

		void copyContentsInto(Data *destination) {
			IntVectorData * dest = ((IntVectorData *)destination);
			dest->vec.clear();
			for(unsigned int k=0; k<vec.size(); k++)
				dest->addElementToEnd(vec.at(k));
		}

	private:
		vector<int> vec;
	};


	/**
	 * @brief A data object to store a double vector
	 * @date 10-22-09
	 * @author garrit
	 */
	class DoubleVectorData : public Data {
	public:
		/// default constructor
		DoubleVectorData(string dataName, int number_of_elements);

		virtual ~DoubleVectorData() {};

		void printContent(ostream &out);

		double getDouble(int pos) const { return vec[pos];};
		void setDouble(double new_value, int pos) { vec[pos] = new_value; };
		void setNumElements(int num_ele) { vec.resize(num_ele); };
		int getNumElements() { return int(vec.size()); };
		void addElementToEnd(double new_value) { vec.push_back(new_value); };

		void copyContentsInto(Data *destination) {
			DoubleVectorData * dest = ((DoubleVectorData *)destination);
			dest->vec.clear();
			for(unsigned int k=0; k<vec.size(); k++)
				dest->addElementToEnd(vec.at(k));
		}

	private:
		vector<double> vec;
	};


	/**
	 * @brief a simple matrix implementation with doubles
	 *
	 * @date 10-22-09
	 * @author garrit
	 */
	class DoubleMatrixData : public Data {
	public:
		DoubleMatrixData(string dataName, int xdim, int ydim);

		virtual ~DoubleMatrixData() {};

		void printContent(ostream &out);

		double getDouble(int xpos, int ypos) const { return mat[xpos][ypos]; };
		int getSizeXDim() { return int(mat.size()); };
		void setDouble(double newValue, int xpos, int ypos) { mat[xpos][ypos] = newValue; };
		void setDimensions(int xdim, int ydim);

		void copyContentsInto(Data *destination) {
			throw(HiveException("You are trying to use DoubleMatrixData? why?  implement this method yourself.",
							"DoubleMatrixData::copyContents(Data *)"));
		}


	private:
		vector<vector<double> > mat;

	};
}


#endif /* PRIMITIVEDATA_HPP_ */
