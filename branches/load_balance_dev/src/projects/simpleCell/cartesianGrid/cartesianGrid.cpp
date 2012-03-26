#include "cartesianGrid.hh"

#ifdef BHIVEMPI
#include <mpi.h>
#endif


using namespace std;
using namespace Hive;
using namespace SimpleCell;

int SimpleCell::translateCoordToID(int x, int y, int z, int xs, int ys, int zs) {
	return (x+y*xs+z*xs*ys);
}

void SimpleCell::CellWorldData_packDataForMPI(CellWorldData* data, char*& buffer, int& size) {
	stringstream ss;
	ss << data->id << endl;
	ss << data->diameter << endl;
	ss << data->dDiameter << endl;
	ss << int(data->coordinates.size()) << endl;
	for (int i=0; i < int(data->coordinates.size()); i++) {
		ss << data->coordinates[i] << endl;
		ss << data->speed[i] << endl;
	}
	ss << int(data->excretionRates.size()) << endl;
	for (int i=0; i < int(data->excretionRates.size()); i++) {
		ss << data->excretionRates[i] << endl;
	}
	//Creating the buffer from the stringstream
	buffer = strdup(ss.str().c_str());
    size = strlen(buffer);
}

CellWorldData* SimpleCell::CellWorldData_unpackDataForMPI(char* buffer, int size) {
	stringstream ss(buffer);
    CellWorldData* data = new CellWorldData;
	ss >> data->id;
	ss >> data->diameter;
	ss >> data->dDiameter;
	int vsize = 0;
	ss >> vsize;
	data->coordinates.resize(vsize);
	data->speed.resize(vsize);
	for (int i=0; i < vsize; i++) {
		ss >> data->coordinates[i];
		ss >> data->speed[i];
	}
	ss >> vsize;
	data->excretionRates.resize(vsize);
	for (int i=0; i < vsize; i++) {
		ss >> data->excretionRates[i];
	}
	return data;
}

void CartesianGrid::printContent(ostream& out) {
    for (int x = 0; x < xSize(); ++x) {
        out << getVoxel(x,5,5)->compoundConcentrations[0] << ";";
		//for (int y = 0; y < ySize(); ++y) {
        //    for (int z = 0; z < zSize(); ++z) {
        //        Voxel* myVoxel = getVoxel(x,y,z);
        //        out << "grid[" << x << " " << y << " " << z <<
        //            "] = " << myVoxel->wall << endl;
        //    }
        //}
    }
	out << endl;
}

void CartesianGrid::printCellLocations() {

    int myLoad = 0;
    
    for( list<CellWorldData*>::iterator IT = cellList.begin(); IT != cellList.end(); IT++) {
        ++myLoad;
        
        CellWorldData* currentCell = (*IT);

        // if (mpisize() > 1) {
        //     printf("[%2d] %-3d (%7f, %7f, %7f)\n", mpirank(), currentCell->id, currentCell->coordinates[0],
        //            currentCell->coordinates[1], currentCell->coordinates[2]);
        // } else {         
        //     printf("%-3d (%7f, %7f, %7f)\n", currentCell->id, currentCell->coordinates[0],
        //            currentCell->coordinates[1], currentCell->coordinates[2]);
        // }        


        if (mpisize() > 1) {
            printf("[%2d] %-3d (%6f, %6f, %6f) (%+3f, %+3f, %+3f)\n", mpirank(), currentCell->id, currentCell->coordinates[0], currentCell->coordinates[1], currentCell->coordinates[2], currentCell->speed[0], currentCell->speed[1], currentCell->speed[2]);
        } else {         
            printf("%-3d (%7f, %7f, %7f)\n", currentCell->id, currentCell->coordinates[0],
                   currentCell->coordinates[1], currentCell->coordinates[2]);
        }        


		Hive::getOutputStream() << get_propagator()->getCurrentTime() << "," << currentCell->id << "," << Hive::mpirank() << currentCell->coordinates[0] << "," << currentCell->coordinates[1] << "," << currentCell->coordinates[2] << "," << currentCell->diameter << ",0,0,0" << endl;

	}

#ifdef BHIVEMPI
    int load[1024];
    MPI_Gather(&myLoad, 1, MPI_INT, load, 1, MPI_INT, 0, MPI_COMM_WORLD);
    if (mpirank() == 0) {
        printf("Load: ");
        for (int i = 0; i < mpisize(); ++i) {
            printf("%d:%4d  ", i, load[i]);
        }
        printf("\n");
    }
#endif

}

CartesianGrid::~CartesianGrid() {
	if (rhs != NULL) {
		delete rhs;
	}
	if (lhs != NULL) {
		delete lhs;
	}
	if (x != NULL) {
		delete x;
	}
	if (H != NULL) {
		delete H;
	}
	for (int i=0; i < xCountPadded; i++) {
		for (int j=0; j < yCountPadded; j++) {
            for (int k=0; k < zCountPadded; k++) {
				if (this->gridVoxelsPadded[i][j][k]->bv != NULL) {
					delete this->gridVoxelsPadded[i][j][k]->bv;
				}
				delete this->gridVoxelsPadded[i][j][k];
            }
		}
	}
}

void CartesianGrid::createGrid(int xCount,int yCount,int zCount, 
    double blockSize, vector<double> startingCoordinates)
{
	this->xCountPadded = xCount + 2;
	this->yCountPadded = yCount + 2;
	this->zCountPadded = zCount + 2;

    this->blockSize_ = blockSize;
	this->blockVolume = blockSize*blockSize*blockSize; 

	this->gridVoxelsPadded.resize(xCountPadded);
	for (int i=0; i < xCountPadded; i++) {
		this->gridVoxelsPadded[i].resize(yCountPadded);
		for (int j=0; j < yCountPadded; j++) {
			this->gridVoxelsPadded[i][j].resize(zCountPadded);
            for (int k=0; k < zCountPadded; k++) {
                Voxel* voxel = new Voxel;
				voxel->coordinates.push_back(i-1);
                voxel->coordinates.push_back(j-1);
                voxel->coordinates.push_back(k-1);
                voxel->wall = false;
                voxel->fixed = false;
                this->gridVoxelsPadded[i][j][k] = voxel;
				if (i == 0 || j == 0 || k == 0 || i == (xCountPadded-1) || j == (yCountPadded-1) || k == (zCountPadded-1)) {
					voxel->bv = new BoundaryVoxel;
					voxel->bv->cpu = -1;
					voxel->bv->id = -1;
					voxel->bv->v = voxel;
				} else {
					voxel->bv = NULL;
				}
            }
		}
	}
    this->xStartingCoord = startingCoordinates[0];
    this->yStartingCoord = startingCoordinates[1];
    this->zStartingCoord = startingCoordinates[2];
};

void CartesianGrid::setWalls(vector<vector<int> > wallVoxels)
{
    for (unsigned int i = 0; i < wallVoxels.size(); i++) {
        if (wallVoxels[i].size() == 3) {
            Voxel* voxel = this->getVoxel(wallVoxels[i]);
            voxel->wall = true;
        } 
    } 
}

Voxel* CartesianGrid::getVoxel(int x, int y, int z)
{
    // fprintf(stderr, "getVoxel(%d, %d %d)\n", x, y, z);
    if ( -1 <= x && x <= this->xSize()+1 && -1 <= y && 
        y <= this->ySize()+1 && -1 <= z && z <= this->zSize()+1 ) {
        // fprintf(stderr, "%x\n", this->gridVoxelsPadded[x+1][y+1][z+1]);
        
        return this->gridVoxelsPadded[x+1][y+1][z+1];
    } else {
        cerr << "Warning: getVoxel with bad coordinates: " << x << "," << y << "," << z << endl;
        return NULL;
    }
};

Voxel* CartesianGrid::getVoxel(vector<int> coordinates)
{ 
    if( coordinates.size() == 3 ) {
        return this->getVoxel(coordinates[0],
            coordinates[1], coordinates[2]);
    } else {
        cerr << "Warning: getVoxel with bad coordinates: " <<
            coordinates[0] << "," << coordinates[1] << "," << coordinates[2] << endl;
        return NULL;
    }
}

Voxel* CartesianGrid::getVoxel(vector<double> coordinates)
{
    if( coordinates.size() == 3 ) {
        return this->getVoxel(coordinates[0],
            coordinates[1], coordinates[2]);
    } else {
        return NULL;
    }
}

