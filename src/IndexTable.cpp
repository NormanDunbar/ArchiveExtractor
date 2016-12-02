#include "ArchiveExtractor.h"

bool doIndexTable() {
    // Decode the index table which is located at 
    // dataSize in the file.
    //
    // The structure is :
    //
    //  8 byte header. Not used.
    //  
    // Each element is:
    // 6 bytes plus (8bytes * number of sorted fields )
    //
    // The format of an element is:
    // 4 bytes offset into file where record starts.
    // 2 bytes size of the record data.
    // 8 bytes per index field. Format unknown. Purpose unknown.
    //   Could be the first 8 characters of the sorted field, but
    //   it's not that for numeric fields as far as I can see.
    //
    // The first index element seems to be a dummy.
    
    // Find the index table.
    dbf->seekg(dataSize, dbf->beg);
    
    cerr << endl << "INDEX TABLE";
    cerr << endl << "===========" << endl << endl;
    
    cerr << "The index table begins at offset:" << endl;
    cerr << "    Data Size." << endl << endl;
    
    cerr << "At Offset: ";
    displayHex(dbf->tellg(), 8, cerr);
    cerr << endl;

    cerr << "Table Size: ";
    displayHex(structureSize, 4, cerr);
    cerr << endl << endl;

    // The 8 byte header.
    tableHeader th;
    
    th.elementSize = getWord();
    th.granularity = getWord();
    th.elementsInUse = getWord();
    th.elementsAllocated = getWord();

    // Display the table header.
    displayTableHeader(&th);
    
    // Worth continuing?
    if (!numSortedFields) {
        cerr << "No sorted fields - index table is random bytes." << endl;
        return true;
    }
        
    // We have 6 bytes per entry, and 8 bytes per sort field.
    // Sanity checks.
    // An index entry is 6 + (8 * numSortedFields).
    if (th.elementSize != 6 + (8 * numSortedFields)) {
        cerr << "The Index Table could be corrupted." << endl;
        cerr << "There are " << numSortedFields << " sorted fields in the database, so the" << endl;
        cerr << "element size should be " << 6 + (8 * numSortedFields) << " and " << th.elementSize;
        cerr << " was detected." << endl << endl;
        cerr << "It is unwise to attempt to print out the index table under these conditions." << endl;
        
        // However, attempt to continue.
        return true;
    }
    
    // Try to print out the index entries.
    indexOffsets.clear();
    for (int x = 0; x < th.elementsAllocated; x++ ) {
        cerr << "Index Entry: " << setw(5) << setfill('0') << x;
        cerr << setw(0) << setfill(' ') << ": ";
        
        uint32_t offset = getLong();
        uint16_t size = getWord();
        
        // Dump the offset.
        cerr << "OFFSET: ";
        displayHex(offset, 8, cerr);
        cerr << " SIZE: ";
        displayHex(size, 4, cerr);
        cerr << " DATA: ";
        
        int limit = (th.elementSize - 6) / 8;
        
        // Dump the remaining index entry bytes.
        for (int y = 0; y < limit ; y++) {
            displayHex(getLong(), 8, cerr);
            cerr << " ";
            displayHex(getLong(), 8, cerr);
            cerr << " ";
        }
        
        // Dummy? Used? Free? Corrupt?
        if (!x) 
            cerr << " (Dummy entry)";
        else {
            if (x <= th.elementsInUse) {
                // Corrupt?
                if (offset > dataSize) {
                    // Yup. Steaming pile of poo here!
                    cerr << " ** CORRUPTED **";
                } else {
                    // Save it for later.
                    indexOffsets.push_back(offset);
                }
            } else
                cerr << " (Unused entry)";
        }
        
        cerr << endl;
    }
    return true;
}

