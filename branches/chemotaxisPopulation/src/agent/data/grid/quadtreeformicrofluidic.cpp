#include "quadtreeformicrofluidic.hh"

using namespace std;
using namespace Hive;

/// implementation of the quadtree node

QuadTreeNodeForMicroFluidic::QuadTreeNodeForMicroFluidic(string name, string type) : Data(name, type) {
	this->parent = NULL;
	this->isleaf = true;
}

QuadTreeNodeForMicroFluidic::QuadTreeNodeForMicroFluidic(QuadTreeNodeForMicroFluidic *p, int num) : Data("quad_tree_node", "quad_tree_node_type"){
	//	cerr << "inside QuadTreeNodeForMicroFluidic::QuadTreeNodeForMicroFluidic(QuadTreeNodeForMicroFluidic, num)" << endl;

	// tell node that it is a leaf
	this->isleaf = true;
	// set the parent
	this->parent = p;
	// set the capactiy
	this->capacity = p->capacity;
	// set the resolution
	this->resolution = p->resolution;

	// set up the coordinates of the bounding box
	this->bounding_box_coordinates.resize(4);
	if (num == 0) {
		this->bounding_box_coordinates[0] = p->bounding_box_coordinates[0];
		this->bounding_box_coordinates[1].setCoordinates(p->center.getX(), p->bounding_box_coordinates[0].getY(), p->center.getZ());
		this->bounding_box_coordinates[2] = p->center;
		this->bounding_box_coordinates[3].setCoordinates(p->bounding_box_coordinates[0].getX(), p->center.getY(), p->center.getZ());
		this->node_id = this->used_ids++;
	} else if (num == 1) {
		this->bounding_box_coordinates[0].setCoordinates(p->center.getX(), p->bounding_box_coordinates[0].getY(), p->center.getZ());
		this->bounding_box_coordinates[1] = p->bounding_box_coordinates[1];
		this->bounding_box_coordinates[2].setCoordinates(p->bounding_box_coordinates[1].getX(), p->center.getY(), p->center.getZ());
		this->bounding_box_coordinates[3] = p->center;
		this->node_id = this->used_ids++;
	} else if (num == 2) {
		this->bounding_box_coordinates[0] = p->center;
		this->bounding_box_coordinates[1].setCoordinates(p->bounding_box_coordinates[1].getX(), p->center.getY(), p->center.getZ());
		this->bounding_box_coordinates[2] = p->bounding_box_coordinates[2];
		this->bounding_box_coordinates[3].setCoordinates(p->center.getX(), p->bounding_box_coordinates[2].getY(), p->center.getZ());
		this->node_id = this->used_ids++;
	} else if (num == 3) {
		this->bounding_box_coordinates[0].setCoordinates(p->bounding_box_coordinates[0].getX(), p->center.getY(), p->center.getZ());
		this->bounding_box_coordinates[1] = p->center;
		this->bounding_box_coordinates[2].setCoordinates(p->center.getX(), p->bounding_box_coordinates[2].getY(), p->center.getZ());
		this->bounding_box_coordinates[3] = p->bounding_box_coordinates[3];
		this->node_id = this->used_ids++;
	}
	// set the coordinates of the center
	double cx, cy, cz;
	cx = (this->bounding_box_coordinates[0].getX() + this->bounding_box_coordinates[1].getX())/2.;
	cy = (this->bounding_box_coordinates[0].getY() + this->bounding_box_coordinates[3].getY())/2.;
	if (cx - resolution*((int)(cx/resolution))!=0)
		cx -= (cx-resolution*((int)(cx/resolution)));
	if (cy - resolution*((int)(cy/resolution))!=0)
		cy -= (cy-resolution*((int)(cy/resolution)));
	cz = p->center.getZ();
	this->center.setCoordinates(cx,cy,cz);

	//	cerr << "bounding_box" << endl;
	//	for (int i=0; i<4; i++)
	//		bounding_box_coordinates[i].printContent(cerr);

	//	cerr << "center" << endl;
	//	center.printContent(cerr);
}

