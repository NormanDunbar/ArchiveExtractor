ArchiveExtractor
================

Examines the internals of an QDOS Archive database file.

I wrote this utility to assist me in tracking down just how corrupted,
an archive database file in my possession, actually was. As it turned
out, very!

Installing
==========

The utility is currently Windows only! There's a surprise eh? And is written in C++, so you;ll need a decent compiler to generate the code. I used the free Borland C++ compiler, version 10.1 as provided absolutely free of charge by Embarcadero who took over Borland some years ago.

Download the Compiler
---------------------

Go to https://www.embarcadero.com/free-tools/ccompiler/start-for-free in your favourite browser, alternatively, use Internet Explorer! Fill in your details to open an account, or login if you already have one, and download the compiler.

Follow the instructions provided to install it, which is pretty much just unzipping it really. Add the ``bin`` folder to the path, and you are ready to go.

Compiling
---------

Download the source code from GitHub. There's a download button on the screen. Download the zip file is what you want to do.

Unzip it into your SourceCode folder, or somewhere suitable. It will be called ``ArchiveExtractor-master``. Open that folder in your File Explorer and locate the ``release`` folder and within that, the file ``build.cmd``.

Double-click on ``build.cmd`` and assuming the compiler is on your path, the utility will be built.


Running ArchiveExtractor
========================

Once compiled, you execute the utility as follows::

	ArchiveExtractor dbf_file >data_file 2>structure_file

The data part of the database, passed as the first parameter, will be extracted and written to the data_file supplied as the second. The third parameter specifies the name of a structure_file where details of the internal structure will be written.

Where possible the utility makes checks to determine if the database is corrupted or not, if it finds anything dubious, the details will be written to the structure_file passed as a parameter.
 

Data File Format
================

Much has been written about the internals of a database file, but much
has been incorrect it seems!

The file is in 6 separate sections, as follows:

+------------------+
| File Header      |
+------------------+
| Data Area        |
+------------------+
| Index Table      |
+------------------+
| Free Space Table |
+------------------+
| Gap              |
+------------------+
| Structure Table  |
+------------------+


File Header
-----------

The file header is 20 bytes in size and holds a number of values which
correspond to the sizes of various internal tables.

+--------+--------+--------------------------------------+
| Offset | Length | Usage                                |
+========+========+======================================+
|      0 |     2  | Control record length ($14).         |
+--------+--------+--------------------------------------+
|      2 |     8  | File Id: 'vrm1dbf0' for QL or OPD.   |
+--------+--------+--------------------------------------+
|     10 |     4  | Size of the data area + control area |
+--------+--------+--------------------------------------+
|     14 |     2  | Size of the Index table              |
+--------+--------+--------------------------------------+
|     16 |     2  | Size of the Free Space table         |
+--------+--------+--------------------------------------+
|     18 |     2  | Size of the structure table          |
+--------+--------+--------------------------------------+

*In theory*, the data area should start at the end of the header, it does.
There is always the first byte of data at offset 20 into the file.

The size of the data area includes the size of the header, and together
this makes up the starting address of the index table.

The size of the index table, when added to the start of itself, gives
the start of the Free Space Table. 

However, when the size of the Free Space Table is added to the start
address of itself, we *do not* get the start of the Structure Table, we
get an address that is exactly 20 bytes short! This is what I call *The
Gap*. To get at the start of the Structure Table, we need to add an
extra 20 bytes.

We shall use the following as an example - it is taken from a real
(uncorrupted) database file. All sizes are in decimal.

+--------+--------+--------------------------------------+
| Offset | Length | Usage                                |
+========+========+======================================+
|    10  | 1914   | Size of the data area + control area |
+--------+--------+--------------------------------------+
|    14  |  470   | Size of the Index table              |
+--------+--------+--------------------------------------+
|    16  |   62   | Size of the Free Space table         |
+--------+--------+--------------------------------------+
|    18  |  188   | Size of the structure table          |
+--------+--------+--------------------------------------+

So, we *expect* to see the following start addresses for the various
tables:

+------------------+-------------+--------+
| AREA             |Calculation  | Offset |
+==================+=============+========+
| Header           |  0          | 0      |
+------------------+-------------+--------+
| Data Area        |  0 + 20     | 20     |
+------------------+-------------+--------+
| Index Table      |  0 + 1914   | 1914   |
+------------------+-------------+--------+
| Free Space Table |  1914 + 470 | 2384   |
+------------------+-------------+--------+
| Structure Table  |  2384 + 62  | 2466   |
+------------------+-------------+--------+

What we really see is as follows:

+------------------+-------------+--------+
| AREA             |Calculation  | Offset |
+==================+=============+========+
| Header           |  0          | 0      |
+------------------+-------------+--------+
| Data Area        |  0 + 20     | 20     |
+------------------+-------------+--------+
| Index Table      |  0 + 1914   | 1914   |
+------------------+-------------+--------+
| Free Space Table |  1914 + 470 | 2384   |
+------------------+-------------+--------+
| THE GAP          |  2384 + 62  | 2466** |
+------------------+-------------+--------+
| Structure Table  |  2466 + 20  | 2486   |
+------------------+-------------+--------+

So, the currently accepted documents appear to be incorrect, and that spare 20 bytes caused me no end of grief I can tell you!


Data Area
---------

The data area starts at address 20, just after the header and extends
right up to the start of the Index Table. The size of the data area is the long word at offset 10 in the header, minus 20 bytes.

