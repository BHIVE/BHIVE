/*
 * boundaryInteractionLib.hh
 *
 *  Created on: Dec 6, 2011
 *      Author: msneddon
 */

#ifndef BOUNDARYINTERACTIONLIB_HH_
#define BOUNDARYINTERACTIONLIB_HH_



using namespace std;
using namespace Hive;

namespace Chemotaxis
{

	class Wall {
		public:
			Wall(double [3] position, double [3] normal) {};
			~Wall() {};

			double calculateDistanceToWall(double [3] point);
			double calculateDistanceToWallAlongVector(double [3] point, double [3] direction);
			double position[3];
			double normal[3];

	};


	class BoundaryInteraction
	{

		public:

			static BoundaryInteraction* getBoundaryInteractionInstance();

			// takes a list of walls and detects a collision, returns true for a
			// collision, false otherwise, returns index of the wall we collided with
			bool detectWallCollision(Wall **walls, pos, dir, distanceTraveled, int wallInTheWayIndex);

			// essentially determines if a cell is within the spaceBuffer of the wall
			bool isStillOnWall(Wall *w, pos, double spaceBuffer) {
				if(w->calculateDistanceToWall(pos)<spaceBuffer) return true;
				return false;
			};

			// given a list of walls, return the index of the closest wall
			int getClosestAbsoluteWall(Wall **w, pos) ;

			// takes a wall, position, direction and advance the position (within the
			// space buffer) and either reflect or align
			// returns distance traveled
			void double advanceToWallAndReflect(Wall w, pos&, dir&, double spaceBuffer);
			void double advanceToWallAndAlign(Wall w, pos, dir, double spaceBuffer);



			//void setWorldAgent(Agent *ag);
			//void setGrid(string gridname);

		protected:
			BoundaryInteraction();
			~BoundaryInteraction();


	};

}

#endif /* BOUNDARYINTERACTIONLIB_HH_ */
