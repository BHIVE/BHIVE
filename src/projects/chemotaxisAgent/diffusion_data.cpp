#include "diffusion_data.hh"

using namespace Chemotaxis;

DiffData::~DiffData() {
	deleteTree(searchTreeRoot);
}

void DiffData::printContent(ostream& out) {
	out << "guess what, not implemented yet... ";
}

Voxel* DiffData::getVoxel(int i) {
	return grid[i];
}

Coordinates DiffData::getVoxelCenterCoordinate(int i) {
	Voxel *v = grid[i];

	return v->center;
}

double DiffData::getDiffusionCoef(int species) {
	return speciesDiffusionCoef[species];
}

int DiffData::getNumberOfVoxels() {
	return int(grid.size());
}

int DiffData::addVertex(double x,double y,double z) {
	int index = int(vertices.size());
	vertices.push_back(initializeCoordinates(x,y,z));
	return index;
}

Coordinates* DiffData::initializeCoordinates(double x,double y,double z) {
	Coordinates* newCoord = new Coordinates;
	newCoord->x = x;
	newCoord->y = y;
	newCoord->z = z;
	return newCoord;
}

void DiffData::setVoxelCenterCoordinate(int i, double x, double y, double z) {
	grid[i]->center.x = x;
	grid[i]->center.y = y;
	grid[i]->center.z = z;
}

void DiffData::setSpecies(vector<string>& names) {
	this->species = names;
}

void DiffData::setSpeciesDiffusionCoefs(vector<double>& diff) {
	this->speciesDiffusionCoef = diff;
}

void DiffData::setupCoordinates(vector<vector<double> >& coor) {
	vertices.resize(coor.size());
	for (unsigned int i=0; i<coor.size(); i++) {
		Coordinates *c = new Coordinates();
		c->x = coor[i].at(0);
		c->y = coor[i].at(1);
		c->z = coor[i].at(2);
		vertices[i] = c;
	}
}

void DiffData::setupGrid(vector<vector<int> >& voxels, vector<vector<int> >& neighborhood) {
	grid.resize(voxels.size());
	for (unsigned int i=0; i<voxels.size(); i++) {
		Voxel *v = new Voxel();
		v->voxel_id = i;
		v->concentrations.resize(species.size());
		// stupid initialisation for testing purposes only!
		v->concentrations[0] = 0; //Util::RANDOM_CLOSED()*10.;
		v->concentrations[1] = 0; //Util::RANDOM_CLOSED()*1.;

		v->newConcentrations.resize(species.size());

		v->vertices.resize(4);
		v->vertices[0] = voxels[i][0];
		v->vertices[1] = voxels[i][1];
		v->vertices[2] = voxels[i][2];
		v->vertices[3] = voxels[i][3];
		v->neighbors.resize(neighborhood[i].size());
		for (unsigned int j=0; j<neighborhood[i].size(); j++)
			v->neighbors[j] = neighborhood[i][j];
		grid[i] = v;
	}
}

// this does the calculation. i believe this should rather be done somewhere else. it
// does not belong here. but hell. just leave it here. change it later on.
void DiffData::setupBaseRates(vector<vector<double> >& interface_areas, vector<double>& volume,
							  vector<vector<double> >& centers) {
	double distance;
	int neigh_index;
	Coordinates *one = new Coordinates();
	Coordinates *two = new Coordinates();
	for (unsigned int i=0; i<grid.size(); i++) {
		grid[i]->baseRates.resize(grid[i]->neighbors.size());
		for (unsigned int j=0; j<grid[i]->neighbors.size(); j++) {
			// do the calculation
			neigh_index = grid[i]->neighbors[j];
			one->x = centers[i][0];
			one->y = centers[i][1];
			one->z = centers[i][2];
			two->x = centers[neigh_index][0];
			two->y = centers[neigh_index][1];
			two->z = centers[neigh_index][2];
			distance = calcDistance(one, two);
			grid[i]->baseRates[j] = interface_areas[i].at(j)/(distance*volume[i]);
//			cerr << grid[i]->baseRates[j] << endl;
		}
	}
	delete one;
	delete two;
}

void DiffData::setupVerticesPointedAtByVoxels(vector<vector<int> >& v) {
	vertices_pointed_at_by_voxels.resize(v.size());
	for (unsigned int i=0; i<v.size(); i++)
		for (unsigned int j=0; j<v[i].size(); j++)
			vertices_pointed_at_by_voxels[i].push_back(v[i].at(j));
}

void DiffData::setupVolumes(vector<double>& volumes) {
	for (unsigned int i=0; i<grid.size(); i++) {
		grid[i]->volume = volumes[i];
	}
}

