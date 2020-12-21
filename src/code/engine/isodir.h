//
// Created by screemer on 23.12.18.
//
#pragma once

#include "../main.h"
#include <string>
#include <vector>
#include "cdreader.h"

#define SECTOR_SIZE 2352



//******************
// IsoDirectory
//******************
class IsoDirectory
{
public:
    std::string systemName;
    std::string volumeName;
    std::vector<std::string> rootDir;
};


//******************
// Isodir
//******************
class Isodir  {
public:
    IsoDirectory getDir(std::string binPath,int maxlevel, bool useCHD);
    void readDir(vector<string> *data, CDReader *reader, unsigned int sector, int maxlevel, int level);
    IsoDirectory getEmptyDir();
   
private:

    std::string removeVersion(std::string input);
    std::ifstream * stream;
};
