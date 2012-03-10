/*
 * chemotaxisMovement.hh
 *
 *  Created on: Oct 21, 2009
 *      Author: msneddon
 */

#ifndef CHEMOTAXISMOVEMENT_HH_
#define CHEMOTAXISMOVEMENT_HH_




#include "../../simulator.hh"
#include "../../../util/util.hh"


using namespace Hive;

namespace Chemotaxis
{

	class ChemotaxisMovementSimulator : public Simulator
	{
		public:
			ChemotaxisMovementSimulator(double eqTime);
			~ChemotaxisMovementSimulator();

			void setAgent(Agent *ag);

			void step(double t);
			void prepare();
			void synchroniseWithDatabase();


		protected:

			void initialise();


		private:

			double eqTime;
			double internalTime;


			static const int X;
			static const int Y;
			static const int Z;

			void changeDirRandom();
			void changeDirDistribution();
			void changeDirRotDiffusion(double elapsedTime);
			void swimToNewPosition(double elapsedTime);
			void moveToNewPosition(double elapsedTime);

			//Basic variables that come from data objects
			double speed;
			double rotDiffConst;
			double movementDt;

			int currentMovementState;
			int lastMovementState;

			int numMotorsCW;
			int numMotorsCCW;



			//Keep track of curent 3d position, direction, and local orientation
			// (could use a quanternion and save less numbers, but I like my intuitive vectors...)
			//  also, I ran a test using double values on a 64bit linux machine to test the error
			//  that develops between the orthogonality between the up and dir vectors.  Running
			//  10 million random rotations on dir and up, I get only a ~.005 error in the degree (meaning
			//  the degree between the two vectors was ~90.005 instead of 90 exactly).  I believe this to
			//  be a reasonable error, as if we apply a new rotation every .01 seconds in the simulation,
			//  we only get an error that is greater than about 0.005 degrees after 27.7 simulation hours,
			//  which is ok for me...  (note, in the same test, the magnitude of the vectors changes by
			//  less than 0.00001 (they were 0.999991 instead of exactly 1)
			double pos[3];
			double dir[3];
			double up[3];
			double rotMat[3][3];

			double lastPos[3];
			double lastDir[3];
			double lastUp[3];

			static const int RUN;
			static const int TUMBLE;

			// a'la howard berg yet again:
			// Gamma distribution with shape = 4 scale = 18.32045567939674 location = -4.606176605609249 it
			Util::GammaSampler *gs;
			static const double DefaultGammaAlpha;
			static const double DefaultGammaBeta;
			static const double DefaultGammaOffset;


	};

}







#endif /* CHEMOTAXISMOVEMENT_HH_ */