double DiffData::calcDistance(Coordinates* One,Coordinates* Two) {
	return pow(pow(Two->x-One->x,2)+pow(Two->y-One->y,2)+pow(Two->z-One->z,2),0.5);
}

void DiffData::deleteTree(node* inNode) {
	if (inNode == NULL) {
		inNode = searchTreeRoot;
	}
	if (inNode != NULL) {
		if (inNode->children != NULL) {
			for (int i=0; i < 8; i++) {
				deleteTree(inNode->children[i]);
			}
			delete [] inNode->children;
		}
		delete inNode->Center;
		delete inNode;
	}
};

void DiffData::printTree(node* inNode) {
		if (inNode->children != NULL)
			for (int i=0; i<8; i++)
				printTree(inNode->children[i]);
		else
			for (int i=0; i<inNode->points.size(); i++)
				cerr << inNode->points[i] << endl;
}

void DiffData::addPointToTree(node* inNode,int index) {
	if (inNode->children == NULL) {
		inNode->points.push_back(index);
		if (inNode->points.size() > TREE_LIMIT) {
			vector<int> temp = inNode->points;
			inNode->points.clear();
			inNode->points.resize(0);
			inNode->children = new node*[8];
			for (int i=0; i < 8; i++) {
				Coordinates* childCenter = initializeCoordinates(inNode->Center->x,inNode->Center->y,inNode->Center->z);
				if (i==0 || i==2 || i==4 || i == 6) {
					childCenter->x += -inNode->Length/4;
				} else {
					childCenter->x += inNode->Length/4;
				}
				if (i==0 || i==1 || i==4 || i == 5) {
					childCenter->y += -inNode->Length/4;
				} else {
					childCenter->y += inNode->Length/4;
				}
				if (i==0 || i==1 || i==2 || i == 3) {
					childCenter->z += -inNode->Length/4;
				} else {
					childCenter->z += inNode->Length/4;
				}
				inNode->children[i] = initializeNode(inNode->Length/2,childCenter,inNode);
			}
			for (int i=0; i < int(temp.size()); i++) {
				addPointToTree(inNode,temp[i]);
			}
		}
	} else {
		int loc = 0;
		if (vertices[index]->x > inNode->Center->x) {
			loc += 1;
		}
		if (vertices[index]->y > inNode->Center->y) {
			loc += 2;
		}
		if (vertices[index]->z > inNode->Center->z) {
			loc += 4;
		}
		addPointToTree(inNode->children[loc],index);
	}
}

int DiffData::nearestNeighbor(double x,double y,double z,node* inNode,bool backtrack) {
	if (inNode == NULL) {
		inNode = searchTreeRoot;
	}

	if (inNode->children != NULL) {
		//I have not reached a leaf node yet
		int loc = 0;
		if (x > inNode->Center->x) {
			loc += 1;
		}
		if (y > inNode->Center->y) {
			loc += 2;
		}
		if (z > inNode->Center->z) {
			loc += 4;
		}
		int result = nearestNeighbor(x,y,z,inNode->children[loc]);
		if (result >= 0) {
			return result;
		} else if (result == -1) {
			cerr << "ich gehe jetzt hier rein!!!" << endl;
			//There were no points in that cube, so we need to search all the neighboring cubes
			Coordinates* point = initializeCoordinates(x,y,z);
			int currentBest = -1;
			double currentBestDistance = -1;
			//Checking all the neighboring children in this cube
			for (int i=0; i < 8; i++) {
				if (i != loc) {
					result = nearestNeighbor(x,y,z,inNode->children[i]);
					if (result >= 0) {
						double newDistance = calcDistance(point,vertices[result]);
						if (currentBest == -1 || newDistance < currentBestDistance) {
							currentBest = result;
							currentBestDistance = newDistance;
						}
					}
				}
			}
			//Now we need to check all the siblings of this current cube
			if (inNode->parent != NULL && backtrack) {
				for (int i=0; i < 8; i++) {
					if (inNode->parent->children[i] != inNode) {
						result = nearestNeighbor(x,y,z,inNode->parent->children[i],false);
						if (result >= 0) {
							double newDistance = calcDistance(point,vertices[result]);
							if (currentBest == -1 || newDistance < currentBestDistance) {
								currentBest = result;
								currentBestDistance = newDistance;
							}
						}
					}
				}
			}
			return currentBest;
		}
	} else {
		double distance = -1;
		int minPoint = -1;
		Coordinates* point = initializeCoordinates(x,y,z);
		for (int i=0; i < int(inNode->points.size()); i++) {
			double currentDistance = calcDistance(vertices[inNode->points[i]],point);
			if (distance == -1 || currentDistance < distance) {
				distance = currentDistance;
				minPoint = inNode->points[i];
			}
		}
		delete point;
		if (minPoint != -1) {
			//Now we look through the voxels containing this vertex to see if any of their indecies are closer to this point
			int newminPoint = minPoint;
			for (unsigned int i=0; i < vertices_pointed_at_by_voxels[minPoint].size(); i++) {
				for (unsigned int j=0; j < grid[vertices_pointed_at_by_voxels[minPoint][i]]->vertices.size(); j++) {
					if (grid[vertices_pointed_at_by_voxels[minPoint][i]]->vertices[j] != minPoint) {
						double currentDistance = calcDistance(vertices[grid[vertices_pointed_at_by_voxels[minPoint][i]]->vertices[j]],point);
						if (currentDistance < distance) {
							distance = currentDistance;
							newminPoint = grid[vertices_pointed_at_by_voxels[minPoint][i]]->vertices[j];
						}
					}
				}
			}
			return minPoint;
		}
	}

	return -1;
}

