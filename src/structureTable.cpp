#include "ArchiveExtractor.h"

bool doStructure() {
    // Decode the structure table which is located at 
    // 20 + dataSize + indexSize + FreeSpaceSize in the file.
    //
    // The structure is :
    //
    //  8 byte header. Not used.
    // 13 bytes fieldName. 
    //  1 byte size of fieldName.
    //  1 byte type: 0 = numeric, 1 = string.
    //  1 byte sort indicator: 0 = unsorted, 1 = sorted.
    //  1 byte sort order: 0 = ascending, 1 = descending.
    //  3 bytes unknown.
    
    // Find the structure table.
    dbf->seekg(20 + dataSize + indexSize + freeSpaceSize, dbf->beg);
    
    cerr << endl << "STRUCTURE TABLE";
    cerr << endl << "===============" << endl << endl;
    
    cerr << "The structure table begins at offset:" << endl;
    cerr << "    20 + Data Size + Index Size + Free Space Size." << endl << endl;
    
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

    // Display the table header - which is unused.
    displayTableHeader(&th);
    cerr << "The above table header is unused." << endl << endl;
        
    // How many fields?
    numFields = ((structureSize - 8) / 20) - 2;
    cerr << "NUM FIELDS: " << numFields << endl;
    
    // Extract the fields.
    numNumericFields = numStringFields = numSortedFields = 0;
    numericFields.clear();
    stringFields.clear();
    fieldNames.clear();

    fieldStructure fs;   
    
    for (int x = 0; x < numFields; x++) {
        dbf->get(fs.fieldName, 13+1);   // Gets 13 bytes.
        fs.fieldName[13] = '\0';       // Just in case!
        dbf->get(fs.nameSize);   
        dbf->get(fs.fieldType);
        dbf->get(fs.fieldSorted);
        dbf->get(fs.sortOrder);
        dbf->get(fs.fieldUnknown, 3+1);    // Gets 3 bytes.
        
        // Sanity checks.
        if (fs.nameSize > 13 || fs.nameSize < 0) {
            cerr << endl << "Oops. It seems the structureTable is corrupt." << endl;
            cerr << "NameSize is outside the valid range 1-13. Detected " << fs.nameSize << endl;
            cerr << "Unable to proceed. Sorry." << endl << endl;
            return false;
        }
        
        if (fs.fieldType != 1 && fs.fieldType != 0) {
            cerr << endl << "Oops. It seems the structureTable is corrupt." << endl;
            cerr << "FieldType is outside the valid range 0-1. Detected " << int(fs.fieldType) << endl;
            cerr << "Unable to proceed. Sorry." << endl << endl;
            return false;
        }
        
        if (fs.fieldSorted != 1 && fs.fieldSorted != 0) {
            cerr << endl << "Oops. It seems the structureTable is corrupt." << endl;
            cerr << "FieldSorted is outside the valid range 0-1. Detected " << int(fs.fieldSorted) << endl;
            cerr << "Unable to proceed. Sorry." << endl << endl;
            return false;
        }
            
        if (fs.sortOrder != 1 && fs.sortOrder != 0){
            cerr << endl << "Oops. It seems the structureTable is corrupt." << endl;
            cerr << "SortOrder is outside the valid range 0-1. Detected " << int(fs.sortOrder) << endl;
            cerr << "Unable to proceed. Sorry." << endl << endl;
            return false;
        }
        
        // Terminate the fieldName properly now.
        fs.fieldName[fs.nameSize] = '\0';
        
        // Print out the field details.
        displayField(x, &fs);
        
        // Update the lists and totals.
        fieldNames.push_back(string(fs.fieldName));
        if (fs.fieldType) {
            // String. Keep a list of all the
            // string field numbers.
            numStringFields++;
            stringFields.push_back(x);
        } else {
            // Numeric. Keep a list of all the
            // numeric field numbers.
            numNumericFields++;
            numericFields.push_back(x);
        }
        
        if (fs.fieldSorted)
            numSortedFields++;
        
    }
    
    return true;
}

