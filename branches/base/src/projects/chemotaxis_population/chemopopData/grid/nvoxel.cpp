/*
 * nvoxel.cpp
 *
 * this file contains the implementation of the classes given in nvoxel.hh
 *
 *@date Created on Nov 16, 2009
 *
 *@author garrit
 */

#include "nvoxel.hh"

using namespace Hive;

//////////////////// Implementation of Coordinate  ///////////////////////////////////////
Coordinate::Coordinate(Coordinate *c) : Data("coordinate", "coordinate_type") {
	this->coor.resize(c->coor.size());
	for (unsigned int i=0; i<coor.size(); i++)
		this->coor[i] = c->coor[i];
}

Coordinate::Coordinate(double x, double y, double z) : Data("coordinate", "coordinate_type") {
	this->coor.resize(3);
	coor[0] = x; coor[1] = y; coor[2] = z;
}

Coordinate::~Coordinate() {}

double Coordinate::calcDistance(Coordinate c) {
	double x,y,z;
	c.getCoordinates(x,y,z);
	double dist = (this->coor[0]-x)*(this->coor[0]-x) +
		(this->coor[1]-y)*(this->coor[1]-y) +
		(this->coor[2]-z)*(this->coor[2]-z);
	return pow(dist,0.5);
}

double Coordinate::calcDistance(Coordinate *c) {
	return this->calcDistance(*c);
}

bool Coordinate::smaller(Coordinate c){
	if (this->coor[0] < c.coor[0] && this->coor[1] < c.coor[1] && this->coor[2] < c.coor[2])
		return true;
	else
		return false;
}

bool Coordinate::larger(Coordinate c) {
	if (this->coor[0] > c.coor[0] && this->coor[1] > c.coor[1] && this->coor[2] > c.coor[2])
		return true;
	else
		return false;
}

void Coordinate::setCoordinates(double x, double y, double z) {
	if (coor.size() < 3) {
		coor.push_back(x);
		coor.push_back(y);
		coor.push_back(z);
	} else {
		coor[0] = x;
		coor[1] = y;
		coor[2] = z;
	}
}

void Coordinate::setCoordinates(Coordinate& c) {
		coor.push_back(c.coor[0]);
		coor.push_back(c.coor[1]);
		coor.push_back(c.coor[2]);
}

void Coordinate::getCoordinates(double& x, double& y, double& z) {
	x = coor[0];
	y = coor[1];
	z = coor[2];
}

void Coordinate::setCoordinatesToDifference(Coordinate c1, Coordinate c2) {
	double x1, y1, z1, x2, y2, z2;
	c1.getCoordinates(x1, y1, z1);
	c2.getCoordinates(x2, y2, z2);

	this->setCoordinates(x1-x2, y1-y2, z1-z2);
}

void Coordinate::setCoordinatesToSum(Coordinate c1, Coordinate c2) {
	double x1, y1, z1, x2, y2, z2;
	c1.getCoordinates(x1, y1, z1);
	c2.getCoordinates(x2, y2, z2);

	this->setCoordinates(x1+x2, y1+y2, z1+z2);
}

double Coordinate::calcScalarProduct(Coordinate c2) {
	double x1, y1, z1, x2, y2, z2;
	this->getCoordinates(x1, y1, z1);
	c2.getCoordinates(x2, y2, z2);

	return x1*x2 + y1*y2 + z1*z2;
}

void Coordinate::calcCrossProduct(Coordinate c1, Coordinate c2) {
	double x1, y1, z1, x2, y2, z2;
	c1.getCoordinates(x1, y1, z1);
	c2.getCoordinates(x2, y2, z2);

	this->setCoordinates(y1*z2 - y2*z1, z1*x2 - x1*z2, x1*y2 - y1*x2);
}

void Coordinate::multiplyWithScalar(double d) {
	for (int i=0; i<coor.size(); i++)
		coor[i] *= d;
}

void Coordinate::normalise() {
	double norm = pow(coor[0]*coor[0] + coor[1]*coor[1] + coor[2]*coor[2], 0.5);
	coor[0] /= norm;
	coor[1] /= norm;
	coor[2] /= norm;
}

void Coordinate::printContent(ostream& out) {
	out << coor[0] << " " << coor[1] << " " << coor[2] << endl;
}



//////////////////////////////////////////////////////////////////////////////////////////
////////////////////   Implementation of NVoxel    ///////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

NVoxel::NVoxel(NVoxel *n):Data("nvoxel","voxel_type") {
	this->concentrations.resize(n->concentrations.size());
	this->old_concentrations.resize(n->old_concentrations.size());
	for (unsigned int i=0; i<concentrations.size(); i++) {
		this->concentrations[i] = n->concentrations.size();
		this->old_concentrations[i] = n->old_concentrations[i];
	}
	this->neighbors.resize(n->neighbors.size());
	this->base_diffusion_rates.resize(n->base_diffusion_rates.size());
	for (unsigned int i=0; i<neighbors.size(); i++) {
		this->neighbors[i] = n->neighbors[i];
		this->base_diffusion_rates[i] = n->base_diffusion_rates[i];
	}
}

NVoxel::~NVoxel() {}

