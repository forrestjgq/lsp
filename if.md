# error
function   | description
--------   | --------------
errno      | error occurred in sys call
perror     | print errno in string with given header
strerror   | transfer specified error number to string
strerror_r | transfer specified error number into a given string buff

# IO
## POSIX File

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

## Standard C FILE IO
function      | description
--------      | --------------
fopen         | open file(r, r+, w, w+, a, a+)
fclose        | close file
fcloseall     | close all files
fgetc         | get one single char from stream
fputc         | put one single char to stream
ungetc        | push char back to stream
fgets         | read string from stream(\n stored if reach)
fputs         | write string(NULL terminated) into stream
fread         | read binary data blocks from stream
fwrite        | write bianry data blocks to stream
fseek         | set file stream position
ferror        | get file operation error
clearerr      | clear file error cause by any FILE operations
feof          | test if the file is in EOF
rewind        | = fseek(f, 0, SEEK_SET);
ftell         | get current position(offset) of file stream
fflush        | flush all writtern data to kernel
fileno        | get file descriptor from FILE stream
setvbuf       | set file stream buffer and type(_IONBF, _IOLBF, _IOFBF)
flockfile     | lock file to start atomic functions
funlockfile   | unlock file
ftrylockfile  | attempt to lock file
xxxx_unlocked | no-locking version of previous file interfaces


## Scatter and Gather IO
function | description
-------- | --------------
readv    | read into vector from file
writev   | write vector data to file
pread    | read from specified position of file into vector without change offset
pwrite   | write to specified position of file from vector without change offset
pread2   | normally do not care
pwrite2  | normally do not care

## Event poll
function      | description
--------      | --------------
poll          | poll event based on fds, better usage than select()
ppoll         | poll and wait signals
epoll_create1 | create an event poll file
epoll_ctl     | control event poll
epoll_wait    | wait event for files

function   | description
--------   | --------------
function   | description
--------   | --------------
function   | description
--------   | --------------
