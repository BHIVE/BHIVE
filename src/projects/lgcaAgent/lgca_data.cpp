/*
 * lgca_data.cpp
 *
 *  Created on: 20/10/2009
 *      Author: ric
 */

#include "lgca_data.hh"

using namespace Hive;

   GLCAData::GLCAData(string name, int size):Data(name,"GLCA"){
	this->size = size;
	array = new int*[size];
	for(int i=0; i<size; i++){
		array[i] = new int[size];
	}

	for(int i=0; i<size; i++){
		for(int j=0; j<size; j++){
			array[i][j]=i*j;
		}
	}
}

void GLCAData::printContent(ostream& out) {
	for(int i=0; i<size; i++){
		for(int j=0; j<size; j++){
				out << array[i][j]<<",";
		}
	}
}

int GLCAData::getValueAt(int x,int y) {
	return array[x][y];
}


void GLCAData::setValueAt(int x,int y, int value) {
	array[x][y]=value;
}


int GLCAData::getSize() {
	return size;
}

