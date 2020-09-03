#include "SrcMain.h"
#include <string>
#include "RleFile.h"

void ProcessCommandArgs(int argc, const char* argv[])
{
	// TODO: Process argc and argv, and use to compress/decompress files

    if(argc == 2) {

        std::string filename = argv[1];

        RleFile r;
        if(filename.substr(filename.size()-4,filename.size()-1) == ".rl1") {
            r.ExtractArchive(filename);
        }
        else {
            r.CreateArchive(filename);
        }

    }

}