/// this constructor is used for constructing the root of the quadtree. it should not really be used
/// otherwise.
QuadTreeNodeForMicroFluidic::QuadTreeNodeForMicroFluidic(Coordinate center, int capacity):Data("quad_tree_node", "quad_tree_node_type") {
	//	cerr << "inside QuadTreeNodeForMicroFluidic::QuadTreeNodeForMicroFluidic(double, Coordinate, int)" << endl;
	this->node_id = this->used_ids;
	this->used_ids++;

	// set the center coordinate
	this->center.setCoordinates(center.getX(),center.getY(),center.getZ());
	//	cerr << "center coordinates: ";
	//	center.printContent(cerr);

	// set the capacity
	this->capacity = capacity;
	// each node starts its life as a leaf
	this->isleaf  = true;

	// note that the bounding box is set by the initialiseRoot method of the tree
}

QuadTreeNodeForMicroFluidic::~QuadTreeNodeForMicroFluidic() { }

void QuadTreeNodeForMicroFluidic::addVoxel(NVoxel *v) {
	//	cerr << "inside addVoxel" << endl;
	// add voxel to this tree node if it is a leaf and the voxel lies within the bound of this tree node
	if (this->isleaf && this->voxelWithinBounds(v)) {
		this->covered_voxels.push_back(v);
		if (covered_voxels.size() > capacity) { // capacity has been reached
			// this node is no longer a leaf
			this->isleaf = false;
			/// make room for the children
			children.resize(4);
			children[0] = new QuadTreeNodeForMicroFluidic(this,0);
			children[1] = new QuadTreeNodeForMicroFluidic(this,1);
			children[2] = new QuadTreeNodeForMicroFluidic(this,2);
			children[3] = new QuadTreeNodeForMicroFluidic(this,3);
			// iterate over covered voxels and try to add them to the children
			for (unsigned int i=0; i<covered_voxels.size(); i++) {
				if (this->children[0]->voxelWithinBounds(covered_voxels[i])) {
					this->children[0]->addVoxel(covered_voxels[i]);
					//	cerr << "adding voxel" << i << " to child0" << endl;
				}
				if (this->children[1]->voxelWithinBounds(covered_voxels[i])) {
					this->children[1]->addVoxel(covered_voxels[i]);
					//	cerr << "adding voxel" << i << " to child1" << endl;
				}
				if (this->children[2]->voxelWithinBounds(covered_voxels[i])) {
					this->children[2]->addVoxel(covered_voxels[i]);
					//	cerr << "adding voxel" << i << " to child2" << endl;
				}
				if (this->children[3]->voxelWithinBounds(covered_voxels[i])) {
					this->children[3]->addVoxel(covered_voxels[i]);
					//	cerr << "adding voxel" << i << " to child3" << endl;
				}
			}
		}
	} else if(!this->isleaf) { // if this voxel is not a leaf try to add it to its children
		if (this->children[0]->voxelWithinBounds(v)) {
			this->children[0]->addVoxel(v);
			//	cerr << "adding to children[0]" << endl;
		}
		if (this->children[1]->voxelWithinBounds(v)) {
			this->children[1]->addVoxel(v);
			//	cerr << "adding to children[1]" << endl;
		}
		if (this->children[2]->voxelWithinBounds(v)) {
			this->children[2]->addVoxel(v);
			//	cerr << "adding to children[2]" << endl;
		}
		if (this->children[3]->voxelWithinBounds(v)) {
			this->children[3]->addVoxel(v);
			//	cerr << "adding to children[3]" << endl;
		}
	}
}

