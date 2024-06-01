## Build
use premake to build with conmmands:   
`premake5 --cc=clang gmake2`  
`premake5 --cc=clang vs2023`   
`premake5 vs2023`   


## Files
`randword.exe`   
 **Create these files**:  
`skipline.dat` - saves the line number reached.   
`words.txt` - list of sentences. Every sentence must be on separate line. Must end with a new line (I have not tested it without a new line at the end).      

The compleated program must contain 3 files. No more. No less. Three shalt be the number of files thou shalt have, and the number of the files shalt be three. Four files shalt thou not have, nor either have thou two files, excepting that thou then proceed to three files. Five files is right out. Once the number of files is three, being the third number, be reached, then thou canne usest thou program.

## Use
1) Start the program.
2) Press Ctrl+Alt+X to iterate the line in the `word.txt` (it must exist) file
3) Clse program to save curent line to `skipline.dat` (it must exist)