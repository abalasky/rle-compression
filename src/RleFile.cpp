#include "RleFile.h"
#include "RleData.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <cstring>


void RleFile::CreateArchive(const std::string& source)
{
    //Declare size variable and char buffer for reading toCompress file
	std::ifstream::pos_type size;
    char* memblock;

    //Open a file object
    std::ifstream file (source, std::ios::in|std::ios::binary|std::ios::ate);

    if(file.is_open()) {

        //Get the size of the file
        size = file.tellg();

        //Allocate an appropriate char buffer
        memblock = new char[static_cast<unsigned int>(size)];

        //Read the file into the buffer
        file.seekg(0, std::ios::beg); //seek back to start of file
        file.read(memblock, size);
        file.close();

        //Run compression method on the char array
        this->mRleData.Compress(memblock, size);

        //Assign compressed filename
        std::string filename = source + ".rl1";

        //Open the file for binary writing
        std::ofstream arc(filename, std::ios::out|std::ios::binary|std::ios::trunc);
        if(arc.is_open()) {

            //Assign header values
            this->mHeader.mFileName = source;
            this->mHeader.mFileNameLength = source.length();
            this->mHeader.mFileSize = size;

            //Write the header
            arc.write(this->mHeader.mSig, 4);

            //Write the original file size
            arc.write(reinterpret_cast<char*>(&(this->mHeader.mFileSize)), 4);

            //Write filename length
            const char* fileNameLenCharArr = reinterpret_cast<char*>(&(this->mHeader.mFileNameLength));

            arc.write(fileNameLenCharArr, strlen(fileNameLenCharArr));

            //Write the filename
            const char* fileNameCharArr = this->mHeader.mFileName.c_str();

            arc.write(fileNameCharArr, strlen(fileNameCharArr));

            //Write the compressed RLE data

            arc.write(this->mRleData.mData.data(), this->mRleData.mData.size());
        }


        //Free memblock
        delete[] memblock;
    }



}

void RleFile::ExtractArchive(const std::string& source)
{
	//Declare size variable and char buffer for reading toCompress file
	std::ifstream::pos_type size;
    char* memblock;

    //Open a file object
    std::ifstream file (source, std::ios::in|std::ios::binary|std::ios::ate);

    if(file.is_open()) {

        //Get the size of the file
        size = file.tellg();

        //Allocate an appropriate char buffer
        memblock = new char[static_cast<unsigned int>(size)];

        //Read the file into the buffer
        file.seekg(0, std::ios::beg); //seek back to start of file
        file.read(memblock, size);
        file.close();


        //Copy values from membloock into member of mHeader
        this->mHeader.mFileSize = *(reinterpret_cast<int*>(&memblock[4])); //located @ 5th byte

        //Get the filename length and use to read filename
        this->mHeader.mFileNameLength = memblock[8];
        std::string fileName;
        for(int i = 0; i < this->mHeader.mFileNameLength; i++) {
            fileName.push_back(memblock[9+i]);
        }

        //Remaining memblock is rle compressed data
        const char* rleData = &(memblock[9+this->mHeader.mFileNameLength]);

        //Decompress data
        int rleDataLength = static_cast<size_t>(size) - (9+this->mHeader.mFileNameLength);
        this->mRleData.Decompress(rleData, rleDataLength, this->mHeader.mFileSize);

        //Write original filename
        std::string filename = source.substr(0,source.size()-4);

        //Open the file for binary writing
        std::ofstream arc(filename, std::ios::out|std::ios::binary|std::ios::trunc);
        if(arc.is_open()) {

            //Write the decompressed data
            arc.write(this->mRleData.mData.data(), this->mHeader.mFileSize);

        }


        //Free memblock
        delete[] memblock;
    }
}