Within the data area we find all the fields of each record, grouped together. The organisation is not as you defined the fields though, all the numeric fields come first, 8 bytes for each one, then the string fields come next - which makes sense as these can all be different sizes.

Numbers are stored in 8 bytes, I have no idea which actual format this is, yet, it could be IEEE, it could be some variant of the QL's internal format, or it could be something else. I'm still working on this part of the file.

Strings are store as a single byte, limiting string data to a maximum of 255 bytes, followed by the bytes of the string itself. Empty fields are indicated by a single byte of zero. There are no apparent padding bytes to make sure that the strings are an even length.

After all the string fields there is a two byte, usually both CHR$(0), record separator.


Index Table
-----------

The third major section of a database file is the index table. This structure is preceded by an 8 byte standard table header - there's one in front of all the "table" areas, but not all are used. This one is.

The address of the start of the header for this table is given in offset 10 of the file header, the long word found there indicates the start address of the index table.

The index table is not used if there are no sorted fields in the database.

Each entry in the index table is 6 bytes plus 8 bytes for each and every sorted field.

Index Table Header
~~~~~~~~~~~~~~~~~~

The index table data is preceded by a standard table header. This is 8 bytes in size and has the following format:

+--------+--------------------+
| Offset | Purpose            |
+========+====================+
|  0     | Element size       |
+--------+--------------------+
|  2     | Granularity        |
+--------+--------------------+
|  4     | Elements in use    |
+--------+--------------------+
|  6     | Elements allocated |
+--------+--------------------+

Element Size
	This defines the size of each element in the following table. For an index table this is 6 + (8 \* sorted_field_count). 

Granularity
	The defines how many elements forth of space is allocated for the table when new entries are required. The number of bytes allocated is element_size \* granularity).

Elements in Use
	How many of the elements in the table are in use. The first entry in any table appears to be a dummy, unused entry. It appears that archive likes to start counting from 1 rather than zero.

Elements Allocated
	How big the table is, in elements.

The other tables in the database file have a similar header.

Index Table Entries
~~~~~~~~~~~~~~~~~~~

The index table entries start immediately after the header.

Each entry has a 6 byte record locator. This comes in two parts, the first 4 bytes are the offset into the *database file*, and not from the start of the *data area*, the next two bytes *appear* to give the length, in bytes, of the entire record - except, it appears that it doesn't quite work out that way in some cases!

Following the record locator is 8 bytes of data for each sorted field in the record. For numeric fields the data here doesn't appear to be the same 8 bytes stored in the data area. I have yet to determine what goes here for string fields.

When the ArchiveExtractor walks the data area of an indexed database file it uses the valid index entries to extract only the valid records. When the database isn't sorted, it has to check each record isn't sitting in a free space area, which indicates that the record isn't valid any more and *might* be a previously deleted record.

Free Space Table
----------------

The free space table starts at the offset given by adding the size of the Index Table to the start of the Index Table address.

There is a standard table header in use by this table. Each element is 6 bytes long. The 6 bytes are made up of a 4 byte offset into the entire file, not just into the data area, where the free space area begins. The remaining two bytes are the size of the free space area, in bytes.


The Gap
-------
The gap starts at the offset given by adding the size of the free space table to the start address of the table itself.

The gap is simply 20 bytes that are never accessed. I suspect it's as a
result of a bug as none of the documents I've seen mention it and, it's
exactly 20 bytes long, which just happens to be the size of the file header.
Interesting?

Structure Table
---------------
The structure table begins at offset 20 bytes on from the start of the gap.

The structure table has a standard 8 byte header, but it doesn't appear to be used. The entries in the structure table after the unused header are 20 bytes, laid out as follows:

+----------+--------------------------------------------+
| 13 bytes | Field name. Space padded.                  |
+----------+--------------------------------------------+
| 1 byte   | Size of field name.                        |
+----------+--------------------------------------------+
| 1 byte   | Type: 0 = numeric, 1 = string.             |
+----------+--------------------------------------------+
| 1 byte   | Sort indicator: 0 = unsorted, 1 = sorted.  |
+----------+--------------------------------------------+
| 1 byte   | Sort order: 0 = ascending, 1 = descending. |
+----------+--------------------------------------------+
| 3 bytes  | Unknown. Always $00, $01, $02.             |
+----------+--------------------------------------------+

It's interesting that the size of the field name comes at the end of the bytes making up the field name!

As the standard header before this table is not used, then calculating the size of the table appears to be::

    ((StructureTableSize - 8) / 20) -2
    
However, it also appears that the final two entries in the table are not used. Hence the final subtraction above.

Reading an Archive File
=======================

Reading a file begins by checking that the header is a word of 20, followed by the appropriate flag bytes. If the 'v' at offset 3 is a CHR$(0) then this file was opened in Archive but never closed. It's possible to revert this by opening the file and writing a 'v' to byte 3. When one of these files is deteced, the utility will print out a small SuperBASIC program to fix it for you.

If the file appears to be valid, the structure table is found and read next. This tells the organisation of the data area and whether the index table is in use or not.

The Free Space table has to be read next to determine where in the file there are chunks of free space, perhaps still holding potentially valid records which were deleted at some point in the past.

Finally, the data can be read from the start of the file. If the file is sorted, the the index table is read in order and the records found at the given offsets are printed out in order.

If the file is unsorted, the records are found in the data area, but when printed out are flagged with an indication that they may be deleted if the reside on one of the free space chuncks.




