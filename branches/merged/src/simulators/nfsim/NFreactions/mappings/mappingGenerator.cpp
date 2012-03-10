

#include "mappingGenerator.hh"


using namespace NF;


MapGenerator::MapGenerator(unsigned int mappingIndex)
{
	this->mappingIndex = mappingIndex;
}

bool MapGenerator::map(MappingSet *mappingSet, Molecule *molecule)
{
	mappingSet->set(mappingIndex,molecule);
	return true;
}
