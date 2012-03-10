#include "cubegeometryconstructor.hh"

using namespace ChemoPop;

CubeGeometryConstructor::CubeGeometryConstructor() {
	this->length = 0;
	this->stepsize = 0;
	this->number_coordinates = 0;
	this->number_points_height = 0;
	this->number_points_plane = 0;
}

CubeGeometryConstructor::~CubeGeometryConstructor() {
	cerr << "# entering CubeGeometryConstructor:Destructor" << endl;
	cerr << "# leaving CubeGeometryConstructor:Destructor" << endl;
}

Grid* CubeGeometryConstructor::returnGrid(string name) {
	this->grid = new Grid(name);

	this->setupCoordinates();
	this->setupVoxels();
	this->determineNeighborhood();

	grid->setCubeInformation(this->stepsize, this->length);


	grid->setupQuadGrid(this->voxels, this->connectivity, this->rates);

	return this->grid;
}

void CubeGeometryConstructor::setLength(double l) {
	this->length = l;
}

void CubeGeometryConstructor::setResolution(double r) {
	this->stepsize = r;
}

void CubeGeometryConstructor::determineNeighborhood() {
	connectivity.resize(voxels.size());
	connecting_directions.resize(voxels.size());
	rates.resize(voxels.size());

	int voxel_a;
	int voxel_b;
	bool not_included;
	int num_matches;

	double base_rate = 1./(this->stepsize*this->stepsize);
	for (unsigned int i=0; i<node_covered_by_voxels.size(); i++)
		for (unsigned int j=0; j<node_covered_by_voxels[i].size()-1; j++)
			for (unsigned int k=j+1; k<node_covered_by_voxels[i].size(); k++) {
				voxel_a = node_covered_by_voxels[i][j];
				voxel_b = node_covered_by_voxels[i][k];
				not_included = true;
				// check wheter voxel_a and voxel_b are already connected
				for (unsigned int index = 0; index < connectivity[voxel_a].size(); index++)
					if (connectivity[voxel_a][index] == voxel_b) {
						not_included = false;
						break;
					}
				if (not_included) {
					num_matches = 0;
					vector<int> matching_voxels;
					for (unsigned int m=0;  m<voxels[voxel_a].size(); m++)
						for (unsigned int n=0; n<voxels[voxel_b].size(); n++) {
							if (voxels[voxel_a][m] == voxels[voxel_b][n]) {
								num_matches += 1;
								matching_voxels.push_back(voxels[voxel_a][m]);
								break;
							}
						}
					if (num_matches == 4) {
						connectivity[voxel_a].push_back(voxel_b);
						connectivity[voxel_b].push_back(voxel_a);
						rates[voxel_a].push_back(base_rate);
						rates[voxel_b].push_back(base_rate);
					}
				}
			}
}

void CubeGeometryConstructor::setupCoordinates() {
	this->number_points_height = (int)(length/stepsize)+1;
	this->number_points_plane  = (int)(length/stepsize)+1;

	this->coordinate_index_lookup.resize(number_points_height);
	for (int i=0; i<number_points_height; i++) {
		this->coordinate_index_lookup[i].resize(number_points_plane);
		for (int j=0; j<number_points_plane; j++)
			this->coordinate_index_lookup[i][j].resize(number_points_plane);
	}

	for (int i=0; i<number_points_height; i++)
		for (int j=0; j<number_points_plane; j++)
			for (int k=0; k<number_points_plane; k++) {
				Coordinate *coor = new Coordinate();
				coor->setCoordinates(k*stepsize,j*stepsize,i*stepsize);
				this->coordinate_index_lookup[k][j][i] = this->number_coordinates++;
				grid->addCoordinate(coor);
	}
}

void CubeGeometryConstructor::setupVoxels() {
	this->node_covered_by_voxels.resize(number_coordinates);
	for (int i=0; i<number_points_height-1; i++)
		for (int j=0; j<number_points_plane-1; j++)
			for (int k=0; k<number_points_plane-1; k++) {
				vector<int> tmp;
				tmp.push_back(coordinate_index_lookup[k][j][i]);
				tmp.push_back(coordinate_index_lookup[k+1][j][i]);
				tmp.push_back(coordinate_index_lookup[k+1][j+1][i]);
				tmp.push_back(coordinate_index_lookup[k][j+1][i]);
				tmp.push_back(coordinate_index_lookup[k][j][i+1]);
				tmp.push_back(coordinate_index_lookup[k+1][j][i+1]);
				tmp.push_back(coordinate_index_lookup[k+1][j+1][i+1]);
				tmp.push_back(coordinate_index_lookup[k][j+1][i+1]);
				this->voxels.push_back(tmp);
				this->node_covered_by_voxels[coordinate_index_lookup[k][j][i]].push_back(voxels.size()-1);
				this->node_covered_by_voxels[coordinate_index_lookup[k+1][j][i]].push_back(voxels.size()-1);
				this->node_covered_by_voxels[coordinate_index_lookup[k+1][j+1][i]].push_back(voxels.size()-1);
				this->node_covered_by_voxels[coordinate_index_lookup[k][j+1][i]].push_back(voxels.size()-1);
				this->node_covered_by_voxels[coordinate_index_lookup[k][j][i+1]].push_back(voxels.size()-1);
				this->node_covered_by_voxels[coordinate_index_lookup[k+1][j][i+1]].push_back(voxels.size()-1);
				this->node_covered_by_voxels[coordinate_index_lookup[k+1][j+1][i+1]].push_back(voxels.size()-1);
				this->node_covered_by_voxels[coordinate_index_lookup[k][j+1][i+1]].push_back(voxels.size()-1);
			}

}

