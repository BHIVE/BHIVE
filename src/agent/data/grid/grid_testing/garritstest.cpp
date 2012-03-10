#include "grid.hh"
#include "nvoxel.hh"

#include <iostream>
#include <vector>

using namespace Chemotaxis;
using namespace Hive;
using namespace std;

int main () {

  Coordinate n1, n2, n3, n4, n5, n6, n7, n8;
  Coordinate v1, v2, v3, v4;

  Coordinate c1;
  c1.setCoordinates(1.0,1.0,1.0);

  n1.setCoordinates(0.0,0.0,0.0);
  n2.setCoordinates(2.0,0.0,0.0);
  n3.setCoordinates(2.0,2.0,0.0);
  n4.setCoordinates(0.0,2.0,0.0);
  n5.setCoordinates(0.0,0.0,2.0);
  n6.setCoordinates(2.0,0.0,2.0);
  n7.setCoordinates(2.0,2.0,2.0);
  n8.setCoordinates(0.0,2.0,2.0);
  
  v1.setCoordinates(-0.5,0.0,0.0);
  v2.setCoordinates(2.0,0.0,0.0);
  v3.setCoordinates(2.0,0.0,2.0);
  v4.setCoordinates(2.0,2.0,0.0);

  OctTreeNode otn;
  otn.initialiseNode(2.0,c1);

  otn.printContent(cout);

  Grid g;
  g.addCoordinate(&v1);
  g.addCoordinate(&v2);
  g.addCoordinate(&v3);
  g.addCoordinate(&v4);

  NVoxel v;
  v.vertices.push_back(0);
  v.vertices.push_back(1);
  v.vertices.push_back(2);
  v.vertices.push_back(3);
  v.grid_ptr = &g;
  
  //  v.printContent(cout);
  cout << otn.coveringVoxel(&v) << " " << true << endl;
  
  //  cout << otn.lineIntersectingPlane(n1, n5, n8, v2, v1) << endl;
  return 0;

}
