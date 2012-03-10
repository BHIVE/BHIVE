#ifndef HDF5_HANDLE_
#define HDF5_HANDLE_

#include <iostream>
#include <string>
#include <map>
#include <vector>

using namespace std;
using namespace Hive;

#include "../util/hdf5-1.6.10/c++/src/H5Cpp.h"

class hdf5handle {
public:
    hdf5handle();
    ~hdf5handle();
	
	//Opens a new or existing HDF5 file and returns 0 for failure, and 1 for success
	int open(string Filename);
	//Closes the HDF5 file and returns 1 for success, 0 for failure
	int close();

	int create_data_table(string table_name,vector<string> column_names,int length,int depth,vector<string> attributes, vector<double> attribute_values);
	//table_name: name of the table the row should be added to, row_data: vector of data, row_index: y dimension row in table, z_row_index: z dimension row in table
	int write_table_row(string table_name,vector<double> row_data, int row_index,int z_row_index);
	bool check_for_dataset(string table_name);
private:
    H5File* filehandle;
};

#endif