void NVoxel::addVertex(int v) {
	this->vertices.push_back(v);
}

void NVoxel::printContent(ostream& out) {
//	out << "voxel " << voxel_id << " has the following coordinates" << endl;
//	for (unsigned int i=0; i<vertices.size(); i++) {
//		Coordinate *lc = grid_ptr->getVertex(vertices[i]);
//		lc->printContent(out);
//	}

	cerr << voxel_id << ": " ;
	for (unsigned int i=0; i<vertices.size(); i++) {
		cerr << vertices[i] << " ";
	}
	cerr << endl;

}

double NVoxel::getBaseDiffusionRate(int n) {
	return this->base_diffusion_rates[n];
}

void NVoxel::getCoordinate(int i, Coordinate& c) {
	double x,y,z;
	if (i < vertices.size()) {
		Coordinate *lc = grid_ptr->getVertex(vertices[i]);
		lc->getCoordinates(x,y,z);
//		cerr << x << " " << y << " " << z << endl;
		c.setCoordinates(x,y,z);
//		cerr << c.getX() << " " << c.getY() << c.getZ() << endl;
	} else {
		cerr << "NVoxel::getCoordinate " << endl;
		exit(1);
	}
}

int NVoxel::getNumberNeighbors() {
	return this->neighbors.size();
}

int NVoxel::getNeighborID(int n) {
	return this->neighbors[n];
}

bool NVoxel::isBoundary(int wall) {
	for(int i=0; i<this->boundaries.size(); i++) {
		if(wall == boundaries[i])
			return true;
	}
	return false;
}

bool NVoxel::housesCoordinate(Coordinate& c) {
	if (this->grid_ptr->isQuadGrid())
		return this->housesCoordinateQuadGrid(c);
	else
		return this->housesCoordinateTetGrid(c);
}

bool NVoxel::housesCoordinateQuadGrid(Coordinate& c) {
	// local variables
	bool xokay = false;
	bool yokay = false;
	bool zokay = false;

	// first we need to get the coordinates of the vertices of this voxel
	Coordinate *v0 = grid_ptr->getVertex(this->vertices[0]);
	Coordinate *v1 = grid_ptr->getVertex(this->vertices[1]);
	Coordinate *v2 = grid_ptr->getVertex(this->vertices[2]);
	Coordinate *v3 = grid_ptr->getVertex(this->vertices[3]);
	Coordinate *v4 = grid_ptr->getVertex(this->vertices[4]);
	Coordinate *v5 = grid_ptr->getVertex(this->vertices[5]);
	Coordinate *v6 = grid_ptr->getVertex(this->vertices[6]);
	Coordinate *v7 = grid_ptr->getVertex(this->vertices[7]);


	// the x coordinate has to lie in between v0 and v1
	if (((v0->getX() <= c.getX()) && (c.getX() <= v1->getX())) ||
		((v1->getX() <= c.getX()) && (c.getX() <= v0->getX())))
		xokay = true;
	// the y coordinate has to lie in between v0 and v3
	if (((v0->getY() <= c.getY()) && (c.getY() <= v3->getY())) ||
		((v3->getY() <= c.getY()) && (c.getY() <= v0->getY())))
		yokay = true;
	// the z coordinate has to lie between v0 and v4
	if (((v0->getZ() <= c.getZ()) && (c.getZ() <= v4->getZ())) ||
		((v4->getZ() <= c.getZ()) && (c.getZ() <= v0->getZ())))
		zokay = true;
//	cerr << xokay << " " << yokay << " " << zokay << endl;
	return (xokay && yokay && zokay);
}