Voxel* CartesianGrid::getVoxel(double x,double y,double z) {
	int x2 = int(floor(double((x - this->xStartingCoord)/getBlockSize())));
	int y2 = int(floor(double((y - this->yStartingCoord)/getBlockSize())));
	int z2 = int(floor(double((z - this->zStartingCoord)/getBlockSize())));
    // printf("blocksize = %f  getVoxel %7f %7f %7f ==> %d %d %d\n", getBlockSize(), x, y, z, x2, y2, z2);
    return getVoxel(x2,y2,z2);
}

Voxel* CartesianGrid::getVoxelByID(int ID,int nodeID,bool boundary) {
	if (boundary || nodeID != mpirank()) {
		return boundaryVoxelMap[nodeID][ID];
	} else {	
		int z = int(floor(double(ID/(xSize()*ySize()))));
		ID += -z*xSize()*ySize();
		int y = int(floor(double(ID/xSize())));
		ID += -y*xSize();
		int x = ID;
		return getVoxel(x,y,z);
	}
}

Voxel* CartesianGrid::getVoxelPadded(vector<int> coordinates) {
    return this->gridVoxelsPadded[coordinates[0]][coordinates[1]][coordinates[2]];
}

void CartesianGrid::setGridConcentrationsFromBuffer(int compound) {
	for (int i=0; i < this->xSize(); i++) {
		for (int j=0; j < this->ySize(); j++) {
			for (int k=0; k < this->zSize(); k++) {
				Voxel* voxel = this->getVoxel(i,j,k);
				voxel->compoundConcentrations[compound] = voxel->bufferConcentration;
			}
		}
	}

}

void CartesianGrid::setCellSpeed(int cellID,vector<double> speed) {
	CellWorldData* cell = cellData[cellID];
    if (cell != NULL)
        cell->speed = speed;
}

void CartesianGrid::setCellConsumption(int cellID,double consumption) {
	CellWorldData* cell = cellData[cellID];
    if (cell != NULL)
        cell->excretionRates[0] = consumption;
}

int CartesianGrid::getVoxelID(Voxel* inVoxel) {
	if (inVoxel->bv == NULL) {
		return translateCoordToID(inVoxel->coordinates[0],inVoxel->coordinates[1],inVoxel->coordinates[2],xSize(),ySize(),zSize());
	} else {
		return inVoxel->bv->id;
	}
}

int CartesianGrid::getVoxelCPU(Voxel* inVoxel) {
	if (inVoxel->bv == NULL) {
		return mpirank();
	} else {
		return inVoxel->bv->cpu;
	}
}

int CartesianGrid::addCompound(string name,double diffCoeff, double defaultconc) {
	if (compoundMap.count(name) == 0) {
		compoundMap[name] = int(compoundNames.size());
		compoundNames.push_back(name);
		compoundDiffCoef.push_back(diffCoeff);
		for (int i=0; i < (this->xCountPadded); i++) {
			for (int j=0; j < (this->yCountPadded); j++) {
				for (int k=0; k < (this->zCountPadded); k++) {
					this->gridVoxelsPadded[i][j][k]->compoundConcentrations.push_back(defaultconc);
					if (this->gridVoxelsPadded[i][j][k]->bv != NULL) {
						this->gridVoxelsPadded[i][j][k]->bv->interpolatedFlux.push_back(0);
						this->gridVoxelsPadded[i][j][k]->bv->originalConcentration.push_back(defaultconc);
						this->gridVoxelsPadded[i][j][k]->bv->netFlux.push_back(0);
					}
				}			
			}
		}
        return compoundNames.size() - 1;
	} else {
		compoundDiffCoef[compoundMap[name]] = diffCoeff;
        return compoundMap[name];
	}
}

void CartesianGrid::addCell(int id,double x,double y,double z,double diameter) {
	CellWorldData* myCell = cellData[id];
	if (myCell == NULL) {
		myCell = new CellWorldData;
		myCell->diameter = diameter;
		myCell->dDiameter = .0;
		myCell->coordinates.push_back(x);
		myCell->coordinates.push_back(y);
		myCell->coordinates.push_back(z);
        myCell->speed.resize(3, .0);
		myCell->id = id;
        myCell->excretionRates.resize(this->numberOfCompounds(),-0.0005);
        CellHistory* history = new CellHistory;
        history->voxelCoord = (this->getVoxel(myCell->coordinates))->coordinates;
        history->enterTime = 0;
        myCell->history.push_back(history);
		cellData[id] = myCell;
        cellList.push_back(myCell);
		addCellToGrid(myCell);
	}
};

void CartesianGrid::addCell(CellWorldData* cell) {
	if (cellData[cell->id] == NULL) {
		cellData[cell->id] = cell;
        cellList.push_back(cell);
		addCellToGrid(cell);
	}
}

void CartesianGrid::addCellToGrid(CellWorldData* inCell) {
	//Identifying the grid element where the cell is located
    Voxel* voxel = this->getVoxel(inCell->coordinates);
    voxel->cells.push_back(inCell);
}

CellWorldData* CartesianGrid::removeCellFromGrid(CellWorldData* inCell) {
    Voxel* voxel = this->getVoxel(inCell->coordinates);
    voxel->cells.remove(inCell);
	return inCell;
}

//This function removes a cell entirely from the world environment
void CartesianGrid::removeCell(int id) {
	CellWorldData* myCell = cellData[id];
	if (myCell != NULL) {
		removeCellFromGrid(myCell);
        cellList.remove(myCell);
        map<int, CellWorldData* >::iterator it = cellData.find(id);
        cellData.erase(it);
		//delete myCell;
		cellData[id] = NULL;
	}
}

//This function updates all world data based on a message sent by the cell to the world
void CartesianGrid::updateCell(int id,vector<double> dCoord,double diameter,vector<double> concChanges,bool dead) {
	CellWorldData* myCell = cellData[id];
	if (myCell != NULL) {
		//Removing the cell from its current location in the grid
		removeCellFromGrid(myCell);
        Voxel* oldVoxel = getVoxel(myCell->coordinates);
		for (int i=0; i < int(concChanges.size()); i++) {
			//Note I'm allowing a negative concentration at this moment, 
            //as diffusion into this element may be sufficient to compensate
			oldVoxel->compoundConcentrations[i] += concChanges[i];
		}
		//If the cell is now dead, then we simply remove it from the system
		if (dead) {
			removeCell(id);
		} else {
			//Updating diameter and location of the cell
			myCell->diameter = diameter;
			for (int i=0; i < 3; i++) {
				myCell->coordinates[i] += dCoord[i];
			}
			//Replacing the cell within the grid
			addCellToGrid(myCell);
		}
	}
};


vector<double> CartesianGrid::getPointConcentration(double x,double y,double z) {
    // fprintf(stderr, "x=%g y=%g z=%g\n", x, y, z);
    
    Voxel* myVoxel = getVoxel(x, y, z); 
    vector<int> element = myVoxel->coordinates;
    vector<int> coordOne(3);
    vector<int> coordTwo(3);

    // fprintf(stderr, "element x=%d, y=%d, z=%d\n", element[0], element[1], element[2]);
    
    // Get center of current voxel
    vector<double> centerOfmyVoxel(3,0);
    for (int i = 0; i < 3; i++) {
        centerOfmyVoxel[i] = this->startingCoord(i) + (myVoxel->coordinates[i]*this->getBlockSize()); 
    }
    
	//Storing the location of the input point inside the surrounding element
	vector<double> fraction(3);
	fraction[0] = (x - centerOfmyVoxel[0])/getBlockSize();
	fraction[1] = (y - centerOfmyVoxel[1])/getBlockSize();
	fraction[2] = (z - centerOfmyVoxel[2])/getBlockSize();

	//Finding the cell centers that surround point x,y,z
	vector<double> centers(6,0);
	for (int i=0; i < 3; i++) {
		if (fraction[i] >= 0.5) {
			fraction[i] = fraction[i]-0.5;
			centers[2*i] = element[i];
			centers[2*i+1] = element[i]+1;
		} else {
			fraction[i] = fraction[i]+0.5;
			centers[2*i] = element[i]-1;
			centers[2*i+1] = element[i];
		}
	}
	//Interpolating the point concentration from the concentrations at 
    //the cell centers around point x,y,z
	vector<double> allConcentrations(numberOfCompounds(),0);
	for (int i=0; i < numberOfCompounds(); i++) {
		coordOne = element;
		coordTwo = element;
		for (int j=0; j < 3; j++) {
			coordOne[j] = int(centers[2*j]);
			coordTwo[j] = int(centers[2*j+1]);
            // fprintf(stderr, "coordOne x=%d y=%d z=%d\n", coordOne[0], coordOne[1], coordOne[2]);
            // fprintf(stderr, "coordTwo x=%d y=%d z=%d\n", coordTwo[0], coordTwo[1], coordTwo[2]);
			double concOne = getGridConcentration(coordOne[0],coordOne[1],coordOne[2],i);
			double concTwo = getGridConcentration(coordTwo[0],coordTwo[1],coordTwo[2],i);
			allConcentrations[i] += (concTwo-concOne)*fraction[j]+concOne;
		}
		allConcentrations[i] = allConcentrations[i]/3;
	}
	return allConcentrations;
}

