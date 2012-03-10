/**
 *
 * @date March 18, 2010
 * @author Christopher Henry
 */

#ifndef CARTESIAN_GRID_
#define CARTESIAN_GRID_

#include "../simpleCell.hh"

using namespace std;
using namespace Hive;

namespace SimpleCell {

/*
 * Cartesian Grid Database
 */
struct CellHistory {
    vector<int> voxelCoord;
    double enterTime;
};

struct CellWorldData;
struct BoundaryVoxel;
class BacteriaDatabase;
class UniverseDatabase;

ostream& operator<<(ostream& stream, CellWorldData cellWorldData);
istream& operator>>(istream& stream, CellWorldData& cellWorldData);

int translateCoordToID(int x, int y, int z, int xs, int ys, int zs);

void CellWorldData_packDataForMPI(CellWorldData* data, char*& buffer, int& size);
CellWorldData* CellWorldData_unpackDataForMPI(char* buffer, int size);

struct CellWorldData {
    int id;
    double diameter;
    double dDiameter;
    vector<double> coordinates;
    vector<double> speed;
    vector<double> excretionRates;
    list<CellHistory*> history;
    friend ostream& SimpleCell::operator<< (ostream& stream, CellWorldData cellWorldData);
    friend istream& SimpleCell::operator>> (istream& stream, CellWorldData& cellWorldData);
};

template<class T> ostream& vector_os(ostream& stream, vector<T>& vd);
template<class T> void     vector_is(istream& stream, vector<T>& vd);

template<class T> ostream& pvector_os(ostream& stream, vector<T*>& vd);
template<class T> void     pvector_is(istream& stream, vector<T*>& vd);

struct Voxel {
    bool fixed;
	bool wall; //This is a boolean indicating whether this voxel is a wall or an open volume
    double bufferConcentration; //This is a simple buffer used in diffusion calculations involving this voxel
    vector<int> coordinates; //This is the x,y,z coordinate of the voxel
    list<CellWorldData*> cells; //This is the list of world data for the cells located in this voxel
    vector<double> compoundConcentrations; //This is the vector of concentrations for compounds included in this voxel
	BoundaryVoxel* bv; //This points to data relating to voxels in the boundary layer of the world
};

struct BoundaryVoxel {
    int cpu; //This is the rank of the cpu containing this voxel
	int id; //This is the id of this voxel in its home cpu
	//This is the rate at which each compound entered this voxel from outside the world during the previous time step
	//This is used to interpolate the concentration over time during this time step
	vector<double> interpolatedFlux; 
	vector<double> originalConcentration; //This is the concentration at the end of the previous time step 
	vector<double> netFlux; //This is the net flux that has entered this voxel over the current time step
	Voxel* v;
};

class CartesianGrid : public Data {
  public:
    CartesianGrid(string name=GRID, string type=GRID):Data(name, type) {rhs = NULL;
        x = NULL; H = NULL;	lhs = NULL;};
    ~CartesianGrid();
    void createGrid(int xCount,int yCount,int zCount, double blockSize,
        vector<double> startingCoordinates);
    void setWalls(vector<vector<int> > wallVoxels);
    void printContent(ostream& out);
	void printDBObservables(OutputWriter *ow);
	void setDBObservables(OutputWriter *ow);
    void printCellLocations();
    // Access functions for getting voxel
    Voxel* getVoxel(int x, int y, int z);           // int's are [0, xSize();]
    Voxel* getVoxel(vector<int> coordinates);
    Voxel* getVoxel(double x, double y, double z);  // doubles are in absoluteCoordinates
    Voxel* getVoxel(vector<double> coordinates);    // e.g. (int x * getBlockSize()) + xStartingCoord;
	Voxel* getVoxelByID(int ID,int nodeID,bool boundary = false);

    void setGridConcentration(Voxel* voxel, int compound, double newConc) {
        voxel->compoundConcentrations[compound] = newConc;};
    void setGridConcentrationsFromBuffer(int compound);
    void setBufferConcentration(Voxel* voxel, double concentration) {
        voxel->bufferConcentration = concentration;};

    void addCell(int id, double x, double y, double z, double diameter); // also add's cell to grid
    void addCell(CellWorldData* cell);

    void addCellToGrid(CellWorldData* inCell);
    void removeCell(int id);                                             // should be renamed "deleteCell"
    CellWorldData* removeCellFromGrid(CellWorldData* inCell);

    void updateCell(int id,vector<double> dCoord,double diameter,vector<double> concChanges,bool dead);
    void updateCell(int id,double dx,double dy,double dz,double diameter);
    void setCellSpeed(int cellID,vector<double> speed);
    void setCellConsumption(int cellId, double consumption);

	int getVoxelID(Voxel* inVoxel);
	int getVoxelCPU(Voxel* inVoxel);
    int addCompound(string name,double diffCoeff, double defaultconc = 0);
    string getCompound(int index) {return compoundNames[index];};
    double getDiffCoeff(int index) {return compoundDiffCoef[index];};
    int getCompoundIndex(string name) {return compoundMap[name];};
    int numberOfCompounds() {return int(compoundNames.size());};
    int getNumCells() {return cellList.size();};
    CellWorldData* getCellByIndex(int index);
    CellWorldData* getCellByAgent(int agentId);