bool NVoxel::housesCoordinateTetGrid(Coordinate& c) {
	bool subvolume = false;

	double x,y,z;
	c.getCoordinates(x,y,z);

	double m[4][4];
	double backup[4][4];

	double res[5];

	m[0][0] = grid_ptr->getVertex(this->vertices[0])->getX();
	m[0][1] = grid_ptr->getVertex(this->vertices[0])->getY();
	m[0][2] = grid_ptr->getVertex(this->vertices[0])->getZ();
	m[0][3] = 1.;

	m[1][0] = grid_ptr->getVertex(this->vertices[1])->getX();
	m[1][1] = grid_ptr->getVertex(this->vertices[1])->getY();
	m[1][2] = grid_ptr->getVertex(this->vertices[1])->getZ();
	m[1][3] = 1.;

	m[2][0] = grid_ptr->getVertex(this->vertices[1])->getX();
	m[2][1] = grid_ptr->getVertex(this->vertices[1])->getY();
	m[2][2] = grid_ptr->getVertex(this->vertices[1])->getZ();
	m[2][3] = 1.;

	m[3][0] = grid_ptr->getVertex(this->vertices[1])->getX();
	m[3][1] = grid_ptr->getVertex(this->vertices[1])->getY();
	m[3][2] = grid_ptr->getVertex(this->vertices[1])->getZ();
	m[3][3] = 1.;

	for (int i=0; i<4; i++)
		for (int j=0; j<4; j++)
			backup[i][j] = m[i][j];

	// calculate D0
	res[0] = m[0][3]*m[1][2]*m[2][1]*m[3][0] - m[0][2]*m[1][3]*m[2][1]*m[3][0]
	                        - m[0][3]*m[1][1]*m[2][2]*m[3][0] + m[0][1]*m[1][3]*m[2][2]*m[3][0]
	                        + m[0][2]*m[1][1]*m[2][3]*m[3][0] - m[0][1]*m[1][2]*m[2][3]*m[3][0]
	                        - m[0][3]*m[1][2]*m[2][0]*m[3][1] + m[0][2]*m[1][3]*m[2][0]*m[3][1]
	                        + m[0][3]*m[1][0]*m[2][2]*m[3][1] - m[0][0]*m[1][3]*m[2][2]*m[3][1]
	                        - m[0][2]*m[1][0]*m[2][3]*m[3][1] + m[0][0]*m[1][2]*m[2][3]*m[3][1]
	                        + m[0][3]*m[1][1]*m[2][0]*m[3][2] - m[0][1]*m[1][3]*m[2][0]*m[3][2]
	                        - m[0][3]*m[1][0]*m[2][1]*m[3][2] + m[0][0]*m[1][3]*m[2][1]*m[3][2]
	                        + m[0][1]*m[1][0]*m[2][3]*m[3][2] - m[0][0]*m[1][1]*m[2][3]*m[3][2]
	                        - m[0][2]*m[1][1]*m[2][0]*m[3][3] + m[0][1]*m[1][2]*m[2][0]*m[3][3]
	                        + m[0][2]*m[1][0]*m[2][1]*m[3][3] - m[0][0]*m[1][2]*m[2][1]*m[3][3]
	                        - m[0][1]*m[1][0]*m[2][2]*m[3][3] + m[0][0]*m[1][1]*m[2][2]*m[3][3];

	// calculate D1
	for (int i=0; i<4; i++)
		for (int j=0; j<4; j++)
			m[i][j] = backup[i][j];
	m[0][0] = x;
	m[0][1] = y;
	m[0][2] = z;
	res[1] = m[0][3]*m[1][2]*m[2][1]*m[3][0] - m[0][2]*m[1][3]*m[2][1]*m[3][0]
	                        - m[0][3]*m[1][1]*m[2][2]*m[3][0] + m[0][1]*m[1][3]*m[2][2]*m[3][0]
	                        + m[0][2]*m[1][1]*m[2][3]*m[3][0] - m[0][1]*m[1][2]*m[2][3]*m[3][0]
	                        - m[0][3]*m[1][2]*m[2][0]*m[3][1] + m[0][2]*m[1][3]*m[2][0]*m[3][1]
	                        + m[0][3]*m[1][0]*m[2][2]*m[3][1] - m[0][0]*m[1][3]*m[2][2]*m[3][1]
	                        - m[0][2]*m[1][0]*m[2][3]*m[3][1] + m[0][0]*m[1][2]*m[2][3]*m[3][1]
	                        + m[0][3]*m[1][1]*m[2][0]*m[3][2] - m[0][1]*m[1][3]*m[2][0]*m[3][2]
	                        - m[0][3]*m[1][0]*m[2][1]*m[3][2] + m[0][0]*m[1][3]*m[2][1]*m[3][2]
	                        + m[0][1]*m[1][0]*m[2][3]*m[3][2] - m[0][0]*m[1][1]*m[2][3]*m[3][2]
	                        - m[0][2]*m[1][1]*m[2][0]*m[3][3] + m[0][1]*m[1][2]*m[2][0]*m[3][3]
	                        + m[0][2]*m[1][0]*m[2][1]*m[3][3] - m[0][0]*m[1][2]*m[2][1]*m[3][3]
	                        - m[0][1]*m[1][0]*m[2][2]*m[3][3] + m[0][0]*m[1][1]*m[2][2]*m[3][3];

	// calculate D2
	for (int i=0; i<4; i++)
		for (int j=0; j<4; j++)
			m[i][j] = backup[i][j];

	m[1][0] = x;
	m[1][1] = y;
	m[1][2] = z;
	res[2] = m[0][3]*m[1][2]*m[2][1]*m[3][0] - m[0][2]*m[1][3]*m[2][1]*m[3][0]
	                        - m[0][3]*m[1][1]*m[2][2]*m[3][0] + m[0][1]*m[1][3]*m[2][2]*m[3][0]
	                        + m[0][2]*m[1][1]*m[2][3]*m[3][0] - m[0][1]*m[1][2]*m[2][3]*m[3][0]
	                        - m[0][3]*m[1][2]*m[2][0]*m[3][1] + m[0][2]*m[1][3]*m[2][0]*m[3][1]
	                        + m[0][3]*m[1][0]*m[2][2]*m[3][1] - m[0][0]*m[1][3]*m[2][2]*m[3][1]
	                        - m[0][2]*m[1][0]*m[2][3]*m[3][1] + m[0][0]*m[1][2]*m[2][3]*m[3][1]
	                        + m[0][3]*m[1][1]*m[2][0]*m[3][2] - m[0][1]*m[1][3]*m[2][0]*m[3][2]
	                        - m[0][3]*m[1][0]*m[2][1]*m[3][2] + m[0][0]*m[1][3]*m[2][1]*m[3][2]
	                        + m[0][1]*m[1][0]*m[2][3]*m[3][2] - m[0][0]*m[1][1]*m[2][3]*m[3][2]
	                        - m[0][2]*m[1][1]*m[2][0]*m[3][3] + m[0][1]*m[1][2]*m[2][0]*m[3][3]
	                        + m[0][2]*m[1][0]*m[2][1]*m[3][3] - m[0][0]*m[1][2]*m[2][1]*m[3][3]
	                        - m[0][1]*m[1][0]*m[2][2]*m[3][3] + m[0][0]*m[1][1]*m[2][2]*m[3][3];


	// calculate D3
	for (int i=0; i<4; i++)
		for (int j=0; j<4; j++)
			m[i][j] = backup[i][j];

	m[2][0] = x;
	m[2][1] = y;
	m[2][2] = z;
	res[3]= m[0][3]*m[1][2]*m[2][1]*m[3][0] - m[0][2]*m[1][3]*m[2][1]*m[3][0]
	                        - m[0][3]*m[1][1]*m[2][2]*m[3][0] + m[0][1]*m[1][3]*m[2][2]*m[3][0]
	                        + m[0][2]*m[1][1]*m[2][3]*m[3][0] - m[0][1]*m[1][2]*m[2][3]*m[3][0]
	                        - m[0][3]*m[1][2]*m[2][0]*m[3][1] + m[0][2]*m[1][3]*m[2][0]*m[3][1]
	                        + m[0][3]*m[1][0]*m[2][2]*m[3][1] - m[0][0]*m[1][3]*m[2][2]*m[3][1]
	                        - m[0][2]*m[1][0]*m[2][3]*m[3][1] + m[0][0]*m[1][2]*m[2][3]*m[3][1]
	                        + m[0][3]*m[1][1]*m[2][0]*m[3][2] - m[0][1]*m[1][3]*m[2][0]*m[3][2]
	                        - m[0][3]*m[1][0]*m[2][1]*m[3][2] + m[0][0]*m[1][3]*m[2][1]*m[3][2]
	                        + m[0][1]*m[1][0]*m[2][3]*m[3][2] - m[0][0]*m[1][1]*m[2][3]*m[3][2]
	                        - m[0][2]*m[1][1]*m[2][0]*m[3][3] + m[0][1]*m[1][2]*m[2][0]*m[3][3]
	                        + m[0][2]*m[1][0]*m[2][1]*m[3][3] - m[0][0]*m[1][2]*m[2][1]*m[3][3]
	                        - m[0][1]*m[1][0]*m[2][2]*m[3][3] + m[0][0]*m[1][1]*m[2][2]*m[3][3];

	// calculate D4
	for (int i=0; i<4; i++)
		for (int j=0; j<4; j++)
			m[i][j] = backup[i][j];
	m[3][0] = x;
	m[3][1] = y;
	m[3][2] = z;
	res[4] = m[0][3]*m[1][2]*m[2][1]*m[3][0] - m[0][2]*m[1][3]*m[2][1]*m[3][0]
	                        - m[0][3]*m[1][1]*m[2][2]*m[3][0] + m[0][1]*m[1][3]*m[2][2]*m[3][0]
	                        + m[0][2]*m[1][1]*m[2][3]*m[3][0] - m[0][1]*m[1][2]*m[2][3]*m[3][0]
	                        - m[0][3]*m[1][2]*m[2][0]*m[3][1] + m[0][2]*m[1][3]*m[2][0]*m[3][1]
	                        + m[0][3]*m[1][0]*m[2][2]*m[3][1] - m[0][0]*m[1][3]*m[2][2]*m[3][1]
	                        - m[0][2]*m[1][0]*m[2][3]*m[3][1] + m[0][0]*m[1][2]*m[2][3]*m[3][1]
	                        + m[0][3]*m[1][1]*m[2][0]*m[3][2] - m[0][1]*m[1][3]*m[2][0]*m[3][2]
	                        - m[0][3]*m[1][0]*m[2][1]*m[3][2] + m[0][0]*m[1][3]*m[2][1]*m[3][2]
	                        + m[0][1]*m[1][0]*m[2][3]*m[3][2] - m[0][0]*m[1][1]*m[2][3]*m[3][2]
	                        - m[0][2]*m[1][1]*m[2][0]*m[3][3] + m[0][1]*m[1][2]*m[2][0]*m[3][3]
	                        + m[0][2]*m[1][0]*m[2][1]*m[3][3] - m[0][0]*m[1][2]*m[2][1]*m[3][3]
	                        - m[0][1]*m[1][0]*m[2][2]*m[3][3] + m[0][0]*m[1][1]*m[2][2]*m[3][3];

	// do the checking
	if ((res[0] > 0) && (res[1] > 0) && (res[2] > 0) && (res[3] > 0) && (res[4] > 0)) {
		subvolume = true;
     }
     if ((res[0] < 0) && (res[1] < 0) && (res[2] < 0) && (res[3] < 0) && (res[4] < 0)) {
    	 subvolume = true;
     }
     // point lies on the boundary of the voxel
     if ((res[1] == 0) || (res[2] == 0) || (res[3] == 0) || (res[4] == 0)) {
    	 subvolume = false;
     }
     return subvolume;
}

