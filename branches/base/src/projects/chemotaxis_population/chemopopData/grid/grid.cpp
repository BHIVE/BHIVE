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
	num_voxels  = 0;
	num_species = 0;
}

void Grid::printContent(ostream& out) {
	cerr << "Grid::printContent is not implemented" << endl;
}

void Grid::addCoordinate(double x, double y, double z) {
	Coordinate *c = new Coordinate(x,y,z);
	vertices.push_back(c);
}

int Grid::coordinateAlreadyPresent(Coordinate *c) {
	return this->coordinateAlreadyPresent(c->getX(), c->getY(), c->getZ());
}

int Grid::coordinateAlreadyPresent(double x, double y, double z) {
	int result = -1;
	// iterate over all vertices
	for (unsigned int i=0; i<vertices.size(); i++)
		// if all coordinates are identical
		if (vertices[i]->getX() == x && vertices[i]->getY() == y && vertices[i]->getZ() == z) {
			// assign correct index
			result = i;
			break;
		}
	// return result
	return result;
}

void Grid::setupSearchTree() {
	this->search_tree = new OctTree();
	search_tree->createTree(this->grid, this->vertices);
}


void Grid::getCenterOfVoxel(int vindex, vector<double>& coord) {
	if (vindex <0 || vindex >= this->num_voxels)
		throw HiveException("wrong voxel index", "Grid::getCenterOfVoxel");
	vector<int> coordinate_indices;
	grid[vindex]->getVertices(coordinate_indices);
	int cindex = 0;
	coord[0] = 0; coord[1] = 0; coord[2] = 0;
	int num_coor = 0;
	for (int i=0; i<coordinate_indices.size(); i++) {
		cindex = coordinate_indices[i];
		coord[0] += this->vertices[cindex]->getX();
		coord[1] += this->vertices[cindex]->getY();
		coord[2] += this->vertices[cindex]->getZ();
		num_coor += 1;
	}
	coord[0] /= num_coor;
	coord[1] /= num_coor;
	coord[2] /= num_coor;

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

//// this is a little weired.
double Grid::getValue(vector<double>& pos, double t) {

	//this->outputGridForValidation();

	//cerr<<"here"<<endl;
	//cerr<<"pos size: "<<pos.size()<<endl;
	//cerr<<"t "<<t<<endl;

	int index = 0;
	double conc = 0;

	//cerr<<"is_cube "<<is_cube<<endl;
	if (!is_cube) {
		index = this->insideVoxel(pos[0], pos[1], pos[2]);
		conc = this->getVoxel(index)->getConcentration(0);
	} else {
		int i = pos[0]/stepsize;
		int j = pos[1]/stepsize;
		int k = pos[2]/stepsize;
		int num_per_dim = sidelength/stepsize;
		index = k*num_per_dim*num_per_dim + j*num_per_dim + i;


		//cerr<<"pos "<<pos[0]<<" "<<pos[1]<<" "<<pos[2]<<endl;
		//cerr<<"stepsize "<<stepsize<<endl;
		//cerr<<"sidelength "<<sidelength<<endl;
		//cerr<<"looking up index "<<index<<endl;
		conc = this->getVoxel(index)->getConcentration(0);
	}
	return conc;
}

Coordinate* Grid::getVertex(int i) {
	return vertices[i];
}

Coordinate* Grid::getVertexOfVoxel(int ver, int vox) {
	NVoxel *vptr = this->grid[vox];
	return this->vertices[vptr->vertices[ver]];
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
	int num_per_dim = sidelength/stepsize;
//	cerr << num_per_dim << endl;
	if (is_cube) {
		int i = x/stepsize;
		int j = y/stepsize;
		int k = z/stepsize;
//		cerr << i << " " << j << " " << k << endl;
		index = k*num_per_dim*num_per_dim + j*num_per_dim + i;
//		cerr<<" index= "<<index<<endl;
	} else {
		while(index<grid.size()) {
			// get coordinates of the lower left and uppper right vertex
			llx = vertices[grid[index]->vertices[0]]->getX();
			lly = vertices[grid[index]->vertices[0]]->getY();
			llz = vertices[grid[index]->vertices[0]]->getZ();
			urx = vertices[grid[index]->vertices[6]]->getX();
			ury = vertices[grid[index]->vertices[6]]->getY();
			urz = vertices[grid[index++]->vertices[6]]->getZ();
			if (x >= llx && y>= lly && z >= llz && x <= urx && y <= ury && z <= urz) {
				index -= 1;
				break;
			}
		}
	}
	if (index != grid.size())
		return grid[index]->getVoxelID();
	else {
		cerr << "# error in Grid::insideVoxel(double, double, double) point does not lie inside the grid" << endl;
		cerr << "# point= "<<x<<","<<y<<","<<z<<endl;
		cerr << "# aborting ... sorry. " << endl;
		exit(1);
	}
}

bool Grid::insideThisVoxel(double x, double y, double z, int voxid) {
	double llx = vertices[grid[voxid]->vertices[0]]->getX();
	double lly = vertices[grid[voxid]->vertices[0]]->getY();
	double llz = vertices[grid[voxid]->vertices[0]]->getZ();
	double urx = vertices[grid[voxid]->vertices[6]]->getX();
	double ury = vertices[grid[voxid]->vertices[6]]->getY();
	double urz = vertices[grid[voxid]->vertices[6]]->getZ();
	return (x >= llx && y>= lly && z >= llz && x <= urx && y <= ury && z <= urz);
}

void Grid::addSpecies(string name, double d) {
	species.push_back(name);
	species_diffusion_coef.push_back(d);
	num_species += 1;
}

void Grid::setConcentration(int vindex, int sindex, double concentration) {
	// including error checking
	if (vindex < 0 || vindex >= num_voxels)
		throw HiveException("Wrong voxel index", "Grid::setConcentration");
	if (sindex < 0 || sindex >= num_species)
		throw HiveException("wrong number of species", "Grid::setConcentration");

	grid[vindex]->setConcentration(sindex, concentration);
}

void Grid::setSpecies(vector<string>& s) {
	this->species = s;
	this->num_species = s.size();
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
	this->num_voxels = voxels.size();

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
	this->num_voxels = voxels.size();
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
		v->boundaries.resize(neighborhood[i].size()-rates[i].size());
		v->neighbors.resize(rates[i].size());
		v->base_diffusion_rates.resize(rates[i].size());
		int neighbor = 0;
		// j=0:lower x, 1:upper x, 2:lower y, 3:upper y, 4:lower z, 5:upper z
		for (unsigned int j=0; j<neighborhood[i].size(); j++) {
			if(neighborhood[i][j] > -1) {
				// save voxel_ID of neighbor to list of neighbors
				v->neighbors[neighbor] = neighborhood[i][j];
				neighbor++;
			}
			else
				// j tells you which wall is boundary
				v->boundaries[j-neighbor] = j;
		}
		for (unsigned int j=0; j<rates[i].size(); j++) {
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
		for (int j=0; j<4; j++)
			cerr << grid[i]->getNeighborID(j) << " ";
		cerr << endl;
	}
	cerr << endl << endl;

	cerr << "outputting concentration" << endl;
	for (unsigned int i=0; i<grid.size(); i++)
		cerr << i << ": " << grid[i]->getConcentration(0) << endl;
}









