#include "ArchiveExtractor.h"

uint8_t getByte() {
    // Read an unsigned byte from the dbf file.
    char c;
    uint8_t result;
    
    dbf->get(c);    // Signed. 
    result = c;     // Unsigned. But -2 becomes 2.
    if (c < 0)
        result += 256;
    
    return result;
}


uint16_t getWord() {
    // Read a big endian word from a dbf file.
    uint16_t result = 0;

    result = getByte();
    result *= 256;
    result += getByte();
    
    return result;
}


uint32_t getLong() {
    // Read  big endian long word from a dbf file.
    uint32_t result = 0;
    
    result = getWord();
    result *= 65536;
    result += getWord();
    
    return result; 
}

void displayTableHeader(const tableHeader *th) {
    // Display the various table headers in the structure.
    
    cerr << "Table Header" << endl;
    cerr << "------------" << endl << endl;
    cerr << "Element Size  : " << th->elementSize << endl;
    cerr << "Granularity   : " << th->granularity << endl;
    cerr << "Used Elements : " << th->elementsInUse << endl;
    cerr << "Alloc Elements: " << th->elementsAllocated << endl << endl;;
}

void displayHex(const uint32_t value, const int wide, ostream &where) {
    // Display a hex number in 'wide' digits, with leading zeros.
    where << "$" << setbase(16) << setw(wide) << setfill('0') << value;
    where << setbase(10) << setfill(' ') << setw(0);
}

void displayDec(const uint32_t value, const int wide, const char filler, ostream &where) {
    // Display a decimal number in 'wide' digits, with 
    // leading spaces or zeros as chosen.
    where << setbase(10) << setw(wide) << setfill(filler) << value << setw(0) << setfill(' ');
    
}

void displayDecHex(const uint32_t value, const int wide) {
    // Output a value in decimal and the hex.
    // Decimal are space filled into 8 characters.
    // Hex are zero filled into 4 or 8 according to the 'wide' parameter.
    displayDec(value, 8, ' ', cerr);
    cerr << " (";
    displayHex(value, wide, cerr);
    cerr << ")";
}

void displayField(const int fieldNumber, const fieldStructure *fs, const uint32_t offset) {
        // Print out the field details.
        
        // The following caters for 99 fields maximum.
        // change setw() to 3 to cope with 999. And add an extra '-' to the underline.
        cerr << endl << "FIELD: " << setw(2) << setbase(10) << setfill(' ') << fieldNumber;
        cerr << " - At Offset: ";
        displayHex(offset, 8, cerr);
        cerr << endl << "---------";
        cerr << endl << "NAME      :" << fs->fieldName;
        cerr << endl << "TYPE      :" << int(fs->fieldType) << ((fs->fieldType == 0) ? "=Numeric" : "=String") ;
        cerr << endl << "SORTED    :" << int(fs->fieldSorted) << ((fs->fieldSorted == 0) ? "=Unsorted" : "=Sorted");
        cerr << endl << "SORT ORDER:" << int(fs->sortOrder);
        
        if (fs->fieldSorted){
            cerr << (fs->sortOrder == 0 ? "=Ascending" : "=Descending");
        } else {
            cerr << " (N/A)";
        }
        
        cerr << endl << "UNKNOWN   : ";
        for (int y = 0; y < 3; y++) {
            cerr << "$" << setw(2) << setfill('0') << setbase(16); 
            cerr << int(fs->fieldUnknown[y]) << " ";
        }
        cerr << endl;    
}