void NVoxel::setConcentration(int species, double value) {
	this->concentrations[species] = value;
}

void NVoxel::copyConcentration() {
	for (int i=0; i<concentrations.size(); i++)
		this->old_concentrations[i] = this->concentrations[i];
}

//////////////////// Implementation of OctTreeNode ///////////////////////////////////////
OctTreeNode::~OctTreeNode() {}

double OctTreeNode::getDistance(Coordinate c) {
}

void OctTreeNode::initialiseNode(double l, Coordinate c) {
	this->length = l;
	this->center = c;
	this->parent = NULL;
}

void OctTreeNode::initialiseNode(double l, Coordinate c, OctTreeNode *parent) {
	this->length = l;
	this->center = c;
	this->parent = parent;
}

NVoxel* OctTreeNode::findVoxel(Coordinate& c) {
	// if we have arrived at the leaf-level
	if (this->children.size() == 0) {
		for (unsigned int i=0; i<this->coveredVoxels.size(); i++) {
			if (this->coveredVoxels[i]->housesCoordinate(c))
				return this->coveredVoxels[i];
		}
	} else {
		if ((c.getX() < this->center.getX()) && (c.getY() < this->center.getY()) && (c.getZ() < this->center.getZ()))
			this->children[0]->findVoxel(c);
		else if ((c.getX() > this->center.getX()) && (c.getY() < this->center.getY()) && (c.getZ() < this->center.getZ()))
			this->children[1]->findVoxel(c);
		else if ((c.getX() < this->center.getX()) && (c.getY() > this->center.getY()) && (c.getZ() < this->center.getZ()))
			this->children[2]->findVoxel(c);
		else if ((c.getX() > this->center.getX()) && (c.getY() > this->center.getY()) && (c.getZ() < this->center.getZ()))
			this->children[3]->findVoxel(c);
		else if ((c.getX() < this->center.getX()) && (c.getY() < this->center.getY()) && (c.getZ() > this->center.getZ()))
			this->children[4]->findVoxel(c);
		else if ((c.getX() > this->center.getX()) && (c.getY() < this->center.getY()) && (c.getZ() > this->center.getZ()))
			this->children[5]->findVoxel(c);
		else if ((c.getX() < this->center.getX()) && (c.getY() > this->center.getY()) && (c.getZ() > this->center.getZ()))
			this->children[6]->findVoxel(c);
		else if ((c.getX() > this->center.getX()) && (c.getY() > this->center.getY()) && (c.getZ() > this->center.getZ()))
			this->children[7]->findVoxel(c);
		else {
			cerr << "in octTreeNode::findVoxel position is center of a voxel" << endl;
			exit(1);
		}
	}
}

