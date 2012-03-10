/*
 * project.hh
 *
 *  Created on: Dec 17, 2010
 *      Author: Christopher Henry
 */

#ifndef PROJECT_HH_
#define PROJECT_HH_

namespace Hive {
	//This is a singleton class that holds the main function for the project (runProject)
	//Each project will have its own specific "Project.cpp" that implements this class and specifically of the "runProject" function
	class Project {
		public:
			//This function creates the global project object
//			static Project* createProject();
			//This function retrieves the global project object
//			static Project* getProject();
			//This function deletes the global project object
//			static void deleteProject();
			/// constructor
			Project();
			/// destructor
			~Project();
			//This function is called to run the project. The command line arguments are passed into this function
			void runProject(int argc, char *argv[]);
		protected:
	};
}

#endif /* PROJECT_HH_ */
