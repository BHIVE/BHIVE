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
			IntegerData(string dataName="noname", int value=0);
			virtual ~IntegerData() {};

			void printContent(ostream &out);
			void printValue(ostream& out);

			int getInteger() const { return integerValue; };
			void setInteger(int newValue) { integerValue = newValue; };
			void packDataForMPI(char*& buffer,int& size);
			void unpackDataForMPI(char* buffer,int size);
			void initializeFromString(const string s);
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
			DoubleData(string dataName="noname", double value=0.0);

			virtual ~DoubleData() {};

			/// virtual method for print the content of a data item on sterr.
			void printContent(ostream &out);
			void printValue(ostream& out);

			/// adding to the stored value
			void addToValue(double d) { doubleValue += d; };

			/// subtracting from the stored value
			void subtractFromValue(double d) { doubleValue -= d; };

			double getDouble() const { return doubleValue; };
			void setDouble(double newValue) { doubleValue = newValue; };
			void packDataForMPI(char*& buffer,int& size);
			void unpackDataForMPI(char* buffer,int size);
			void initializeFromString(const string s);
		private:
				double doubleValue;

	};

	/// Never use a double_ptr!!!!
	class DoublePtrData : public Data {
	public:
		/// default-constructor
		DoublePtrData(string dataName, double *ptr);
		virtual ~DoublePtrData() {};
		void printContent(ostream& out);
		double *getDoublePtr() { return double_ptr; };
		void setDoublePtr(double *new_ptr) { double_ptr = new_ptr; };
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
			BoolData(string dataName="b", bool value=false);
			virtual ~BoolData() {};

			void printContent(ostream &out);
			void printValue(ostream& out);
			bool getBool() const { return boolValue; };
			void setBool(bool newValue) { boolValue = newValue; };
			void packDataForMPI(char*& buffer,int& size);
			void unpackDataForMPI(char* buffer,int size);
			void initializeFromString(const string s);
		private:
			bool boolValue;
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
		void packDataForMPI(char*& buffer,int& size);
		void unpackDataForMPI(char* buffer,int size);
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
		void packDataForMPI(char*& buffer,int& size);
		void unpackDataForMPI(char* buffer,int size);
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
		void packDataForMPI(char*& buffer,int& size);
		void unpackDataForMPI(char* buffer,int size);
	private:
		vector<vector<double> > mat;

	};


	class StringData : public Data {
	public:
		StringData(string dataName = "noname", string value = "");
		virtual ~StringData() { };

		void printContent(ostream& out);
		void printValue(ostream& out);

		string getString() const { return stringValue; };
		void   setString(string newString) { stringValue = newString; };
		void copyContentsInto(Data *destination) {
			((StringData*)destination)->setString(stringValue);
		};
		void initializeFromString(const string s);
		private:
			string stringValue;
	};


	class RawBufferData : public Data {
	public:
		RawBufferData(string dataName, char *buffer, int size);
		virtual ~RawBufferData() {
			// delete buffer;
			buffer = NULL;};

		int getSize() { return this->size; };
		char* getBuffer() { return this->buffer; };
		void setSize(int s) { this->size = s; };
		void setBuffer(char *buf) { this->buffer = buf; };
		void printContent(ostream &out) {out << buffer << endl;};
		void printValue(ostream& out) {out<<"'"<<buffer<<"'";};
	private:
		int size;
		char *buffer;

	};
}


#endif /* PRIMITIVEDATA_HPP_ */
