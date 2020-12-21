// CD image reader classes for scanner

#pragma once

#include "../main.h"
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <libmamecd/cdrom.h>

#include <stdio.h>
#include <string.h>

#define SECTOR_SIZE 2352
#define DATA_SIZE 2048
#define MAX_OFFSET 500

using namespace std;

// Helper class to read bin file as "CDROM" data
class CDReader
{
private:
    ifstream *stream;
    int offset = 0;
    char *buffer;
    int sectorpos = 0;
    int currentSector = 0;
    bool opened = false;

public:
    void setOffset(int off)
    {
        offset = off;
    }
    int getOffset()
    {
        return offset;
    }
    void setBuffer(char *buff)
    {
        buffer = buff;
    }
    char *getBuffer()
    {
        return buffer;
    }
    int getSectorPos()
    {
        return sectorpos;
    }
    void setSectorpos(int pos)
    {
        sectorpos = pos;
    }

    void setCurrentSector(int pos)
    {
        currentSector = pos;
    }
    int getCurrentSector()
    {
        return currentSector;
    }
    bool isOpen()
    {
        return opened;
    }
    void setOpen(bool state)
    {
        opened =state;
    }

    int calibrate(int maxOffset)
    {
        unsigned char c = 0;
        selectSector(16); // go to sector 16
        for (int i = 0; i < maxOffset; i++)
        {
            c = readChar();
            if (c != 1)
            {
                c = readChar();
            }
            else
            {
                int sectorPosNow = getSectorPos();
                int curSectorNow = getSelSector();
                string str = readString(5);
                rev(5);
                if (str == "CD001")
                {
                    cout << "CD001 found in sector:" << curSectorNow << " at pos " << sectorPosNow << endl;
                    offset = (curSectorNow - 16) * SECTOR_SIZE + sectorPosNow - 1;
                    return 0;
                }
                else
                {
                    c = readChar();
                }
            }
        }
        return -1;
    }
    void ffd(int size)
    {
        for (int i = 0; i < size; i++)
        {
            sectorpos++;
            if (sectorpos >= DATA_SIZE)
            {
                selectSector(currentSector + 1);
            }
        }
    }
    void rev(int size)
    {
        for (int i = 0; i < size; i++)
        {
            sectorpos--;
            if (sectorpos < 0)
            {
                selectSector(currentSector - 1);
                sectorpos = (DATA_SIZE - 1);
            }
        }
    }
    unsigned char readChar()
    {
        unsigned char x;
        x = buffer[sectorpos];
        sectorpos++;
        if (sectorpos >= DATA_SIZE)
        {
            selectSector(currentSector + 1);
        }
        return x;
    }
    std::string readString(int size)
    {
        char str[size + 1];
        str[size] = 0;
        for (int i = 0; i < size; i++)
        {
            str[i] = readChar();
        }
        return str;
    }
    unsigned long readDword()
    {
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
    
    virtual void selectSector(int sectorNum)
    {
        int addr = sectorNum * SECTOR_SIZE + offset;
        stream->seekg(addr, ios::beg);
        stream->read(buffer, SECTOR_SIZE);
        sectorpos = 0;
        currentSector = sectorNum;
    }
    int getSelSector()
    {
        return currentSector;
    }
    virtual int openImage(string imagePath)
    {
        buffer = new char[CD_FRAME_SIZE];
        cout << "Opening ISO image" << endl;
        offset = 0;
        opened = false;
        sectorpos = 0;
        currentSector = 0;
        stream = new ifstream(imagePath, ios::binary | ios::in);
        if (!stream->good())
        {
            delete stream;
            return -1;
        }
        if (!stream->is_open())
        {
            delete stream;
            return -1;
        }
        stream->seekg(0);
        if (calibrate(MAX_OFFSET) != 0)
        {
            return -1;
        };
        opened = true;

        return 0;
    }
    virtual void closeImage()
    {
        delete buffer;
        delete stream;
    }

    virtual bool endStream()
    {
        return stream->tellg() == -1;
    }
};

class CHDReader : public CDReader
{
private:
    chd_file *inputchd = NULL;
    cdrom_file *cdrom_chd = NULL;
    const cdrom_toc *toc = NULL;

public:
    void closeImage() override
    {
        delete getBuffer();
        if (cdrom_chd != NULL)
        {
            cdrom_close(cdrom_chd);
            cdrom_chd = NULL;
        }
        if (inputchd != NULL)
        {
            chd_close(inputchd);
            inputchd = NULL;
        }
    }

    bool endStream() override
    {
        if (getCurrentSector() > toc->tracks[0].frames)
            setCurrentSector(toc->tracks[0].frames - 1);
        return (getCurrentSector() == toc->tracks[0].frames - 1) && (getSectorPos() >= 2048);
    }
    int openImage(string imagePath) override
    {
        setBuffer(new char[CD_FRAME_SIZE]);
        cout << "Opening CHD image" << endl;
        setOpen(false);
        setSectorpos(0);
        setCurrentSector(0);
        chd_error err;

        err = chd_open(imagePath.c_str(), CHD_OPEN_READ, NULL, &inputchd);
        if (err != CHDERR_NONE)
        {
            cout << "Error opening CHD file" << endl;
            return -1;
        }

        cdrom_chd = cdrom_open(inputchd);
        if (cdrom_chd == NULL)
        {
            cout << "Error opening CDROM file" << endl;
            return -1;
        }

        toc = cdrom_get_toc(cdrom_chd);

        cout << "TOC found and read - checking for at least one track" << endl;
        if (toc->numtrks < 1)
        {
            cout << "Image has no tracks" << endl;
            return -1;
        }
        if (calibrate(MAX_OFFSET) != 0)
        {
            cout << "Calibrate failed" << endl;
            return -1;
        }
        setOpen(true);
        return 0;
    }

    void selectSector(int sectorNum) override
    {
        char * buff = getBuffer();
        char tempbuff[CD_FRAME_SIZE];
        cdrom_read_data(cdrom_chd, cdrom_get_track_start(cdrom_chd, 0) + sectorNum, (void*)tempbuff, toc->tracks[0].trktype);
        memcpy(buff, tempbuff+getOffset(),DATA_SIZE);
        setSectorpos(0);
        setCurrentSector(sectorNum);
    }
};