vector<double> CartesianGrid::getCellConcentrations(int cellID) {
	CellWorldData* cell = this->cellData[cellID];
	return getPointConcentration(cell->coordinates[0],cell->coordinates[1],cell->coordinates[2]);
}

CellWorldData* CartesianGrid::getCellByIndex(int index) {
    int count = 0;
	for (list<CellWorldData*>::iterator IT = cellList.begin(); IT != cellList.end(); IT++) {
		if (count == index) {
			return (*IT);
		}
        count++;  
	}
	return NULL;
}

CellWorldData* CartesianGrid::getCellByAgent(int agentId) {
    if ( this->cellData.count(agentId) > 0 ) {
        return this->cellData[agentId];
    } else {
        return NULL;
    } 
}

double CartesianGrid::getGridConcentration(int x,int y,int z,int compound) {
	return getVoxel(x,y,z)->compoundConcentrations[compound];
}


bool CartesianGrid::cellIsInBounds(CellWorldData* inputCell) {
    for (int i = 0; i < 3; i++) {
        if ( inputCell->coordinates[i] < startingCoord(i) ) {
            return false;
        } else if ((Len(i) + startingCoord(i)) < inputCell->coordinates[i]) {
            return false;
        } else {
            continue;
        }
    }
    return true;
}

bool CartesianGrid::checkCellCollision(CellWorldData* inputCell) {
    Voxel* cellVoxel = this->getVoxel(inputCell->coordinates);
    for (list<CellWorldData*>::iterator IT = cellVoxel->cells.begin();
        IT != cellVoxel->cells.end(); IT++) {
        CellWorldData* otherCell = (*IT);
        if (otherCell->id == inputCell->id) { continue; }
        // calculate eclidian distance between cells
        double distance = sqrt( pow(inputCell->coordinates[0] - otherCell->coordinates[0], 
            2) + pow(inputCell->coordinates[1] - otherCell->coordinates[1], 2) +
            pow(inputCell->coordinates[2] - otherCell->coordinates[2], 2) );
        // now check if that distance is less than the sum of the radii
        if ( distance < (inputCell->diameter + otherCell->diameter)/2 ) {
            return true;
        }
    }
    return false;
};

void CartesianGrid::clearCellHistory() {
	for (list<CellWorldData*>::iterator IT = cellList.begin(); IT != cellList.end(); IT++) {
		CellWorldData* currentCell = (*IT);
		CellHistory* last = currentCell->history.back();
		last->enterTime = 0;
		for (list<CellHistory*>::iterator ITT = currentCell->history.begin(); 
            ITT != currentCell->history.end(); ITT++) {
			if ((*ITT) != last) {
				delete (*ITT);
			}
		}
		currentCell->history.clear();
		currentCell->history.push_back(last);
	}
};

vector<CellWorldData*> CartesianGrid::getEdgeCells() {
    vector<CellWorldData* > edgeCells;
    for (list<CellWorldData*>::iterator it = cellList.begin(); it != cellList.end(); ++it) {
        if (! cellIsInBounds(*it)) {
            // removeCellFromGrid(*it);  // gets removed by the caller.removeCell(id)
            edgeCells.push_back(*it);
        }
    }
	return edgeCells;
};

vector<Voxel*> CartesianGrid::getEdgeVoxels() {
	int numberOfCells = xSize() * ySize() * zSize() - (xSize()-2) * (ySize()-2) * (zSize()-2);
	vector<Voxel*> result(numberOfCells);
	//Adding front and back face cells
	int count = 0;
	for (int i=0; i < ySize(); i++) {
		for (int j=0; j < zSize(); j++) {
			result[count] = getVoxel(0,i,j);
			count++;
			result[count] = getVoxel(xSize()-1,i,j);
			count++;
		}
	}
	//Adding left and right face cells
	for (int i=1; i < xSize()-1; i++) {
		for (int j=0; j < (zSize()); j++) {
			result[count] = getVoxel(i,0,j);
			count++;
			result[count] = getVoxel(i,ySize()-1,j);
			count++;
		}
	}
	//Adding bottom and top face cells
	for (int i=1; i < (xSize()-1); i++) {
		for (int j=1; j < (ySize()-1); j++) {
			result[count] = getVoxel(i,j,0);
			count++;
			result[count] = getVoxel(i,j,zSize()-1);
			count++;
		}
	}
    // cout << "count = " << count << endl;
    // cout << "numberOfCells = " << numberOfCells << endl;
	return result;
};

vector<Voxel*> CartesianGrid::getBoundaryVoxels() {
	vector<Voxel*> edgeVoxels = this->getEdgeVoxels();
    // cout << "*********  getEdgeVoxels " << edgeVoxels.size() << endl;
	vector<Voxel*> result;
	for (int i=0; i < int(edgeVoxels.size()); i++) {
		for (int j=0; j < 3; j++) {
            // fprintf(stderr, "i=%d, j=%d  edgeVoxels[i]->coordinates[j] = %d\n", i, j, edgeVoxels[i]->coordinates[j]);
			if (edgeVoxels[i]->coordinates[j] == 0 || edgeVoxels[i]->coordinates[j] == (this->Size(j)-1)) {
				vector<int> coord = edgeVoxels[i]->coordinates;
                // fprintf(stderr, "x=%d  y=%d  z=%d\n", xSize(), ySize(), zSize());
                // fprintf(stderr, "sx=%g sy=%g sz=%g\n", startingCoord(0), startingCoord(1), startingCoord(2));
                // fprintf(stderr, "d0=%d, d1=%d, d2=%d\n", gridVoxelsPadded.size(), gridVoxelsPadded[0].size(), gridVoxelsPadded[0][0].size());
                // cerr << "coord: " << coord[0] << ", " << coord[1] << ", " << coord[2] << endl;
				if (edgeVoxels[i]->coordinates[j] == 0) {
					coord[j] = -1;
				} else {
					coord[j]++;
				}
				result.push_back(this->getVoxel(coord));
                // cerr << "here" << endl;
			}
		}
	}
	return result;
}

 void CartesianGrid::configureBoundaryCells(UniverseDatabase* univDB) {
	//Cofiguring front and back face boundary cells
	Voxel* current;
	for (int i=0; i < ySize(); i++) {
		for (int j=0; j < zSize(); j++) {
			if (univDB->neighborMap[0][1][1][0] != -1) {
				current = getVoxel(-1,i,j);
				current->bv->cpu = univDB->neighborMap[0][1][1][0];
				current->bv->id = translateCoordToID(univDB->neighborMap[0][1][1][1]-1,i,j,univDB->neighborMap[0][1][1][1],univDB->neighborMap[0][1][1][2],univDB->neighborMap[0][1][1][3]);
				if (boundaryVoxelMap[current->bv->cpu].count(current->bv->id) == 0) {
					boundaryVoxelMap[current->bv->cpu][current->bv->id] = current;
				} else {
					current->bv->cpu = -1;
					current->bv->id = -1;
				}
			}
			if (univDB->neighborMap[2][1][1][0] != -1) {
				current = getVoxel(xSize(),i,j);
				current->bv->cpu = univDB->neighborMap[2][1][1][0];
				current->bv->id = translateCoordToID(0,i,j,univDB->neighborMap[2][1][1][1],univDB->neighborMap[2][1][1][2],univDB->neighborMap[2][1][1][3]);
				if (boundaryVoxelMap[current->bv->cpu].count(current->bv->id) == 0) {
					boundaryVoxelMap[current->bv->cpu][current->bv->id] = current;
				} else {
					current->bv->cpu = -1;
					current->bv->id = -1;
				}
			}
		}
	}
	//Cofiguring left and right face boundary cells
	for (int i=0; i < xSize(); i++) {
		for (int j=0; j < zSize(); j++) {
			if (univDB->neighborMap[1][0][1][0] != -1) {
				current = getVoxel(i,-1,j);
				current->bv->cpu = univDB->neighborMap[1][0][1][0];
				current->bv->id = translateCoordToID(i,univDB->neighborMap[1][0][1][2]-1,j,univDB->neighborMap[1][0][1][1],univDB->neighborMap[1][0][1][2],univDB->neighborMap[1][0][1][3]);
				if (boundaryVoxelMap[current->bv->cpu].count(current->bv->id) == 0) {
					boundaryVoxelMap[current->bv->cpu][current->bv->id] = current;
				} else {
					current->bv->cpu = -1;
					current->bv->id = -1;
				}
			}
			if (univDB->neighborMap[1][2][1][0] != -1) {
				current = getVoxel(i,ySize(),j);
				current->bv->cpu = univDB->neighborMap[1][2][1][0];
				current->bv->id = translateCoordToID(i,0,j,univDB->neighborMap[1][2][1][1],univDB->neighborMap[1][2][1][2],univDB->neighborMap[1][2][1][3]);
				if (boundaryVoxelMap[current->bv->cpu].count(current->bv->id) == 0) {
					boundaryVoxelMap[current->bv->cpu][current->bv->id] = current;
				} else {
					current->bv->cpu = -1;
					current->bv->id = -1;
				}
			}
		}
	}
	//Cofiguring bottom and top face boundary cells
	for (int i=0; i < xSize(); i++) {
		for (int j=0; j < ySize(); j++) {
			if (univDB->neighborMap[1][1][0][0] != -1) {
				current = getVoxel(i,j,-1);
				current->bv->cpu = univDB->neighborMap[1][1][0][0];
				current->bv->id = translateCoordToID(i,j,univDB->neighborMap[1][1][0][3]-1,univDB->neighborMap[1][1][0][1],univDB->neighborMap[1][1][0][2],univDB->neighborMap[1][1][0][3]);
				if (boundaryVoxelMap[current->bv->cpu].count(current->bv->id) == 0) {
					boundaryVoxelMap[current->bv->cpu][current->bv->id] = current;
				} else {
					current->bv->cpu = -1;
					current->bv->id = -1;
				}
			}
			if (univDB->neighborMap[1][1][2][0] != -1) {
				current = getVoxel(i,j,zSize());
				current->bv->cpu = univDB->neighborMap[1][1][2][0];
				current->bv->id = translateCoordToID(i,j,0,univDB->neighborMap[1][1][2][1],univDB->neighborMap[1][1][2][2],univDB->neighborMap[1][1][2][3]);
				if (boundaryVoxelMap[current->bv->cpu].count(current->bv->id) == 0) {
					boundaryVoxelMap[current->bv->cpu][current->bv->id] = current;
				} else {
					current->bv->cpu = -1;
					current->bv->id = -1;
				}
			}
		}
	}
};

