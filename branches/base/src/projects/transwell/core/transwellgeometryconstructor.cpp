/*
 * transwellgeometryconstructor.cpp
 *
 *  Created on: Dec 8, 2009
 *      Author: jentsch
 */

#include "transwellgeometryconstructor.hh"

using namespace Transwell;

TranswellGeometryConstructor::TranswellGeometryConstructor() {
	this->inner_height = 0;
	this->outer_height = 0;
	this->thickness = 0.0;
	this->inner_radius = 0.0;
	this->outer_radius = 0.0;
}

TranswellGeometryConstructor::~TranswellGeometryConstructor() {
	cerr << "# entering TranswellGeometryConstructor:Destructor" << endl;
//	delete grid;
	cerr << "# done TranswellGeometryConstructo:Destructor" << endl;
}

int TranswellGeometryConstructor::getVoxelDomain(int i) {
	return this->voxels_domain_lookup[i];
}

Grid* TranswellGeometryConstructor::returnGrid(string name) {
	// set up grid
	grid = new Grid(name);

	// construct grid
	this->setupCoordinates();
	this->setupVoxels();
	this->determineNeighborhood();

	// now we have to give the constructed information to the grid
	// for this we call
	grid->setupQuadGrid(this->voxels, this->connectivity, this->rates);

	return grid;
}

void TranswellGeometryConstructor::setHeights(double inner, double outer) {
	this->inner_height = inner;
	this->outer_height = outer;
}

void TranswellGeometryConstructor::setRadii(double inner, double outer) {
	this->inner_radius = inner;
	this->outer_radius = outer;
}

void TranswellGeometryConstructor::setResolution(double r) {
	this->stepsize = r;
}

void TranswellGeometryConstructor::setThickness(double t) {
	this->thickness = t-(2.*this->stepsize);
}

void TranswellGeometryConstructor::determineNeighborhood() {
	connectivity.resize(voxels.size());
	connecting_directions.resize(voxels.size());
	rates.resize(voxels.size());

	int  voxel_a;
	int  voxel_b;
	bool not_included;
	int  num_matches;
	int direction = 0;

	// base diffusion rate
	double base_rate = 1./(stepsize*stepsize);

	// THIS IS THE FACTOR BY WHICH THE DIFFUSION COEFFICIENT FROM THE OUTER TO THE
	// INNER WELL IS REDUCED!!!
	double reduction_factor = .761905;
	//	double reduction_factor = 0.0125;

	// iterate over the nodes
	for (unsigned int i=0; i<node_covered_by_voxels.size(); i++)
		for (unsigned int j=0; j<node_covered_by_voxels[i].size()-1; j++)
			for(unsigned int k=j+1; k<node_covered_by_voxels[i].size(); k++) {
				voxel_a      = node_covered_by_voxels[i][j];
				voxel_b      = node_covered_by_voxels[i][k];
				not_included = true;
				// check whether voxel_a and voxel_b are already connected
				for (unsigned int index = 0; index < connectivity[voxel_a].size(); index++)
					if (connectivity[voxel_a][index] == voxel_b) {
						not_included = false;
						break;
					}
				if (not_included) {
					num_matches = 0;
					vector<int> matching_voxels;
					// check whether the two voxels have 4 vertices / coordinates in common
					for (unsigned int m=0; m<voxels[voxel_a].size(); m++)
						for (unsigned int n=0; n<voxels[voxel_b].size(); n++)
							if (voxels[voxel_a][m]==voxels[voxel_b][n]) {
								num_matches += 1;
								matching_voxels.push_back(voxels[voxel_a][m]);
								break;
							}
					if (num_matches == 4) {
//						connectivity[voxel_a].push_back(voxel_b);
//						connectivity[voxel_b].push_back(voxel_a);
						// determine direction along which the voxels border
						vector<Coordinate*> coor;
						coor.push_back(grid->getVertex(matching_voxels[0]));
						coor.push_back(grid->getVertex(matching_voxels[1]));
						coor.push_back(grid->getVertex(matching_voxels[2]));
						coor.push_back(grid->getVertex(matching_voxels[3]));
						if ((coor[0]->getX() == coor[1]->getX()) && (coor[0]->getX() == coor[2]->getX()) &&
							(coor[0]->getX() == coor[3]->getX()))
							direction = 0;
						else if ((coor[0]->getY() == coor[1]->getY()) && (coor[0]->getY() == coor[2]->getY()) &&
								 (coor[0]->getY() == coor[3]->getY()))
							direction = 1;
						else if ((coor[0]->getZ() == coor[1]->getZ()) && (coor[0]->getZ() == coor[2]->getZ()) &&
								 (coor[0]->getZ() == coor[3]->getZ()))
							direction = 2;
						if ((this->voxels_domain_lookup[voxel_a] == 2) || (this->voxels_domain_lookup[voxel_b] == 2))
								continue;
						if (this->voxels_domain_lookup[voxel_a] == this->voxels_domain_lookup[voxel_b]) {
							connectivity[voxel_a].push_back(voxel_b);
							connectivity[voxel_b].push_back(voxel_a);
							connecting_directions[voxel_a].push_back(direction);
							connecting_directions[voxel_b].push_back(direction);
							rates[voxel_a].push_back(base_rate);
							rates[voxel_b].push_back(base_rate);
						} else if ( ( ((voxels_domain_lookup[voxel_a] == 1) && (voxels_domain_lookup[voxel_b] == 0)) ||
								      ((voxels_domain_lookup[voxel_b] == 1) && (voxels_domain_lookup[voxel_a] == 0)) ) &&
								   direction == 2) {
							connectivity[voxel_a].push_back(voxel_b);
							connectivity[voxel_b].push_back(voxel_a);
							connecting_directions[voxel_a].push_back(direction);
							connecting_directions[voxel_b].push_back(direction);
							rates[voxel_a].push_back(base_rate*reduction_factor);
							rates[voxel_b].push_back(base_rate*reduction_factor);
						}
					}
				}
			}
}


