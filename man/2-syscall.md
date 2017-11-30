This is a list of linux system call
<!--more-->

syscal                 | description
---------------------- | --------------------------------------------
accept                 | accept connection on socket
accept4                | accept connection on socket
access                 | check real user's permissions for file
acct                   | switch process accounting on/off
add_key                | add key to kernel's key management facility
adjtimex               | tune kernel clock
afs_syscall            | unimplemented system calls
alarm                  | set alarm clock for delivery of signal
alloc_hugepages        | allocate/free huge pages
arch_prctl             | set architecture-specific thread state
arm_fadvise            | predeclare access pattern for file data
arm_fadvise64_64       | predeclare access pattern for file data
arm_sync_file_range    | sync file segment with disk
atkexit                | Terminate LAM process
bdflush                | start, flush, or tune buffer-dirty-flush daemon
bind                   | bind name to socket
break                  | unimplemented system calls
brk                    | change data segment size
cacheflush             | flush contents of instruction/data cache
capget                 | set/get capabilities of thread
capset                 | set/get capabilities of thread
chdir                  | change working directory
chmod                  | change permissions of file
chown                  | change ownership of file
chown32                | change ownership of file
chroot                 | change root directory
clock_getres           | clock/time functions
clock_gettime          | clock/time functions
clock_nanosleep        | high-resolution sleep with specifiable clock
clock_settime          | clock/time functions
clone                  | create child process
__clone2               | create child process
clone2                 | create child process
close                  | close file descriptor
connect                | initiate connection on socket
creat                  | open/possibly create file/device
create_module          | create loadable module entry
dc_ctx_new             | distcache blocking client API
dc_plug_new            | basic DC_PLUG functions
dc_plug_read           | DC_PLUG read/write functions
dc_server_new          | distcache server API
delete_module          | unload kernel module
drecv                  | Send/receive LAM datalink messages
dsend                  | Send/receive LAM datalink messages
dup                    | duplicate file descriptor
dup2                   | duplicate file descriptor
dup3                   | duplicate file descriptor
epoll_create           | open epoll file descriptor
epoll_create1          | open epoll file descriptor
epoll_ctl              | control interface for epoll descriptor
epoll_pwait            | wait for I/O event on epoll file descriptor
epoll_wait             | wait for I/O event on epoll file descriptor
eventfd                | create file descriptor for event notification
eventfd2               | create file descriptor for event notification
execve                 | execute program
_exit                  | terminate calling process
exit                   | terminate calling process
exit_group             | exit all threads in process
faccessat              | check user's permissions of file relative to directory file descriptor
fadvise                | Give advice about file access
fadvise64              | predeclare access pattern for file data
fadvise64_64           | predeclare access pattern for file data
fallocate              | change file space
fattach                | unimplemented system calls
fattch                 | unimplemented system calls
fchdir                 | change working directory
fchmod                 | change permissions of file
fchmodat               | change permissions of file relative to directory file descriptor
fchown                 | change ownership of file
fchown32               | change ownership of file
fchownat               | change ownership of file relative to directory file descriptor
fcntl                  | change file descriptor
fcntl64                | change file descriptor
fdatasync              | synchronize file's in-core state with storage device
fdetach                | unimplemented system calls
fgetxattr              | extended attrib value
finit_module           | load kernel module
flistxattr             | extended attrib names
flock                  | apply/remove advisory lock on open file
fork                   | create child process
free_hugepages         | allocate/free huge pages
fremovexattr           | remove extended attrib
fsetxattr              | set extended attrib value
fstat                  | file status
fstat64                | file status
fstatat                | file status relative to directory file descriptor
fstatat64              | file status relative to directory file descriptor
fstatfs                | file system statistics
fstatfs64              | file system statistics
fstatvfs               | file system statistics
fsync                  | synchronize file's in-core state with storage device
ftruncate              | truncate file to specified length
ftruncate64            | truncate file to specified length
futex                  | fast user-space locking
futimesat              | change timestamps of file relative to directory file descriptor
gall                   | array of LAM node identifiers
gcomps                 | array of LAM node identifiers
get_kernel_syms        | exported kernel/module symbols
get_mempolicy          | NUMA memory policy for process
get_robust_list        | get/set list of robust futexes
get_thread_area        | thread-local storage area
getall                 | array of LAM node identifiers
getcomps               | array of LAM node identifiers
getcontext             | get/set user context
getcpu                 | determine CPU/NUMA node on which calling thread is running
getcwd                 | current working directory
getdents               | directory entries
getdents64             | directory entries
getdomainname          | get/set NIS domain name
getdtablesize          | descriptor table size
getegid                | group identity
getegid32              | group identity
geteuid                | user identity
geteuid32              | user identity
getgid                 | group identity
getgid32               | group identity
getgroups              | get/set list of supplementary group IDs
getgroups32            | get/set list of supplementary group IDs
gethostid              | get/set unique identifier of current host
gethostname            | get/set hostname
getitimer              | get/set value of interval timer
getjones               | array of LAM node identifiers
getmsg                 | unimplemented system calls
getnall                | info on LAM nodes
getnjones              | info on LAM nodes
getnodeid              | info on LAM nodes
getnodes               | array of LAM node identifiers
getnodetype            | info on LAM nodes
getnotb                | info on LAM nodes
getntype               | info on LAM nodes
getorigin              | info on LAM nodes
getotbs                | array of LAM node identifiers
getpagesize            | memory page size
getpeername            | name of connected peer socket
getpgid                | set/get process group
getpgrp                | set/get process group
getpid                 | process identification
getpmsg                | unimplemented system calls
getppid                | process identification
getpriority            | get/set program scheduling priority
getrent                | LAM route info
getrentc               | LAM route info
getresgid              | real, effective and saved user/group IDs
getresgid32            | real, effective and saved user/group IDs
getresuid              | real, effective and saved user/group IDs
getresuid32            | real, effective and saved user/group IDs
getrlimit              | get/set resource limits
getroute               | LAM route info
getroute2              | LAM route info
getrtype               | LAM route info
getrusage              | resource usage
getsid                 | session ID
getsockname            | socket name
getsockopt             | get/set options on sockets
gettid                 | thread identification
gettimeofday           | get/set time
getuid                 | user identity
getuid32               | user identity
getunwind              | copy unwind data to caller's buffer
getxattr               | extended attrib value
gjones                 | array of LAM node identifiers
gnodes                 | array of LAM node identifiers
gotbs                  | array of LAM node identifiers
gtty                   | unimplemented system calls
idle                   | make process 0 idle
ignall                 | info on LAM nodes
igncmp                 | info on LAM nodes
igndid                 | info on LAM nodes
igndtp                 | info on LAM nodes
ignjon                 | info on LAM nodes
ignotb                 | info on LAM nodes
igntp                  | info on LAM nodes
igorgn                 | info on LAM nodes
igrtp                  | LAM route info
inb                    | port I/O
inb_p                  | port I/O
init_module            | load kernel module
inl                    | port I/O
inl_p                  | port I/O
inotify_add_watch      | add watch to initialized inotify instance
inotify_init           | initialize inotify instance
inotify_init1          | initialize inotify instance
inotify_rm_watch       | remove existing watch from inotify instance
insb                   | port I/O
insl                   | port I/O
insw                   | port I/O
intro                  | introduction to system calls
introc                 | introduction to LAM C programming functions
introf                 | introduction to LAM Fortran programming routines
inw                    | port I/O
inw_p                  | port I/O
io_cancel              | cancel outstanding asynchronous I/O operation
io_destroy             | destroy asynchronous I/O context
io_getevents           | read asynchronous I/O events from completion queue
io_setup               | create asynchronous I/O context
io_submit              | submit asynchronous I/O blocks for processing
ioctl                  | control device
ioctl_list             | ioctl calls in /i386 kernel
ioperm                 | set port input/output permissions
iopl                   | change I/O privilege level
ioprio_get             | get/set I/O scheduling class/priority
ioprio_set             | get/set I/O scheduling class/priority
ipc                    | System V IPC system calls
isastream              | unimplemented system calls
kattach                | Attach/detach process to/from local LAM daemon
kcmp                   | compare two processes to determine if they share kernel resource
kcreate                | Create LAM process from executable program
kdetach                | Attach/detach process to/from local LAM daemon
kdoom                  | Deliver signal to LAM process
kenter                 | Enter process into LAM session
kentr                  | Enter process into LAM session
kexec_load             | load new kernel for later execution
_kexit                 | Terminate LAM process
kexit                  | Terminate LAM process
keyctl                 | change kernel's key management facility
kill                   | send signal to process
killpg                 | send signal to process group
kinit                  | Initialize LAM process
krecv                  | Send/receive local node LAM messages
ksend                  | Send/receive local node LAM messages
ksr                    | Send/receive local node LAM messages
kstate                 | synchronization status of LAM process
kxit                   | Terminate LAM process
lam_kpause             | change LAM signal handling policy
lam_ksigblock          | change LAM signal handling policy
lam_ksignal            | Specify signal handler for LAM signal
lam_ksigretry          | change LAM signal handling policy
lam_ksigsetmask        | change LAM signal handling policy
lam_ksigsetretry       | change LAM signal handling policy
lam_rfclose            | LAM POSIX-like remote file service
lam_rfincr             | Control LAM specific file daemon services
lam_rflseek            | LAM POSIX-like remote file service
lam_rfopen             | LAM POSIX-like remote file service
lam_rfposix            | LAM POSIX-like remote file service
lam_rfread             | LAM POSIX-like remote file service
lam_rfrmfd             | Control LAM specific file daemon services
lam_rfstat             | LAM POSIX-like remote file service
lam_rfstate            | Report status of remote LAM file descriptors
lam_rfwrite            | LAM POSIX-like remote file service
lam_rtrfforget         | Unload LAM trace data
lam_rtrfget            | Unload LAM trace data
lam_rtrforget          | Unload LAM trace data
lam_rtrget             | Unload LAM trace data
lam_rtrstore           | Store LAM trace data
lam_rtrsweep           | Remove LAM trace data
lam_rtrudie            | Remove LAM trace data
lam_rtrwipe            | Remove LAM trace data
lamf_rfclose           | Open/close remote file
lamf_rfopen            | Open/close remote file
lamf_rfread            | Read/write data from/to remote file
lamf_rfwrite           | Read/write data from/to remote file
lchown                 | change ownership of file
lchown32               | change ownership of file
lgetxattr              | extended attrib value
link                   | make new name for file
linkat                 | create file link relative to directory file descriptors
listen                 | listen for connections on socket
listxattr              | extended attrib names
llistxattr             | extended attrib names
_llseek                | reposition read/write file offset
llseek                 | reposition read/write file offset
lock                   | unimplemented system calls
lookup_dcookie         | return directory entry's path
lpattach               | Attach, detach LAM process from remote process management
lpdetach               | Attach, detach LAM process from remote process management
lremovexattr           | remove extended attrib
lseek                  | reposition read/write file offset
lsetxattr              | set extended attrib value
lstat                  | file status
lstat64                | file status
madvise                | give advice about use of memory
madvise1               | unimplemented system calls
mbind                  | set memory policy for memory range
migrate_pages          | move all pages in process to another set of nodes
mincore                | determine whether pages are resident in memory
mkdir                  | create directory
mkdirat                | create directory relative to directory file descriptor
mknod                  | create special/ordinary file
mknodat                | create special/ordinary file relative to directory file descriptor
mlock                  | lock/unlock memory
mlockall               | lock/unlock memory
mmap                   | map/unmap files/devices into memory
mmap2                  | map files/devices into memory
modify_ldt             | get/set ldt
mount                  | mount file system
move_pages             | move individual pages of process to another node
mprotect               | set protection on region of memory
mpx                    | unimplemented system calls
mq_getsetattr          | get/set message queue attribs
mq_notify              | register for notification when message is available
mq_open                | open message queue
mq_timedreceive        | receive message from message queue
mq_timedsend           | send message to message queue
mq_unlink              | remove message queue
mremap                 | remap virtual memory address
msgctl                 | message control operations
msgget                 | message queue identifier
msgop                  | message operations
msgrcv                 | message operations
msgsnd                 | message operations
msync                  | synchronize file with memory map
multiplexer            | unimplemented system calls
munlock                | lock/unlock memory
munlockall             | lock/unlock memory
munmap                 | map/unmap files/devices into memory
nal_address_new        | libnal addressing functions
nal_buffer_new         | libnal buffer functions
nal_connection_new     | libnal connection functions
nal_decode_uint32      | libnal serialisation functions
nal_listener_new       | libnal listener functions
nal_selector_new       | libnal selector functions
nanosleep              | high-resolution sleep
_newselect             | synchronous I/O multiplexing
nfsservctl             | syscall interface to kernel nfs daemon
nice                   | change process priority
nprob                  | Report if LAM bufferd network message is ready to be received
nprobe                 | Report if LAM bufferd network message is ready to be received
nrcv                   | Send/receive LAM network messages
nrecv                  | Send/receive LAM network messages
nsend                  | Send/receive LAM network messages
nsnd                   | Send/receive LAM network messages
obsolete               | obsolete system calls
oldfstat               | file status
oldlstat               | file status
oldolduname            | name/info about current kernel
oldstat                | file status
olduname               | name/info about current kernel
open                   | open/possibly create file/device
openat                 | open file relative to directory file descriptor
outb                   | port I/O
outb_p                 | port I/O
outl                   | port I/O
outl_p                 | port I/O
outsb                  | port I/O
outsl                  | port I/O
outsw                  | port I/O
outw                   | port I/O
outw_p                 | port I/O
path_resolution        | find file referred to by filename
pause                  | wait for signal
pciconfig_iobase       | pci device info handling
pciconfig_read         | pci device info handling
pciconfig_write        | pci device info handling
perf_event_open        | set up performance monitoring
perfmonctl             | interface to IA-64 performance monitoring unit
personality            | set process execution domain
phys                   | unimplemented system calls
pipe                   | create pipe
pipe2                  | create pipe
pivot_root             | change root file system
poll                   | wait for some event on file descriptor
posix_fadvise          | predeclare access pattern for file data
ppoll                  | wait for some event on file descriptor
prctl                  | operations on process
prcv                   | LAM physical layer message passing
prcvc                  | LAM physical layer message passing
prcvo                  | LAM physical layer message passing
pread                  | read from/write to file descriptor at given offset
pread64                | read from/write to file descriptor at given offset
preadv                 | read/write data into multiple buffers
precv                  | LAM physical layer message passing
prlimit                | get/set resource limits
process_vm_readv       | transfer data between process address spaces
process_vm_writev      | transfer data between process address spaces
prof                   | unimplemented system calls
pselect                | synchronous I/O multiplexing
pselect6               | synchronous I/O multiplexing
psend                  | LAM physical layer message passing
psnd                   | LAM physical layer message passing
psndc                  | LAM physical layer message passing
psndo                  | LAM physical layer message passing
ptrace                 | process trace
putmsg                 | unimplemented system calls
putpmsg                | unimplemented system calls
pwrite                 | read from/write to file descriptor at given offset
pwrite64               | read from/write to file descriptor at given offset
pwritev                | read/write data into multiple buffers
query_module           | query kernel for various bits pertaining to modules
quotactl               | change disk quotas
rbflook                | copy of buffered LAM message packet
rbfparms               | Control LAM remote buffers
rbfstate               | Report status of remote LAM buffers
rbfsweep               | Control LAM remote buffers
rbfudie                | Control LAM remote buffers
rbfwipe                | Control LAM remote buffers
read                   | read from file descriptor
readahead              | perform file readahead into page cache
readdir                | read directory entry
readlink               | read value of symbolic link
readlinkat             | read value of symbolic link relative to directory file descriptor
readv                  | read/write data into multiple buffers
reboot                 | reboot/enable/disable Ctrl-Alt-Del
recho                  | Send messages to LAM echo server
recv                   | receive message from socket
recvfrom               | receive message from socket
recvmmsg               | receive multiple messages on socket
recvmsg                | receive message from socket
remap_file_pages       | create nonlinear file mapping
removexattr            | remove extended attrib
rename                 | change name/location of file
renameat               | rename file relative to directory file descriptors
request_key            | request key from kernel's key management facility
restart_syscall        | Restart system call
rflat                  | Tag/load storage on LAM remote nodes
rflclean               | Tag/load storage on LAM remote nodes
rforget                | Find tagged storage on LAM remote node
rget                   | Find tagged storage on LAM remote node
rload                  | Load file onto LAM remote node
rmdir                  | delete directory
rpcreate               | Create LAM process on remote node
rpdoom                 | Signal LAM processes on remote node
rpgo                   | Create LAM process on remote node from tagged storage
rpldgo                 | Load/execute LAM program on remote node
rploadgo               | Load/execute LAM program on remote node
rpspawn                | Spawn LAM process onto remote node
rpstate                | Report status of LAM processes on remote node
rpwait                 | Wait for child LAM process to terminate
rpwt                   | Wait for child LAM process to terminate
rrsetrents             | Set LAM route info
rt_sigaction           | examine/change signal action
rt_sigpending          | examine pending signals
rt_sigprocmask         | examine/change blocked signals
rt_sigqueueinfo        | queue signal/data
rt_sigreturn           | return from signal handler/cleanup stack frame
rt_sigsuspend          | wait for signal
rt_sigtimedwait        | synchronously wait for queued signals
rt_tgsigqueueinfo      | queue signal/data
rtas                   | Allows userspace to call RTAS
s390_runtime_instr     | enable/disable s390 CPU run-time instrumentation
sbrk                   | change data segment size
sched_get_priority_max | static priority range
sched_get_priority_min | static priority range
sched_getaffinity      | set/get process's CPU affinity mask
sched_getparam         | set/get scheduling parameters
sched_getscheduler     | set/get scheduling policy/parameters
sched_rr_get_interval  | SCHED_RR interval for named process
sched_setaffinity      | set/get process's CPU affinity mask
sched_setparam         | set/get scheduling parameters
sched_setscheduler     | set/get scheduling policy/parameters
sched_yield            | yield processor
security               | unimplemented system calls
select                 | synchronous I/O multiplexing
select_tut             | synchronous I/O multiplexing
semctl                 | semaphore control operations
semget                 | semaphore set identifier
semop                  | semaphore operations
semtimedop             | semaphore operations
send                   | send message on socket
sendfile               | transfer data between file descriptors
sendfile64             | transfer data between file descriptors
sendmmsg               | send multiple messages on socket
sendmsg                | send message on socket
sendto                 | send message on socket
set_mempolicy          | set default NUMA memory policy for process/its children
set_robust_list        | get/set list of robust futexes
set_thread_area        | set thread local storage area
set_tid_address        | set pointer to thread ID
setcontext             | get/set user context
setdomainname          | get/set NIS domain name
setegid                | set effective user/group ID
seteuid                | set effective user/group ID
setfsgid               | set group identity used for file system checks
setfsgid32             | set group identity used for file system checks
setfsuid               | set user identity used for file system checks
setfsuid32             | set user identity used for file system checks
setgid                 | set group identity
setgid32               | set group identity
setgroups              | get/set list of supplementary group IDs
setgroups32            | get/set list of supplementary group IDs
sethostid              | get/set unique identifier of current host
sethostname            | get/set hostname
setitimer              | get/set value of interval timer
setns                  | reassociate thread with namespace
setpgid                | set/get process group
setpgrp                | set/get process group
setpriority            | get/set program scheduling priority
setregid               | set real/effective user/group ID
setregid32             | set real/effective user/group ID
setresgid              | set real, effective and saved user or group ID
setresgid32            | set real, effective and saved user or group ID
setresuid              | set real, effective and saved user or group ID
setresuid32            | set real, effective and saved user or group ID
setreuid               | set real/effective user/group ID
setreuid32             | set real/effective user/group ID
setrlimit              | get/set resource limits
setsid                 | creates session/sets process group ID
setsockopt             | get/set options on sockets
settimeofday           | get/set time
setuid                 | set user identity
setuid32               | set user identity
setup                  | setup devices and file systems, mount root file system
setxattr               | set extended attrib value
sgetmask               | change of signal mask
shmat                  | shared memory operations
shmctl                 | shared memory control
shmdt                  | shared memory operations
shmget                 | allocates shared memory segment
shmop                  | shared memory operations
shutdown               | shut down part of full-duplex connection
sigaction              | examine/change signal action
sigaltstack            | set/get signal stack context
sigblock               | change signal mask
siggetmask             | change signal mask
sigmask                | change signal mask
signal                 | ANSI C signal handling
signalfd               | create file descriptor for accepting signals
signalfd4              | create file descriptor for accepting signals
sigpause               | atomically release blocked signals/wait for interrupt
sigpending             | examine pending signals
sigprocmask            | examine/change blocked signals
sigqueue               | queue signal/data to process
sigreturn              | return from signal handler/cleanup stack frame
sigsetmask             | change signal mask
sigsuspend             | wait for signal
sigtimedwait           | synchronously wait for queued signals
sigvec                 | BSD software signal facilities
sigwaitinfo            | synchronously wait for queued signals
socket                 | create endpoint for communication
socketcall             | socket system calls
socketpair             | create pair of connected sockets
splice                 | splice data to/from pipe
spu_create             | create new spu context
spu_run                | execute SPU context
spufs                  | SPU file system
ssetmask               | change of signal mask
sstk                   | change stack size
stat                   | file status
stat64                 | file status
statfs                 | file system statistics
statfs64               | file system statistics
statvfs                | file system statistics
stime                  | set time
stty                   | unimplemented system calls
subpage_prot           | define subpage protection for address range
swapcontext            | Swap out old context with new context
swapoff                | start/stop swapping to file/device
swapon                 | start/stop swapping to file/device
symlink                | make new name for file
symlinkat              | create symbolic link relative to directory file descriptor
sync                   | commit buffer cache to disk
sync_file_range        | sync file segment with disk
sync_file_range2       | sync file segment with disk
syncfs                 | commit buffer cache to disk
_syscall               | invoking system call without library support
syscall                | indirect system call
syscalls               | system calls
_sysctl                | read/write system parameters
sysctl                 | read/write system parameters
sysfs                  | file system type info
sysinfo                | returns info on overall system statistics
syslog                 | read/clear kernel message ring buffer; set console_loglevel
tee                    | duplicating pipe content
tgkill                 | send signal to thread
time                   | time in seconds
timer_create           | create POSIX per-process timer
timer_delete           | delete POSIX per-process timer
timer_getoverrun       | overrun count for POSIX per-process timer
timer_gettime          | arm/disarm/fetch state of POSIX per-process timer
timer_settime          | arm/disarm/fetch state of POSIX per-process timer
timerfd_create         | timers that notify via file descriptors
timerfd_gettime        | timers that notify via file descriptors
timerfd_settime        | timers that notify via file descriptors
times                  | process times
tkill                  | send signal to thread
trcv                   | Send/receive LAM transport messages
trecv                  | Send/receive LAM transport messages
trror                  | Print LAM system error message
truncate               | truncate file to specified length
truncate64             | truncate file to specified length
tsend                  | Send/receive LAM transport messages
tsnd                   | Send/receive LAM transport messages
tux                    | interact with TUX kernel subsystem
tuxcall                | unimplemented system calls
ugetrlimit             | get/set resource limits
umask                  | set file mode creation mask
umount                 | unmount file system
umount2                | unmount file system
uname                  | name/info about current kernel
undocumented           | undocumented system calls
unimplemented          | unimplemented system calls
unlink                 | delete name/possibly file it refers to
unlinkat               | remove directory entry relative to directory file descriptor
unshare                | disassociate parts of process execution context
uselib                 | load shared library
ustat                  | file system statistics
utime                  | change file last access/modification times
utimensat              | change file timestamps with nanosecond precision
utimes                 | change file last access/modification times
vfork                  | create child process/block parent
vhangup                | virtually hangup current terminal
vm86                   | enter virtual 8086 mode
vm86old                | enter virtual 8086 mode
vmsplice               | splice user pages into pipe
vserver                | unimplemented system calls
wait                   | wait for process to change state
wait3                  | wait for process to change state, BSD style
wait4                  | wait for process to change state, BSD style
waitid                 | wait for process to change state
waitpid                | wait for process to change state
write                  | to file descriptor
writev                 | read/write data into multiple buffers
