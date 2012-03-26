/*
 * grid.cpp
 *
 * this file contains the implementation of the grid class
 *
 *@date Created on Nov 19, 2009
 *
 *@author garrit
 */

#include "grid.hh"

using namespace Hive;

Grid::~Grid() {
	for (unsigned int i=0; i<vertices.size(); i++) {
		delete vertices[i];
		vertices[i] = NULL;
	}
	for (unsigned int i=0; i<grid.size(); i++) {
		delete grid[i];
		grid[i] = NULL;
	}
	if (this->search_tree != NULL)
		delete this->search_tree;
}

void Grid::printContent(ostream& out) {
	cerr << "Grid::printContent is not implemented" << endl;
}

void Grid::setupSearchTree() {
	this->search_tree = new OctTree();
	search_tree->createTree(this->grid, this->vertices);
}

double Grid::getCenterX(int index) {
	return (vertices[grid[index]->vertices[0]]->getX() + vertices[grid[index]->vertices[1]]->getX())/2.;
}

double Grid::getCenterY(int index) {
	return (vertices[grid[index]->vertices[0]]->getY() + vertices[grid[index]->vertices[3]]->getY())/2.;
}

double Grid::getCenterZ(int index) {
	return (vertices[grid[index]->vertices[0]]->getZ() + vertices[grid[index]->vertices[6]]->getZ())/2.;
}

double Grid::getDiffusionCoeff(int index) {
	return species_diffusion_coef[index];
}

int Grid::getNumberOfVoxels() {
	return this->grid.size();
}

int Grid::getTotalNumberOfNeighbors() {
	int total_number = 0;
	for (unsigned int i = 0; i<grid.size(); i++)
		total_number += grid[i]->getNumberNeighbors();
	return total_number;
}

Coordinate* Grid::getVertex(int i) {
	return vertices[i];
}

NVoxel* Grid::getVoxel(Coordinate& c) {
	return this->search_tree->findVoxel(c);
}

NVoxel* Grid::getVoxel(int i) {
	return grid[i];
}

int Grid::insideVoxel(double x, double y, double z) {
	int index = 0;
	double llx, lly, llz, urx, ury, urz;
	while(index<grid.size()) {
		// get coordinates of the lower left and uppper right vertex
		llx = vertices[grid[index]->vertices[0]]->getX();
		lly = vertices[grid[index]->vertices[0]]->getY();
		llz = vertices[grid[index]->vertices[0]]->getZ();
		urx = vertices[grid[index]->vertices[6]]->getX();
		ury = vertices[grid[index]->vertices[6]]->getY();
		urz = vertices[grid[index++]->vertices[6]]->getZ();
		if (x >= llx && y>= lly && z >= llz && x <= urx && y <= ury && z <= urz)
			break;
	}
	if (index != grid.size())
		return grid[index-1]->getVoxelID();
	else {
		cerr << "# error in Grid::insideVoxel(double, double, double) point does not lie inside the grid" << endl;
		cerr << "# aborting ... sorry. " << endl;
		exit(1);
	}
}



void Grid::addSpecies(string name, double d) {
	species.push_back(name);
	species_diffusion_coef.push_back(d);
}

void Grid::setSpecies(vector<string>& s) {
	this->species = s;
}

void Grid::setSpeciesDiffusionCoeffs(vector<double>& d) {
	this->species_diffusion_coef = d;
}

void Grid::setupCoordinates(vector<vector<double> >& coord) {
	vertices.resize(coord.size());
	for (unsigned int i = 0; i<coord.size(); i++) {
		vertices[i] = new Coordinate();
		vertices[i]->setCoordinates(coord[i].at(0), coord[i].at(1), coord[i].at(2));
	}
}

void Grid::setupTetraGrid(vector<vector<int> >& voxels, vector<vector<int> >& neighborhood,
					 vector<vector<double> >& interface_areas, vector<double>& volumes,
					 vector<vector<double> >& center_coor) {
	double distance;
	grid.resize(voxels.size());

	for (unsigned int i=0; i<voxels.size(); i++) {
		NVoxel *v = new NVoxel();
		v->concentrations.resize(species.size());
		v->old_concentrations.resize(species.size());
		v->voxel_id = i;
		v->grid_ptr = this;
		v->vertices.resize(4);
		v->vertices[0] = voxels[i][0];
		v->vertices[1] = voxels[i][1];
		v->vertices[2] = voxels[i][2];
		v->vertices[3] = voxels[i][3];
		v->neighbors.resize(neighborhood[i].size());
		v->base_diffusion_rates.resize(neighborhood[i].size());
		v->volume = volumes[i];
		Coordinate c;
		c.setCoordinates(center_coor[i][0], center_coor[i][1], center_coor[i][2]);
		v->center = c;
		for (unsigned int j=0; j<neighborhood[i].size(); j++) {
			v->neighbors[j] = neighborhood[i][j];
			Coordinate c2;
			c2.setCoordinates(center_coor[neighborhood[i][j]][0],
							  center_coor[neighborhood[i][j]][1],
							  center_coor[neighborhood[i][j]][2]);
			distance = v->center.calcDistance(c2);
			v->base_diffusion_rates[j] = interface_areas[i][j]/(distance*volumes[i]);
		}
		grid[i] = v;
	}
}

void Grid::setupQuadGrid(vector<vector<int> >& voxels, vector<vector<int> >& neighborhood,
						 vector<vector<double> >& rates) {
	grid.resize(voxels.size());
	for (unsigned int i=0; i<voxels.size(); i++) {
		NVoxel *v = new NVoxel();
		v->concentrations.resize(species.size());
		v->old_concentrations.resize(species.size());
		v->voxel_id = i;
		v->grid_ptr = this;
		v->vertices.resize(8);
		v->vertices[0] = voxels[i][0];
		v->vertices[1] = voxels[i][1];
		v->vertices[2] = voxels[i][2];
		v->vertices[3] = voxels[i][3];
		v->vertices[4] = voxels[i][4];
		v->vertices[5] = voxels[i][5];
		v->vertices[6] = voxels[i][6];
		v->vertices[7] = voxels[i][7];
		v->neighbors.resize(neighborhood[i].size());
		v->base_diffusion_rates.resize(neighborhood[i].size());
		for (unsigned int j=0; j<neighborhood[i].size(); j++) {
			v->neighbors[j] = neighborhood[i][j];
			v->base_diffusion_rates[j] = rates[i][j];
		}
		grid[i] = v;
	}
}

void Grid::outputGridForValidation() {
	cerr << "there are " << vertices.size() << " coordinates on the grid" << endl;
	cerr << "they are: " << endl;
	for (unsigned int i=0; i<vertices.size(); i++)
		cerr << i << ": " << vertices[i]->getX() << " " << vertices[i]->getY() << " " <<  vertices[i]->getZ() << endl;

	cerr << endl << endl;

	cerr << "there are " << grid.size() << " voxel on the grid" << endl;
	for (unsigned  int i=0; i<grid.size(); i++)
		grid[i]->printContent(cerr);

	cerr << endl << endl;
	cerr << "outputting neighborhood" << endl;
	for (unsigned int i=0; i<grid.size(); i++) {
		cerr << i << ": ";
		for (int j=0; j<grid[i]->getNumberNeighbors(); j++)
			cerr << grid[i]->getNeighborID(j) << " ";
		cerr << endl;
	}
	cerr << endl << endl;

	cerr << "outputting concentration" << endl;
	for (unsigned int i=0; i<grid.size(); i++)
		cerr << i << ": " << grid[i]->getConcentration(0) << endl;
}