void DiffData::createTree() {
	//Deleting the tree if it already exists
	if (searchTreeRoot != NULL) {
		deleteTree(searchTreeRoot);
	}

	double length = 0;
	double minMax[2][3];
	for (int i=0; i < 2; i++) {
		for (int j=0; j < 3; j++) {
			minMax[i][j] = 0;
		}
	}

	for (unsigned int i=0; i < vertices.size(); i++) {
		if (vertices[i]->x <= minMax[0][0]) {
			minMax[0][0] = vertices[i]->x-0.01;
		} else if (vertices[i]->x >= minMax[1][0]) {
			minMax[1][0] = vertices[i]->x+0.01;
		}
		if (vertices[i]->y <= minMax[0][1]) {
			minMax[0][1] = vertices[i]->y-0.01;
		} else if (vertices[i]->y >= minMax[1][1]) {
			minMax[1][1] = vertices[i]->y+0.01;
		}
		if (vertices[i]->z <= minMax[0][2]) {
			minMax[0][2] = vertices[i]->z-0.01;
		} else if (vertices[i]->z >= minMax[1][2]) {
			minMax[1][2] = vertices[i]->z+0.01;
		}
	}
	for (unsigned int i=0; i < 3; i++) {
		if ((minMax[1][i]-minMax[0][i]) > length) {
			length = minMax[1][i]-minMax[0][i];
		}
	}

	//Adding all vertices to the tree
	cout << "Grid dimensions: " << minMax[1][0]-minMax[0][0] << " " << minMax[1][1]-minMax[0][1] << " " <<  minMax[1][2]-minMax[0][2] << endl;
	searchTreeRoot = initializeNode(length,initializeCoordinates((minMax[0][0]+minMax[1][0])/2,(minMax[0][1]+minMax[1][1])/2,(minMax[0][2]+minMax[1][2])/2),NULL);
	for (unsigned int i=0; i < vertices.size(); i++) {
		addPointToTree(searchTreeRoot,i);
	}
}

node* DiffData::initializeNode(double length,Coordinates* center,node* parent) {
	node* newNode = new node;
	newNode->parent = parent;
	newNode->Center = center;
	newNode->Length = length;
	newNode->children = NULL;
	return newNode;
}

Voxel* DiffData::getVoxel(double x,double y,double z) {
	//Finding the vertex closest to the input point (or almost the closest)
	int closest_vertex = nearestNeighbor(x,y,z,NULL);
	if (closest_vertex == -1) {
		return NULL;
	}
	// simple check whether the thing really finds the nearest neighbor
	// as of 11.11. it does not look like it.
	double distance = 100000; double dist; int hans;
	for (int i=0; i<vertices.size(); i++) {
		dist = pow((vertices[i]->x - x)*(vertices[i]->x - x) +
			(vertices[i]->y -y)*(vertices[i]->y -y) +
			(vertices[i]->z -z)*(vertices[i]->z -z), 0.5);
		if (dist <= distance) {
			distance = dist;
			hans = i;
		}
	}

	cerr << hans << " " << closest_vertex << endl;


	Voxel* containingVoxel = findVetexVoxelWithPoint(x,y,z,closest_vertex);
	if (containingVoxel == NULL) {
		//Now to be vigorous, we need to check every vertex of a voxel that includes the nearest point as a vertex
		for (unsigned int i=0; i < vertices_pointed_at_by_voxels[closest_vertex].size(); i++) {
			for (unsigned int j=0; j < grid[vertices_pointed_at_by_voxels[closest_vertex][i]]->vertices.size(); j++) {
				if (grid[vertices_pointed_at_by_voxels[closest_vertex][i]]->vertices[j] != closest_vertex) {
					containingVoxel = findVetexVoxelWithPoint(x,y,z,grid[vertices_pointed_at_by_voxels[closest_vertex][i]]->vertices[j]);
					if (containingVoxel != NULL) {
						return containingVoxel;
					}
				}
			}
		}
	}
	return containingVoxel;
}

