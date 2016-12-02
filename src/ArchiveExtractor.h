#ifndef __ARCHIVEEXTRACTOR_H__
#define __ARCHIVEEXTRACTOR_H__

#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <list>
#include <vector>

// In the main file, dont use extern.
#ifndef __ARCHIVEEXTRACTOR__
#define EXTERN extern
#else
#define EXTERN    
#endif

using std::string;
using std::getline;
using std::cin;
using std::cout;
using std::cerr;
using std::endl;
using std::setw;
using std::setfill;
using std::ifstream;
using std::setbase;
using std::list;
using std::string;
using std::vector;

//======================================================================
// GLOBALS (and yes, I know ....)
//======================================================================

//======================================================================
// Input stream for the database file, and the current file name.
//======================================================================
EXTERN ifstream *dbf;
EXTERN char *fileName;

//======================================================================
// Details from the header of the dbf file.
//======================================================================
EXTERN uint16_t headerSize;   
EXTERN char vrmb[9];  // Should be vrmb1dbf0. (QL and OPD only.) 
EXTERN uint32_t dataSize;
EXTERN uint16_t indexSize;
EXTERN uint16_t freeSpaceSize;
EXTERN uint16_t structureSize;

//======================================================================
// How many fields in the database structure?
//======================================================================
EXTERN int      numFields;
EXTERN int      numSortedFields;

//======================================================================
// Structure Table entry for a single field.
//======================================================================
typedef struct fieldStructure {
    char fieldName[14];    // Extra byte for terminator.
    char nameSize;
    char fieldType;
    char fieldSorted;
    char sortOrder;
    char fieldUnknown[3];   // No terminateor - not required.
} fieldStructure;

//======================================================================
// Structure of the various table headers.
//======================================================================
typedef struct tableHeader {
    uint16_t elementSize;
    uint16_t granularity;
    uint16_t elementsInUse;
    uint16_t elementsAllocated;
} tableHeader;

//======================================================================
// Structure Table element for the Free Space Table. The first two 
// fields match the actual free space table elements. The last one is
// added for ease of use.
//======================================================================
typedef struct freeSpaceElement {
    uint32_t offset;
    uint16_t size;
    uint32_t lastFreeByte;
} freeSpaceElement;

//======================================================================
// List of Free Space chunks in the data area.
//======================================================================
EXTERN list <freeSpaceElement> freeSpaces;

//======================================================================
// List of Numeric Fields detected.
// These all appear first in the data area.
//======================================================================
EXTERN int numNumericFields;
EXTERN list <int> numericFields;

//======================================================================
// List of string Fields detected.
// These appear after all the numeric fields.
//======================================================================
EXTERN int numStringFields;
EXTERN list <int> stringFields;

//======================================================================
// List of in use index offsets - in other words, where valid records
// can be fouond in the data.
//======================================================================
EXTERN list <uint32_t> indexOffsets;

//======================================================================
// Array of all fieldnames.
//======================================================================
EXTERN vector <string> fieldNames;



//======================================================================
// Function prototypes.
//======================================================================
uint8_t getByte();
uint16_t getWord();
uint32_t getLong();
void displayTableHeader(const tableHeader *th);
void displayHex(const uint32_t value, const int wide, ostream &where);
void displayDecHex(const uint32_t value, const int wide);
bool readHeader();
void displayField(const int fieldNumber, const fieldStructure *fs);
bool doIndexTable(); 
bool doStructure();
bool doData();
bool doIndexedRecords();
void doOneRecord(const uint32_t currentOffset, const int recordNumber);
bool doFreeSpace();
void doFile(const char *fname);

#endif