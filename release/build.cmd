:: Make sure that the Borland C++ compiler is on %PATH%
:: before running this file to build the HexDump utility.
::
:: Norman Dunbar April 2016


bcc32c -o ArchiveExtractor.exe ..\src\*.cpp  wildargs.obj

if exist ArchiveExtractor.pdb (
    del /f ArchiveExtractor.pdb 2>nul
)

if exist ArchiveExtractor.tds (
    del /f ArchiveExtractor.tds 2>nul
)

pause