bool QuadTreeNodeForMicroFluidic::voxelWithinBounds(NVoxel* v) {
	Coordinate c0, c1, c2, c3;
	/// get the four bounding coordinates
	v->getCoordinate(0,c0);
	v->getCoordinate(1,c1);
	v->getCoordinate(2,c2);
	v->getCoordinate(3,c3);
	//	cerr << "inside voxelWithinBounds" << endl;
	//	c0.printContent(cerr);
	//	c1.printContent(cerr);
	//	c2.printContent(cerr);
	//	c3.printContent(cerr);

	// not that we do not consider the z coordinate below
	bool check0, check1, check2, check3;
	/// check whether coordinate c0 is inside the bounding box
	if (c0.getX() >= this->bounding_box_coordinates[0].getX() &&
		c0.getX() <  this->bounding_box_coordinates[1].getX() &&
		c0.getY() <= this->bounding_box_coordinates[0].getY() &&
		c0.getY() >  this->bounding_box_coordinates[3].getY())
		check0 = true;
	else
		check0 = false;
	/// check whether coordinate c1 is inside the bounding box
	if (c1.getX() >  this->bounding_box_coordinates[0].getX() &&
		c1.getX() <= this->bounding_box_coordinates[1].getX() &&
		c1.getY() <= this->bounding_box_coordinates[0].getY() &&
		c1.getY() >  this->bounding_box_coordinates[3].getY())
		check1 = true;
	else
		check1 = false;
	/// check whether coordinate c2 is inside the bounding box
	if (c2.getX() > this->bounding_box_coordinates[3].getX() &&
		c2.getX() <=  this->bounding_box_coordinates[2].getX() &&
		c2.getY() <  this->bounding_box_coordinates[0].getY() &&
		c2.getY() >= this->bounding_box_coordinates[3].getY())
			check2 = true;
		else
			check2 = false;
	/// check whether coordinate c3 is inside the bounding box
	if (c3.getX() >= this->bounding_box_coordinates[3].getX() &&
		c3.getX() <  this->bounding_box_coordinates[2].getX() &&
		c3.getY() <  this->bounding_box_coordinates[0].getY() &&
		c3.getY() >= this->bounding_box_coordinates[3].getY())
		check3 = true;
	else
		check3 = false;
	//	cerr << check0 << " " << check1 << " " << check2 << " " << check3 << endl;

	// return
	if (check0 && check1 && check2 && check3)
		return true;
	else
		return false;
}

/// returns voxel index in which the coordinate c lies
int QuadTreeNodeForMicroFluidic::coordinateInsideCoveredVoxels(Coordinate& c) {
	bool success = false;
	if (!this->isleaf) { // this is not a leaf the coordinate has to propagate downward
		// we need to find out in which quadrant the coordinate will lie.
		int quad = this->getQuadrant(c);
		// recursive call with the correct child
		return this->children[quad]->coordinateInsideCoveredVoxels(c);
	} else { // we have reached the bottom of the tree
		//		cerr << "hier " << this->covered_voxels.size() << " " <<  endl;
		//		cerr << this->covered_voxels[0]->getVoxelID() << endl;
		//		cerr << true << " " << this->covered_voxels[0]->housesCoordinate(c) << endl;
		for (unsigned int i=0; i<this->covered_voxels.size(); i++) {
			if (this->covered_voxels[i]->housesCoordinate(c)) {
				success = true;
				return covered_voxels[i]->getVoxelID();
				break;
			}
		}
		if (!success)
			return -1;
	}
}


int QuadTreeNodeForMicroFluidic::coordinateInsideCoveredVoxels(Coordinate *c) {
	Coordinate c1;
	c1.setCoordinates(c->getX(), c->getY(), c->getZ());
	return coordinateInsideCoveredVoxels(c1);
}

int QuadTreeNodeForMicroFluidic::getQuadrant(Coordinate& c) {
	int result;
	if (c.getX() < this->center.getX() && c.getY() >= this->center.getY())
		result = 0;
	else if (c.getX() >= this->center.getX() && c.getY() >= this->center.getY())
		result = 1;
	else if (c.getX() >= this->center.getX() && c.getY() < this->center.getY())
		result = 2;
	else if (c.getX() < this->center.getX() && c.getY() < this->center.getY())
		result = 3;
	return result;
}

void QuadTreeNodeForMicroFluidic::printContent(ostream& out) {
	if (this->isleaf) {
		out << this->node_id << " covers voxels ";
		for (unsigned int i=0; i<covered_voxels.size(); i++)
			out << covered_voxels[i]->getVoxelID() << " ";
		out << endl;
	} else {
		for (unsigned int i=0; i<children.size(); i++)
			children[i]->printContent(out);
	}
}

/// implementation of the quadtree
QuadTreeForMicroFluidic::QuadTreeForMicroFluidic(string name, string type) : Data(name,type) {
	this->root       = NULL;
	this->g          = NULL;
	this->resolution = 0;
}

QuadTreeForMicroFluidic::QuadTreeForMicroFluidic(Grid *g) : Data("quad_tree_for_micro_fluidic", "quad_tree_type") {
	this->resolution = 0;
	this->g          = g;
	this->constructTree(g);
}

QuadTreeForMicroFluidic::~QuadTreeForMicroFluidic() { }

