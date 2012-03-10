/*
 * lgca_data.hh
 *
 *  Created on: 20/10/2009
 *      Author: ric
 */

#ifndef LGCA_DATA_HH_
#define LGCA_DATA_HH_

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "../../agent/data/data.hh"

using namespace std;

namespace Hive {




class GLCAData : public Data {
public:

	/**
	 * constructor
	 * @param s typename
	 */
	GLCAData(string name, int size);

	/// Destructor
	~GLCAData() {};

	/// virtual method for print the content of a data item on sterr.
	void printContent(ostream& out);

	/**
	 * returns type name of data
	 * @return typename
	 */
	string getType() const { return datatype; };

	int getValueAt(int x, int y);

	void setValueAt(int x, int y, int value);

	int getSize();

protected:
	int **array;;

	int size;

	/// name of type of data
	string datatype;
};

}

#endif /* LGCA_DATA_HH_ */
