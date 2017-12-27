
Bibliography
[Anderson, 1991] Thomas E. Anderson, Brian N. Bershad, Edward D. Lazowska,
and Henry M. Levy, "Scheduler Activations: Effective Kernel Support for the
User-Level Management of Parallelism," Proceedings of the Thirteenth ACM
Symposium on Operating Systems Principles, October 1991.
Research paper describing the addition of an efficient "two-level scheduler"
mechanism for operating systems. This is where all modern two-level sched-
uler systems started-everyone's read it, everyone references it, and every-
one's been inspired by it.
[Birrell, 1989] Andrew D. Birrell, An Introduction to Programming with Threads,
SRC Research Report 35, Digital Systems Research Center, 130 Lytton Ave.,
Palo Alto, CA 94301, January 1989. Available on Internet from http://
www.research.digital.com/SRC/publications/src-rr.html
An introduction to the concepts of threaded programming. Although specifi-
cally oriented toward Modula-2+ and SRC's Taos multithreaded operating
system, many essential concepts remain easily recognizable in Pthreads.
[Boykin, 1993] Joseph Boykin, David Kirschen, Alan Langerman, and Susan
LoVerso, Programming under Mach, Addison-Wesley, Reading, MA, ISBN
0-201-52739-1, 1993.
[Custer, 1993] Helen Custer, Inside Windows NT, Microsoft Press, ISBN 1-55615-
481-X, 1993.
[Digital, 1996] Digital Equipment Corporation, Guide to DECthreads, Digital
Equipment Corporation, part number AA-Q2DPC-TK, 1996.
Reference manual for Digital's DECthreads implementation of the Pthreads
standard. An appendix (which will be removed after the Digital UNIX 4.0 and
OpenVMS 7.0 versions) provides reference information on the obsolete cma
and DCE threads (POSIX 1003.4a draft 4) interfaces.
[Dijkstra, 1965] E. W. Dijkstra, "Solution of a Problem in Concurrent Program-
ming Control," Communications of the ACM, Vol. 8 (9), September 1965, pp.
569-570.
[Dijkstra, 1968a] E. W. Dijkstra, "Cooperating Sequential Processes," Program-
ming Languages, edited by F. Genuys, Academic Press, New York, 1968, pp.
43-112.
[Dijkstra, 1968b] E. W. Dijkstra, 'The Structure of the THE'-Multiprogramming
System," Communications of the ACM, Vol. 11 E), 1968, pp. 341-346.
363
364 Bibliography
[Gallmeister, 1995] Bill O. Gallmeister, POSIX.4: Programming for the Real World,
O'Reilly, Sebastopol, CA, ISBN 1-56592-074-0, 1995.
POSIX 1003. lb-1993 realtime programming (based on a near-final draft of the
standard).
[Hoare, 1974] C.A.R. Hoare, "Monitors: An Operating System Structuring Con-
cept," Communications of the ACM, Vol. 17 A0), 1974, pp. 549-557.
[IEEE, 1996] 9945-1:1996 (ISO/IEC) [IEEE/ANSI Std 1003.1 1996 Edition] Infor-
mation Technology-Portable Operating System Interface (POSIX)-Part 1:
System Application: Program Interface (API) [C Language] (ANSI), IEEE Stan-
dards Press, ISBN 1-55937-573-6, 1996.
The POSIX C Language interfaces, including realtime and threads.
[Jones, 1991] Michael B. Jones, "Bringing the C Libraries With Us into a Multi-
Threaded Future," Winter 1991 Usenix Conference Proceedings, Dallas, TX,
January 1991, pp. 81-91.
[Kleiman, 1996] Steve Kleiman, Devang Shah, and Bart Smaalders, Program-
ming with Threads, Prentice Hall, Englewood Cliffs, NJ, ISBN 0-13-172389-8,
1996.
This book shares some characteristics with the book you are holding. Both,
for example, involve authors who were directly involved in the POSIX standard
working group, and were also principal architects of their respective compa-
nies' thread implementations.
[Lea, 1997] Doug Lea, Concurrent Programming in Java?, Addison-Wesley, Read-
ing, MA, ISBN 0-201-69581-2, 1997.
A different view of threads, from the perspective of the Java? language, which
provides unique constructs for thread synchronization within the language.
[Lewis, 1995] Bil Lewis and Daniel J. Berg, Threads Primer, SunSoft Press, ISBN
0-13-443698-9, 1995.
A good introduction to threaded programming for the novice. The first edition
primarily deals with Solaris "UI threads," with some information on POSIX
threads.
[Lockhart, 1994] Harold W. Lockhart, Jr., OSFDCE, Guide to Developing Distrib-
uted Applications, McGraw-Hill, New York, ISBN 0-07-911481-4, 1994.
A chapter on DCE threads describes how to use threads in building DCE
applications.
[McJones, 1989] Paul F. McJones and Garret F. Swart, "Evolving the UNIX Sys-
tem Interface to Support Multithreaded Programs," SRC Research Report 21,
Digital Systems Research Center, 130 Lytton Ave., Palo Alto, CA 94301, Sep-
tember 1989. Available on Internet from http://www.research.digital.com/
SRC/publications/src-rr.html
Report on adaptation of UNIX system for multithreaded programming.
Bibliography 365
[Schimmel, 1994] Curt Schimmel, UNIX Systems for Modern Architectures, Addi-
son-Wesley, Reading, MA, ISBN 0-201-63338-8, 1994.
Substantial detail on the implementation of multiprocessors and shared
memory systems. If Section 3.4 in the book you're holding doesn't satisfy your
thirst for knowledge, this is where you should go.
Thread resources on the Internet
In the midst of the word he was trying to say,
In the midst of his laughter and glee,
He had softly and suddenly vanished away-
For the Snark was a Boojum, you see.
THE END
-Lewis Carroll, The Hunting of the Snark
This list provides a few starting points for information. Of course, the web
changes all the time, so no list committed to paper will ever be completely correct.
That's life in the information age.
Newsgroups
comp.programming.threads
General, unmoderated discussion of anything related to threads. This group is
frequented by a number of people highly knowledgeable about threads in general,
and about various specific implementations of Pthreads. It's a nice, friendly place
to ask about problems you're having, or things you would like to do. Please, don't
ask about screensavers! And, if you want to ask about a problem, always remem-
ber to tell us what type of hardware and operating system you're using and
include the version.
comp.unix.os?.os?1
The primary discussion group for the Digital UNIX operating system. There
are, of course, historical reasons for the nonintuitive name. This is a reasonable
place to ask questions about using threads on Digital UNIX. If the question (or
problem) doesn't seem to be specific to Digital UNIX, comp. programming. threads
may be more appropriate, because it presents your question to a larger audience
of thread experts, and makes the answer available to a larger audience of thread
users.
comp.unix.Solaris
The primary discussion group for the Solaris operating system. This is a rea-
sonable place to ask questions about using threads on Solaris. If the question (or
problem) doesn't seem to be specific to Solaris, comp.programming.threads may
367
368 Thread resources on the Internet
be more appropriate, because it presents your question to a larger audience of
thread experts, and makes the answer available to a larger audience of thread
users.
Web sites
http://altavista.digital.com/
AltaVista is a multithreaded web search engine developed by Digital Equip-
ment Corporation. It is also an excellent search engine that you can use to find
out about nearly anything. Always a good place to start.
http://www.aw.com/cp/butenhof/posix.html
The Addison-Wesley web page containing information about this book, includ-
ing the source for all the example programs.
http://www.best.com/-bos/threads-faq/
This page is a list of frequently asked questions (FAQ) from the comp. program-
ming, threads newsgroup. Please read this before you read comp. programming.
threads, in order to avoid asking a wide range of questions that have been asked
a million times before. The information in this page is also posted to the news-
group at regular intervals.
http://liinwww.ira.uka.de/bibliography/Os/threads.html
A searchable bibliography of terms related to threading, maintained by the
University of Oslo in Norway.
http://www.digital.com/
Digital Equipment Corporation web site. This site includes a lot of information
on the Digital UNIX and OpenVMS operating systems, including information on
threads and multiprocessor systems.
http://www.sun.com/
Sun Microsystems, Inc., web site. This site includes, as you might guess, a lot
of information on the Solaris operating system. You can also find information
about the Java language, which provides an interesting variant of thread support
by making thread synchronization an explicit attribute of a class method.
http://www.sgi.com/
Silicon Graphics, Inc., web site. Information on SGI systems and the IRIX
operating system.
http: / /www. netcom. com/-brownel 1 /pthreads++. html
Information on an attempt to "define a standardized approach to the use of
threading in the C++ language."
Index
Abort thread, 361
alo\_read, 230
aio\_write, 230
Allocation domain, 181-183
Amdahl's Law, 21-22, 297
ANSIC
and cancellation, 151
and deadlocks, 298
and destructor functions, 168
and fork handlers, 199-200
and kernel entities, 189-190
library, runtime, 284
and priority inversions, 300
prototype for Pthread interfaces,
310-346
and sequence races, 296
and signals, 217
and threading, 24-25
void*, use of, 311
See also Stdio (function)
Apple Macintosh toolbox, 23
asctime\_r (function), 213, 339-340
Assembly language and threading,
24-25
Assembly line. See Pipeline program-
ming model
Asynchronous
cancelability, 150-154, 249
communication between processes, 8
definition of, 2, 4
I/O operations, 22-23
programming, 8-12, 15-21
sigwait, 227-230
and UNIX, 9-11
See also Memory, visibility
Async-signal safe functions, 198, 234
Atomicity, 61, 72-73, 93
Attribute objects
condition variable attributes, 137-138
definition of, 134-135
and detaching, 37, 43, 231
mutex attributes, 135-136, 185
thread attributes, 138-141, 174, 181,
231
Bailing programmers
barriers analogy, 242
cancellation analogy, 143
condition variable analogy, 71
mutex analogy, 47-48
overview of, 3-4
thread-specific data analogy, 162-163
Barriers
definition of, 241
memory, 92-95
POSIX 1003.lj, 358
and synchronization, extended,
242-253
See also Condition variables; Mutexes
Blocking threads, 42
Broadcast
and condition variables, 72-76, 80-82
and memory visibility, 89
and work crews, 109
C language. See ANSI C
Cache lines, 304-305
Cancellation
asynchronous, 150-154, 249
cleanup handlers, 147, 154-161
deferred, 147-150, 249
definition of, 142-147
fail-safe in POSIX 1003. lj, 361
interfaces, 323-325
points, list of, 147-148
points in XSH5, 355-356
state, 143-144
type, 143-144
Cleanup handlers, 147, 154-161
Client server programming model,
120-129
369
370
Index
clock\_gettime (function), 13
CLOCK\_MONOTONIC (value), 360
Code. See Programs, examples of
Communication. See Condition variables
Computing overhead and threads, 26
Concurrency
benefit of threading, 22-24
control functions, definition of,
7-8
definition of, 4-5
level in XSH5, 351-353
and parallelism, 5-6
serialization, avoidance of, 302-303
and thread implementation, 190
See also Asynchronous; Threads
Condition variables
attributes, 137-138
and blocked threads, 42
and client-server programming model,
121
broadcasting, 81-82
creating and destroying, 74-76
definition of, 8
interfaces, 319-322
and pipeline programming model, 101
and semaphores, 237
sharing, avoidance of, 300
signaling, 81-82
as a synchronization mechanism, 4, 8,
30, 70-73
and thread-safety, 6-7
wait clock (POSIX 1003.1J), 359-361
waiting on, 77-81
waking waiters, 81-82
and work crew programming model,
107
See also Barriers; Mutexes; Predicates;
Signals
Contention scope, 181-183
Context structure, 6-7
Critical sections, 46
ctime\_r (function), 213, 340
DCE threads, 154, 290, 353
Deadlocks
avoidance of, 26-27, 297-299
and lock hierarchy, 63-69
and signals, 215
and thread-safe libraries, 284-285
See also Priority inversions
Debugging, threads
cache lines, 304-305
concurrent serialization, avoidance of,
302-303
condition variable sharing, avoidance
of, 300
and costs of threading, 13, 27-28
deadlocks, avoidance of, 297-299
introduction to, 13
memory corrupters, avoidance of, 301
mutexes, right number of, 303-304
priority inversions, avoidance of,
299-300
thread inertia, avoidance of, 291-293
thread race, avoidance of, 293-297
tools for, 290, 302
See also Errors
Default mutexes, 349-351
Deferred cancelability, 147-150, 249
Destructor functions, 167-172
Detach
creating and using threads, 37-39
and multiple threads, 17-19
termination, 43
and thread attributes, 138-141
Digital UNIX
mutex error detection and reporting,
311
thread debugging library, 290
programming examples, introduction
to, 12-13
detecting memory corrupters, 301
SIGEV-THREAD implementation, 231
Dijkstra, EdsgerW., 47
Directory searching function, 212
E
EAGAIN (error), 314, 318, 326, 344, 353
EBUSY (error), 49, 58, 65, 318-321, 345
EDEADLK (error), 32, 318-319, 346
EINTR (error), 346
EINVAL (error), 312-326, 343-346,
351-355
Encapsulation, 24, 75
ENOMEM (error), 313, 317-321, 326, 336
ENOSPC (error), 345
ENOSYS (error), 312-316, 344-346
Index
371
ENXIO (error), 355
EOVERFLOW (error), 355
EPERM (error), 318, 345
ERANGE (error), 210, 341-342
err\_abort, 33-34
errno, 31-34, 117, 228
errno\_abort, 33-34
errno.h (header file), 32-33
Error detection and reporting, 310-311
Errorcheck mutexes, 349-351
Errors
EAGAIN, 314, 318, 326, 344, 353
EBUSY, 49, 58, 65, 318-321, 345
EDEADLK, 32, 318-319, 346
EINTR, 346
EINVAL, 312-326, 343-346, 351-355
ENOMEM, 313, 317-321, 326, 336
ENOSPC, 345
ENOSYS, 312-316, 344-346
ENXIO, 355
EOVERFLOW, 355
EPERM, 318, 345
ERANGE, 210, 341-342
ESPIPE, 355
ESRCH, 32, 314, 323, 343
ETIMEDOUT, 78, 80, 86, 322
errors.h (header file), 13
ESPIPE (error), 355
ESRCH (error), 32, 314, 323, 343
ETIMEDOUT (error), 78, 80, 86, 322
Events
and concurrency, 23-24
as a synchronization mechanism, 8
Exec, 204
Execution context
architectural overview, 30
definition of, 7-8
fgets (function), 14
flockfile (function), 121, 205-207,
336-337
Fork
in asynchronous program, 15, 19
definition of, 197-198
handlers, 199-203, 336
ftrylockfile (function), 207, 337
funlockfile (function), 121, 205-207, 337
G
Gallmeister, Bill, 294
getc (function), 207
getchar (function), 207
getchar\_unlocked (function), 207-209,
338
getc\_unlocked (function), 207-208, 337
getgrgid\_r (function), 341
getgrnam\_r (function), 341
getlogin\_r (function), 210, 339
getpwnam\_r (function), 342
getpwuid\_r (function), 342
gmtime\_r (function), 340
Group and user database function,
213-214
H
Hard realtime, 172-173
inheritsched (attribute), 138-141, 176
Initial (main) thread
and creation, 40-42
initialization, onetime, 131-134
overview of, 36-37
signal mask, 216
Initialization, 131-134
Invariants
and condition variables, 71, 75
definition of, 45-46
and mutexes, 61-64
See also Predicates
I/O operations
and blocked threads, 42
and cancellation points, 148-149
candidate for threads, 29
and concurrency, 22-23
parallel in XSH5, 354-355
Join, 37-39, 139, 145
Kernel entities
and contention scope thread, 182
implementation of, 189
many to few (two level), 193-195
many to one (user level), 190-191
372
Index
Kernel entities (continued)
one to one (kernel level), 191-193
setting in XSH5, 351-353
L
Libraries
for debugging, 290
implementation, 190
legacy, 285-287
thread-safe, 283-285
Light weight processes, 1
limits.h (file header), 308
localtime\_r (function), 340
LWP. See Kernel entities
M
Main (initial) thread
and creation, 40-42
initialization, one time, 131-134
overview of, 36-37
signal mask, 216
Many to few (two level) implementation,
193-195
Many to one (user level) implementation,
190-191
Memory
barriers, 92-95
coherence, 47, 92
conflict, 94
corrupters, avoidance of, 301
leaks, 198
ordering, 48, 90, 92-93
stack guard size in XSH5, 353-354
visibility, 26, 88-95, 294
Message queues, 30
Microsoft Windows, 23
MIMD (multiple instruction, multiple
data), 107
MPP (massively parallel processor), 5
Multiprocessors
and allocation domain, 182
candidate for threads, 29
and concurrency, 1, 4-5
and deadlock, 65
definition of, 5
memory architecture, 91-95
and parallelism, 5-6, 20-22
and thread implementation, 191-193
and thread inertia, 291-293
and thread race, 293-297
Multithreaded programming model. See
Threads
Mutexes
and atomicity, 61, 72-73
attributes, 135-136
and blocked threads, 42, 51
and client-server programming model,
121
creating and destroying, 49-51
and deadlocks, 26-27, 63, 66-69,
297-299
definition of, 8, 47-49
interfaces, 316-319
and invariants, 61-64
lock chaining, 70
lock hierarchy, 63-70
locking and unlocking, 52-58
multiple, use of, 63-64
non-blocking locks, 58-61
number of, 303-304
and pipeline programming model,
100-101
priority ceiling, 186-187, 300
priority inheritance, 186-188, 300,
307
priority inversions, 26-27, 63,
184-186, 299-300
priority-aware, 185-186
and semaphores, 236
sizing of, 62-63
as a synchronization mechanism, 8,
30
and thread-safety, 6-7, 62
types inXSH5, 349-351
and work crew programming model,
108-110
See also Barriers; Condition variables;
Memory, visibility; Read/write locks
Mutually exclusive. See Mutexes
N
Network servers and clients,
99 9*3
Normal mutexes, 349-351
NULL (value), 167
Index
373
Object oriented programming and
threading, 25
One to one (kernel level) implementation,
191-193
Opaque, 31, 36, 163
Open Software Foundation's Distributed
Computing Environment, 154
Paradigms, 25
Parallel decomposition. See Work crews
Parallelism
and asynchronous programming,
11-12
benefit of threading, 20-25
candidate for threads, 29
definition of, 5-6
and thread-safety, 6-7
See also Concurrency
Performance
as a benefit of threading, 20-22
as a cost of threading, 26
and mutexes, 62-63
problems, avoidance of, 302-305
Pipeline programming model, 97-105
Pipes, 30
Polymorphism, 24
POSIX
architectural overview, 30
conformance document, 307, 308
error checking, 31-34
realtime scheduling options, 173
signal mechanisms, 40-41, 81-82
types and interfaces, 30-31
POSIX 1003.1 (ISO/IEC 9945-1:1996),
29-30
POSIX 1003.1-1990, 31, 209
POSIX 1003.1b-1993 (realtime)
and condition variables, 3.94, 80
and semaphores, 236-237, 345
and signals, 230-232
thread concepts, 29-30
void *, use of, 311
POSIX 1003.1c-1995 (threads)
and cancellation, 154
cancellation, interfaces, 323-325
condition variables, interfaces,
319-322
error detection and reporting, 310-311
fork handlers, interfaces, 336
interfaces, overview, 309-310
limits, 308-309
mutexes, interfaces, 316-319
options, 307-308
and realtime scheduling, 173
realtime scheduling, interfaces,
326-335
semaphores, interfaces, 345-346
signals, interfaces, 342-345
stdio, interfaces, 336-338
thread concepts, 29-30
threads, interfaces, 311-316
thread-safe, interfaces, 338-342
thread-specific data, interfaces,
325-326
void *, use of, 311
POSIX 1003. li-1995 (corrections to
1003.1b-1993), 29-30
POSIX 1003.1J (additional realtime
extension)
barriers, 249, 356-358
read/write locks, 358
spinlocks, 359
thread abort, 361
wait clock, condition variable,
359-361
POSIX 1003.14 (multiprocessor profile),
30, 182, 361-362
POSIX\_PRIOJNHERIT, 186
POSIX\_PRIO\_NONE (value), 186
POSIX\_PRIO\_PROTECT, 186
\_POSIX\_REALTIME\_SIGNALS (option),
228
\_POSIX\_SEMAPHORES (option), 235
\_POSIX\_THREAD\_ATTR\_STACKADDR
(option), 139, 308, 348
JPOSIX\_THREAD\_ATTR\_STACKSIZE
(option), 139, 308, 348
\_POSrX\_THREAD\_PRIO\_INHERIT
(option), 185-186, 308, 349
\_POSIX\_THREAD\_PRIO\_PROTECT
(option), 185-186, 308, 349
\_POSIX\_THREAD\_PRIORTTY\_SCHEDUL-
ING (option), 173-176, 179, 308,
349
\_POSIX\_THREAD\_PROCESS\_SHARED
(option), 136-137, 308, 348
374
Index
\_POSIX\_THREADS (option), 308, 348
\_POSIX\_THREAD\_SAFE\_FUNCTIONS
(option), 308, 349
\_POSIX\_TIMERS (option), 360
pread (function), 354-355
Predicates
definition of, 46
loose, 80
wakeup, 80-81
waking waiters, 81
See also Condition variables
printf (function), 15
prioceiling (attribute), 135-138
Priority ceiling, 186-187, 300
Priority inheritance
definition of, 186
mutexes, 188
and POSIX 1003.1c options, 307
priority inversion, avoidance of, 300
Priority inversions
avoidance of, 299-300
as a cost of threading, 26-27
mutexes and priority scheduling, 63
as a realtime scheduling problem, 184
See also Deadlocks
Process contention, 181-185
Process exit, 204
Processes
asynchronous, 8
lightweight, 1
threads, compared to, 10, 20
variable weight, 1
Processors
and blocked threads, 42
and thread implementation, 190-195
See also Multiprocessors;
Uniprocessors
Programs, examples of
barriers, 245-248, 250-253
cancellation, 145-161
client server, 121-129
condition variables, creating and
destroying, 74-76
condition variables, timed condition
wait, 83-88
condition variables, waiting on, 78-80
creating and using threads, 38-39
errors, 32-34
flockfile, 205-207
fork handlers, 201-203
initialization, 133-134
multiple processes, 15-16
multiple threads, 17-19
mutex attributes object, 136
mutexes, deadlock avoidance, 66-69
mutexes, dynamic, 50-68
mutexes, locking and unlocking,
52-57
mutexes, non-blocking locks, 58-61
mutexes, static mutex, 50
pipeline, 99-105
putchar, 208-209
read/write locks, 255-269
realtime scheduling, 175-181
sample information, 13
semaphore, 238-240
SIGEV\_THREAD, 232-234
sigwait, 228-230
suspend and resume, 218-227
synchronous programming, 13-15, 27
thread attributes, 140-141
thread inertia, 292
thread-specific, 164-165, 169-172
user and terminal identification, 211
work crews, 108-120
work queue manager, 271-283
protocol (attribute), 135-138, 186
pshared (attribute), 135-138, 204
pthread\_abort (function), 361
pthread\_atfork (function), 199, 336
pthread\_attr\_destroy (function), 312
pthread\_attr\_getdetachstate (function),
312
pthread\_attr\_getguardsize (function),
353
pthread\_attr\_getinheritsched (function),
327
pthread\_attr\_getschedparam (function),
327
pthread\_attr\_getschedpolicy (function),
327-328
pthread\_attr\_getscope (function), 328
pthread\_attr\_getstackaddr (function),
312-313
pthread\_attr\_getstacksize (function),
135, 139,313
pthread\_attr\_init (function), 139, 313
Index
375
pthread\_attr\_setdetachstate (function),
313
pthread\_attr\_setguardsize (function),
354
pthread\_attr\_setinheritsched (function),
176, 329
pthread\_attr\_setschedparam (function),
175, 329
pthread\_attr\_setschedpolicy (function),
175, 329-330
pthread\_attr\_setscope (function), 182,
330
pthread\_attr\_setstackaddr (function),
314
pthread\_attr\_setstacksize (function),
135,314
pthread\_attr\_t (datatype), 135, 139, 231
pthread\_cancel (function)
asynchronous cancelability, 151
deferred cancelability, 148
definition of, 323
and pthreadjdll, 217
termination, 43, 143-145
PTHREAD\_CANCEL\_ASYNCHRONOUS
(value), 152, 324
PTHREAD\_CANCEL\_DEFERRED
(value), 145, 147, 324
PTHREAD\_CANCEL\_DISABLE (value),
145,149,324
PTHREAD\_CANCELED (value), 43, 145
PTHREAD\_CANCEL\_ENABLE (value),
147, 324
pthread\_cleanup\_pop (function), 43,
147, 155, 323
pthread\_cleanup\_push (function), 43,
147, 155, 323
pthread\_condattr\_destroy (function),
319
pthread\_condattr\_getclock (function),
360
pthread\_condattr\_getpshared (function),
320
pthread\_condattr\_init (function), 137,
320
pthread\_condattr\_setclock (function),
360
pthread\_condattr\_setpshared (function),
137, 320
pthread\_condattr\_t (datatype), 135
pthread\_cond\_broadcast (function) ,81,
256, 300, 321
pthread\_cond\_destroy (function), 76,
321
pthread\_cond\_init (function), 75, 137,
321
pthread\_cond\_signal (function), 81, 300,
322
pthread\_cond\_t (datatype), 74, 137
pthread\_cond\_timedwait (function), 78,
80,322
pthread\_cond\_wait (function), 77, 85,
322
pthread\_create (function)
and attributes objects, 139
creating and using threads, 36-42,
189
definition of, 314
execution context, 30
and memory visibility, 89
and multiple threads, 17
and thread identifier, 144-145, 266
PTHREAD\_CREATE\_DETACHED
(value), 44, 125, 139, 231, 312-313
PTHREAD\_CREATE\_JOINABLE (value),
139, 231,312-313
PTHREAD\_DESTRUCTORJTERATIONS
(limit), 168, 309
pthread\_detach (function)
cleaning up, 158
creating and using threads, 37
definition of, 315
and multiple threads, 17
termination, 43-44
pthread\_equal (function), 36, 315
pthread\_exit (function)
and attributes objects, 140
cleaning up, 155
creating and using threads, 37-38
definition of, 204, 315
and fork, 197
and memory visibility, 89
and multiple threads, 17
termination, 30, 40-44, 53
PTHREADJEXPLICITJSCHED (value),
176, 327-329
pthread\_getconcurrency (function), 352
376
Index
pthread\_getschedparam (function), 331
pthread\_getspeciflc (function), 34, 164,
166, 325
pthread.h (header file), 13
PTHREADJNHERIT\_SCHED (value),
176, 327-329
pthreadjoin (function)
and attributes objects, 139
cleaning up, 158
creating and using threads, 37-38
definition of, 315
and error checking, 32
and memory visibility, 89
and pthread\_kill, 225
termination, 43-44, 128, 145
pthread\_key\_create (function), 163-166,
325-326
pthread\_key\_delete (function), 166, 326
PTHREAD\_KEYS\_MAX (limit), 166, 309
pthread\_key\_t (datatype), 163-166
pthread\_kill (function), 217-227, 343
pthread\_mutexattr\_destroy (function),
316
pthread\_mutexattr\_getprioceiling (func-
tion), 332
pthread\_mutexattr\_getprotocol (func-
tion), 332-333
pthread\_mutexattr\_getpshared (func-
tion), 317
pthread\_mutexattr\_gettype (function),
350-351
pthread\_mutexattr\_init (function), 135,
317
pthread\_mutexattr\_setprioceiling (func-
tion), 333
pthread\_mutexattr\_setprotocol (func-
tion), 186, 333-334
pthread\_mutexattr\_setpshared (func-
tion), 136,317
pthread\_mutexattr\_settype (function),
351
pthread\_mutexattr\_t (datatype), 135
FTHREAD\_MUTEX\_DEFAULT (value),
349-351
pthread\_mutex\_destroy (function), 51,
318,350
PTHREAD\_MUTEX\_ERRORCHECK
(value), 349
pthread\_mutex\_getprioceiling (func-
tion), 331
pthread\_mutex\_init (function)
and attributes objects, 135
creating and destroying mutexes,
50-51
definition of, 318
initialization of, 132, 186
standardization, future, 350
PTHREAD\_MUTEX\_INITIALIZER
(macro), 50-52, 74-76
pthread\_mutex\_lock (function)
asynchronous cancelability, 151
definition of, 318
lock hierarchy, 64-65
locking and unlocking, 52, 58
and memory visibility, 90, 93
mutexes, number of, 303-304
standardization, future, 350
XSHS mutex types, 350
PTHREAD\_MUTEX\_NORMAL (value),
349
PTHREAD\_MUTEX\_RECURSIVE (value),
349
pthread\_mutex\_setprioceiling (func-
tion), 332
pthread\_mutex\_t (datatype), 49, 62, 136
pthread\_mutex\_trylock (function)
creating and destroying mutexes, 49
definition of, 319
and flockfile and funlockfile, 207
lock hierarchy, 64-65
locking and unlocking, 52, 58
mutexes, number of, 303-304
read/write locks, 257
standardization, future, 350
XSHS mutex types, 350
pthread\_mutex\_unlock (function)
creating and destroying mutexes, 49
definition of, 319
and flockfile and funlockfile, 207
lock hierarchy, 64-65
locking and unlocking, 52, 58
and memory visibility, 90
Index
377
mutexes, number of, 303-304
standardization, future, 350
XSHS mutex types, 350
pthread\_once (function)
initialization, condition variables, 75
initialization, mutexes, 50
or statically initialized mutex, 132
in suspend/resume, 220-221
and thread races, 295-296
thread-specific data, 163-164
PTHREAD\_ONCE\_INIT (macro), 132
pthread\_once\_t (datatype), 132
PTHREAD\_PRIOJNHERIT (value), 186,
333-334
PTHREAD\_PRIO\_NONE (value), 333-334
PTHREAD\_PRIO\_PROTECT (value), 186,
333-334
PTHREAD\_PROCESS\_PRTVATE (value),
136-137, 317-320
PTHREAD\_PROCESS\_SHARED (value),
136-138, 204, 317-320
PTHREAD\_SCOPE\_PROCESS (value),
182,328-330
PTHREAD\_SCOPE\_SYSTEM (value),
182, 328-330
pthread\_self (function), 17, 36-37,
144-145, 316
pthread\_setcancelstate (function), 147,
149, 151, 324
pthread\_setcanceltype (function), 151,
324
pthread\_setconcurrency (function),
352-353
pthread\_setschedparam (function), 334
pthread\_setspecific (function), 166, 326
pthread\_sigmask (function), 215-216,
343
pthread\_spin\_lock (function), 359
PTHREAD\_STACK\_MIN (limit), 139, 309
pthread\_t (datatype)
creating and using threads, 36-37,
189, 266
and pthread\_kill, 217
termination, 43, 144-145
thread-specific data, 161-162
pthread\_testcancel (function), 144-145,
150, 158, 325
PTHREAD\_THREAD\_MAX (limit), 309
putc (function), 207
putchar (function), 6, 207
putchar\_unlocked (function), 207-209,
338
putc\_unlocked (function), 207-208, 338
pwrite (function), 355
R
Races
avoidance of, 26-27
and condition variables, 73
and memory visibility, 91
overview, 293-295
sequence race, 284-285, 295-297
synchronization race, 294-296
thread inertia, 291-293
raise (function), 217
Random number generation function,
213
rand\_r (function), 213, 341
readdir\_r (function)
definition of, 339
directory searching, 212
reentrancy, 7, 297
thread-safe function, 210
and work crews, 107-109
Read/write locks, 242, 253-269, 358
Read/write ordering, 92-95
Ready threads, 39-42, 53
Realtime scheduling
allocation domain, 181-183
architectural overview, 30
contention scope, 181-183
definition of, 7-8, 172-173
hard realtime, 172-173
interfaces, 326-335
mutexes, priority ceiling, 186-187,
300
mutexes, priority inheritance,
186-188, 300, 307
mutexes, priority-aware, 185-186
policies and priorities, 174-181
POSIX options, 173
and priority inversion, 299-300, 326
problems with, 183-185
soft realtime, 172-173
and synchronization, 295
Recursive mutexes, 349-351
378
Index
Recycling threads, 43-44
Reentrant, 6-7, 297
See also Readdir\_r (function)
Resume. See Pthread\_kill (function)
Running threads, 42
Scaling, 20-22
SC\_GETGR\_R\_SIZE\_MAX (value), 214
SC\_GETPW\_R\_SIZE\_MAX (value), 214
SCHED\_BG\_NP (value), 175
SCHED\_FG\_NP (value), 175
SCHED\_FIFO (value)
problems with, 183-185
as a scheduling interface value,
328-331, 334-335
scheduling policies and priorities,
174-175
and thread race, 295
sched\_get\_priority\_max (function), 174,
335
sched\_get\_priority\_min (function), 174,
335
SCHED\_OTHER (value), 175,328-331,
334-335
schedparam (attribute), 138-141, 175
schedpolicy (attribute), 138-141, 175
SCHED\_RR (value), 174, 185, 328-331,
334-335
Scheduler Activations model, 194
Scheduling. See Realtime scheduling
schedjrield (function), 53-54, 65, 221,
316
Schimmel, Curt, 94
scope (attribute), 138-141, 182
\_SC\_THREAD\_ATTR\_STACKADDR
(option), 308
\_SC\_THREAD\_ATTR\_STACKSIZE
(option), 308
\_SC\_THREAD\_DESTRUCTOR\_ITERA-
TIONS (limit), 309
\_SC\_THREAD\_KEYS\_MAX (limit), 309
\_SC\_THREAD\_PRIO\_INHERIT (option),
185, 308
\_SC\_THREAD\_PRIO\_PROTECT (option),
185-186, 308
\_SC\_THREAD\_PRIORITY\_SCHEDULING
(option), 308
\_SC\_THREAD\_PROCESS\_SHARED
(option), 308
\_SC\_THREADS (limit), 308
\_SC\_THREAD\_SAFE\_FUNCTIONS
(option), 308
\_SC\_THREAD\_STACK\_MIN (limit), 309
\_SC\_THREAD\_THREADS\_MAX (limit),
309
Semaphores
functions, definition of, 345-346
as a synchronization mechanism, 8,
30
synchronization with signal catching,
234-240
sem\_destroy (function), 237, 345
sem\_getvalue (function), 237
sem\_init (function), 237, 345
sem\_post (function), 235-237, 346
sem\_t (value), 237
semjxywait (function), 237, 346
sem\_wait (function), 236-237, 346
Sequence races, 284-285, 295-297
Serial programming, 25
Serial regions. See Predicates
Serialization, 21
Shared data, 3
sigaction (function), 215
SIG\_BLOCK (value), 343
SIGCONT (action), 217
sigevent, 311
sigev\_notify\_attributes, 231
sigev\_notify\_function, 231
SIGEV\_THREAD (function), 40, 231-234
sigevjvalue (function), 311
SIGFPE (function), 215-216
SIGKILL (function), 216-217
Signals
actions, 215-216
background, 214-215
and condition variables, 72-76,
80-81
handlers, 91-92
interfaces, 342-345
masks, 216
and memory visibility, 89
pthread\_kill, 217-227
running and blocking threads, 42
semaphores, 234-240
Index
379
SIGEV^THREAD, 231-234
sigwait, 227-230
See also Condition variables
SIGPIPE (action), 215
sigprocmask (function), 216
sigqueue (function), 230
SIGSEGV (action), 216
SIG\_SETMASK (value), 343
SIGSTOP (action), 216-217
sigtimedwait (function), 228, 343-344
SIGTRAP signal, 216, 290
SIGJJNBLOCK (value), 343
sigwait (function)
definition of, 227-230, 344
running and blocking, 42
and semaphores, 234
sigwaitinfo (function), 228, 234, 344-345
SIMD (single instruction, multiple data),
106
sleep (function), 15
SMP. See Multiprocessors
Soft realtime, 172-173
Solaris 2.5
concurrency level, setting of, 58, 119,
128, 145, 152, 266
thread debugging library, 290
programming examples, introduction
to, 12-13
realtime scheduling, 176, 179
SIGEVJTHREAD implementation, 231
Spinlocks. 359
Spurious wakeups, 80-81
stackaddr (attribute), 138-141
stacksize (attribute), 138-141
Startup threads, 41-42
stderr, 33
stdin (function)
in asynchronous program example,
14, 18
and client servers, 121
in pipeline program example, 98, 105
and stdio, 205-207
stdio (function)
and concurrency, 23
and fork handlers, 199
interfaces, 336-338
and realtime scheduling. 190
thread-safety. 6. 205-207
stdout (function)
and client servers, 121
in pipeline program example, 98
and stdio, 205
in suspend program example, 224
strerror (function), 33
String token function, 212
strtok\_r (function), 212, 339
struct aiocb, 230
struct dirent, 109, 210
struct sigevent, 230
Suspend. See Pthread\_kill (function)
Synchronization
architectural overview, 30
and computing overhead, 26
critical sections, 46
definition of, 7-8
objects, 3
and programming model, 24-25
protocols, 26
races, 284-285
and reentrant code, 6-7
and scheduling, 295
and semaphores, 234-240
and sequence race, 294-297
and UNIX, 9-11
See also Barriers; Condition variables;
Invariants; Memory, visibility;
Mutexes; Parallelism; Read/write
locks
Synchronous
I/O operations, 22-23
programming, 13-15, 27
sysconf (function), 185, 214, 307-308
System contention, 181-185
Termination of threads, 43-44
thd\_continue (interface), 217, 223-224
thd\_suspend (interface), 217-221, 224
Threads
abort (POSIX 1003.1J), 361
architectural overview, 30
and asynchronous programming,
8-12
attributes, 138-141
benefits of, 10-11, 20-25
blocking, 42
380
Index
Threads (continued)
candidates for, 28-29
client server programming model,
120-129
costs of, 25-28
creating and using, 25-41
definition of, 8
error checking, 31-34
identifier, 36
implementation of, 189
initial (main), 36-42, 119, 131-134
interfaces, 311-316
introduction to, 1-3
many to few (two level), 193-195
many to one (user level), 190-191
one to one (kernel level), 191-193
as part of a process, 10
pipeline programming model, 97-105
processes, compared to, 10, 20
programmers, compared to, 3
programming model, 24-25
ready, 39-42, 53
recycling, 43-44
running, 42
startup, 41-42
states of, 39-41
termination, 43-44
and traditional programming, 4, 27
types and interfaces, 30-31
work crew programming model,
105-120
See also Concurrency; Debugging,
threads
Thread-safe
definition of, 6-7
interfaces, 286-287, 338-342
libraries, 283-285
library, 303-304
and mutexes, 62
and programming discipline, 26-27
Thread-safe functions
directory searching, 212
group and user database, 213-214
random number generation, 213
string token, 212
time representation, 212-213
user and terminal identification,
209-211
Thread-specific data
creating, 163-166
destructor functions, 167-172
interfaces, 325-326
overview, 161-163
and termination, 43
and thread-safety, 6-7
use of, 166-167
thr\_setconcurrency (function), 13, 58,
119, 128, 145, 152
Time representation function, 212-213
Timer signals, 23
timer\_create (function), 230
Timeslice, 8, 42, 174
Tokens, 3, 47
tty\_name\_r (function), 210
U
Uniprocessors
and allocation domain, 182
and concurrency, 4-5
and deadlock, 65
definition of, 5
and thread inertia, 291-293
and thread race, 293-297
unistd.h (header file), 307
University of Washington, 194
UNIX
and asynchronous, 9-11, 22
and error checking, 31-34
kernel, 154
programming examples, introduction
to, 12-13
UNIX Systems for Modern Architectures,
94
UNIX98. SeeXSH5
User and terminal identification func-
tion, 210-211
Variable weight processes, 1
void*
creating and using threads, 36
definition of, 311
thread startup, 42
thread termination, 43
Index
381
W
waitpid (function), 15, 19
WNOHANG (flag), 15
Word tearing, 95
Work crews. 105-120, 270-283
Work queue. See Work crews
X Windows. 28
X/Open. Sct'XSH5
X/Open CAE Specification, System Inter-
faces and Headers, Issue 5. See
XSH5
XSH5
cancellation points, 148, 355-356
concurrency level, 351-353
mutex error detection and reporting,
311
mutex types, 349-351
parallel I/O, 354-355
POSIX options for, 348-349
stack guard size, 353-354