void OctTreeNode::printContent(ostream& out) {
	Coordinate p1, p2, p3, p4, p5, p6, p7, p8;
	double x, y, z;
	center.getCoordinates(x, y, z);
	p1.setCoordinates(x - length/2, y - length/2, z - length/2);
	p2.setCoordinates(x + length/2, y - length/2, z - length/2);
	p3.setCoordinates(x + length/2, y + length/2, z - length/2);
	p4.setCoordinates(x - length/2, y + length/2, z - length/2);
	p5.setCoordinates(x - length/2, y - length/2, z + length/2);
	p6.setCoordinates(x + length/2, y - length/2, z + length/2);
	p7.setCoordinates(x + length/2, y + length/2, z + length/2);
	p8.setCoordinates(x - length/2, y + length/2, z + length/2);

	out << "OctTreeNode " << this->node_index << " has the following coordinates: " << endl;
	p1.printContent(out);
	p2.printContent(out);
	p3.printContent(out);
	p4.printContent(out);
	p5.printContent(out);
	p6.printContent(out);
	p7.printContent(out);
	p8.printContent(out);
	out << "it houses the following voxels: " << endl;
	for (unsigned int i=0; i<this->coveredVoxels.size(); i++)
		out << this->coveredVoxels[i]->getVoxelID() << endl;

}

