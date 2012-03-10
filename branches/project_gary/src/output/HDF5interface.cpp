#include "HDF5interface.h"

hdf5handle::hdf5handle() {
	filehandle = NULL;
}

hdf5handle::~hdf5handle() {
	//If the filehandle is not null, we close the file
	if (filehandle != NULL) {
		close();
	}
}

int hdf5handle::open(string Filename) {
	//Creating the hdf5 file
	if (Filename.length() == 0) {
		cerr << "Cannot create HDF5 file. No filename provided." << endl;
		return 0;
	}
	
	//If the filehandle is not null, we close the file
	if (filehandle != NULL) {
		close();
	}

	//Opening file
	try {
		Exception::dontPrint();
		filehandle = new H5File();
		filehandle->openFile(Filename.data(),H5F_ACC_RDWR);
	} catch( FileIException error ) {
		delete filehandle;
		filehandle = new H5File(Filename.data(),H5F_ACC_TRUNC);
	}

	return 1;
}

int hdf5handle::close() {
	//If the filehandle is not null, we close the file
	if (filehandle != NULL) {
		filehandle->close();
	}
	delete filehandle;
	filehandle = NULL;
	return 1;
}

//This function creates three datasets and one group in the hdf5 file, and links all these things together into a table entity
int hdf5handle::create_data_table(string table_name,vector<string> column_names,int length,int depth,vector<string> attributes, vector<double> attribute_values) {
	//Checking if the table already exists
	if (check_for_dataset(table_name)) {
		cerr << "Cannot create new table because data table already exists." << endl;
		return 0;
	}
	
	//Checking that the file is open
	if (filehandle == NULL) {
		cerr << "Cannot create data table until the HDF5 file is open." << endl;
		return 0;
	}
	
	//First we set the number of entries in each column of the dataset
	hsize_t dim[] = {length};
    DataSpace space(1,dim);
	
	//Now we create the columns of the database: initially we're only allowing doubles
	CompType table_column_types( int(column_names.size())*sizeof(double) );
	for (int i=0; i < int(column_names.size()); i++) {
		table_column_types.insertMember( column_names[i].data(), i*sizeof(double), PredType::NATIVE_DOUBLE);
	}
	
	//Setting initial values for table data
	double* TableData = new double[length*int(column_names.size())];
	
	//Now we create the dataset
	DataSet* new_table = new DataSet(filehandle->createDataSet(table_name.data(), table_column_types, space));
	
	//Writing default values to dataset
	new_table->write(TableData,table_column_types);

	//Adding the attributes to the dataset
	for (int i=0; i < int(attributes.size()); i++) {
		dim[0] = 1;
		DataSpace attspace(1,dim);
		Attribute* myAttribute = new Attribute(new_table->createAttribute(attributes[i].data(),PredType::NATIVE_DOUBLE,attspace));
		myAttribute->write(PredType::NATIVE_DOUBLE,&attribute_values[i]);
	}

	//Now we store the dataset pointer in a map for easy access later
	delete [] TableData;
	delete new_table;

	return 1;
}

int hdf5handle::write_table_row(string table_name,vector<double> row_data, int row_index,int z_row_index) {
	//Checking that the file is open
	if (filehandle == NULL) {
		cerr << "Cannot create data table until the HDF5 file is open." << endl;
		return 0;
	}
	
	//First looking for the table
	DataSet destination_table;
	try {
		Exception::dontPrint();
		destination_table = filehandle->openDataSet(table_name.data());
	} catch( FileIException error ) {
		cerr << "Cannot find data table " << table_name << " in HDF5 file." << endl;
		return 0;
	}

	//Creating output vector
	double* temp_data = new double[row_data.size()];
	for (int i=0; i < int(row_data.size()); i++) {
		temp_data[i] = row_data[i];
	}
	
	//Setting up the source dataspace
	hsize_t dim[] = {1};
	DataSpace source_space(1,dim);
	hsize_t start[] = {0};
	hsize_t stride[] = {1};
	hsize_t count[] = {1};
	hsize_t block[] = {1};
	source_space.selectHyperslab( H5S_SELECT_SET, count, start, stride, block);

	//Setting up the destination dataspace
	DataSpace destination_space = destination_table.getSpace();
	start[0]  = row_index; stride[0] = 1; count[0]  = 1; block[0]  = 1;
	destination_space.selectHyperslab( H5S_SELECT_SET, count, start, stride, block);

	//Writing the data into the dataset
	destination_table.write( temp_data, destination_table.getDataType(), source_space, destination_space );
	
	//Cleaning up
	delete [] temp_data;

	return 1;
}

bool hdf5handle::check_for_dataset(string table_name) {
	try {
		Exception::dontPrint();
		DataSet destination_table = filehandle->openDataSet(table_name.data());
	} catch( FileIException error ) {
		return false;
	}
	return true;
}