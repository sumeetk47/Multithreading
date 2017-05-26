CHANGES:
Previously, this code replaced all punctuation characters with space which eventually acted as separator. The changes were made wherein any other character except Alphanumeric was replaced. This could have been the reason of word counts being wrong. 
Secondly, finishlock was removed and finished variable was merged with queue so that they both could be locked together and acted as single critical section. This way race condition and missing the processing of text files left in the queue was avoided. 
Thirdly, to avoid polling, notify and wait method was used with the condition variable cv. All the consumer threads waited for the notification from the producer to avoid polling and thus improve the performance.
Further optimization was possible by having local maps to write data without frequently locking and unlocking the shared map. Once the whole data in the text file is processed, it could be written to shared map. This avoids unnecessary frequent locking and unlocking of mutex but at the cost of added memory consumption. The code to have local maps has been commented. Currently, the code uses shared map.

Following is the sample output with modified code and added functionality:
sumeet@ubuntu:/mnt/share$ ./ssfi -t 4 path
The top 10 words are:
the	320120
of	160100
and	153252
to	115064
in	88200
a	84612
that	50048
he	49604
was	45640
it	42724

This was taken by processing four big files within a folder with each file being around 6.5 Mb.

Scenarios handled :
This code handles different scenarios and checks if directory is present or not, displays lesser values if words present are less than 10, validate -t flag on the command line and make sure that user gives correct input.
Extensive testing was done in following cases:
- If single big file is present
- If single small file is present
- If no text file is present or text files are blank
- If directory does not exist
- If multiple text files are present in multiple folders in a directory
- If multiple big files or small files are present

Moreover, parallelization was also checked by adding a CPU core on Virtual Box Ubuntu.

Extensive character filtering check was done by trying this project with range of special characters. Following is a sample of the line which was tested :
'An preost wes on leoden, Laȝamon was ihoten
He wes Leovenaðes sone -- liðe him be Drihten
He wonede at Ernleȝe at æðelen are chirechen'

Code handled all the situations correctly and all the special characters were replaced with space.

--------------------------------------------------------------------------

# FileIndexer

This project implements super simple Fileindexer. It displays top ten words with highest counts. This is a multithreaded code which spaws a single master thread and multiple consumer threads. Master thread parses directories to search text files and passes the system path of the text files found to the consumer threads using a queue. While doing this, consumer threads pick the file paths from the queue and processes that file to find word counts. This word counts are stored in global map by having key as the word and value as the count. After all the threads are done, this map is firstly sorted in descending order on the basis of values and top 10 values and respective words are printed. Word matching is case insensitive. Makefile is included with project for smooth compilation. This code has been tested with Linux Ubuntu.

Usage:
This is the way executable file needs to be spawned for smooth working:
./ssfi -t 3 ./path

Where -t parameter represents number of consumer threads to be run and './path' is the directory path.

Help functionality is also provided. Just type in './ssfi -h' to get the help.

Test cases :
This code has been tested for different test scenarios and also handles different error conditions. It checks the existence of directory and if any text files are present or not. Moreover, it also checks for error conditions in passing arguments. It separates the words on the basis of punctuations. So, any punctuational characters are treated as separators.