bool OctTreeNode::coveringVoxel(NVoxel *voxel) {

	cerr << "in OctTreeNode::coveringVoxel" << endl;
	voxel->printContent(cout);

	// get corners of this voxel
	Coordinate p1, p2, p3, p4, p5, p6, p7, p8;
	double x, y, z;
	center.getCoordinates(x, y, z);
	p1.setCoordinates(x - length/2, y - length/2, z - length/2);
	p2.setCoordinates(x + length/2, y - length/2, z - length/2);
	p3.setCoordinates(x + length/2, y + length/2, z - length/2);
	p4.setCoordinates(x - length/2, y + length/2, z - length/2);
	p5.setCoordinates(x - length/2, y - length/2, z + length/2);
	p6.setCoordinates(x + length/2, y - length/2, z + length/2);
	p7.setCoordinates(x + length/2, y + length/2, z + length/2);
	p8.setCoordinates(x - length/2, y + length/2, z + length/2);
/*
	p1.printContent(cerr);
	p2.printContent(cerr);
	p3.printContent(cerr);
	p4.printContent(cerr);
	p5.printContent(cerr);
	p6.printContent(cerr);
	p7.printContent(cerr);
	p8.printContent(cerr);
*/
	// get the voxel's points
	Coordinate v1, v2, v3, v4;
	voxel->getCoordinate(0,v1);
	voxel->getCoordinate(1,v2);
	voxel->getCoordinate(2,v3);
	voxel->getCoordinate(3,v4);
/*
	cerr << "Voxel" << endl;
	v1.printContent(cerr);
	v2.printContent(cerr);
	v3.printContent(cerr);
	v4.printContent(cerr);
*/

	if (voxel->housesCoordinate(p1))
		return true;
	else if (voxel->housesCoordinate(p2))
		return true;
	else if (voxel->housesCoordinate(p3))
		return true;
	else if (voxel->housesCoordinate(p4))
		return true;
	else if (voxel->housesCoordinate(p5))
		return true;
	else if (voxel->housesCoordinate(p6))
		return true;
	else if (voxel->housesCoordinate(p7))
		return true;
	else if (voxel->housesCoordinate(p8))
		return true;


	// then we have to check whether a any of the point of the voxel
	// lies inside the node ...
	if (v1.smaller(p7) && v1.larger(p1))
		return true;
	else if (v2.smaller(p7) && v2.larger(p1))
		return true;
	else if (v3.smaller(p7) && v3.larger(p1))
		return true;
	else if (v4.smaller(p7) && v4.larger(p1))
		return true;
	// now: if none of the points lies inside the boundary of the node of the oct-tree,
	// we have to do the following check:
	// for each plane making up this node, we have to iterate over each of the
	// voxel's sides to check whether they intersect.
	// first plane
	if (lineIntersectingPlane(p1,p2,p3,v1,v2))
		return true;
	else if (lineIntersectingPlane(p1,p2,p3,v1,v3))
		return true;
	else if (lineIntersectingPlane(p1,p2,p3,v1,v4))
		return true;
	else if (lineIntersectingPlane(p1,p2,p3,v2,v3))
		return true;
	else if (lineIntersectingPlane(p1,p2,p3,v2,v4))
		return true;
	else if (lineIntersectingPlane(p1,p2,p3,v3,v4))
		return true;
	// second plane
	else if (lineIntersectingPlane(p1,p2,p6,v1,v2))
		return true;
	else if (lineIntersectingPlane(p1,p2,p6,v1,v3))
		return true;
	else if (lineIntersectingPlane(p1,p2,p6,v1,v4))
		return true;
	else if (lineIntersectingPlane(p1,p2,p6,v2,v3))
		return true;
	else if (lineIntersectingPlane(p1,p2,p6,v2,v4))
		return true;
	else if (lineIntersectingPlane(p1,p2,p6,v3,v4))
		return true;
	// third plane
	else if (lineIntersectingPlane(p1,p5,p8,v1,v2))
		return true;
	else if (lineIntersectingPlane(p1,p5,p8,v1,v3))
		return true;
	else if (lineIntersectingPlane(p1,p5,p8,v1,v4))
		return true;
	else if (lineIntersectingPlane(p1,p5,p8,v2,v3))
		return true;
	else if (lineIntersectingPlane(p1,p5,p8,v2,v4))
		return true;
	else if (lineIntersectingPlane(p1,p5,p8,v3,v4))
		return true;
	// fourth plane
	else if (lineIntersectingPlane(p2,p6,p7,v1,v2))
		return true;
	else if (lineIntersectingPlane(p2,p6,p7,v1,v3))
		return true;
	else if (lineIntersectingPlane(p2,p6,p7,v1,v4))
		return true;
	else if (lineIntersectingPlane(p2,p6,p7,v2,v3))
		return true;
	else if (lineIntersectingPlane(p2,p6,p7,v2,v4))
		return true;
	else if (lineIntersectingPlane(p2,p6,p7,v3,v4))
		return true;
	// fifth plane
	else if (lineIntersectingPlane(p4,p3,p7,v1,v2))
		return true;
	else if (lineIntersectingPlane(p4,p3,p7,v1,v3))
		return true;
	else if (lineIntersectingPlane(p4,p3,p7,v1,v4))
		return true;
	else if (lineIntersectingPlane(p4,p3,p7,v2,v3))
		return true;
	else if (lineIntersectingPlane(p4,p3,p7,v2,v4))
		return true;
	else if (lineIntersectingPlane(p4,p3,p7,v3,v4))
		return true;
	// sixth plane
	else if (lineIntersectingPlane(p5,p6,p7,v1,v2))
		return true;
	else if (lineIntersectingPlane(p5,p6,p7,v1,v3))
		return true;
	else if (lineIntersectingPlane(p5,p6,p7,v1,v4))
		return true;
	else if (lineIntersectingPlane(p5,p6,p7,v2,v3))
		return true;
	else if (lineIntersectingPlane(p5,p6,p7,v2,v4))
		return true;
	else if (lineIntersectingPlane(p5,p6,p7,v3,v4))
		return true;
	else {
		return false;
	}
}