void TranswellGeometryConstructor::setupCoordinates() {
	// set size of transwell
	double width  = 2*outer_radius;
	double height = outer_height;

	int num_coordinates = 0;
	// calculate how many coordinates there will be
	this->number_points_plane  = (int)(width/stepsize)+1;
	this->number_points_height = (int)(height/stepsize)+1;

	// reserve some memory for the temporary stuff ...
	this->coordinate_index_lookup.resize(number_points_plane);
	this->coordinate_domain_lookup.resize(number_points_plane);

	for (int i=0; i<number_points_plane; i++) {
		coordinate_index_lookup[i].resize(number_points_plane);
		coordinate_domain_lookup[i].resize(number_points_plane);
		for (int j=0; j<number_points_plane; j++) {
			coordinate_index_lookup[i][j].resize(number_points_height);
			coordinate_domain_lookup[i][j].resize(number_points_height);
		}
	}
	// generate the coordinates of the cylinder
	for (int i=0; i<number_points_height; i++)
		for (int j=0; j<number_points_plane; j++)
			for (int k=0; k<number_points_plane; k++)
				if ( ( (j*stepsize-width/2)*(j*stepsize-width/2) +
					   (k*stepsize-width/2)*(k*stepsize-width/2) ) < width*width/4.) {
					Coordinate *coor = new Coordinate();
					coor->setCoordinates(j*stepsize-width/2, k*stepsize-width/2, i*stepsize);
					grid->addCoordinate(coor);
					coordinate_index_lookup[j][k][i] = num_coordinates++;
					// now we have to check whether the coordinate belongs to the inner well, outer well or to the wall
					if ( ((coor->getZ()-inner_height) >= 0.01) &&
						(coor->getX()*coor->getX() + coor->getY()*coor->getY() -
							(this->inner_radius+thickness)*(this->inner_radius+thickness) < 0.0001))
						coordinate_domain_lookup[j][k][i] = 10;
					else
						coordinate_domain_lookup[j][k][i] = 0;
					if ( coor->getZ() >= inner_height &&
						(coor->getX()*coor->getX() + coor->getY()*coor->getY() <= this->inner_radius*this->inner_radius ))
						coordinate_domain_lookup[j][k][i] = 1;
				}
				else
					coordinate_index_lookup[j][k][i] = -1;

	this->number_coordinates = num_coordinates;
}

