/*global_data.hh
 *Created on: 8/5/2010
 *Author: henry*/

#ifndef GLOBAL_DATA_H
#define GLOBAL_DATA_H

using namespace std;

namespace Hive {
	class GlobalData {
		public:
			GlobalData();
			~GlobalData();

			int initialize(int argc, char *argv[]);
			int loadParameters(vector<string> filenames);

			OutputWriter* getWriter();
			PropagatorClass* getPropagator();

			string getParameter(string parameter);
			ofstream& getOutputStream();

			//Reads in a fileline and returns it as a string
			string getFileline(ifstream &input);
			//Reads in a fileline and returns it as a vector of strings for each element separated by delimiters in delim
			vector<string> getStringsFileline(ifstream &input, const char* delim, bool treatConsecutiveDelimAsOne = true);
			//Splits the input string into a vector of strings using the elements in delim as the delimiters
			vector<string> stringToStrings(string fullString, const char* delim, bool treatConsecutiveDelimAsOne = true);
		private:
			ofstream outputStream;
			OutputWriter* currentWriter;
			PropagatorClass* currentPropagator;
			map<string , string , std::less<string> > parameters;



	};
}

#endif /* GLOBAL_DATA_H */