bool OctTreeNode::lineIntersectingPlane(Coordinate& p1, Coordinate& p2, Coordinate& p3, Coordinate& l1, Coordinate& l2) {

//	cerr << "in OctTreeNode::lineIntersectingPlane" << endl;
//	cerr << "plane spanned  by: " << endl;
//	p1.printContent(cerr);
//	p2.printContent(cerr);
//	p3.printContent(cerr);
//	cerr << "line given by start and end point: " << endl;
//	l1.printContent(cerr);
//	l2.printContent(cerr);


	bool intersect = false;

	Coordinate diff1, diff2, norm, unit, intersection;
	double distance, s1, s2, c, alpha;
	double x1, y1, z1, x2, y2, z2, x3, y3, z3, xi, yi, zi;
	double xmin, xmax, ymin, ymax, zmin, zmax;


	// first get the quantities that we need  for testing
	// calculate the normal vector of the plane spanned by p1, p2, p3
	diff1.setCoordinatesToDifference(p2, p1);
	diff2.setCoordinatesToDifference(p3, p1);

	norm.calcCrossProduct(diff1, diff2);
	norm.normalise();
//	cerr << "the norm of the plane is given by: " << endl;
//	norm.printContent(cerr);

	// calculate distance of plane from the origin
	distance = p1.calcScalarProduct(norm);
//	cerr << "distance: " << distance << endl;
	// calculate unit vector of the line spanned by l1 and l2
	unit.setCoordinatesToDifference(l2, l1);
	unit.normalise();
//	cerr << "unit vector spanned by the two points" << endl;
//	unit.printContent(cerr);

	// now do the actual testing:
	// check whether the two points are on opposite sides of the plane
	// spanned by p1-p3
	s1 = l1.calcScalarProduct(norm) - distance;
	s2 = l2.calcScalarProduct(norm) - distance;
//	cerr << s1 << "  " << s2 << endl;
	// if so
	if ((s1 > 0 && s2 < 0) || (s1 < 0 && s2 > 0) ) {
//		cerr << "hier in intersection" << endl;
		// calculate intersection point
		c     = unit.calcScalarProduct(norm);
		alpha = (distance - l1.calcScalarProduct(norm))/c;
		unit.multiplyWithScalar(alpha);
		intersection.setCoordinatesToSum(l1,unit);
		// now check whether the intersection point is in between the points
		// NOTE we always assume that x1 is the lower left point and x3 is the upper right point

		intersection.getCoordinates(xi, yi, zi);
//		intersection.printContent(cerr);
		xmin = INT_MAX; ymin = INT_MAX; zmin = INT_MAX;
		xmax = INT_MIN; zmax = INT_MIN; zmax = INT_MIN;

//		cerr << "hier1: " << endl;
//		p1.printContent(cerr);
//		p2.printContent(cerr);
//		p3.printContent(cerr);
//		cerr << "hier2" << endl;

		p1.getCoordinates(x1, y1, z1);
		p2.getCoordinates(x2, y2, z2);
		p3.getCoordinates(x3, y3, z3);

		if (x1 >= xmax)
			xmax = x1;
		if (x2 >= xmax)
			xmax = x2;
		if (x3 >= xmax)
			xmax = x3;
		if (x1 <= xmin)
			xmin = x1;
		if (x2 <= xmin)
			xmin = x2;
		if (x3 <= xmin)
			xmin = x3;

		if (y1 >= ymax)
			ymax = y1;
		if (y2 >= ymax)
			ymax = y2;
		if (y3 >= ymax)
			ymax = y3;
		if (y1 <= ymin)
			ymin = y1;
		if (y2 <= ymin)
			ymin = y2;
		if (y3 <= ymin)
			ymin = y3;

		if (z1 >= xmax)
			zmax = z1;
		if (z2 >= zmax)
			zmax = z2;
		if (z3 >= zmax)
			zmax = z3;
		if (z1 <= zmin)
			zmin = z1;
		if (z2 <= zmin)
			zmin = z2;
		if (z3 <= zmin)
			zmin = z3;

//		cerr << xmin << " " << ymin << " " << zmin << endl;
//		cerr << xmax << " " << ymax << " " << zmax << endl;
//		p1.printContent(cerr);
//		p3.printContent(cerr);
		// here we have to determine xmin, xmax, ymin, ymax, zmin, zmax;
		if (xi>= xmin && yi >= ymin && zi >= zmin && xi <= xmax && yi <= ymax && zi <= zmax)
			intersect = true;
		else
			intersect = false;
		return intersect;
	} else if ((s1 > 0 && s2 > 0) || (s1 < 0 && s2 < 0)) {
		return intersect;
	} else {
		/*
		cerr << "in OctTreeNode::lineIntersectingPlane at least one point lies exactly on the surface spanned by p1-p3"
			 << endl;
		cerr << s1 << " " << s2 << endl;
		exit(1);
		*/
		return intersect;
	}
}