/// note that this will produce rubbish, if there is already a grid in the quad tree
void QuadTreeForMicroFluidic::constructTree(Grid *g) {
	// set grid
	this->g = g;
	// initialise the root node
	this->initialiseRoot();
	// add voxels to tree
	for (int i=0; i<g->getNumberOfVoxels(); i++)
		root->addVoxel(g->getVoxel(i));
}

int QuadTreeForMicroFluidic::getVoxelIndex(Coordinate& c) {
	return root->coordinateInsideCoveredVoxels(c);
}

int QuadTreeForMicroFluidic::getVoxelIndex(Coordinate *c) {
	Coordinate c1;
	c1.setCoordinates(c->getX(), c->getY(), c->getZ());
	return root->coordinateInsideCoveredVoxels(c1);
}

NVoxel* QuadTreeForMicroFluidic::getVoxel(Coordinate& c) {
	int index = root->coordinateInsideCoveredVoxels(c);
	return g->getVoxel(index);
}

NVoxel* QuadTreeForMicroFluidic::getVoxel(Coordinate *c) {
	Coordinate c1;
	c1.setCoordinates(c->getX(), c->getY(), c->getZ());
	int index = root->coordinateInsideCoveredVoxels(c1);
	return g->getVoxel(index);
}

void QuadTreeForMicroFluidic::printContent(ostream& out) {
	this->root->printContent(out);
}

void QuadTreeForMicroFluidic::initialiseRoot() {
	// local variables
	double xlength, ylength;
	double x,y;
	double minx, miny, maxx, maxy;
	double xcen, ycen;
	int    capacity;
	Coordinate center;

	// check whether a grid has been assigned
	if (this->g == NULL) {
		throw HiveException("grid has not been assigned", "QuadTreeForMicroFluidic::initialiseRoot()");
		exit(1);
	}

	// determine the resolution of the grid
	if (this->resolution == 0) {
		double x1 = g->getVertexOfVoxel(0,0)->getX();
		double x2 = g->getVertexOfVoxel(1,0)->getX();
		this->resolution = x2-x1;
	}

	// iterate over the grid and get the center coordinate and determine the maximal extension.
	// the center coordinate will become the root coordinate and needs to be identical with one of
	// the coordinates of the grid. otherwise a voxel may lie in multiple quadrants which has to be avoided.
	vector<Coordinate* >& gv = g->getCoordinates();
	minx = numeric_limits<double>::max();
	miny = numeric_limits<double>::max();
	maxx = numeric_limits<double>::min();
	maxy = numeric_limits<double>::min();
	for (unsigned int i=0; i<gv.size(); i++) {
		x = gv[i]->getX();
		y = gv[i]->getY();
		if (x < minx)
			minx = x;
		if (y < miny)
			miny = y;
		if (x > maxx)
			maxx = x;
		if (y > maxy)
			maxy = y;
	}
	// extensions in x and y direction
	xlength = maxx - minx;
	ylength = maxy - miny;

	// determine center coordinate
	xcen = minx + xlength/2;
	ycen = miny + ylength/2;
	// here we have to check whether the center is a coordinate or a center of a voxel
	// the latter is not allowed.
	//	cerr << xlength << " " << ylength << endl;
	//	cerr << xcen << " " << ycen << endl;
	if (xcen - resolution*((int)(xcen/resolution)) > 0) {
		xcen -= (xcen - resolution*((int)(xcen/resolution)));
	}
	if (ycen - resolution*((int)(ycen/resolution)) != 0) {
		ycen -= (ycen - resolution*((int)(ycen/resolution)));
	}
	center.setCoordinates(xcen,ycen,0);
	//	center.printContent(cerr);

	// we are hard coding the capacity i.e. the number of voxels a node can store to 9 ... not good.
	capacity = 4;
	this->root = new QuadTreeNodeForMicroFluidic(center, capacity);
	this->root->resolution = this->resolution;
	this->root->bounding_box_coordinates.resize(4);
	this->root->bounding_box_coordinates[0].setCoordinates(minx, maxy, 0);
	this->root->bounding_box_coordinates[1].setCoordinates(maxx, maxy, 0);
	this->root->bounding_box_coordinates[2].setCoordinates(maxx, miny, 0);
	this->root->bounding_box_coordinates[3].setCoordinates(minx, miny, 0);
}


