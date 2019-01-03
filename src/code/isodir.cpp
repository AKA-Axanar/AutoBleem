//
// Created by screemer on 23.12.18.
//

#include "isodir.h"

string Isodir::removeVersion(string input)
{
    int len=input.length();
    if (len>2)
    {
        int semiColon = input.find(';',0);
        if (semiColon!=-1)
        {
            input = input.substr(0,input.find(';',0));
        }
    }
    return input;
}

int Isodir::getSectorAddress(int sector) {
    return sector * SECTOR_SIZE + offset;
}

unsigned char Isodir::readChar() {
    unsigned char c;
    stream.read((char*)&c, 1);
    return c;
}

string Isodir::readString(int size) {
    char str[size + 1];
    str[size] = 0;
    stream.read(str, size);
    return str;
}

unsigned long Isodir::readDword() {
    unsigned long res = 0;
    unsigned long c;
    c = readChar();
    res += c;
    c = readChar();
    res += c << (1 * 8);
    c = readChar();
    res += c << (2 * 8);
    c = readChar();
    res += c << (3 * 8);
    return res;

}

int Isodir::findPrimaryDescriptor(int maxOffset) {
    int addressStart = getSectorAddress(16);
    int address = addressStart;
    stream.seekg(addressStart, ios::beg);
    for (int i = 0; i < maxOffset; i++) {
        char c = readChar();
        if (c != 1) {
            address++;
            c = readChar();
        } else {
            int pos = stream.tellg();
            string str = readString(5);
            address = pos;
            stream.seekg(pos, ios::beg);
            if (str == "CD001") {
                int addr = stream.tellg();
                offset = pos - 1 - addressStart;
                return addr - 1;
            } else {
                address++;
                c = readChar();
            }
        }

    }
    return -1;
}

IsoDirectory Isodir::getEmptyDir()
{
    IsoDirectory dir;
    dir.systemName="UNKNOWN";
    dir.volumeName = "UNKNOWN";
    return dir;
}

void Isodir::readDir(vector<string> * data, unsigned int sector, int maxlevel, int level)
{
    if (level>=maxlevel)
    {
        return;
    }
    unsigned int currSector=sector;
    long currentPos = stream.tellg();
    long sectorAddr = getSectorAddress(sector);
    stream.seekg(sectorAddr, ios::beg);

    for (int i=0;i<200;i++){ // max 200 entries - it will be less and it will just break the loop
        long start = stream.tellg();
        if (start==-1) break;
        long readInSector = start-sectorAddr;
        if (readInSector>=2048)
        {
            currSector++;
            sectorAddr = getSectorAddress(currSector);
            stream.seekg(sectorAddr, ios::beg);
            continue;
        }
        long len = readChar();
        if (len==0)
        {

            currSector++;
            sectorAddr = getSectorAddress(currSector);
            stream.seekg(sectorAddr, ios::beg);
            if (readInSector<2048-62)
            {
                break;
            }
            continue;
        }

        int ealen = readChar();
        unsigned int loc = readDword();
        //stream.seekg(26, ios::cur);
        stream.seekg(4, ios::cur);
        stream.seekg(8, ios::cur);
        stream.seekg(7, ios::cur);
        int attr = readChar();
        stream.seekg(6, ios::cur);

        int fileNameLen = readChar();
        if (fileNameLen < 2) {
            stream.seekg(start + len);
            if (fileNameLen == 0) break;
            continue;
        }
        string fileName = readString(fileNameLen);
        data->push_back(removeVersion(fileName));
        if ((attr>>1) && 1)
        {
            readDir(data,loc, maxlevel,level+1);
        }
        stream.seekg(start + len);
    }

    stream.seekg(currentPos, ios::beg);
}
 IsoDirectory Isodir::getDir(string binPath,int maxlevel) {
    offset=0;
    stream.open(binPath, ios::binary);
    if (!stream.good()) {
        return getEmptyDir();
    }
    stream.seekg(0,ios::beg);
    int pdvAddress = findPrimaryDescriptor(50);
    if (pdvAddress==-1)
    {
        return getEmptyDir();
    }
    int sectorAddr = getSectorAddress(16);
    stream.seekg(sectorAddr, ios::beg);
    stream.seekg(8, ios::cur);
    string system = readString(32);
    string volname = readString(32);
    stream.seekg(86, ios::cur);
    int sector = readDword();
    stream.seekg(getSectorAddress(sector), ios::beg);
    IsoDirectory result;
    result.systemName = trim(system);
    result.volumeName = trim(volname);
    readDir(&result.rootDir,sector,maxlevel,0);
    stream.close();

    if (result.rootDir.empty())
    {
        return getEmptyDir();
    }
    return result;
}