double CartesianGrid::getBlockSize() {
	return blockSize_;
};

double CartesianGrid::getVoxelVolume() {
	return blockVolume;
};

double CartesianGrid::xLen() {
	return xSize() * getBlockSize();
};

double CartesianGrid::yLen() {
	return ySize() * getBlockSize();
};

double CartesianGrid::zLen() {
	return zSize() * getBlockSize();
};

double CartesianGrid::Len(int i) {
    if(0 == i) {
        return xLen();
    } else if (1 == i) {
        return yLen();
    } else if (2 == i) {
        return zLen();
    } else {
        return NULL;
    }
}
        

int CartesianGrid::xSize() {
	return this->xCountPadded - 2;
};

int CartesianGrid::ySize() {
	return this->yCountPadded - 2;
};

int CartesianGrid::zSize() {
	return this->zCountPadded - 2;
};

int CartesianGrid::Size(int i) {
    if(0 == i) {
        return xSize();
    } else if (1 == i) {
        return ySize();
    } else if (2 == i) {
        return zSize();
    } else {
        return NULL;
    }
}

double CartesianGrid::startingCoord(int i) {
    if(0 == i) {
        return xStartingCoord;
    } else if (1 == i) {
        return yStartingCoord;
    } else if (2 == i) {
        return zStartingCoord;
    } else {
        return NULL;
    }
}
vector<double> CartesianGrid::getAbsoluteCoordinates(vector<double> relativeCoords) {
    vector<double> absoluteCoords(3,0);
    absoluteCoords[0] = relativeCoords[0] + xStartingCoord;    
    absoluteCoords[1] = relativeCoords[1] + yStartingCoord;    
    absoluteCoords[2] = relativeCoords[2] + zStartingCoord;    
    return absoluteCoords;
}

void CartesianGrid::createCKData(double dt) {
	int counter = 0;
	int row_counter = 0;
	int numberOfVoxels = gridVoxelsPadded.size()*gridVoxelsPadded[0].size()*gridVoxelsPadded[0][0].size();
	int dim = this->numberOfCompounds()*numberOfVoxels;
	int numMatEntries = dim;
	//Counting the neighbors
	for (int m=0; m < numberOfCompounds(); m++) {
		for (int i=0; i < int(gridVoxelsPadded.size()); i++) {
			for (int j=0; j < int(gridVoxelsPadded[i].size()); j++) {
				for (int k=0; k < int(gridVoxelsPadded[i][j].size()); k++) {
					numMatEntries += 6;
					if (i == 0 || i == int(gridVoxelsPadded.size()-1)) {
						numMatEntries--;
					}
					if (j == 0 || j == int(gridVoxelsPadded[i].size()-1)) {
						numMatEntries--;
					}
					if (k == 0 || k == int(gridVoxelsPadded[i][j].size()-1)) {
						numMatEntries--;
					}
				}
			}
		}
	}
	//Allocating arrays
	double* values = new double[numMatEntries];
	int* col_ind = new int[numMatEntries];
	int* row_ptr = new int[dim+1];
	rhs = new VECTOR_double(dim,0.0);
	x = new VECTOR_double(dim,0.0);
	H = new MATRIX_double(dim+1,dim,0.0);

	//Populating the matrix
	double outward;
	double local_diffusion_coeff;
	int voxelCounter = 0;
	double basic_rate = 1;
	for (int m=0; m < numberOfCompounds(); m++) {
		double diff_coeff = this->getDiffCoeff(m)*dt;
		double basic_rate;
		for (int i=0; i < int(gridVoxelsPadded.size()); i++) {
			for (int j=0; j < int(gridVoxelsPadded[i].size()); j++) {
				for (int k=0; k < int(gridVoxelsPadded[i][j].size()); k++) {
					double value = gridVoxelsPadded[i][j][k]->compoundConcentrations[m];
					row_ptr[i] = row_counter;
					outward = 1;
					local_diffusion_coeff = basic_rate*diff_coeff/2.0;
					if (i > 0) { 
						values[counter] = -local_diffusion_coeff;
						col_ind[counter++] = voxelCounter-int(gridVoxelsPadded[i].size()*gridVoxelsPadded[i][j].size());
						value -= gridVoxelsPadded[i][j][k]->compoundConcentrations[m]*basic_rate;
						value += gridVoxelsPadded[i-1][j][k]->compoundConcentrations[m]*basic_rate;
						outward += local_diffusion_coeff;
						row_counter++;
					}
					if (i < int(gridVoxelsPadded.size()-1)) { 
						values[counter] = -local_diffusion_coeff;
						col_ind[counter++] = voxelCounter+int(gridVoxelsPadded[i].size()*gridVoxelsPadded[i][j].size());
						value -= gridVoxelsPadded[i][j][k]->compoundConcentrations[m]*basic_rate;
						value += gridVoxelsPadded[i+1][j][k]->compoundConcentrations[m]*basic_rate;
						outward += local_diffusion_coeff;
						row_counter++;
					}
					if (j > 0) { 
						values[counter] = -local_diffusion_coeff;
						col_ind[counter++] = voxelCounter-int(gridVoxelsPadded[i][j].size());
						value -= gridVoxelsPadded[i][j][k]->compoundConcentrations[m]*basic_rate;
						value += gridVoxelsPadded[i][j-1][k]->compoundConcentrations[m]*basic_rate;
						outward += local_diffusion_coeff;
						row_counter++;
					}
					if (j < int(gridVoxelsPadded[i].size()-1)) { 
						values[counter] = -local_diffusion_coeff;
						col_ind[counter++] = voxelCounter+int(gridVoxelsPadded[i][j].size());
						value -= gridVoxelsPadded[i][j][k]->compoundConcentrations[m]*basic_rate;
						value += gridVoxelsPadded[i][j+1][k]->compoundConcentrations[m]*basic_rate;
						outward += local_diffusion_coeff;
						row_counter++;
					}
					if (k > 0) { 
						values[counter] = -local_diffusion_coeff;
						col_ind[counter++] = voxelCounter-1;
						value -= gridVoxelsPadded[i][j][k]->compoundConcentrations[m]*basic_rate;
						value += gridVoxelsPadded[i][j][k-1]->compoundConcentrations[m]*basic_rate;
						outward += local_diffusion_coeff;
						row_counter++;
					}
					if (k < int(gridVoxelsPadded[i][j].size()-1)) { 
						values[counter] = -local_diffusion_coeff;
						col_ind[counter++] = voxelCounter+1;
						value -= gridVoxelsPadded[i][j][k]->compoundConcentrations[m]*basic_rate;
						value += gridVoxelsPadded[i][j][k+1]->compoundConcentrations[m]*basic_rate;
						outward += local_diffusion_coeff;
						row_counter++;
					}
					(*rhs)[voxelCounter] = value;
					col_ind[counter] = voxelCounter;
					values[counter++] = outward;
					row_counter++;
					voxelCounter++;
				}
			}
		}
	}
	row_ptr[dim] = numMatEntries;
	lhs = new CompRow_Mat_double(dim, dim, numMatEntries, &values[0], &row_ptr[0], &col_ind[0]);
	precond = new CompRow_ILUPreconditioner_double(*lhs);
	delete [] values;
	delete [] col_ind;
	delete [] row_ptr;
}