Voxel* DiffData::findVetexVoxelWithPoint(double x,double y,double z,int closest_vertex) {
	int count;
	vector<int> SharedNodes(3);
	for (int i=0; i < int(vertices_pointed_at_by_voxels[closest_vertex].size()); i++) {
		Voxel* current = getVoxel(vertices_pointed_at_by_voxels[closest_vertex][i]);
		//If there are four vertices in the voxel, we generate every possible set of three
		if (current->vertices.size() == 4) {
			bool Contained = true;
			for (unsigned int k=0; k < current->vertices.size(); k++) {
				count = 0;
				for (unsigned int j=0; j < current->vertices.size(); j++) {
					if (j != k) {
						SharedNodes[count] = current->vertices[j];
						count++;
					}
				}
				double A = (vertices[SharedNodes[1]]->y - vertices[SharedNodes[0]]->y)*(vertices[SharedNodes[2]]->z - vertices[SharedNodes[0]]->z) + (vertices[SharedNodes[0]]->z - vertices[SharedNodes[1]]->z)*(vertices[SharedNodes[2]]->y - vertices[SharedNodes[0]]->y);
				double B = (vertices[SharedNodes[1]]->z - vertices[SharedNodes[0]]->z)*(vertices[SharedNodes[2]]->x - vertices[SharedNodes[0]]->x) + (vertices[SharedNodes[0]]->x - vertices[SharedNodes[1]]->x)*(vertices[SharedNodes[2]]->z - vertices[SharedNodes[0]]->z);
				double C = (vertices[SharedNodes[1]]->x - vertices[SharedNodes[0]]->x)*(vertices[SharedNodes[2]]->y - vertices[SharedNodes[0]]->y) + (vertices[SharedNodes[0]]->y - vertices[SharedNodes[1]]->y)*(vertices[SharedNodes[2]]->x - vertices[SharedNodes[0]]->x);
				double constant = -vertices[SharedNodes[0]]->x*(vertices[SharedNodes[1]]->y - vertices[SharedNodes[0]]->y)*(vertices[SharedNodes[2]]->z - vertices[SharedNodes[0]]->z) - vertices[SharedNodes[0]]->x*(vertices[SharedNodes[0]]->z - vertices[SharedNodes[1]]->z)*(vertices[SharedNodes[2]]->y - vertices[SharedNodes[0]]->y);
				constant += -vertices[SharedNodes[0]]->y*(vertices[SharedNodes[1]]->z - vertices[SharedNodes[0]]->z)*(vertices[SharedNodes[2]]->x - vertices[SharedNodes[0]]->x) - vertices[SharedNodes[0]]->y*(vertices[SharedNodes[0]]->x - vertices[SharedNodes[1]]->x)*(vertices[SharedNodes[2]]->z - vertices[SharedNodes[0]]->z);
				constant += -vertices[SharedNodes[0]]->z*(vertices[SharedNodes[1]]->x - vertices[SharedNodes[0]]->x)*(vertices[SharedNodes[2]]->y - vertices[SharedNodes[0]]->y) - vertices[SharedNodes[0]]->z*(vertices[SharedNodes[0]]->y - vertices[SharedNodes[1]]->y)*(vertices[SharedNodes[2]]->x - vertices[SharedNodes[0]]->x);
				Coordinates* outside = vertices[current->vertices[k]];
				if (((x*A+y*B+z*C+constant) < -0.000001 || (x*A+y*B+z*C+constant) > 0.000001) && (x*A+y*B+z*C+constant)*(outside->x*A+outside->y*B+outside->z*C+constant) < 0) {
					Contained = false;
				}
			}
			if (Contained) {
				return current;
			}
		}
		//Currently the system does not work for grid types with more than 4 vertices... TODO
	}

	return NULL;
}

void DiffData::printVerticesOfVoxelOnSterr(int i) {
	vector<int> hans = grid[i]->vertices;
	for (int i=0; i<4; i++)
		cerr << vertices[hans[i]]->x << " " << vertices[hans[i]]->y << " " << vertices[hans[i]]->z << endl;
}
