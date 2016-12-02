#include "ArchiveExtractor.h"

bool readHeader() {
    // Read the DBF file header & validate it.
    headerSize = 0;   
    dataSize = 0;
    indexSize = 0;
    freeSpaceSize = 0;
    structureSize = 0;
    
    // Header size must be exactly 20.
    headerSize = getWord();
    if (headerSize != 20) {
        cerr << "Incorrect header size. Expected 20, got " << headerSize << endl;
        cerr << "Not a valid archive dbf file." << endl;
        return false;
    }
    
    // Do we have a valid header string?
    dbf->get(vrmb, 8+1);
    vrmb[8] = '\0';
    if (vrmb[0] == '\0') {
        cerr << endl << "WARNING: Are you aware that this file may not have been closed properly" << endl;
        cerr << "at some point in the past? Corruption detected in header." << endl;
        cerr << endl << "This will help fix it ..." << endl << endl;
        cerr << "1000 OPEN #3, 'your_file_dbf'" << endl;
        cerr << "1010 x = CODE(INKEY$(#3))" << endl;
        cerr << "1020 x = CODE(INKEY$(#3))" << endl;
        cerr << "1030 PRINT #3, 'v' ; : REMark The semi-colon is extremely important!" << endl;
        cerr << "1040 CLOSE #3" << endl << endl;
        
        // Temporary fix.
        vrmb[0] = 'v';
    }
    
    if (!(strnicmp (vrmb, "vrmb1dbf0", 8))) {
        cerr << endl << "Sorry, this doesn't look like an archive dbf file." << endl;
        cerr << "The header flag is wrong. Expected 'vrmb1dbf0', found '" << vrmb << "'" << endl;
        return false;
    }
    
    // Get the various file sizes.
    dataSize = getLong();
    indexSize = getWord();
    freeSpaceSize = getWord();
    structureSize = getWord();
    
    
    
    // Everything seems ok. Display the header info to cerr.
    cerr << endl <<"HEADER DETAILS" << endl;
    cerr << "==============" << endl << endl;
    cerr << "At Offset: ";
    displayHex(0, 8, cerr);
    cerr << endl << endl;
    cerr << "Header Size     : " ; displayDecHex(headerSize, 4); cerr << endl;
    cerr << "Flag            : " << vrmb << endl;
    cerr << "Data size       : " ; displayDecHex(dataSize, 8); cerr << endl;
    cerr << "Index size      : " ; displayDecHex(indexSize, 4); cerr << endl;
    cerr << "Free space size : " ; displayDecHex(freeSpaceSize, 4); cerr << endl;
    cerr << "Structure size  : " ; displayDecHex(structureSize, 4); cerr << endl << endl;
    return true;
}