void CartesianGrid::enterCKSolution() {
	int count = 0;
	for (int m=0; m < numberOfCompounds(); m++) {
		for (int i=0; i < int(gridVoxelsPadded.size()); i++) {
			for (int j=0; j < int(gridVoxelsPadded[i].size()); j++) {
				for (int k=0; k < int(gridVoxelsPadded[i][j].size()); k++) {
					gridVoxelsPadded[i][j][k]->compoundConcentrations[m] = (*x)[count];
					count++;
				}
			}
		}
	}
}

/*
 * CarteisanGrid Actions
 * act on messages from cell, universe
 */

void SyncWorldToCell::fire (Agent* ag,Data *d,int source) {
	//First translating the input data into a cell message if necessary
    // cerr << "SyncWorldToCell::fire" << endl;
    
	CellMessageToWorld *message;
	if(d->getType().compare("RawBufferData")==0) {
		RawBufferData* rbd = (RawBufferData *)d;
		message = new CellMessageToWorld();
		message->unpackDataForMPI(rbd->getBuffer(), rbd->getSize());
	} else {
		message = (CellMessageToWorld *) d;
	}
	//Obtaining the grid database and instructing database to update the cell data
	CartesianGrid* grid = (CartesianGrid*) ag->getDatabase()->getDataItem(GRID);
	grid->setCellSpeed(source, message->getSpeed());
	grid->setCellConsumption(source, message->getConsumption());
}

/*
 * Message Generators : generate messages to cells and universe 
 *
 */
void GenerateWorldMessageToCell::placeMessage(int destID,int node) {
	CartesianGrid* worldDB = (CartesianGrid*)source->getDatabase()->getDataItem(GRID);
    // Only send message to cell if it in bounds...
    if (worldDB->cellIsInBounds(worldDB->getCellByAgent(destID))) {
        Message *msg = new Message();
        msg->setAction(0);
        msg->setDestination(destID,node);
        msg->setSourceID(source->getAgentId());
        WorldMessageToCell* msgDB = new WorldMessageToCell;
        //Getting the world database that has the concentration data
        //Setting message concentrations to cell's current concentrations
        msgDB->setConc(worldDB->getCellConcentrations(destID));
        //Adding message database to message
        msg->setArgument(msgDB);
        //Placing message in outbox
        source->placeMessageInOutbox(msg);
    }
}




/*
 * World Simulators
 *
 */
void CrankNicolson::step(Agent* inAgent,double t) {
	//Getting the grid database
	CartesianGrid* g = (CartesianGrid*)inAgent->getDatabase()->getDataItem(GRID);
	int numVoxels = 1;
	for (int i=0; i < 3; i++) {
		numVoxels = numVoxels*(g->Size(i)+2);
	}
	int numCompounds = g->numberOfCompounds();
	int dim = numVoxels*numCompounds;
	int maxit = 100;   // how fast does that normally converge? is there any heuristic for choosing maxit?
	double tol = 1e-6;  // precision
	VECTOR_double* x = new VECTOR_double(dim,0);
	MATRIX_double* H = new MATRIX_double(dim+1,dim,0.0);
	int result = 0;// = GMRES(g->getCKLHS(), x, g->getCKRHS(), g->getPrecond(), H, dim, maxit, tol);
	g->setCKVar(x);
	// error checking
	if (result != 0) {
		cerr << "CrankNicolson::step did not converge. don't really know what to do so i just abort" << endl;
		exit(1);
	}
	//Updating the database with the solution
	g->enterCKSolution();
}

void ForwardEuler::step(Agent* inAgent,double t) {
	//Getting the grid database
    // cout << "ForwardEuler::step()" << endl;
    
	CartesianGrid* g = (CartesianGrid*)inAgent->getDatabase()->getDataItem(GRID);
	//Identifying the dt that will guarantee stability
	double dt = t;
	for (int m=0; m < g->numberOfCompounds(); m++) {
		double newdt = MAXH()*g->getBlockSize()*g->getBlockSize()/g->getDiffCoeff(m);
		if (newdt < dt) {
			dt = newdt;
		}
	}

	//Ensuring that the time step is hit exactly
	int numSteps = int(t/dt+1);
	dt = double(t/numSteps);
	getOutputStream() << get_propagator()->getCurrentTime() << ";" << dt << ";" << numSteps << ";";
	for (int i = 0; i < g->xSize(); ++i) {
        getOutputStream() << g->getVoxel(i,4,4)->compoundConcentrations[0] << ";";
    }
	getOutputStream() << endl;
    
	//Clearing the netflux variables in the boundary cells from the previous iteration
	vector<Voxel*> boundaryVoxels = g->getBoundaryVoxels();
    
	for (int i=0; i < int(boundaryVoxels.size()); i++) {
		if (boundaryVoxels[i]->bv != NULL) {
			for (int m=0; m < g->numberOfCompounds(); m++) {
				boundaryVoxels[i]->bv->netFlux[m];
				boundaryVoxels[i]->compoundConcentrations[m] = boundaryVoxels[i]->bv->originalConcentration[m];
			}
		}
	}
	//Running the forward euler method
	for (int n=0; n < numSteps; n++) {
		for (int m=0; m < g->numberOfCompounds(); m++) {
			double coef = dt*g->getDiffCoeff(m)/(g->getBlockSize()*g->getBlockSize());
			for (int i=0; i < g->xSize(); i++) {
				for (int j=0; j < g->ySize(); j++) {
					for (int k=0; k < g->zSize(); k++) {
						if (!g->getVoxel(i,j,k)->wall && !g->getVoxel(i,j,k)->fixed) {
							double conc = g->getGridConcentration(i,j,k,m);
							conc += this->calcDiffusionBetweenVoxels(i,j,k,i-1,j,k,m,coef,g);
							conc += this->calcDiffusionBetweenVoxels(i,j,k,i+1,j,k,m,coef,g);
							conc += this->calcDiffusionBetweenVoxels(i,j,k,i,j-1,k,m,coef,g);
							conc += this->calcDiffusionBetweenVoxels(i,j,k,i,j+1,k,m,coef,g);
							conc += this->calcDiffusionBetweenVoxels(i,j,k,i,j,k-1,m,coef,g);
							conc += this->calcDiffusionBetweenVoxels(i,j,k,i,j,k+1,m,coef,g);
							g->setBufferConcentration(g->getVoxel(i,j,k),conc);
						}
					}	
				}
			}
			//Updating the boundary cells with interpolated fluxes and net flux from current round of diffusion
			for (int i=0; i < int(boundaryVoxels.size()); i++) {
				if (boundaryVoxels[i]->bv != NULL && !boundaryVoxels[i]->wall && !boundaryVoxels[i]->fixed) {
					boundaryVoxels[i]->bufferConcentration += boundaryVoxels[i]->bv->interpolatedFlux[m]*dt/t;
					boundaryVoxels[i]->compoundConcentrations[m] = boundaryVoxels[i]->bufferConcentration;
				}
			}
			g->setGridConcentrationsFromBuffer(m);
		}
		if (subSim != NULL) {
			subSim->step(inAgent,dt);
		}
	}
}

double ForwardEuler::calcDiffusionBetweenVoxels(int x,int y,int z,int x2, int y2, int z2, int compound,double coef,CartesianGrid* g) {
	double concChange = 0;
	Voxel* temp = g->getVoxel(x2,y2,z2);
	if (!temp->wall) {
		concChange += coef*(g->getGridConcentration(x2,y2,z2,compound)-g->getGridConcentration(x,y,z,compound));
		if (temp->bv != NULL) {
			temp->bv->netFlux[compound] += -concChange*g->getVoxelVolume();
			temp->bufferConcentration = temp->compoundConcentrations[compound]-concChange;
		}
	}
	return concChange;
}