void TranswellGeometryConstructor::setupVoxels() {
	map<int, int> hans;
	this->node_covered_by_voxels.resize(number_coordinates);
	for (int i=0; i<number_points_height-1; i++)
		for (int j=0; j<number_points_plane-1; j++)
			for (int k=0; k<number_points_plane-1; k++) {
				if (coordinate_index_lookup[j][k][i] != -1 &&
					coordinate_index_lookup[j+1][k][i] != -1 &&
					coordinate_index_lookup[j][k+1][i] != -1 &&
					coordinate_index_lookup[j][k][i+1] != -1 &&
					coordinate_index_lookup[j+1][k+1][i] != -1 &&
					coordinate_index_lookup[j+1][k][i+1] != -1 &&
					coordinate_index_lookup[j][k+1][i+1] != -1 &&
					coordinate_index_lookup[j+1][k+1][i+1] != -1) {
					// determine which coordinates belong to the new voxel ...
					vector<int> tmp;
					tmp.push_back(coordinate_index_lookup[j][k][i]);
					tmp.push_back(coordinate_index_lookup[j+1][k][i]);
					tmp.push_back(coordinate_index_lookup[j+1][k+1][i]);
					tmp.push_back(coordinate_index_lookup[j][k+1][i]);
					tmp.push_back(coordinate_index_lookup[j][k][i+1]);
					tmp.push_back(coordinate_index_lookup[j+1][k][i+1]);
					tmp.push_back(coordinate_index_lookup[j+1][k+1][i+1]);
					tmp.push_back(coordinate_index_lookup[j][k+1][i+1]);
					this->voxels.push_back(tmp);
					int blup = coordinate_domain_lookup[j][k][i] +
					coordinate_domain_lookup[j+1][k][i] +
					coordinate_domain_lookup[j+1][k+1][i] +
					coordinate_domain_lookup[j][k+1][i] +
					coordinate_domain_lookup[j][k][i+1] +
					coordinate_domain_lookup[j+1][k][i+1] +
					coordinate_domain_lookup[j+1][k+1][i+1] +
					coordinate_domain_lookup[j][k+1][i+1];


					// determine whether the voxel belongs to the inner well, outer well or to the wall
					hans[blup] += 1;
					if (blup == 8)
						voxels_domain_lookup.push_back(1);
					else if (blup > 8)
						voxels_domain_lookup.push_back(2);
				//	else if (blup > 40)
				//		voxels_domain_lookup.push_back(2);
				//	else if (coordinate_domain_lookup[j][k][i] == 10 && coordinate_domain_lookup[j+1][k][i] == 10 &&
				//		coordinate_domain_lookup[j+1][k+1][i] == 10  && coordinate_domain_lookup[j+1][k+1][i+1] == 10 &&
				//		coordinate_domain_lookup[j+1][k][i+1] == 10 && coordinate_domain_lookup[j][k+1][i+1] == 10 &&
				//		coordinate_domain_lookup[j][k][i+1] == 10 && coordinate_domain_lookup[j][k+1][i] == 10 )
				//		voxels_domain_lookup.push_back(2);
					else if (blup < 8)
						voxels_domain_lookup.push_back(0);
					node_covered_by_voxels[coordinate_index_lookup[j][k][i]].push_back(voxels.size()-1);
					node_covered_by_voxels[coordinate_index_lookup[j+1][k][i]].push_back(voxels.size()-1);
					node_covered_by_voxels[coordinate_index_lookup[j+1][k+1][i]].push_back(voxels.size()-1);
					node_covered_by_voxels[coordinate_index_lookup[j][k+1][i]].push_back(voxels.size()-1);
					node_covered_by_voxels[coordinate_index_lookup[j][k][i+1]].push_back(voxels.size()-1);
					node_covered_by_voxels[coordinate_index_lookup[j+1][k][i+1]].push_back(voxels.size()-1);
					node_covered_by_voxels[coordinate_index_lookup[j+1][k+1][i+1]].push_back(voxels.size()-1);
					node_covered_by_voxels[coordinate_index_lookup[j][k+1][i+1]].push_back(voxels.size()-1);
				}
			}
	for (map<int,int>::iterator iter = hans.begin(); iter != hans.end() ; iter++)
		cout << iter->first << " " << iter->second << endl;
}

