bool OctTreeNode::pointOnTriangle(Coordinate& c, Coordinate& p1, Coordinate& p2, Coordinate& p3) {
	// declare vectors
	Coordinate v0;
	Coordinate v1;
	Coordinate v2;

	// calculate vectors
	v0.setCoordinatesToDifference(p3, p1);
	v1.setCoordinatesToDifference(p2, p1);
	v2.setCoordinatesToDifference(c, p1);

	// calculate dot products
	double dot00 = v0.calcScalarProduct(v0);
	double dot01 = v0.calcScalarProduct(v1);
	double dot02 = v0.calcScalarProduct(v2);
	double dot11 = v1.calcScalarProduct(v1);
	double dot12 = v1.calcScalarProduct(v2);

	// Compute barycentric coordinates
	double invDenom = 1./(dot00*dot11-dot01*dot01);
	double u        = (dot11*dot02-dot01*dot12)*invDenom;
	double v        = (dot00*dot12-dot01*dot02)*invDenom;

	// Check if point is in triangle
	return (u > 0) && (v > 0) && (u + v < 1);

}

////////////////////   Implementation of OctTree   ///////////////////////////////////////
OctTree::~OctTree() {}

void OctTree::addVoxelToTree(OctTreeNode *node, NVoxel *vox) {
	// if this node has no children
	if (node->children.size() == 0) {
//		if (node->node_index == 1) {
//			cerr << "adding to one " << vox->getVoxelID() << endl;
//			node->center.printContent(cerr) ;
//		}
		// add voxel to the nodes covered by this voxel
		node->coveredVoxels.push_back(vox);
		// if the node is full
		if (node->coveredVoxels.size() > node->OCT_TREE_LIMIT) {
			// remember which voxels were covered by the node
			vector<NVoxel* > temp = node->coveredVoxels;
			// clear coveredVoxels vector
			node->coveredVoxels.clear();
			node->coveredVoxels.resize(0);
			// add children to this node
			node->children.resize(8);
			for (int i=0; i<8; i++) {
				node->children[i] = new OctTreeNode();
				node->children[i]->node_index = total_number_nodes++;
				Coordinate center;
				center.setCoordinates(node->center);
				if (i == 0 || i == 2 || i ==4 || i == 6)
					center.coor[0] -= node->length/4.;
				else
					center.coor[0] += node->length/4.;
				if (i == 0 || i == 1 || i == 4 || i == 5)
					center.coor[1] -= node->length/4.;
				else
					center.coor[1] += node->length/4.;
				if (i == 0 || i == 1 || i == 2 || i == 3)
					center.coor[2] -= node->length/4.;
				else
					center.coor[2] += node->length/4.;
				node->children[i]->initialiseNode(node->length/2., center, node);
			}
			for (unsigned int i=0; i<temp.size(); i++)
				addVoxelToTree(node,temp[i]);
		}
	} else {
		for (unsigned int i=0; i<node->children.size(); i++)
			if (node->children[i]->coveringVoxel(vox)) {
			//	if (node->node_index == 1)
			//		cerr << "adding shit to voxel number one" << endl;
				this->addVoxelToTree(node->children[i],vox);
				cerr << "adding " << endl ; //vox->getVoxelID() << " to " << node->children[i]->node_index <<  endl;
			}
	}
}

void OctTree::createTree(vector<NVoxel*>& voxels, vector<Coordinate* >& vertices) {
	double x,y,z;
	if (root_node != NULL)
		deleteTree(root_node);

	// determine bounding box
	double length = 0;
	double min_max[2][3];
	for (int j=0; j<3; j++) {
		min_max[0][j] = INT_MAX;
		min_max[1][j] = INT_MIN;
	}
	for (unsigned int i = 0; i<vertices.size(); i++) {
		vertices[i]->getCoordinates(x,y,z);
		if (x<= min_max[0][0])
			min_max[0][0] = x - 0.001;
		if (y <= min_max[0][1])
			min_max[0][1] = y - 0.001;
		if (z <= min_max[0][2])
			min_max[0][2] = z -0.001;
		if (x >= min_max[1][0])
			min_max[1][0] = x + 0.001;
		if (y >= min_max[1][1])
			min_max[1][1] = y + 0.001;
		if (z >= min_max[1][2])
			min_max[1][2] = z + 0.001;
	}

	for (unsigned int i=0; i<3; i++)
		if (min_max[1][i] - min_max[0][i] > length)
			length = min_max[1][i] - min_max[0][i];

	// initialise the root of the tree
	Coordinate center;
	center.setCoordinates(min_max[0][0] + length/2, min_max[0][1] + length/2, min_max[0][2] + length/2);
//	cerr << min_max[0][0] << " " << min_max[0][1] << " " << min_max[0][2] << endl;
//	center.printContent(cerr);
	this->root_node = new OctTreeNode();
	root_node->initialiseNode(length,center);
	root_node->node_index = 0;
//	cerr << "after initialising root" << endl;

//	cerr << root_node->coveringVoxel(voxels[0]) << endl;
//	cerr << true << endl;
// 	exit(1);

	for (unsigned int i=0; i<voxels.size(); i++) {
		addVoxelToTree(root_node, voxels[i]);
		cerr << "added " << i << " of " << voxels.size() << endl;
	}

}

void OctTree::deleteTree(OctTreeNode *node) {
	if (node != NULL) {
		if (node->children.size() != 0)
			for (int i=0; i<8; i++)
				deleteTree(node->children[i]);
			delete this;
	}
}

NVoxel* OctTree::findVoxel(Coordinate& c) {
	return this->root_node->findVoxel(c);
}

void OctTree::printContent(ostream& out) {
	out << "printContent of OctTree: not yet implemented" << endl;
}








