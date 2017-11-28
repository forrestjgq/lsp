# error
function   | description
--------   | --------------
errno      | error occurred in sys call
perror     | print errno in string with given header
strerror   | transfer specified error number to string
strerror_r | transfer specified error number into a given string buff


# File

function  | description
--------  | --------------
open      | open or create file
creat     | create a file
read      | read data from file
write     | write data to file
pread     | read from specified position of file
pwrite    | write data to specified position of file
lseek     | seek to given position of file
close     | close file
ftruncate | truncate file to a smaller or larger file
truncate  | truncate file as a path to a smaller or larger file
select    | monitor multiple fd until IO is ready or timeout
pselect   | select multiple fds and waiting signals
poll      | poll event based on fds, better usage than select()
ppoll     | poll and wait signals

# IO
function   | description
--------   | --------------
fopen | open file(r, r+, w, w+, a, a+)
fclose | close file
fcloseall | close all files
fgetc | get one single char from stream
ungetc | push char back to stream
fgets | read string from stream(\n stored if reach)

function   | description
--------   | --------------
function   | description
--------   | --------------
function   | description
--------   | --------------
function   | description
--------   | --------------
function   | description
--------   | --------------
