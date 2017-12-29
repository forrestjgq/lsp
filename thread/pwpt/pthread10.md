# 10 Future standardization
Three primary standardization efforts affect Pthreads programmers. X/Open's
XSH5 is a new interface specification that includes POSIX. lb, Pthreads, and a set
of additional thread functions (part of the Aspen fast-track submission). The
POSIX. lj draft standard proposes to add barriers, read/write locks, spinlocks,
and improved support for "relative time" waits on condition variables. The
POSIX. 14 draft standard (a "POSIX Standard Profile") gives direction for manag-
ing the various options of Pthreads in a multiprocessor environment.
## 10.1 X/Open XSH5 (UNIX98)
Mutex type attribute:
int pthread\_mutexattr\_gettype (
const pthread\_mutexattr\_t *attr, int *type);
int pthread\_mutexattr\_settype (
pthread\_mutexattr\_t *attr, int type);
Read/write locks:
int pthread\_rwlock\_init (pthread\_rwlock\_t *rwlock,
const pthread\_rwlockattr\_t *attr);
int pthread\_rwlock\_destroy (pthread\_rwlock\_t *rwlock);
pthread\_rwlock\_t rwlock = PTHREAD\_RWLOCK\_INITIALIZER;
int pthread\_rwlock\_rdlock (pthread\_rwlock\_t *rwlock);
int pthread\_rwlock\_tryrdlock (
pthread\_rwlock\_t *rwlock);
int pthread\_rwlock\_unlock (pthread\_rwlock\_t *rwlock);
int pthread\_rwlock\_wrlock (pthread\_rwlock\_t *rwlock);
int pthread\_rwlock\_trywrlock (
pthread\_rwlock\_t *rwlock);
int pthread\_rwlockattr\_init (
pthread\_rwlockattr\_t *attr);
int pthread\_rwlockattr\_destroy (
pthread\_rwlockattr\_t *attr);
int pthread\_rwlockattr\_getpshared (
const pthread\_rwlockattr\_t *attr, int *pshared);
int pthread\_rwlockattr\_setpshared (
pthread\_rwlockattr\_t *attr, int pshared);
347
348 CHAPTER 10 Future standardization
Parallel I/O:
size\_t pread (int fildes,
void *buf, size\_t nbyte, off\_t offset);
size\_t pwrite (int fildes,
const void *buf, size\_t nbyte, off\_t offset);
Miscellaneous:
int pthread\_attr\_getguardsize (
const pthread\_attr\_t *attr, size\_t *guardsize);
int pthread\_attr\_setguardsize (
pthread\_attr\_t *attr, size\_t guardsize);
int pthread\_getconcurrency ();
int pthread\_setconcurrency (int new\_level);
X/Open, which is part of The Open Group, owns the UNIX trademark and
develops UNIX industry portability specifications and brands. The X/Open brands
include XPG3, XPG4, UNIX93, and UNIX95. UNIX95 is also known as "SPEC 1170"
or the "Single UNIX Specification."
X/Open recently published the X/Open CAE Specification, System Interfaces
and Headers, Issue 5 (also known as XSH5), which is part of the new UNIX98
brand. XSH5 requires conformance to the POSIX. 1-1996 standard, which in-
cludes the POSIX. lb and POSIX. lc amendments. The XSH5 specification also
adds a set of extensions to POSIX. This section discusses the XSH5 extensions
that specifically affect threaded programs. You can recognize a system conform-
ing to XSH5 by a definition for the \_XOPEN\_VERSION symbol, in \<unistd. h\>, to the
value 500 or higher.
The most valuable contribution of UNIX98 to the threaded programming
industry, however, is possibly the development of a standardized, portable testing
system. A number of complicated issues arise when developing an implementa-
tion of Pthreads, and some subtle aspects of the standard are ambiguous. Such
an industry-wide testing system will require all vendors implementing UNIX98
branded systems to agree on interpretations of Pthreads.
### 10.1.1 POSIX options for XSH5
Some of the features that are options in the Pthreads standard are required
by XSH5. If your code relies on these Pthreads options, it will work on any sys-
tem conforming to XSH5:
? \_posix\_threads: Threads are supported.
? \_posix\_thread\_attr\_stackaddr: The stackaddr attribute is supported.
? \_POSIX\_THREAD\_ATTR\_STACKSIZE: The stacksize attribute is supported.
? \_posix\_thread\_process\_SHARED: Mutexes, condition variables, and XSH5
read/write locks can be shared between processes.
X/OpenXSH5 [UNIX98]
349
? \_posix\_thread\_safe\_functions: The Pthreads thread-safe functions are
supported.
Several additional Pthreads options are "bundled" into the XSH5 realtime
threads option group. If your system conforms to XSH5 and supports the \_xopen\_
realtime\_threads option, then these Pthreads options are also supported:
? \_posix\_thread\_priority\_scheduling: Realtime priority scheduling is
supported.
? \_posix\_thread\_prio\_protect: Priority ceiling mutexes are supported.
? \_posix\_thread\_prio\_inherit: Priority inheritance mutexes are supported.
### 10.1.2 Mutextype
The DCE threads package provided an extension that allowed the program-
mer to specify the "kind" of mutex to be created. DCE threads supplied fast,
recursive, and nonrecursive mutex kinds. The XSH5 specification changes the
attribute name from "kind" to "type," renames fast to default, renames nonrecur-
sive to errorcheck, and adds a new type, normal (Table 10.1).
A normal mutex is not allowed to detect deadlock errors-that is, a thread will
hang if it tries to lock a normal mutex that it already owns. The default mutex
type, like the DCE fast mutex,* provides implementation-defined error checking.
That is, default may be mapped to one of the other standard types or may be
something entirely different.
Mutex type
PTHREAD MUTEX NORMAL
PTHREAD\_MUTEX\_RECURSIVE
PTHREAD MUTEX ERRORCHECK
PTHREAD\_MUTEX\_DEFAULT
Definition
Basic mutex with no specific error checking built
in. Does not report a deadlock error.
Allows any thread to lock the mutex "recursively"
-it must unlock an equal number of times to
release the mutex.
Detects and reports simple usage errors-an
attempt to unlock a mutex that's not locked by
the calling thread (or that isn't locked at all), or an
attempt to relock a mutex the thread already
owns.
The default mutex type, with very loose semantics
to allow unfettered innovation and experimenta-
tion. May be mapped to any of the other three de-
fined types, or may be something else entirely.
TABLE 10.1 XSH5 mutex types
*DCE threads implemented fast mutexes much like the definition ofXSH5 normal mutexes,
with no error checking. This was not, however, specification of intent.
350 CHAPTER 10 Future standardization
As an application developer, you can use any of the mutex types almost inter-
changeably as long as your code does not depend on the implementation to detect
(or fail to detect) any particular errors. Never write code that counts on an imple-
mentation Jailing to detect any error. Do not lock a mutex in one thread and
unlock it in another thread, for example, even if you are sure that the error won't
be reported-use a semaphore instead, which has no "ownership" semantics.
All mutexes, regardless of type, are created using pthread\_mutex\_init, de-
stroyed using pthread\_mutex\_destroy, and manipulated using pthread\_mutex\_
lock, pthread\_mutex\_unlock, and pthread\_mutex\_trylock.
Normal mutexes will usually be the fastest implementation possible for the
machine, but will provide the least error checking.
Recursive mutexes are primarily useful for converting old code where it is dif-
ficult to establish clear boundaries of synchronization, for example, when you
must call a function with a mutex locked and the function you call-or some
function it calls-may need to lock the same mutex. I have never seen a situation
where recursive mutexes were required to solve a problem, but I have seen many
cases where the alternate (and usually "better") solutions were impractical. Such
situations frequently lead developers to create recursive mutexes, and it makes
more sense to have a single implementation available to everyone. (But your code
will usually be easier to follow, and perform better, if you avoid recursive
mutexes.)
Errorcheck mutexes were devised as a debugging tool, although less intrusive
debugging tools (where available) can be more powerful. To use errorcheck
mutexes you must recompile code to turn the debugging feature on and off. It is far
more useful to have an external option to force all mutexes to record debugging
data. You may want to use errorcheck mutexes in final "production" code, of
course, to detect serious problems early, but be aware that errorcheck mutexes
will almost always be much slower than normal mutexes due to the extra state and
checking.
Default mutexes allow each implementation to provide the mutex semantics
the vendor feels will be most useful to the target audience. It may be useful to
make errorcheck mutexes the default, for example, to improve the threaded
debugging environment of a system. Or the vendor may choose to make normal
mutexes the default to give most programs the benefit of any extra speed.
I pthread\_mutexattr\_gettype
int pthread\_mutexattr\_gettype (
const pthread\_mutexattr\_t *attr,
int *type);
Specify the type of mutexes created with attr.
X/OpenXSH5 [UNIX98]
351
PTHREAD
PTHREAD
PTHREAD
PTHREAD\_
MUTEX
MUTEX
MUTEX
MUTEX\_
DEFAULT
NORMAL
RECURSIVE
ERRORCHECK
type
Unspecified type.
Basic mutex, with no error
checking.
Thread can relock a mutex it
owns.
Checks for usage errors.
References:
Errors:
Hint:
3.2, 5.2.1, 10.1.2
[EINVAL] type invalid.
[EINVAL] attr invalid.
Normal mutexes will usually be fastest; errorcheck mutexes are use-
ful for debugging; recursive mutexes can be useful for making old
interfaces thread-safe.
I pthread\_mutexattr\_settype
int pthread\_mutexattr\_settype
pthread\_mutexattr\_t
int
*attr,
type);
Determine the type of mutexes created with attr.
type
PTHREAD MUTEX DEFAULT
Unspecified type.
PTHREAD MUTEX NORMAL
pthreadmutexrecursive
Basic mutex, with no error
checking.
Thread can relock a mutex it
owns.
pthread\_mutex\_errorcheck Checks for usage errors.
References: 3.2,5.2.1,10.1.2
Errors: [EINVAL] type invalid.
[EINVAL] attr invalid.
Hint: Normal mutexes will usually be fastest; errorcheck mutexes are use-
ful for debugging; recursive mutexes can be useful for making old
interfaces thread-safe.
### 10.1.3 Set concurrency level
When you use Pthreads implementations that schedule user threads onto
some smaller set of kernel entities (see Section 5.6.3), it may be possible to have
ready user threads while all kernel entities allocated to the process are busy.
352 CHAPTER 10 Future standardization
Some implementations, for example, "lock" a kernel entity to a user thread that
blocks in the kernel, until the blocking condition, for example an I/O request, is
completed. The system will create some reasonable number of kernel execution
entities for the process, but eventually the pool of kernel entities may become
exhausted. The process may be left with threads capable of performing useful
work for the application, but no way to schedule them.
The pthread\_setconcurrency function addresses this limitation by allowing
the application to ask for more kernel entities. If the application designer realizes
that 10 out of 15 threads may at any time become blocked in the kernel, and it is
important for those other 5 threads to be able to continue processing, then the
application may request that the kernel supply 15 kernel entities. If it is impor-
tant that at least 1 of those 5 continue, but not that all continue, then the
application could request the more conservative number of 11 kernel entities. Or
if it is OK for all threads to block once in a while, but not often, and you know
that only rarely will more than 6 threads block at any time, the application could
request 7 kernel entities.
The pthread\_setconcurrency function is a hint, and implementations may
ignore it or modify the advice. You may use it freely on any system that conforms
to the UNIX98 brand, but many systems will do nothing more than set a value
that is returned by pthread\_getconcurrency. On Digital UNIX, for example,
there is no need to set a fixed concurrency level, because the kernel mode and
user mode schedulers cooperate to ensure that ready user threads cannot be pre-
vented from running by other threads blocked in the kernel.
I pthread\_getconcurrency
int pthread\_getconcurrency ();
Returns the value set by a previous pthread\_setconcurrency call. If there have
been no previous calls to pthread\_setconcurrency, returns 0 to indicate that the
implementation is maintaining the concurrency level automatically.
References: 5.6.3, 10.1.3
Errors: none.
Hint: Concurrency level is a hint. It may be ignored by any implementa-
tion, and will be ignored by an implementation that does not need
it to ensure concurrency.
I pthread\_setconcurrency
int pthread\_getconcurrency (int new\_level);
Allows the application to inform the threads implementation of its desired mini-
mum concurrency level. The actual level of concurrency resulting from this call is
unspecified.
X/OpenXSH5[UNIX981 353
References: 5.6.3, 10.1.3
Errors: [einvalj new\_level is negative.
[EAGAIN] new\_level exceeds a system resource.
Hint: Concurrency level is a hint. It may be ignored by any implementa-
tion, and will be ignored by an implementation that does not need
it to ensure concurrency.
### 10.1.4 Stack guard size
Guard size comes from DCE threads. Most thread implementations add to the
thread's stack a "guard" region, a page or more of protected memory. This pro-
tected page is a safety zone, to prevent a stack overflow in one thread from
corrupting another thread's stack. There are two good reasons for wanting to
control a thread's guard size:
1. It allows an application or library that allocates large data arrays on the
stack to increase the default guard size. For example, if a thread allocates
two pages at once, a single guard page provides little protection against
stack overflows-the thread can corrupt adjoining memory without touch-
ing the protected page.
2. When creating a large number of threads, it may be that the extra page for
each stack can become a severe burden. In addition to the extra page, the
kernel's memory manager has to keep track of the differing protection on
adjoining pages, which may strain system resources. Therefore, you may
sometimes need to ask the system to "trust you" and avoid allocating any
guard pages at all for your threads. You can do this by requesting a guard
size of 0 bytes.
I pthread\_attr\_getguardsize
int pthread\_attr\_getguardsize (
const pthread\_attr\_t *attr,
size\_t *guardsize);
Determine the size of the guard region for the stack on which threads created with
attr will run.
References: 2, 5.2.3
Errors: [EINVAL] attr invalid.
Hint: Specify 0 to fit lots of stacks in an address space, or increase default
guardsize for threads that allocate large buffers on the stack.
354 CHAPTER 10 Future standardization
I pthread\_attr\_setguardsize
int pthread\_attr\_setguardsize (
pthread\_attr\_t *attr,
size\_t guardsize);
Threads created with attr will run on a stack with guardsize bytes protected
against stack overflow. The implementation may round guardsize up to the next
multiple of PAGESIZE. Specifying a value of 0 for guardsize will cause threads
created using the attributes object to run without stack overflow protection.
References: 2, 5.2.3
Errors: [EINVAL] guardsize or attr invalid.
Hint: Specify 0 to fit lots of stacks in an address space, or increase default
guardsize for threads that allocate large buffers on the stack.
### 10.1.5 Parallel I/O
Many high-performance systems, such as database engines, use threads, at
least in part, to gain performance through parallel I/O. Unfortunately, Pthreads
doesn't directly support parallel I/O. That is, two threads can independently
issue I/O operations for files, or even for the same file, but the POSIX file I/O
model places some restrictions on the level of parallelism.
One bottleneck is that the current file position is an attribute of the file descrip-
tor. To read or write data from or to a specific position within a file, a thread must
call lseek to seek to the proper byte offset in the file, and then read or write. If
more than one thread does this at the same time, the first thread might seek, and
then the second thread seek to a different place before the first thread can issue
the read or write operation.
The X/Open pread and pwrite functions offer a solution, by making the seek
and read or write combination atomic. Threads can issue pread or pwrite opera-
tions in parallel, and, in principle, the system can process those I/O requests
completely in parallel without locking the file descriptor.
I pread
size\_t pread (
int fildes,
void *buf,
size\_t nbyte,
off\_t offset);
Read nbyte bytes from offset offset in the file opened on file descriptor fildes,
placing the result into buf. The file descriptor's current offset is not affected, allow-
ing multiple pread and/or pwrite operations to proceed in parallel.
X/OpenXSH5[UNIX98] 355
References: none
Errors: [einval] offset is negative.
[ eoverflow] attempt to read beyond maximum.
[ENXIO] request outside capabilities of device.
[ESPIPE] file is pipe.
Hint: Allows high-performance parallel I/O.
pwrite
size t pwrite (
int
const void
size t
off t
fildes,
*buf,
nbyte,
offset);
Write nbyte bytes to offset offset in the file opened on file descriptor f ildes, from
buf. The file descriptor's current offset is not affected, allowing multiplepread and/
or pwrite operations to proceed in parallel.
References: none
Errors: [EIKVAL] offset is negative.
[ESPIPE] file is pipe.
Hint: Allows high-performance parallel I/O.
### 10.1.6 Cancellation points
Most UNIX systems support a substantial number of interfaces that do not
come from POSIX. The select and poll interfaces, for example, should be
deferred cancellation points. Pthreads did not require these functions to be can-
cellation points, however, because they do not exist within POSIX. 1.
The select and poll functions, however, along with many others, exist in
X/Open. The XSH5 standard includes an expanded list of cancellation points
covering X/Open interfaces.
Additional functions that must be cancellation points in XSH5:
getmsg
getpmsg
lockf
msgrcv
msgsnd
poll
pread
putmsg
putpmsg
pwrite
readv
select
sigpause
usleep
wait3
waitid
writev
356
CHAPTER 10 Future standardization
Additional functions that may be cancellation points in XSH5:
catclose
catgets
catopen
closelog
dbm close
dbm delete
dbm fetch
dbm nextkey
dbm open
dbm store
dlclose
dlopen
endgrent
endpwent
endutxent
fgetwc
fgetws
fputwc
fputws
fseeko
fsetpos
ftello
ftw
fwprintf
fwscanf
getgrent
getpwent
getutxent
getutxid
getutxline
getw
getwc
getwchar
iconv close
iconv open
ioctl
mkstemp
nf tw
openlog
pclose
popen
pututxline
putw
putwc
putwchar
readdir r
seekdir
semop
setgrent
setpwent
setutxent
syslog
ungetwc
vfprintf
vfwprintf
vprintf
vwprintf
wprintf
wscanf
## 10.2 POSIX1003.1J
Condition variable wait clock:
int pthread\_condattr\_getclock (
const pthread\_condattr\_t *attr,
clockid\_t *clock\_id);
int pthread\_condattr\_setclock (
pthread\_condattr\_t *attr,
clockid t clock id);
Barriers:
int barrier\_attr\_init (barrier\_attr\_t *attr);
int barrier\_attr\_destroy (barrier\_attr\_t *attr);
int barrier\_attr\_getpshared (
const barrier\_attr\_t *attr, int *pshared);
int barrier\_attr\_setpshared (
barrier\_attr\_t *attr, int pshared);
int barrier\_init (barrier\_t *barrier,
const barrier\_attr\_t *attr, int count);
int barrier\_destroy (barrier\_t *barrier);
int barrier wait (barrier t *barrier);
POSIX 1003. lj
357
Reader/writer locks:
int rwlock\_attr\_init (rwlock\_attr\_t *attr);
int rwlock\_attr\_destroy (rwlock\_attr\_t *attr);
int rwlock\_attr\_getpshared (
const rwlock\_attr\_t *attr, int *pshared);
int rwlock\_attr\_setpshared (
rwlock\_attr\_t *attr, int pshared);
int rwlock\_init (
rwlock\_t *lock, const rwlock\_attr\_t *attr);
int rwlock\_destroy (rwlock\_t *lock);
int rwlock\_rlock (rwlock\_t *lock);
int rwlock\_timedrlock (rwlock\_t *lock,
const struct timespec *timeout);
int rwlock\_tryrlock (rwlock\_t *lock);
int rwlock\_wlock (rwlock\_t *lock);
int rwlock\_timedwlock (rwlock\_t *lock,
const struct timespec *timeout);
int rwlock\_trywlock (rwlock\_t *lock);
int rwlock unlock (rwlock t *lock);
Spinlocks:
int spin\_init (spinlock\_t *lock);
int spin\_destroy (spinlock\_t *lock);
int spin\_lock (spinlock\_t *lock);
int spin\_trylock (spinlock\_t *lock);
int spin\_unlock (spinlock\_t *lock);
int pthread\_spin\_init (pthread\_spinlock\_t *lock);
int pthread\_spin\_destroy (pthread\_spinlock\_t *lock);
int pthread\_spin\_lock (pthread\_spinlock\_t *lock);
int pthread\_spin\_trylock (pthread\_spinlock\_t *lock);
int pthread spin unlock (pthread spinlock\_t *lock);
Thread abort:
int pthread\_abort (pthread\_t thread);
The same POSIX working group that developed POSIX. lb and Pthreads has
developed a new set of extensions for realtime and threaded programming. Most
of the extensions relevant to threads (and to this book) are the result of proposals
developed by the POSIX 1003.14 profile group, which specialized in "tuning" the
existing POSIX standards for multiprocessor systems.
POSIX. lj adds some thread synchronization mechanisms that have been com-
mon in a wide range of multiprocessor and thread programming, but that had been
omitted from the original Pthreads standard. Barriers and spinlocks are primarily
useful for fine-grained parallelism, for example, in systems that automatically
358 CHAPTER 10 Future standardization
generate parallel code from program loops. Read/write locks are useful in shared
data algorithms where many threads are allowed to read simultaneously, but only
one thread can be allowed to update data.
### 10.2.1 Barriers
"Barriers" are a form of synchronization most commonly used in parallel
decomposition of loops. They're almost never used except in code designed to run
only on multiprocessor systems. A barrier is a "meeting place" for a group of
associated threads, where each will wait until all have reached the barrier. When
the last one waits on the barrier, all the participating threads are released.
See Section 7.1.1 for details of barrier behavior and for an example showing
how to implement a barrier using standard Pthreads synchronization. (Note that
the behavior of this example is not precisely the same as that proposed by
POSIX.lj.)
### 10.2.2 Read/write locks
A read/write lock (also sometimes known as "reader/writer lock") allows one
thread to exclusively lock some shared data to write or modify that data, but also
allows multiple threads to simultaneously lock the data for read access. UNIX98
specifies "read/write locks" very similar to POSIX. lj reader /writer locks. Although
X/Open intends that the two specifications will be functionally identical, the
names are different to avoid conflict should the POSIX standard change before
approval.*
If your code relies on a data structure that is frequently referenced, but only
occasionally updated, you should consider using a read/write lock rather than a
mutex to access that data. Most threads will be able to read the data without
waiting; they'll need to block only when some thread is in the process of modify-
ing the data. (Similarly, a thread that desires to write the data will be blocked if
any threads are reading the data.)
See Section 7.1.2 for details of read/write lock behavior and for an example
showing how to implement a read/write lock using standard Pthreads synchroni-
zation. (Note that the behavior of this example is not precisely the same as that
proposed by POSIX. lj.)
*The POSIX working group is considering the possibility of adapting the XSH5 read/write
lock definition and abandoning the original POSIX. lj names, but the decision hasn't yet been
made.
POSIX 1003. lj 359
### 10.2.3 Spinlocks
Spinlocks are much like mutexes. There's been a lot of discussion about
whether it even makes sense to standardize on a spinlock interface-since POSIX
specifies only a source level API, there's very little POSIX. lj says about them that
distinguishes them from mutexes. The essential idea is that a spinlock is the
most primitive and fastest synchronization mechanism available on a given hard-
ware architecture. On some systems, that may be a single "test and set"
instruction-on others, it may be a substantial sequence of "load locked, test,
store conditional, memory barrier" instructions.
The critical distinction is that a thread trying to lock a spinlock does not nec-
essarily block when the spinlock is already held by another thread. The intent is
that the thread will "spin," retrying the lock rapidly until it succeeds in locking
the spinlock. (This is one of the "iffy" spots-on a uniprocessor it had better
block, or it'll do nothing but spin out the rest of its timeslice ... or spin to eter-
nity if it isn't timesliced.)
Spinlocks are great for fine-grained parallelism, when the code is intended to
run only on a multiprocessor, carefully tuned to hold the spinlock for only a few
instructions, and getting ultimate performance is more important than sharing
the system resources cordially with other processes. To be effective, a spinlock
must never be locked for as long as it takes to "context switch" from one thread to
another. If it does take as long or longer, you'll get better overall performance by
blocking and allowing some other thread to do useful work.
POSIX. lj contains two sets of spinlock functions: one set with a spin\_prefix,
which allows spinlock synchronization between processes; and the other set with
a pthread\_prefix, allowing spinlock synchronization between threads within a
process. This, you will notice, is very different from the model used for mutexes,
condition variables, and read/write locks, where the same functions were used
and the pshared attribute specifies whether the resulting synchronization object
can be shared between processes.
The rationale for this is that spinlocks are intended to be very fast, and should
not be subject to any possible overhead as a result of needing to decide, at run
time, how to behave. It is, in fact, unlikely that the implementation of spin\_lock
and pthread\_spin\_lock will differ on most systems, but the standard allows
them to be different.
### 10.2.4 Condition variable wait clock
Pthreads condition variables support only "absolute time" timeouts. That is,
the thread specifies that it is willing to wait until "Jan 1 00:00:00 GMT 2001,"
rather than being able to specify that it wants to wait for  hour, 10 minutes."
The reason for this is that a condition variable wait is subject to wakeups for var-
ious reasons that are beyond your control or not easy to control. When you wake
early from a " 1 hour, 10 minute" wait it is difficult to determine how much of that
360 CHAPTER 10 Future standardization
time is left. But when you wake early from the absolute wait, your target time is
still "Jan 1 00:00:00 GMT 2001." (The reasons for early wakeup are discussed in
Section 3.3.2.)
Despite all this excellent reasoning, "relative time" waits are useful. One
important advantage is that absolute system time is subject to external changes.
It might be modified to correct for an inaccurate clock chip, or brought up-to-date
with a network time server, or adjusted for any number of other reasons. Both
relative time waits and absolute time waits remain correct across that adjust-
ment, but a relative time wait expressed as if it were an absolute time wait
cannot. That is, when you want to wait for " 1 hour, 10 minutes," but the best you
can do is add that interval to the current clock and wait until that clock time, the
system can't adjust the absolute timeout for you when the system time is
changed.
POSIX. lj addresses this issue as part of a substantial and pervasive "cleanup"
of POSIX time services. The standard (building on top of POSIX. lb, which intro-
duced the realtime clock functions, and the clock\_realtime clock) introduces a
new system clock called clock\_monotonic. This new clock isn't a "relative timer"
in the traditional sense, but it is never decreased, and it is never modified by date
or time changes on the system. It increases at a constant rate. A "relative time"
wait is nothing more than taking the current absolute value of the clock\_monotonic
clock, adding some fixed offset D200 seconds for a wait of 1 hour and 10 minutes),
and waiting until that value of the clock is reached.
This is accomplished by adding the condition variable attribute clock. You set
the clock attribute in a thread attributes object using pthread\_condattr\_setclock
and request the current value by calling pthread\_condattr\_getclock. The default
value is clock\_monotonic, on the assumption that most condition waits are
intervals.
While this assumption may be incorrect, and it may seem to be an incompati-
ble change from Pthreads (and it is, in a way), this was swept under the rug due
to the fact that the timed condition wait function suffered from a problem that
POSIX. lj found to be extremely common through the existing body of POSIX
standards. "Time" in general was only very loosely defined. A timed condition
wait, for example, does not say precisely what the timeout argument means. Only
that "an error is returned if the absolute time specified by abstime passes (that is,
system time equals or exceeds abstime)." The intent is clear-but there are no spe-
cific implementation or usage directives. One might reasonably assume that one
should acquire the current time using clock\_gettime (clock\_realtime, snow), as
suggested in the associated rationale. However, POSIX "rationale" is little more
than historical commentary, and is not part of the formal standard. Furthermore,
clock\_gettime is a part of the optional \_posix\_timers subset of POSIX. lb, and
therefore may not exist on many systems supporting threads.
POSIX. lj is attempting to "rationalize" all of these loose ends, at least for systems
that implement the eventual POSIX. lj standard. Of course, the clock\_monotonic
feature is under an option of its own, and additionally relies on the \_posix\_timers
POSIX 1003.14 361
option, so it isn't a cure-all. In the absence of these options, there is no clock
attribute, and no way to be sure of relative timeout behavior-or even completely
portable behavior.
### 10.2.5 Thread abort
The pthread\_abort function is essentially fail-safe cancellation. It is used only
when you want to be sure the thread will terminate immediately. The dangerous
aspect of pthread\_abort is that the thread does not run cleanup handlers or have
any other opportunity to clean up after itself. That is, if the target thread has a
mutex locked, the thread will terminate with the mutex still locked. Because you
cannot unlock the mutex from another thread, the application must be prepared
to abandon that mutex entirely. Further, it means that any other threads that
might be waiting for the abandoned mutex will continue to wait for the mutex for-
ever unless they are also terminated by calling pthread\_abort.
In general, real applications cannot recover from aborting a thread, and you
should never, ever, use pthread\_abort. However, for a certain class of applications
this capability is required. Imagine, for example, a realtime embedded control sys-
tem that cannot shut down and must run reliably across any transient failure in
some algorithm. Should a thread encounter a rare boundary condition bug, and
hang, the application must recover.
In such a system, all wait operations use timeouts, because realtime response
is critical. Should one thread detect that something hasn't happened in a reason-
able time, for example, a navigational thread hasn't received sensor input, it will
notify an "error manager." If the error manager cannot determine why the thread
monitoring the sensor hasn't responded, it will try to recover. It may attempt to
cancel the sensor thread to achieve a safe shutdown, but if the sensor thread fails
to respond to the cancel in a reasonable time, the application must continue any-
way. The error manager would then abort the sensor thread, analyze and correct
any data structures it might have corrupted, create and advertise new mutexes if
necessary, and create a new sensor thread.
## 10.3 POSIX 1003.14
POSIX.14 is a different sort of standard, a "POSIX Standard profile." Unlike
Pthreads and POSIX. lj, POSIX. 14 does not add any new capabilities to the POSIX
family. Instead, it attempts to provide some order to the maze of options that
faces implementors and users of POSIX.
The POSIX.14 specifies which POSIX optional behavior should be considered
"required" for multiprocessor hardware systems. It also raises some of the mini-
mum values denned for various POSIX limits. The POSIX.14 working group also
362 CHAPTER 10 Future standardization
devised recommendations for additional POSIX interfaces based on the substan-
tial multiprocessing and threading experience of the members. Many of the inter-
faces developed by POSIX. 14 have been included in the POSIX. lj draft standard.
Once POSIX. 14 becomes a standard, in theory, producers of POSIX imple-
mentations will be able to claim conformance to POSIX. 14. And those who wish
to develop multithreaded applications may find it convenient to look for POSIX. 14
conformance rather than simply Pthreads conformance. (It remains to be seen
whether vendors or users will actually do this, since experience with POSIX Stan-
dard Profiles is currently slight.}
The POSIX. 14 working group also tried to address important issues such as
these:
? Providing a way for threaded code to determine the number of active
processors.
? Providing a way for threads to be "bound" onto physical processors.
? Providing a "processor management" command to control which processors
are used by the system.
Although these capabilities are universally available in all multiprocessor sys-
tems of which the working group was aware, they were dropped from the
standard because of many unresolved issues, including these:
? What good does it do to know how many processors there are, if you cannot
tell how many your code may use at any time? Remember, the information
can change while you are asking for it. What is really needed is a function
asking the question "Would the current process benefit from creation of
another thread?" We don't know how to answer that question, or how to
provide enough information on all reasonable architectures that the appli-
cation can answer it.
? How can we bind a thread to a processor across a wide range of multipro-
cessor architecture? On a nonuniform memory access system, for example,
representing the processors as a uniform array of integer identifiers would
be misleading and useless-binding one thread to processor 0 and another
closely cooperative thread to processor 1 might put them across a relatively
slow communications port rather than on two processors sharing a bank of
memory.
Eventually, some standards organization (possibly POSIX) will need to address
these issues and develop portable interfaces. The folks who attempt this feat may
find that they need to limit the scope of the standard to a field narrower than
"systems on which people may wish to use threads."
