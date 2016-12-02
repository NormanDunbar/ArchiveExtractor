# ArchiveExtractor
Examines the internals of an QDOS Archive database file.

I wrote this utility to assist me in tracking down just how corrupted, an archive database file in my possession, actually was. As it turned out, very!

# Data File Format
Much has been written about the internals of a database file, but much has been incorrect it seems!

The file is in 6 separate sections, as follows:

````
Header
Data Area
Index Table
Free Space Table
Gap
Structure Table
````

## Header
The header is 20 bytes in size and holds a number of values which correspond to the sizes of various internal tables.

````
 Offset  Length Usage
    0      2    Control record length ($14).
    2      8    File Id:
                     QL:  'vrm1dbf0'
                     OPD: 'vrm1dbf0'
                     DOS: ??????
   10      4    Size of the data area + control area
   14      2    Size of the Index table
   16      2    Size of the Free Space table
   18      2    Size of the structure table
````

In theory, the data area should start at the end of the header, it does. There is always the first byte of data at offset 20 into the file.

The size of the data area includes the size of the header, and together this makes up the starting address of the index table.

The size of the index table, when added to the start of itself, gives the start of the Free Space Table. And indeed, the files do match up.

However, when the size of the Free Space Table is added to the start address of itself, we *do not* get the start of the Structure Table, we get an address that is exactly 20 bytes short! This is what I call *The Gap*. To get at the start of the Structure Table, we need to add an extra 20 bytes.

We shall use the following as an example - it is taken from a real database file:

````
 Offset Value Usage
   10   1914  Size of the data area + control area
   14    470  Size of the Index table
   16     62  Size of the Free Space table
   18    188  Size of the structure table
````

So, we *expect* to see the following start addresses for the various tables:

````
Header           = 0          = 0
Data Area        = 0 + 20     = 20
Index Table      = 0 + 1914   = 1914
Free Space Table = 1914 + 470 = 2384
Structure Table  = 2384 + 62  = 2466
````

What we really see is as follows:

````
Header           = 0          = 0
Data Area        = 0 + 20     = 20
Index Table      = 0 + 1914   = 1914
Free Space Table = 1914 + 470 = 2384
THE GAP          = 2384 + 62  = 2466
Structure Table  = 2466 + 20  = 2486
````

## Data Area
The data area starts at address 20, just after the header and extends right up to the start of the Index Table, whihc is at the address given by the long word in the header, defining the size of the header and data area.


## Index Table

## Free Space Table

## Gap
The gap is simply 20 bytes that are never accessed. I suspect it's as a result of a bug as none of the documents I've seen mention it and, it's exactly 20 bytes long, which just happens to be the size of the header. Interesting?

## Structure Table
