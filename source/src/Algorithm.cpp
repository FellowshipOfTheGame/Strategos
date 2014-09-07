#include "Algorithm.h"

#if defined(_UNIX_) || defined(__APPLE_CC__)
    #include <sys/types.h>
    #include <sys/stat.h>
#endif // _UNIX_



#include <dirent.h>

Algorithm::Algorithm(int _armyType)
	: armyType(_armyType)
{
    directory = "GA/"+std::to_string(armyType)+"/";
}

Algorithm::~Algorithm()
{
    //dtor
}
void createDir(const std::string& dir);

void Algorithm::initialize()
{
    DIR *dir;
    dirent *ent;

    createNeededDirectory();

    std::string dirpath = "assets/saves/GA/" + std::to_string(armyType) + "/";

    dir = opendir(dirpath.c_str());
    if (dir == nullptr)
    {
        printf("Error opening directory: %s\n", dirpath.c_str());
        exit(-5);
    }

    // Carregar exercitos prontos
    while ((ent = readdir (dir)) != nullptr)
    {
        std::string tmp = "GA/" + std::to_string(armyType) + "/";

        tmp += ent->d_name;
        tmp.replace(tmp.length()-4, 4, "\0");

        Army *army = Army::loadArmy(tmp);

        if(army != nullptr)
            this->addInitialArmy(army);
    }

    closedir (dir);
}

void Algorithm::createNeededDirectory()
{
    DIR *dir;
    std::string dirpath = "assets/saves/";

    dir = opendir(dirpath.c_str());
        if (!dir)
            createDir(dirpath);
        else
            closedir(dir);

    dirpath += "GA/";
    dir = opendir(dirpath.c_str());
        if (!dir)
            createDir(dirpath);
        else
            closedir(dir);

    dirpath += std::to_string(armyType)+"/";
    dir = opendir(dirpath.c_str());
        if (!dir)
            createDir(dirpath);
        else
            closedir(dir);
}
void createDir(const std::string& dir)
{
    int error = 0;
    printf("Creating dir: %s\n", dir.c_str());
    #if defined(_UNIX_) || defined (__APPLE_CC__)
        error = mkdir(dir.c_str(), S_IRWXU | S_IRWXG | S_IRUSR | S_IWUSR | S_IROTH | S_IWOTH | S_IXOTH );
    #else
        error = mkdir(dir.c_str());
    #endif

    if (error == -1)
    {
        printf("Failed to create dir. Error: %d\n", errno);
    }
}

void Algorithm::saveArmy(Army *army, int armyNumber)
{
    char buffer[8];
    sprintf(buffer, "r%d", armyNumber+1);

    army->setArmyName(buffer);
    Army::saveArmy(army, directory.c_str());

#ifdef _DEBUG_
    printf("saving army[%d] as %s\n", armyNumber+1, buffer);
#endif
}