    double getGridConcentration(int x,int y,int z,int compound);
    vector<double> getPointConcentration(double x,double y,double z);
    vector<double> getCellConcentrations(int cellID);
    // Determines if the input cell is within the bounds of the world
    bool cellIsInBounds(CellWorldData* inputCell);
    // Clears each cell's memmory of the voxels it has visited
    void clearCellHistory();
    // Checks to see if the input cell is overlapping with any other
    // cell and rolls back the input cell if overlap occurs
    bool checkCellCollision(CellWorldData* inputCell);
    // Moves every cell and simulates cell excretions according to the input time step.
    // Collisions are not allowed.
    void moveCells(double dt, double time);

	//This function returns a list of the world data for cells located in the boundary layer of voxels
    vector<CellWorldData*> getEdgeCells();
	//This function returns a list of the voxels included in the boundary layer
	vector<Voxel*> getEdgeVoxels();
	vector<Voxel*> getBoundaryVoxels();
	void configureBoundaryCells(UniverseDatabase* univDB);

    double getBlockSize(); 
    double getVoxelVolume();
	double xLen(); // returns real size of grid, i.e. xSize() * getBlockSize() 
    double yLen();
    double zLen();
    double Len(int i);

    int xSize();   // returns number of voxels along x-axis
    int ySize();
    int zSize();
    int Size(int i);
    double startingCoord(int i);
    // takes coordinates starting at "top left" of grid as 0,0,0
    vector<double> getAbsoluteCoordinates(vector<double>);
    void createCKData(double dt);
    VECTOR_double* getCKRHS() {return rhs;};
    void setCKVar(VECTOR_double* inVar) {x = inVar;};
    MATRIX_double* getCKMatrix() {return H;};
    CompRow_Mat_double* getCKLHS() {return lhs;};
    CompRow_ILUPreconditioner_double* getPrecond() {return precond;};
    void enterCKSolution();

  protected:
    Voxel* getVoxelPadded(vector<int> coordinates);
    int xCountPadded;
    int yCountPadded;
    int zCountPadded;
    double blockSize_;
	double blockVolume;

    double xStartingCoord; // absolute coordinates for starting at "top left" corner of grid
    double yStartingCoord; // used for all function calls requiring coordinates except
    double zStartingCoord; // getAbsoluteCoordinates() which returns absolute coordinates

    vector<vector<vector<struct Voxel*> > > gridVoxelsPadded;
    map<int, CellWorldData* > cellData;
    list<CellWorldData*> cellList;

    vector<string> compoundNames;
    vector<double> compoundDiffCoef;
    map<string,int> compoundMap;

	map<int,map<int, Voxel*> > boundaryVoxelMap;

    VECTOR_double* rhs;
    VECTOR_double* x;
    MATRIX_double* H;
    CompRow_Mat_double* lhs;
    CompRow_ILUPreconditioner_double* precond;
};

/*
 * World Message Databases
 *
 */
class WorldMessageToCell : public Data {
  public:
    WorldMessageToCell(string name="world_message", string type="world_message"):Data(name, type){};
    ~WorldMessageToCell() {};
    void printContent(ostream& out) {};
    void printDBObservables(OutputWriter *ow) {};
	void setDBObservables(OutputWriter *ow) {};
	void setConc(vector<double> inputConc) {currentConcentrations = inputConc;};
    vector<double> getConc() {return currentConcentrations;};
    void unpackDataForMPI(char* buffer,int size) {};
  protected:
    vector<double> currentConcentrations;
};

/*
 * World Message Generators
 *
 */
class GenerateWorldMessageToCell : public MessageGenerator {
    public:
    GenerateWorldMessageToCell(Agent *ag, int inCommunicationRound = 0) :
        MessageGenerator(ag, false, inCommunicationRound) {}
    void placeMessage(int destID,int node=0);
};

/* 
 * World Actions
 *
 */
class SyncWorldToCell : public Action {
    public:
        void fire (Agent* ag,Data *d,int source);
};

/*
 * World Simulators
 *
 */
class CrankNicolson : public Simulator {
  public:
    CrankNicolson() {dt = 0.001;};
    ~CrankNicolson() {};
    void step(Agent* inAgent,double t);
  protected:
    void initialise() {};
  private:
    double dt;
};
class ForwardEuler : public Simulator {
  public:
    ForwardEuler() {};
    ~ForwardEuler() {};
    void step(Agent* inAgent,double t);
    double calcDiffusionBetweenVoxels(int x,int y,int z,int x2, int y2, int z2, int compound,double coef,CartesianGrid* g);
	static inline const double MAXH() { return 0.05; };
  protected:
    void initialise() {};
};
class CellMovement : public Simulator {
  public:
    CellMovement() {};
    ~CellMovement() {};
    void step(Agent* inAgent,double t);
    static inline const double dt() { return 0.001; };
  protected:
    void initialise() {};
};

/*
 * World Factory
 *
 */
class WorldFactory : public AgentFactory {
  public:
    WorldFactory();
    WorldFactory(const int type);
    WorldFactory(InputSystemReader *isr);
    ~WorldFactory();
    Agent *createAgent();
    Agent *createAgent(Action **ac);
    Agent *createAgent(xmlParsedNode* node);
    Agent *duplicateAgent(Agent *ag);
    void addChild(Agent* parent, Agent* child, xmlParsedNode* node);
    void finalise();
  protected:
    void addActionsToAgentsActionSet(Agent *ag);
    void addSimulatorToAgent(Agent *ag);
  private:
    int simulatorType;
};

}

#endif
