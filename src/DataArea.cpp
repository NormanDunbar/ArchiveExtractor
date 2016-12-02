#include "ArchiveExtractor.h"

bool doData() {
    // Decode the data area which is located at offset 20 in the file.
    //
    // The structure is :
    //
    // 8 bytes for each of the numeric fields.
    // For each string field:
    //   1 byte string length.
    //   n bytes string contents.
    // 2 bytes of 'rubbish' at the end of each record.
    
    // Data dumps go to cout, not cerr!

    // Find the data area.
    dbf->seekg(20, dbf->beg);
    
    cout << endl << "FILE: " << fileName << endl;
    cout << endl << "DATA AREA";
    cout << endl << "=========" << endl << endl;
    cout << "At Offset: ";
    displayHex(dbf->tellg(), 8, cout);
    cout << endl;

    // Print out each record's data.
    int recordNumber = 0;
    
    // If we have an index then print those rows
    // otherwise scan the data area looking for records.
    
    if (numSortedFields) {
        cout << "Indexed Record Print" << endl;
        return doIndexedRecords();
    }
    
    // Otherwise, drop in below.
    while (true) {
        // Are we done yet?
        size_t currentOffset = dbf->tellg();
        
        if (currentOffset >= dataSize /* - 20 */)
            break;
        
        doOneRecord(currentOffset, recordNumber);
        
        // Next record.
        recordNumber++;
        
    }

    return true;
}

bool doIndexedRecords() {
    // Lists the records using the index. This should mean
    // that only valid, undeleted records are printed.
    int recordNumber = 0;
    for (list<uint32_t>::iterator x = indexOffsets.begin(); x != indexOffsets.end(); x++) {
        doOneRecord(*x, recordNumber);
        recordNumber++;
    }
    return true;
}


void doOneRecord(const uint32_t currentOffset, const int recordNumber) {
        // We account for up to 99999 records here.
        
        cout << endl << "RECORD: " ;
        cout << setw(5) << setfill('0') << recordNumber;
        cout << " - At Offset: ";
        displayHex(currentOffset, 8, cout);
        
        // Is this a possibly deleted record?
        for (list<freeSpaceElement>::iterator e=freeSpaces.begin(); e != freeSpaces.end(); e++) {
            freeSpaceElement fse = *e;
            if (currentOffset >= fse.offset && currentOffset <= fse.lastFreeByte) {
                // Yup, deleted. It lives in a free space chunk.
                cout << " (DELETED RECORD)";
                break;
            }
        }
        
        cout << endl;
        cout << "-------------" << setfill(' ') << endl;
        
        // Are there numeric fields?
        if (numNumericFields) {
            for (list<int>::iterator f = numericFields.begin(); f != numericFields.end(); f++) {
                cout << setw(15) << fieldNames[*f] << ": ";
                for (int x = 0; x < 8; x++) {
                    displayHex(getByte(), 2, cout);
                }
                cout << setfill(' ') << endl;
            }
        }
        
        // Are there string fields?
        if (numStringFields) {
            for (list<int>::iterator f = stringFields.begin(); f != stringFields.end(); f++) {
                cout << setw(15) << setfill(' ') << fieldNames[*f] << ": ";
                int dataSize = getByte();
                if (!dataSize) {
                    cout << endl;
                    continue;
                }
                
                for (int x = 0; x < dataSize; x++) {
                    char c = getByte();
                    if (c == '`')
                        c = '£';
                    
                    cout << c;
                }
                cout << endl;
            }           
        }

        // Rubbish?
        cout << setw(15) << setfill(' ') << "Rec Separator" << ":";
        for (int x = 0; x < 2; x++) {
            displayHex(int(getByte()), 2, cout);
        }
        cout << endl;
    
}