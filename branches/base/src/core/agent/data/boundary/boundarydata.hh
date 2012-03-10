/*
 * mathfunctiondata.hh
 *
 *  Created on: Sept, 2011
 *      Author: msneddon
 */

#ifndef BOUNDARYDATA_HH_
#define BOUNDARYDATA_HH_

#include "../data.hh"
#include "../../../exception/exception.hh"
#include "../../../util/util.hh"

#include <cmath>
#include <iostream>
#include <vector>
#include <exception>

using namespace std;

namespace Hive {
	// simple boundary class that just can reflect a vector if the vector attempts
    // to cross it.  Can be used by worlds, and is used for chemotaxis simulations.
    // boundaries can only be defined in the x,y,or z directions (not arbitrary directions)
    // and are infinite in the other two dimensions
	class SimpleBoundaryData : public Data {
	public:
		SimpleBoundaryData(string dataName):Data(dataName,"BoundaryType") {

		};
		virtual ~SimpleBoundaryData() {};


		virtual void printContent(ostream& out){out<<"hello, i am your friendly boundary.\n";};

		// coordinate is 0 for x, 1 for y, or 2 for z.
		// position is the value along the coordinate which will reflect
		// boundaryType indicates the direction of the plane.  set to true reflects cells
		// moving from the
		void setCoordinate(int coordinate, double position, int boundaryType) {
			if(coordinate<0 || coordinate>2) {
				cerr<<"setCoordinate(): coordinate must be 0, 1, or 2 (for x, y, or z)"<<endl; exit(0);
			}
			this->coordinate = coordinate;
			this->position = position;
			this->boundaryType = boundaryType;
		};

		// this reflection is not that smart- all it does is redirect the vector and postion, and does not
		// actually chop up the track to make a true reflection by updating the distTraveled.  This is clearly
		// a hack for now and is admittedly ugly.
		void reflectIfNeeded(const double (&startPoint) [3], double (&endPoint) [3], double (&direction) [3], double distTraveled) {
			// check if we started out of bounds, in which case throw an error

			if(boundaryType==blockBothCrossings) {
				if(startPoint[coordinate]<position && endPoint[coordinate]>=position) {
					direction[coordinate]=-direction[coordinate];
					redirect(startPoint,endPoint,direction,distTraveled);
				}
				if(startPoint[coordinate]>position && endPoint[coordinate]<=position)
				{
					direction[coordinate]=-direction[coordinate];
					redirect(startPoint,endPoint,direction,distTraveled);
				}

			} else if (boundaryType==blockPlusToMinusErrorOtherwise) {
				if(startPoint[coordinate]<=position) {
					cerr<<"\n\nCell started out of bounds!  This may occur if boundaries are set closer to each other \n";
					cerr<<"Than a cell's maximum step, because this reflect just sends cells the other way and does\n";
					cerr<<"not account for multiple reflections off multiple boundaries in any nice way."<<endl;
					cerr<<"Error may also occur if you began cells outside of a boundary."<<endl;
					cerr<<"cell started at: "<< startPoint[0] <<" "<<startPoint[1]<<" "<<startPoint[2]<<endl;
					cerr<<"cell trying to move to: "<< endPoint[0] <<" "<<endPoint[1]<<" "<<endPoint[2]<<endl;
					cerr<<"boundary is: coordinate:"<< coordinate <<" position:"<<position<<" type:"<<boundaryType<<endl;
					exit(1);
				}
				if(endPoint[coordinate]<=position) {
					direction[coordinate]=-direction[coordinate];
					redirect(startPoint,endPoint,direction,distTraveled);
				}

			} else if (boundaryType==blockMinusToPlusErrorOtherwise) {
				if(startPoint[coordinate]>=position) {
					cerr<<"\n\nCell started out of bounds!  This may occur if boundaries are set closer to each other \n";
					cerr<<"Than a cell's maximum step, because this reflect just sends cells the other way and does\n";
					cerr<<"not account for multiple reflections off multiple boundaries in any nice way."<<endl;
					cerr<<"Error may also occur if you began cells outside of a boundary."<<endl;
					cerr<<"cell started at: "<< startPoint[0] <<" "<<startPoint[1]<<" "<<startPoint[2]<<endl;
					cerr<<"cell trying to move to: "<< endPoint[0] <<" "<<endPoint[1]<<" "<<endPoint[2]<<endl;
					cerr<<"boundary is: coordinate:"<< coordinate <<" position:"<<position<<" type:"<<boundaryType<<endl;
					exit(1);
				}
				if(endPoint[coordinate]>=position) {
					direction[coordinate]=-direction[coordinate];
					redirect(startPoint,endPoint,direction,distTraveled);
				}

			} else if (boundaryType==blockPlusToMinusButDontWorryTheOtherWay) {
				if(startPoint[coordinate]>position && endPoint[coordinate]<=position) {
					direction[coordinate]=-direction[coordinate];
					redirect(startPoint,endPoint,direction,distTraveled);
				}
			} else if (boundaryType==blockMinusToPlusButDontWorryTheOtherWay) {
				if(startPoint[coordinate]<position && endPoint[coordinate]>=position) {
					direction[coordinate]=-direction[coordinate];
					redirect(startPoint,endPoint,direction,distTraveled);
				}
			}


		};

		void redirect(const double (&startPoint) [3], double (&endPoint) [3], const double (&direction) [3], double distTraveled) {

			endPoint[0] = startPoint[0] + distTraveled*direction[0];
			endPoint[1] = startPoint[1] + distTraveled*direction[1];
			endPoint[2] = startPoint[2] + distTraveled*direction[2];

		}


	protected:
		static const int X_COOR=0;
		static const int Y_COOR=1;
		static const int Z_COOR=2;
		int coordinate;
		double position;

		static const int blockBothCrossings=0;
		static const int blockPlusToMinusErrorOtherwise=1;
		static const int blockMinusToPlusErrorOtherwise=2;
		static const int blockPlusToMinusButDontWorryTheOtherWay=3;
		static const int blockMinusToPlusButDontWorryTheOtherWay=4;

		int boundaryType;

	};

}

#endif /* MATHFUNCTIONDATA_HH_ */
