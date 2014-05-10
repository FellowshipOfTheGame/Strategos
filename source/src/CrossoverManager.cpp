#include <CrossoverManager.h>

CrossoverManager::CrossoverManager()
{

}
CrossoverManager::~CrossoverManager()
{
	
}

void CrossoverManager::crossOver(const Army *parent1, const Army *parent2, std::vector<Army*>& ind)
{
		Army* child1 = new Army(parent1->getName(), parent1->getDictionary());
    Army* child2 = new Army(parent2->getName(), parent2->getDictionary());

    unsigned int i;
    for (i = 0; i < parent1->nUnits() && i < parent2->nUnits(); ++i )
    {
        // Adiciona do pai 1 para filho 1, 2 -> 2
        if (rand()%2 == 0)
        {
            child1->addUnit( new Unit( parent1->getUnitAtIndex(i) ) );
            child2->addUnit( new Unit( parent2->getUnitAtIndex(i) ) );
        }
        else // Adiciona do pai 1 no filho 2, 2 -> 1
        {
            child1->addUnit( new Unit( parent2->getUnitAtIndex(i) ) );
            child2->addUnit( new Unit( parent1->getUnitAtIndex(i) ) );
        }
    }

    // Colocar as restantes nos respectivos filhos 1 e 2
    // Apenas um desses 2 loops deve ocorrer, por isso a mesma variavel i eh usada
    for (; i < parent1->nUnits(); ++i){
        child1->addUnit( new Unit( parent1->getUnitAtIndex(i) ) );
    }
    for (; i < parent2->nUnits(); ++i){
        child2->addUnit( new Unit( parent2->getUnitAtIndex(i) ) );
    }

    //Save the new armies
    ind.push_back(child1);
    ind.push_back(child2);	
}