void CellMovement::step(Agent* inAgent,double t) {
	//Getting the grid database
	CartesianGrid* g = (CartesianGrid*)inAgent->getDatabase()->getDataItem(GRID);
	//Ensuring that the time step is hit exactly
	int numSteps = int(t/dt()+1);
	double dt = double(t/numSteps);
	//Running the timesteps
	for (int i=0; i < numSteps; i++) {
		for (int j=0; j < g->getNumCells(); j++) {
			CellWorldData* currentCell = g->getCellByIndex(j);
			// First check that the cell lies within the world. If it does not, it is not simulated
			if (g->cellIsInBounds(currentCell)) {
				Voxel* oldVoxel = g->getVoxel(currentCell->coordinates);
				// Adjusting grid concentraions based on cell excretions
				for (int k=0; k < g->numberOfCompounds(); k++) {
					double amount = oldVoxel->compoundConcentrations[k]*g->getVoxelVolume();
					amount += currentCell->excretionRates[k]*dt;
					oldVoxel->compoundConcentrations[k] = amount / ( 
						g->getBlockSize() * g->getBlockSize() * g->getBlockSize());
				}
				// Changing coordinates and diameters
				for (int i=0; i < 3; i++) {
					currentCell->coordinates[i] += dt * currentCell->speed[i];
				}
				// Check if this move causes the cell to run into a wall or another cell
				bool collision = false;
				Voxel* newVoxel = g->getVoxel(currentCell->coordinates);
				if(newVoxel->wall || g->checkCellCollision(currentCell)) {
					// if it does, move the cell back to it's old location
					// FIXME: this is not physically accurate
					for (int i=0; i < 3; i++) {
						currentCell->coordinates[i] -= dt * currentCell->speed[i];
					}
					collision = true;
				}
				currentCell->diameter += currentCell->dDiameter;
				// Check if the cell is moving into a different voxel
				bool different = false;
				for (int i=0; i < 3; i++) {
					if (newVoxel->coordinates[i] != oldVoxel->coordinates[i]) {
						different = true;
					}
				}
				if (different && ! collision) {
					// Removing the cell from its previous voxel
					for (list<CellWorldData*>::iterator IT = oldVoxel->cells.begin(); 
						IT != oldVoxel->cells.end(); IT++) {
						if ((*IT) == currentCell) {
							oldVoxel->cells.erase(IT);
							break;
						}
					}
					// Adding cell to its new voxel
					newVoxel->cells.push_back(currentCell);
					// Adding new voxel to history
					CellHistory* newHistory = new CellHistory;
					newHistory->voxelCoord = newVoxel->coordinates;
					newHistory->enterTime = get_propagator()->getCurrentTime();
					currentCell->history.push_back(newHistory);
				}
			}
		}
		if (subSim != NULL) {
			subSim->step(inAgent,dt);
		}
	}
	g->printCellLocations();
}

/*
 * cartesianGrid AgentFactory
 *
 */

// Helper functions for parsing xml for grid
void setCompoundInVoxel(xmlParsedNode* compoundNode, Voxel* voxel, CartesianGrid* grid);
double getNodeConcentration(xmlParsedNode* node);
Voxel* getVoxelFromNode(xmlParsedNode* voxelNode, CartesianGrid* grid);
double calcMean(vector<double>& values);

// Helper functions for linear interpolation
double linearInterpolate(int x, int x1, int x2, double f1, double f2);
double bilinearInterpolate(int x, int y,
                           int x1, int x2, int y1, int y2,
                           double f11, double f12, double f21, double f22);
double trilinearInterpolate(int x, int y, int z,
                            int x1, int x2, int y1, int y2, int z1, int z2,
                            double f111, double f112, double f121, double f122,
                            double f211, double f212, double f221, double f222);
void linearInterpolateGrid(CartesianGrid* grid);

WorldFactory::WorldFactory(const int type):AgentFactory() {
	simulatorType = type;
}

WorldFactory::WorldFactory(InputSystemReader *isr):AgentFactory(isr) {}

WorldFactory::~WorldFactory() {}

Agent *WorldFactory::createAgent() {
	//Creating agent
	Agent *a = new Agent();
	//Creating message generator
	GenerateWorldMessageToCell* msgGen = new GenerateWorldMessageToCell(a);
	//Adding message generator to world
	a->addMessageGenerator(msgGen);
	//Creating message generator
	//Creating database
	CartesianGrid* worldDB = new CartesianGrid;
	//Add database to agent
	a->getDatabase()->addData(worldDB->getName(),worldDB);
	//Adding simulator to agent
	addSimulatorToAgent(a);	
	//Creating world action
	SyncWorldToCell* worldAction = new SyncWorldToCell();
	//Adding action to the world
	a->addAction(worldAction);

    // universe
    GenerateWorldMessageToUniverse* msgGen2 = new GenerateWorldMessageToUniverse(a);
    a->addMessageGenerator(msgGen2);
	SyncUniverseToWorld* worldAction2 = new SyncUniverseToWorld();
    a->addAction(worldAction2);
	GenerateWorldFluxMessageToUniverse* msgGen3 = new GenerateWorldFluxMessageToUniverse(a);
	a->addMessageGenerator(msgGen3);
	RecvUniverseFluxMessage* worldAction3 = new RecvUniverseFluxMessage();
    a->addAction(worldAction3);
    

	return a;
};

Agent *WorldFactory::createAgent(xmlParsedNode* node)
{
    // Basic initializaiton
	Agent *a = new Agent();
    // Cofigure grid bounds, size, starting point
	CartesianGrid* grid = new CartesianGrid;
	a->getDatabase()->addData(GRID,grid);
    vector<double> Start(3,0);
    int xLen, yLen, zLen;
    double blockSize;
    if (node->attr.count("xLen") == 0 ||
        node->attr.count("yLen") == 0 ||
        node->attr.count("zLen") == 0 ||
        node->attr.count("blockSize") == 0 ) {
        cerr << "Error in parsing world size into composer.";
        exit(1);
    }
    xLen = atoi(node->attr["xLen"].c_str());
    yLen = atoi(node->attr["yLen"].c_str());
    zLen = atoi(node->attr["zLen"].c_str());
    blockSize = atof(node->attr["blockSize"].c_str());
    if (node->attr.count("xStart") != 0) {
        Start[0] = atof(node->attr["xStart"].c_str());
    }
    if (node->attr.count("yStart") != 0) {
        Start[1] = atof(node->attr["xStart"].c_str());
    }
    if (node->attr.count("zStart") != 0) {
        Start[2] = atof(node->attr["xStart"].c_str());
    }
	grid->createGrid(xLen, yLen, zLen, blockSize, Start);
    // Initialize walls
    if (node->attr.count("edgeWalls") != 0) {
        vector<vector<int> > wallCoords; 
        for (int i=0; i < 3; i++) { // do z=0 & z=Len(3)-1 walls first, then x, then y
            for (int j=0; j < grid->Len(i); j++) { // j ranges over Len of x, y, then z
                for (int k=0; k < grid->Len((i+1)%3); k++) { // k ranges over y, z, then x
                    vector<int> wallCoord_1(3,0); 
                    vector<int> wallCoord_2(3,0); 
                    wallCoord_1[i] = j;
                    wallCoord_1[(i+1) % 3] = k;
                    wallCoord_1[(i+2) % 3] = 0; // (i+2) % 3 is 2, then 0, then 1
                    wallCoord_2[i] = j;
                    wallCoord_2[(i+1) % 3] = k;
                    wallCoord_2[(i+2) % 3] = int(grid->Len((i+2) % 3)-1);
                    wallCoords.push_back(wallCoord_1);
                    wallCoords.push_back(wallCoord_2);
                }
            }
        }
        grid->setWalls(wallCoords);
    }
    // Now iterate through children (either voxels or compounds)
    for(vector<xmlParsedNode*>::iterator IT = node->children.begin();
         IT != node->children.end(); IT++) {
        xmlParsedNode* node = (*IT);
        if (node->type == "Element" && node->value == "Compound") {
            // Got compound, get parameters, initialize
            if (node->attr.count("name") == 0) {
                cerr << "Error in parsing compound." << endl;
            } else {
                string cpdName = (string) node->attr["name"];
                double diffusion = 1;
				double defaultconc = 0;
                if (node->attr.count("diffusion") != 0) {
                   diffusion = atof(node->attr["diffusion"].c_str());
                }
				if (node->attr.count("defaultconc") != 0) {
                   defaultconc = atof(node->attr["defaultconc"].c_str());
                }
                int cpdId = grid->addCompound(cpdName, diffusion, defaultconc);
            // Then iterate through children (voxels) and add compound to them
                for(vector<xmlParsedNode*>::iterator it = node->children.begin();
                    it != node->children.end(); it++) {
                    xmlParsedNode* voxelNode = (*it);
                    Voxel* myVoxel = getVoxelFromNode(voxelNode, grid);
					double concentration = getNodeConcentration(voxelNode);
					if (myVoxel->bv != NULL) {
						myVoxel->bv->originalConcentration[cpdId] = concentration;
					}
                    grid->setGridConcentration(myVoxel, cpdId, concentration);
                }
            }
        } else if (node->type == "Element" && node->value == "Voxel") {
            // Got voxel node. Get voxel, then set compound nodes for children
            Voxel* myVoxel = getVoxelFromNode(node, grid);
            for(vector<xmlParsedNode*>::iterator it = node->children.begin();
                it != node->children.end(); it++) {
                xmlParsedNode* cpdNode = (*it); 
                setCompoundInVoxel(cpdNode, myVoxel, grid);
            }
        } else {
            cerr << "Unknown node type under World!" << endl;
        }
    }
    // Now do linear interpolation...
    linearInterpolateGrid(grid);
	// Adding simulators to agent
	addSimulatorToAgent(a);	
	// Creating world actions
	SyncWorldToCell* worldAction = new SyncWorldToCell();
    SyncUniverseToWorld* worldAction2 = new SyncUniverseToWorld();
	a->addAction(worldAction);
    a->addAction(worldAction2);
    // Creating world message generators
	GenerateWorldMessageToCell* msgGen = new GenerateWorldMessageToCell(a);
    GenerateWorldMessageToUniverse* msgGen2 = new GenerateWorldMessageToUniverse(a);
	a->addMessageGenerator(msgGen);
    a->addMessageGenerator(msgGen2);
    // Setting communicator:
    Communicator* c = new Communicator();
    a->addCommunicator(c);
    return a;
}

