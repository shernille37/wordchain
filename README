## Word Chain Aleatory


File Included: 

Data structure used:
HashMap

• main.c
Main function/entry for the entire project

• dict.c / .h
Header and C files to implement the functions of the HashMap

• file.c / .h
Header and C files to manage files (Read/Write)

• states.c
C file to implement the states (Finite State Machine) used to read the file in input

• utils.c / .h
Header and C files to implement the utilities function used for the entire project

• multiProcess.c / .h
Header and C files to implement the multi-process version of the project

• multiUtils.c
C file to implement the functions done by the 3 concurrent and inter-communicating processes


-- User Manual --
Input parameters are provided through options via the Command Line

Options:
-u      Mono-process version
-m      Multi-process version

-c      Compito 1
-r      Compito 2

Options (with required parameters):
-f <filename>                       Provide the filename for the input
-n <Number_of_Words>                Provide the number of words to be generated (Compito 2)
-p <Previous_Word> (Optional)       Provide for the previous word from which to start the generation of text (Compito 2)  

NAME(Executable file):
project

Pre-requisite:
Make sure to run the command "make" first to generate the executable file.

Output Files:

Compito 1:
<filename>.csv

Compito 2:
output.txt

-- Sample Uses --

Compito 1:

• Mono-process version: 
./project -u -c -f sample.txt                       Create the CSV file with "sample.txt"


• Multi-process version:
./project -m -c -f sample.txt                       Create the CSV file with "sample.txt" with 3 processes


Compito 2:

• Mono-process version:
./project -u -r -f sample.csv -n 100 -p hello       Generate text from "sample.csv" with 100 words and previous word hello


• Multi-process version:
./project -m -r -f sample.csv -n 100                Generate text from "sample.csv" with 100 words and pick a random separator to start with



