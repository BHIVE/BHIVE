#include "gridreader.hh"

GridReader::GridReader() {
	inputset = false;
	number_species = 0;
}

GridReader::~GridReader() { }

Grid* GridReader::returnGrid() {
	this->readGrid();
	return g;
}

Grid* GridReader::returnGrid(string input) {
	this->setInputFile(input);
	this->readGrid();
	return g;
}

void GridReader::setInputFile(string name) {
	cerr << name << endl;
	input.open(name.c_str());

	inputset = true;
}

void GridReader::readGrid() {
	// throw an exception here that you are trying to read a grid with
	// a base class!

}

void GridReader::addSpecies(string name, double d) {
	this->number_species += 1;
	this->names.push_back(name);
	this->diffs.push_back(d);
}

void GridReader::setSpecies(vector<string>& n, vector<double>& d) {
	this->number_species = n.size();
	for (unsigned int i=0; i<n.size(); i++) {
		names.push_back(n[i]);
		diffs.push_back(d[i]);
	}
}
