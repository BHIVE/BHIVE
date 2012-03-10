#ifndef _analysis_h
#define _analysis_h

#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <vector>
#include <stdlib.h>
#include <string>

using namespace std;

class Analysis {
public:
  Analysis();
  Analysis(int, int, double);
  //  Analysis(int, double, bool);

  ~Analysis();

  void analysePositions();
  void analyseDistances();

  void getBlock(vector<vector<double> > *);
  int getBlockBinary(vector<vector<double> >*);

  int getNumberOfOutputs();

  void outputMap();

  void setCellDataInputFile(string, bool=false);
  void setCellPositionsInputFile(string, bool=false);
  void setInitialPosition(double x, double y, double z);
  void setOutputFile(string);

  void setCellClasses(map<double,vector<int> >& );

  void calculateCenterOfMass(vector<vector<double> > *,
			     vector<double>&);

  // returns vector with average distance and
  // std-dev of the average distances.
  void calculateAverageDistanceFromInitialPosition(vector<vector<double> >*,
						   vector<double>& );

  /// generates for each output time the histogram of the x positions for the population
  /// with a key of x
  void outputHistogramOfXposForPop(double x);

  void outputHistogramOfDistForPop(double x);


  // Here you can calculate the histogram with set limits (note that if the limits
  // do not account for the entire distribution, you will segfault) and any given
  // axis, where axis=0/1/2 for X/Y/Z
  void outputHistogramOfPosWithLimits(double pop, int axis, double minLimit, double maxLimit);


  // some more helper functions
  void getMinMaxXPos(vector<vector<double> >*, vector<double>&);

  void getMinMaxDist(vector<double>*, vector<double>&);

  void generateHistogramXPos(vector<vector<double> >*, vector<double>&, double min);
  void generateHistogramPos(vector<vector<double> > *, vector<double>&, double min, int axis);

  void generateHistogramDist(vector<double>*, vector<double>&, double);
  void setBinWidth(double d) { this->bin_width = d; } ;


private:
  map<double,vector<int> > cell_classes;
  map<double,vector<int> >::iterator iter;

  ifstream cell_data_input;
  ifstream position_input;

  ofstream outputfile;

  bool binary_position_input;
  bool binary_cell_data_input;

  int number_cells;
  int number_sweeps;
  double output_interval;

  vector<double> initial_position;

  double bin_width;

};

#endif
