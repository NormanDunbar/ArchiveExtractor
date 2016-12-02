#include "ArchiveExtractor.h"

bool doFreeSpace() {
    // Dump the free space table, in hex.
    // This starts at offset dataSize + indexSize.
    //
    // The format is:
    //
    // 8 byte header.
    // ??? bytes table entries.
    // Each entry appears to be 6 bytes in size. 
    // Each entry is as follows:
    // LONG offset to free area.
    // WORD size of free space?
    // The first entry appears unused though.
    
    // Set position.
    dbf->seekg(dataSize + indexSize, dbf->beg);

    cerr << endl << "FREE SPACE TABLE";
    cerr << endl << "================" << endl << endl;
        
    cerr << "The free space table begins at offset:" << endl;
    cerr << "    Data Size + Index Size." << endl << endl;
    
    cerr << "At Offset: ";
    displayHex(dbf->tellg(), 8, cerr);
    cerr << endl;
    cerr << "Table Size: ";
    displayHex(freeSpaceSize, 4, cerr);
    cerr << endl << endl;

    // The 8 byte header.
    tableHeader th;
    
    th.elementSize = getWord();
    th.granularity = getWord();
    th.elementsInUse = getWord();
    th.elementsAllocated = getWord();

    displayTableHeader(&th);
    
    // Attempt to catch potential corruption.
    bool freeTableCorrupt = false;
    if (th.elementSize != 6) {
        // Yup. Steaming pile of poo here!
        cerr << "CORRUPT! Free Space element size incorrect. ";
        cerr << "Expected size 6, found " << th.elementSize << "." << endl << endl;
        
        // Ok, attempt to continue...
        freeTableCorrupt = true;
    }

    freeSpaceElement fse;
    freeSpaces.clear();
    
    // The rest of the table. in th.elementSize byte (aka 6) chunks.
    cerr << "Space Table" << endl;
    cerr << "-----------" << endl;
    
    // If space table is corrupt, attempt to use the table size from the
    // dbf header instead as a limit.
    int limit = (freeTableCorrupt) ? (freeSpaceSize - 8) / 6 : th.elementsAllocated;
    for (int x = 0; x < limit; x++) {
        fse.offset = getLong();
        fse.size = getWord();
        fse.lastFreeByte = fse.offset + fse.size - 1;
        
        cerr << "Element: " << x << ", Offset: ";
        displayHex(fse.offset, 8, cerr);
        cerr << ", Size: ";
        displayHex(fse.size, 4, cerr);
        
        // Dummy? Used? Free? Corrupt?
        if (!x) 
            cerr << " (Dummy entry)";
        else {
            if (x <= th.elementsInUse) {
                // Corrupt?
                if (fse.offset > dataSize || freeTableCorrupt) {
                    // Yup. Steaming pile of poo here!
                    cerr << " ** CORRUPTED **";
                } else {
                    // Show the range of free bytes.
                    cerr << " From ";
                    displayHex(fse.offset, 8, cerr);
                    cerr << " to ";
                    displayHex(fse.lastFreeByte, 8, cerr);
                    
                    // Add to the list.
                    freeSpaces.push_back(fse);
                }
            } else
                cerr << " (Unused entry)";
        }
        
        cerr << endl;
    }
    cerr << endl;
    return true;
}