void WorldFactory::addChild(Agent* parent, Agent* child, xmlParsedNode* config){
    // Get World grid
    CartesianGrid* grid = (SimpleCell::CartesianGrid*) parent->getDatabase()->getDataItem("grid");
    if (grid == 0) {
        cerr << "Unable to find World grid!" << endl;
        exit(1);
    }
    double x, y, z, diameter;
    if (config->attr.count("x") == 0 && config->attr.count("y") == 0 &&
        config->attr.count("z") == 0 && config->attr.count("diameter") == 0 ) {
        cerr << "Invalid cell type givent to addChild in WorldAgentFactory." << endl;
        exit(1);
    }
    x = atof((const char*)config->attr["x"].c_str());
    y = atof(config->attr["y"].c_str());
    z = atof(config->attr["z"].c_str());
    diameter = atof(config->attr["diameter"].c_str());
    grid->addCell(child->getAgentId(),x,y,z,diameter);
    parent->addChild(child->getAgentId(), child->getAgentNode());
    child->setParent(parent->getAgentId(), parent->getAgentNode());
}

void setCompoundInVoxel(xmlParsedNode* compoundNode, Voxel* voxel, CartesianGrid* grid)
{
    if (compoundNode->type != "Element" || compoundNode->value != "Compound") { 
        cerr << "Warning: invalid type passed to setCompoundInVoxel," <<
             " in WorldFactory." << endl;
        exit(1);
    }
    int cpdId;
    if (compoundNode->attr.count("name") == 0) {
        cerr << "Error in parsing compound." << endl;
        exit(1);
    } 
    string cpdName = (string) compoundNode->attr["name"];
    double diffusion = 1;
    if (compoundNode->attr.count("diffusion") != 0) {
       diffusion = atof(compoundNode->attr["diffusion"].c_str());
    }
    cpdId = grid->addCompound(cpdName, diffusion); 
    double concentration = getNodeConcentration(compoundNode);
    grid->setGridConcentration(voxel, cpdId, concentration); 
}

double getNodeConcentration(xmlParsedNode* node)
{
    double concentration;
    if ( node->type != "Element" ) {
        cerr << "Invalid type for node in getNodeConcentration" << endl;
        exit(1);
    }
    for(vector<xmlParsedNode*>::iterator it = node->children.begin();
        it != node->children.end(); it++) {
        xmlParsedNode* text = (*it);
        if (text->type == "Text") {
            concentration = atof(text->value.c_str());
            break;
        }
    }
    return concentration;
}

Voxel* getVoxelFromNode(xmlParsedNode* voxelNode, CartesianGrid* grid)
{
    if (voxelNode->type != "Element" || voxelNode->value != "Voxel") {
        cerr << "Invalid type passed to makeVoxel in WorldFactory" << endl;
        exit(1);
    }
    int x, y, z;
    if (voxelNode->attr.count("x") == 0 ||
        voxelNode->attr.count("y") == 0 ||
        voxelNode->attr.count("z") == 0) {
        cerr << "Error in parsing voxel.";
        exit(1);
    } 
    x = atoi(voxelNode->attr["x"].c_str());
    y = atoi(voxelNode->attr["y"].c_str());
    z = atoi(voxelNode->attr["z"].c_str());
    return grid->getVoxel(x,y,z);
}
    
     
double calcMean(vector<double>& values) 
{
    if (values.size() == 0) return .0;
    double sum = .0;
    for (unsigned int i = 0; i < values.size(); ++i) {
        sum += values[i];
    }
    return sum / values.size();
}

double linearInterpolate(int x, int x1, int x2, double f1, double f2) 
{
    if (x1 == x2) return f1;
    return f1 + (f2 - f1) / (x2 - x1) * (x - x1);
}
double bilinearInterpolate(int x, int y,
                           int x1, int x2, int y1, int y2,
                           double f11, double f12, double f21, double f22) 
{
    double vx1 = linearInterpolate(y, y1, y2, f11, f12);
    double vx2 = linearInterpolate(y, y1, y2, f21, f22);
    return linearInterpolate(x, x1, x2, vx1, vx2);
}

double trilinearInterpolate(int x, int y, int z,
                            int x1, int x2, int y1, int y2, int z1, int z2,
                            double f111, double f112, double f121, double f122,
                            double f211, double f212, double f221, double f222) 
{
    double vx1 = bilinearInterpolate(y, z, y1, y2, z1, z2, f111, f112, f121, f122);
    double vx2 = bilinearInterpolate(y, z, y1, y2, z1, z2, f211, f212, f221, f222);
    return linearInterpolate(x, x1, x2, vx1, vx2);
}


