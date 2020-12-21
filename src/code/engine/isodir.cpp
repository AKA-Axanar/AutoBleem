//
// Created by screemer on 23.12.18.
//

#include "isodir.h"
#include <fstream>
#include <iostream>

using namespace std;




//*******************************
// Isodir::removeVersion
//*******************************
string Isodir::removeVersion(string input)
{
    int len = input.length();
    if (len > 2)
    {
        int semiColon = input.find(';', 0);
        if (semiColon != -1)
        {
            input = input.substr(0, input.find(';', 0));
        }
    }
    return input;
}

//*******************************
// Isodir::getEmptyDir
//*******************************
IsoDirectory Isodir::getEmptyDir()
{
    IsoDirectory dir;
    dir.systemName = "UNKNOWN";
    dir.volumeName = "UNKNOWN";
    return dir;
}

void Isodir::readDir(vector<string> *data, CDReader *reader, unsigned int sector, int maxlevel, int level)
{
    int originalSector = sector;
    if (level >= maxlevel)
    {
        return;
    }
    reader->selectSector(sector);
    for (int i = 0; i < 200; i++)
    {
        sector = reader->getSelSector();
        if (reader->endStream())
        {
            break;
        }
        int startpos = reader->getSectorPos();
        int startsector = reader->getSelSector();
        long len = reader->readChar();

        
        if (len == 0)
        {
            int lastReadPos = reader->getSectorPos();
            reader->selectSector(sector + 1);
            sector = reader->getSelSector();
            if (lastReadPos < 2048 - 62)
            {
                break;
            }
            continue;
        }
        int ealen = reader->readChar();
        unsigned int loc = reader->readDword();
        reader->ffd(4);
        reader->ffd(8);
        reader->ffd(7);
        int attr = reader->readChar();
        reader->ffd(6);

        int fileNameLen = reader->readChar();
        if (fileNameLen < 2)
        {
            reader->selectSector(startsector);
            reader->ffd(startpos + len);
            if (fileNameLen == 0)
                break;
            continue;
        }
        string fileName = reader->readString(fileNameLen);
        data->push_back(removeVersion(fileName));
        if ((attr >> 1) & 1)
        {
            readDir(data, reader, loc, maxlevel, level + 1);
        }
        reader->selectSector(sector);
        reader->ffd(startpos + len);
    }
    reader->selectSector(originalSector);
}

//*******************************
// Isodir::getDir
//*******************************
IsoDirectory Isodir::getDir(string binPath, int maxlevel, bool useCHD)
{
    CDReader *reader;

    if (!useCHD)
    {
        reader = new CDReader();
    }
    else
    {
        reader = new CHDReader();
    }
    reader->openImage(binPath);
    if (!reader->isOpen())
    {
        delete reader;
        return getEmptyDir();
    }
    reader->selectSector(16);
    reader->ffd(8);
    string system = reader->readString(32);
    string volname = reader->readString(32);
    reader->ffd(86);
    int sector = reader->readDword();
    reader->selectSector(sector);
    IsoDirectory result;
    result.systemName = trim(system);
    result.volumeName = trim(volname);
    readDir(&result.rootDir, reader, sector, maxlevel, 0);
    reader->closeImage();
    delete reader;
    if (result.rootDir.empty())
    {
        return getEmptyDir();
    }
    return result;
}
