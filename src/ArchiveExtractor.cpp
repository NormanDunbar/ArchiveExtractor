//=========================================================================
// An attempt to extract data from a corrupted Archive File.
// Which doesn't actually have to be corrupted! It works on perfectly good
// files too!
//
// Call it thus:
//
//      ArchiveExtractor archive_dbf_filename
//
//
// Output is to the console, so it can be redirected:
//
//      ArchiveExtractor archive_dbf_filename > data.log
//
// Debugging, logging, structure information messages etc go to stderr, so
// those can be redirected too:
//
//      ArchiveExtractor archive_dbf_filename >data.log 2>structure.log
//
//
//=========================================================================
// 2016/11/29  Norman Dunbar  Created.
//=========================================================================

#define __ARCHIVEEXTRACTOR__
#include "ArchiveExtractor.h"

int main (int argc, char *argv[])
{
    // We need at least one argument.
    if (argc <= 1) {
        cerr << "No parameters supplied. Nothing to do." << endl;
        return 0;
    }

    // We have a/some file(s), try to process it/them.
    for (int x = 1; x < argc; x++) {
        fileName = argv[x];
        doFile(fileName);
    }
    
    return 0;
}


void doFile(const char *fname)
{
    // Open a dbf file, extract the structure and the data. We hope!

    dbf = new ifstream(fname, std::ifstream::in | std::ifstream::binary);
    if (!(dbf->good())) {
        cerr << "Cannot open file '" << fname << "'." << endl;
        return;
    }

    cerr << endl << "ARCHIVE FILE" << endl;
    cerr << "============" << endl << endl;
    cerr << "File Name       : " << fname << endl;
    
    bool good = readHeader();
    if (!good) {
        cerr << "Cannot continue. Header invalid." << endl;
        goto closeFile;
    }
    
    good = doStructure();
    if (!good) {
        cerr << "Cannot continue. Structure Table invalid." << endl;
        goto closeFile;
    }

    // This MUST go after the structure table above.
    // If not, numSortedFields is always zero when this runs.
    good = doIndexTable();
    if (!good) {
        cerr << "Cannot continue. Index Table invalid." << endl;
        goto closeFile;
    }
    
    good = doFreeSpace();
    if (!good) {
        cerr << "Cannot continue. Free Space Table appears corrupt." << endl;
        goto closeFile;
    }
   
    
    good = doData();
    if (!good) {
        cerr << "Cannot continue. Data area appears corrupt." << endl;
        goto closeFile;
    }
    

closeFile:    
    dbf->close();
    delete dbf;
    
}