void linearInterpolateGrid(CartesianGrid* grid) 
{
    double decreaseRate = 0.1;
    int nCompounds = grid->numberOfCompounds();

    for (int c = 0; c < nCompounds; ++c) {
        vector<int> xs(grid->xSize(), 0);
        vector<int> ys(grid->ySize(), 0);
        vector<int> zs(grid->zSize(), 0);
        for (int x = 0; x < grid->xSize(); ++x) {
            for (int y = 0; y < grid->ySize(); ++y) {
                for (int z = 0; z < grid->zSize(); ++z) {
                    if (grid->getGridConcentration(x, y, z, c) > .0 ||
                        ((x == 0 || x == grid->xSize() - 1) &&
                         (y == 0 || y == grid->ySize() - 1) &&
                         (z == 0 || z == grid->zSize() - 1))) {
                        xs[x] = ys[y] = zs[z] = 1;
                    }
                }
            }
        }

        vector<int> dxs;
        for (int x = 0; x < grid->xSize(); ++x) {
            if (xs[x] == 1) dxs.push_back(x);
        }
        vector<int> dys;
        for (int y = 0; y < grid->ySize(); ++y) {
            if (ys[y] == 1) dys.push_back(y);
        }
        vector<int> dzs;
        for (int z = 0; z < grid->zSize(); ++z) {
            if (zs[z] == 1) dzs.push_back(z);
        }

        // first fill points on the discretization grid
        for (unsigned int i = 0; i < dxs.size(); ++i) {
            for (unsigned int j = 0; j < dys.size(); ++j) {
                for (unsigned int k = 0; k < dzs.size(); ++k) {
                    int x = dxs[i];
                    int y = dys[j];
                    int z = dzs[k];
                    if (grid->getGridConcentration(x, y, z, c) > .0) continue;
                    
                    vector<double> vs; 

                    int i1 = i - 1;
                    while (i1 >= 0) {
                        int x1 = dxs[i1];
                        double gc = grid->getGridConcentration(x1, y, z, c);
                        if (gc > .0) {
                            double ngc = gc - (x - x1) * decreaseRate;
                            if (ngc > .0) vs.push_back(ngc);
                        }
						i1--;
                    }
                    int i2 = i + 1;
                    while (i2 < (int) dxs.size()) {
                        int x2 = dxs[i2];
                        double gc = grid->getGridConcentration(x2, y, z, c);
                        if (gc > .0) {
                            double ngc = gc - (x2 - x) * decreaseRate;
                            if (ngc > .0) vs.push_back(ngc);
                        }
						i2++;
                    }
                    int j1 = j - 1;
                    while (j1 >= 0) {
                        int y1 = dys[j1];
                        double gc = grid->getGridConcentration(x, y1, z, c);
                        if (gc > .0) {
                            double ngc = gc - (y - y1) * decreaseRate;
                            if (ngc > .0) vs.push_back(ngc);
                        }
						j1--;
                    }
                    int j2 = j + 1;
                    while (j2 < (int) dys.size()) {
                        int y2 = dys[j2];
                        double gc = grid->getGridConcentration(x, y2, z, c);
                        if (gc > .0) {
                            double ngc = gc - (y2 - y) * decreaseRate;
                            if (ngc > .0) vs.push_back(ngc);
                        }
						j2++;
                    }
                    int k1 = k - 1;
                    while (k1 >= 0) {
                        int z1 = dzs[k1];
                        double gc = grid->getGridConcentration(x, y, z1, c);
                        if (gc > .0) {
                            double ngc = gc - (z - z1) * decreaseRate;
                            if (ngc > .0) vs.push_back(ngc);
                        }
						k1--;
                    }
                    int k2 = k + 1;
                    while (k2 < (int) dzs.size()) {
                        int z2 = dzs[k2];
                        double gc = grid->getGridConcentration(x, y, z2, c);
                        if (gc > .0) {
                            double ngc = gc - (z2 - z) * decreaseRate;
                            if (ngc > .0) vs.push_back(ngc);
                        }
						k2++;
                    }
                    Voxel* voxel = grid->getVoxel(x,y,z); 
                    grid->setGridConcentration(voxel, c, calcMean(vs));
                }
            }
        }
        for (int i = (int)dxs.size() - 1; i >= 0; --i) {
            for (int j = (int)dys.size() - 1; j >= 0; --j) {
                for (int k = (int)dzs.size() - 1; k >= 0; --k) {
                    int x = dxs[i];
                    int y = dys[j];
                    int z = dzs[k];
                    if (grid->getGridConcentration(x, y, z, c) > .0) continue;
                    
                    vector<double> vs; 

                    int i1 = i - 1;
                    while (i1 >= 0) {
                        int x1 = dxs[i1];
                        double gc = grid->getGridConcentration(x1, y, z, c);
                        if (gc > .0) {
                            double ngc = gc - (x - x1) * decreaseRate;
                            if (ngc > .0) vs.push_back(ngc);
                        }
						i1--;
                    }
                    int i2 = i + 1;
                    while (i2 < (int) dxs.size()) {
                        int x2 = dxs[i2];
                        double gc = grid->getGridConcentration(x2, y, z, c);
                        if (gc > .0) {
                            double ngc = gc - (x2 - x) * decreaseRate;
                            if (ngc > .0) vs.push_back(ngc);
                        }
						i2++;
                    }
                    int j1 = j - 1;
                    while (j1 >= 0) {
                        int y1 = dys[j1];
                        double gc = grid->getGridConcentration(x, y1, z, c);
                        if (gc > .0) {
                            double ngc = gc - (y - y1) * decreaseRate;
                            if (ngc > .0) vs.push_back(ngc);
                        }
						j1--;
                    }
                    int j2 = j + 1;
                    while (j2 < (int) dys.size()) {
                        int y2 = dys[j2];
                        double gc = grid->getGridConcentration(x, y2, z, c);
                        if (gc > .0) {
                            double ngc = gc - (y2 - y) * decreaseRate;
                            if (ngc > .0) vs.push_back(ngc);
                        }
						j2++;
                    }
                    int k1 = k - 1;
                    while (k1 >= 0) {
                        int z1 = dzs[k1];
                        double gc = grid->getGridConcentration(x, y, z1, c);
                        if (gc > .0) {
                            double ngc = gc - (z - z1) * decreaseRate;
                            if (ngc > .0) vs.push_back(ngc);
                        }
						k1--;
                    }
                    int k2 = k + 1;
                    while (k2 < (int) dzs.size()) {
                        int z2 = dzs[k2];
                        double gc = grid->getGridConcentration(x, y, z2, c);
                        if (gc > .0) {
                            double ngc = gc - (z2 - z) * decreaseRate;
                            if (ngc > .0) vs.push_back(ngc);
                        }
						k2++;
                    }
                    Voxel* voxel = grid->getVoxel(x,y,z);
                    grid->setGridConcentration(voxel, c, calcMean(vs));
                }
            }
        }

        // lookup table for lower and upper discretization lines
        vector<int> xl(grid->xSize(), 0); // lower 
        vector<int> xu(grid->xSize(), 0); // upper
        for (int i = 0; i < grid->xSize(); ++i) {
            int j = grid->xSize() - i - 1;
            xl[i] = (xs[i] == 1 || i == 0) ? i : xl[i - 1];
            xu[j] = (xs[j] == 1 || i == 0) ? j : xu[j + 1]; // intentional: i == 0 
        }

        vector<int> yl(grid->ySize(), 0);  
        vector<int> yu(grid->ySize(), 0); 
        for (int i = 0; i < grid->ySize(); ++i) {
            int j = grid->ySize() - i - 1;
            yl[i] = (ys[i] == 1 || i == 0) ? i : yl[i - 1];
            yu[j] = (ys[j] == 1 || i == 0) ? j : yu[j + 1]; // intentional: i == 0 
        }

        vector<int> zu(grid->zSize(), 0); 
        vector<int> zl(grid->zSize(), 0);  
        for (int i = 0; i < grid->zSize(); ++i) {
            int j = grid->zSize() - i - 1;
            zl[i] = (zs[i] == 1 || i == 0) ? i : zl[i - 1];
            zu[j] = (zs[j] == 1 || i == 0) ? j : zu[j + 1]; // intentional: i == 0 
        }

        for (int x = 0; x < grid->xSize(); ++x) {
            for (int y = 0; y < grid->ySize(); ++y) {
                for (int z = 0; z < grid->zSize(); ++z) {
                    double gc = grid->getGridConcentration(x, y, z, c);
                    if (gc > .0) continue;

                    int x1 = xl[x];
                    int x2 = xu[x];
                    int y1 = yl[y];
                    int y2 = yu[y];
                    int z1 = zl[z];
                    int z2 = zu[z];

                    double f111 = grid->getGridConcentration(x1, y1, z1, c);
                    double f112 = grid->getGridConcentration(x1, y1, z2, c);
                    double f121 = grid->getGridConcentration(x1, y2, z1, c);
                    double f122 = grid->getGridConcentration(x1, y2, z2, c);
                    double f211 = grid->getGridConcentration(x2, y1, z1, c);
                    double f212 = grid->getGridConcentration(x2, y1, z2, c);
                    double f221 = grid->getGridConcentration(x2, y2, z1, c);
                    double f222 = grid->getGridConcentration(x2, y2, z2, c);
                    
                    double ngc = trilinearInterpolate(x, y, z,
                                                      x1, x2, y1, y2, z1, z2,
                                                      f111, f112, f121, f122,
                                                      f211, f212, f221, f222);
                    Voxel* voxel = grid->getVoxel(x,y,z);
                    grid->setGridConcentration(voxel, c, ngc);
                }
            }
        }
    }
}

void WorldFactory::addActionsToAgentsActionSet(Agent *ag)
{
	SyncWorldToCell* newAction = new SyncWorldToCell();
	ag->addAction(newAction);
}

void WorldFactory::addSimulatorToAgent(Agent *ag) {
	ForwardEuler* diffSim = new ForwardEuler();
	ag->addSimulator(diffSim);
	CellMovement* cellSim = new CellMovement();
	diffSim->setSubSimulator(cellSim);

	// CellMovement* cellSim = new CellMovement();
	// ag->addSimulator(cellSim);
};

Agent *WorldFactory::createAgent(Action **ac) {
	return NULL;
};

Agent *WorldFactory::duplicateAgent(Agent *ag) {
	return 0;
};

void WorldFactory::finalise() {};
