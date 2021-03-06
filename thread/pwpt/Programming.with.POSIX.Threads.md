# 5 Advanced threaded programming
"Take some more tea," the March Hare said to Alice, very earnestly.
"I've had nothing yet," Alice replied in an offended tone:
"so I ca'n't take more."
"You mean you ca'n't take less,"said the Hatter:
"it's very easy to take more than nothing."
-Lewis Carroll, Alice's Adventures in Wonderland
The Pthreads standard provides many capabilities that aren't needed by many
programs. To keep the sections dealing with synchronization and threads rela-
tively simple, the more advanced capabilities are collected into this additional
section.
Section 5.1 describes a facility to manage initialization of data, particularly
within a library, in a multithreaded environment.
Section 5.2 describes "attributes objects," a way to control various character-
istics of your threads, mutexes, and condition variables when you create them.
Section 5.3 describes cancellation, a way to ask your threads to "go away"
when you don't need them to continue.
Section 5.4 describes thread-specific data, a sort of database mechanism that
allows a library to associate data with individual threads that it encounters and
to retrieve that data later.
Section 5.5 describes the Pthreads facilities for realtime scheduling, to help
your program interact with the cold, cruel world in a predictable way.
## 5.1 One-time initialization
'"Tis the voice of the Jubjub!" he suddenly cried.
(This man, that they used to call "Dunce.")
"As the Bellman would tell you," he added with pride,
"I have uttered that sentiment once."
-Lewis Carroll, The Hunting of the Snark
pthread\_once\_t once\_control = PTHREAD\_ONCE\_INIT;
int pthread\_once (pthread\_once\_t *once\_control,
void (*init\_routine) (void));
131
132 CHAPTER 5 Advanced threaded programming
Some things need to be done once and only once, no matter what. When you
are Initializing an application, it is often easiest to do all that from main, before
calling anything else that might depend on the initialization-and, in particular,
before creating any threads that might depend on having initialized mutexes, cre-
ated thread-specific data keys, and so forth.
If you are writing a library, you usually don't have that luxury. But you must
still be sure that the necessary initialization has been completed before you can
use anything that needs to be initialized. Statically initialized mutexes can help a
lot, but sometimes you may find this "one-time initialization" feature more
convenient.
In traditional sequential programming, one-time initialization is often man-
aged by a boolean variable. A control variable is statically initialized to 0, and any
code that depends on the initialization can test the variable. If the value is still 0
it can perform the initialization and then set the variable to 1. Later checks will
skip the initialization.
When you are using multiple threads, it is not that easy. If more than one
thread executes the initialization sequence concurrently, two threads may both
find initializer to be 0, and both perform the initialization, which, presumably,
should have been performed only once. The state of initialization is a shared
invariant that must be protected by a mutex.
You can code your own one-time initialization using a boolean variable and a
statically initialized mutex. In many cases this will be more convenient than
pthread\_once, and it will always be more efficient. The main reason for pthread\_
once is that you were not originally allowed to statically initialize a mutex. Thus,
to use a mutex, you had to first call pthread\_mutex\_init. You must initialize a
mutex only once, so the initialization call must be made in one-time initialization
code. The pthread\_once function solved this recursive problem. When static ini-
tialization of mutexes was added to the standard, pthread\_once was retained as
a convenience function. If it's convenient, use it, but remember that you don't
have to use it.
First, you declare a control variable of type pthread\_once\_t. The control vari-
able must be statically initialized using the PTHREAD\_ONCE\_init macro, as shown
in the following program, called once.c. You must also create a function contain-
ing the code to perform all initialization that is to be associated with the control
variable. Now, at any time, a thread may call pthread\_once, specifying a pointer
to the control variable and a pointer to the associated initialization function.
The pthread\_once function first checks the control variable to determine
whether the initialization has already completed. If so, pthread\_once simply
returns. If initialization has not yet been started, pthread\_once calls the initiali-
zation function (with no arguments), and then records that initialization has been
completed. If a thread calls pthread\_once while initialization is in progress in
another thread, the calling thread will wait until that other thread completes ini-
tialization, and then return. In other words, when any call to pthread\_once
returns successfully, the caller can be certain that all states initialized by the
associated initialization function are ready to go.
One-time initialization 133
13-20 The function once\_init\_routine initializes the mutex when called-the use of
pthread\_once ensures that it will be called exactly one time.
29 The thread function thread\_routine calls pthread\_once before using mutex,
to ensure that it exists even if it had not already been created by main.
51 The main program also calls pthread\_once before using mutex, so that the
program will execute correctly regardless of when thread\_routine runs. Notice
that, while I normally stress that all shared data must be initialized before creat-
ing any thread that uses it, in this case, the only critical shared data is really the
once\_block-it is irrelevant that the mutex is not initialized, because the use of
pthread\_once ensures proper synchronization.
| once.c
1 #include \<pthread.h\>
2 #include "errors.h"
3
4 pthread\_once\_t once\_block = PTHREAD\_ONCE\_INIT;
5 pthread\_mutex\_t mutex;
6
7 /*
8 * This is the one-time initialization routine. It will be
9 * called exactly once, no matter how many calls to pthread\_once
10 * with the same control structure are made during the course of
11 * the program.
12 */
13 void once\_init\_routine (void)
14 {
15 int status;
16
17 status = pthread\_mutex\_init (Smutex, NULL);
18 if (status != 0)
19 err\_abort (status, "Init Mutex");
20 }
21
22 /*
23 * Thread start routine that calls pthread\_once.
24 */
25 void *thread\_routine (void *arg)
26 {
27 int status;
28
29 status = pthread\_once (&once\_block, once\_init\_routine);
30 if (status != 0)
31 err\_abort (status, "Once init");
32 status = pthread\_mutex\_lock (Smutex);
33 if (status != 0)
34 err\_abort (status, "Lock mutex");
35 printf ("thread\_routine has locked the mutex.\n");
134 CHAPTER 5 Advanced threaded programming
36 status = pthread\_mutex\_unlock (Smutex);
37 if (status != 0)
38 err\_abort (status, "Unlock mutex");
39 return NULL;
40 }
41
42 int main (int argc, char *argv[])
43 {
44 pthread\_t thread\_id;
45 char *input, buffer[64];
46 int status;
47
48 status = pthread\_create (&thread\_id, NULL, thread\_routine, NULL);
49 if (status != 0)
50 err\_abort (status, "Create thread");
51 status = pthread\_once (&once\_block, once\_init\_routine);
52 if (status != 0)
53 err\_abort (status, "Once init");
54 status = pthread\_mutex\_lock (Smutex);
55 if (status != 0)
56 err\_abort (status, "Lock mutex");
57 printf ("Main has locked the mutex.\n");
58 status = pthread\_mutex\_unlock (Smutex);
59 if (status != 0)
60 err\_abort (status, "Unlock mutex");
61 status = pthread\_join (thread\_id, NULL);
62 if (status != 0)
63 err\_abort (status, "Join thread");
64 return 0;
65 }
## 5.2 Attributes objects
The fifth is ambition. It next will be right
To describe each particular batch:
Distinguishing those that have feathers, and bite,
From those that have whiskers, and scratch.
-Lewis Carroll, The Hunting of the Snark
So far, when we created threads, or dynamically initialized mutexes and con-
dition variables, we have usually used the pointer value NULL as the second
argument. That argument is actually a pointer to an attributes object. The value
Attributes objects 135
NULL indicates that Pthreads should assume the default value for all attributes-
just as it does when statically initializing a mutex or condition variable.
An attributes object is an extended argument list provided when you initialize
an object. It allows the main interfaces (for example, pthread\_create) to be rela-
tively simple, while allowing "expert" capability when you need it. Later POSIX
standards will be able to add options without requiring source changes to existing
code. In addition to standard attributes provided by Pthreads, an implementation
can provide specialized options without creating nonstandard parameters.
You can think of an attributes object as a private structure. You read or write
the "members" of the structure by calling special functions, rather than by
accessing public member names. For example, you read the stacksize attribute
from a thread attributes object by calling pthread\_attr\_getstacksize, or write
it by calling pthread\_attr\_setstacksize.
In a simple implementation of Pthreads the type pthread\_attr\_t might be a
typedef struct and the get and set functions might be macros to read or write
members of the variable. Another implementation might allocate memory when
you initialize an attributes object, and it may implement the get and set opera-
tions as real functions that perform validity checking.
Threads, mutexes, and condition variables each have their own special
attributes object type. Respectively, the types are pthread\_attr\_t, pthread\_
mutexattr\_t, and pthread\_condattr\_t.
### 5.2.1 Mutex attributes
```c
pthread\_mutexattr\_t attr;
int pthread\_mutexattr\_init (pthread\_mutexattr\_t *attr);
int pthread\_mutexattr\_destroy (
pthread\_mutexattr\_t *attr);
#ifdef \_POSIX\_THREAD\_PROCESS\_SHARED
int pthread\_mutexattr\_getpshared (
pthread\_mutexattr\_t *attr, int *pshared);
int pthread\_mutexattr\_setpshared (
pthread\_mutexattr\_t *attr, int pshared);
iendif
```
Pthreads defines the following attributes for mutex creation: pshared, protocol,
and prioceiltng. No system is required to implement any of these attributes, how-
ever, so check the system documentation before using them.
You initialize a mutex attributes object by calling pthread\_mutexattr\_init,
specifying a pointer to a variable of type pthread\_mutexattr\_t, as in mutex\_
attr.c, shown next. You use that attributes object by passing its address to
pthread\_mutex\_init instead of the NULL value we've been using so far.
136 CHAPTER 5 Advanced threaded programming
If your system provides the \_posix\_thread\_process\_shared option, then it
supports the pshared attribute, which you can set by calling the function
pthread\_mutexattr\_setpshared. If you set the pshared attribute to the value
PTHREAD\_PROCESS\_SHARED, you can use the mutex to synchronize threads within
separate processes that have access to the memory where the mutex (pthread\_
mutex\_t) is initialized. The default value for this attribute is pthread\_process\_
PRIVATE.
The mutex\_attr. c program shows how to set a mutex attributes object to cre-
ate a mutex using the pshared attribute. This example uses the default value.
pthread\_process\_private, to avoid the additional complexity of creating shared
memory and forking a process. The other mutex attributes, protocol and prioceil-
ing, will be discussed later in Section 5.5.5.
| mutex\_attr.c
1 #include \<pthread.h\>
2 linclude "errors.h"
3
4 pthread\_mutex\_t mutex;
5
6 int main (int argc, char *argv[])
7 {
8 pthread\_mutexattr\_t mutex\_attr;
9 int status;
10
11 status = pthread\_mutexattr\_init (&mutex\_attr);
12 if (status != 0)
13 err\_abort (status, "Create attr");
14 tifdef \_POSIX\_THREAD\_PROCESS\_SHARED
15 status = pthread\_mutexattr\_setpshared (
16 &mutex\_attr, PTHREAD\_PROCESS\_PRIVATE);
17 if (status != 0)
18 err\_abort (status, "Set pshared");
19 #endif
20 status = pthread\_mutex\_init (Smutex, &mutex\_attr);
21 if (status != 0)
22 err\_abort (status, "Init mutex");
23 return 0;
24 }
| mutex attr.c
Attributes objects 137
### 5.2.2 Condition variable attributes
```c
pthread\_condattr\_t attr;
int pthread\_condattr\_init (pthread\_condattr\_t *attr);
int pthread\_condattr\_destroy (
pthread\_condattr\_t *attr);
#ifdef \_POSIX\_THREAD\_PROCESS\_SHARED
int pthread\_condattr\_getpshared (
pthread\_condattr\_t *attr, int *pshared);
int pthread\_condattr\_setpshared (
pthread\_condattr\_t *attr, int pshared);
#endif
```
Pthreads defines only one attribute for condition variable creation, pshared.
No system is required to implement this attribute, so check the system documen-
tation before using it. You initialize a condition variable attributes object using
pthread\_condattr\_init, specifying a pointer to a variable of type pthread\_
condattr\_t, as in cond\_attr.c, shown next. You use that attributes object by
passing its address to pthread\_cond\_init instead of the null value we've been
using so far.
If your system defines \_posix\_thread\_process\_shared then it supports the
pshared attribute. You set the pshared attribute by calling the function pthread\_
condattr\_setpshared. If you set the pshared attribute to the value PTHREAD\_
process\_shared, the condition variable can be used by threads in separate
processes that have access to the memory where the condition variable (pthread\_
cond\_t) is initialized. The default value for this attribute is pthread\_process\_
PRIVATE.
The cond\_attr.c program shows how to set a condition variable attributes
object to create a condition variable using the pshared attribute. This example
uses the default value, pthread\_process\_private, to avoid the additional com-
plexity of creating shared memory and forking a process.
| cond\_attr. c
1 #include \<pthread.h\>
2 #include "errors.h"
3
4 pthread\_cond\_t cond;
5
6 int main (int argc, char *argv[])
7 {
8 pthread\_condattr\_t cond\_attr;
9 int status;
10
138
CHAPTER 5 Advanced threaded programming
11 status = pthread\_condattr\_init (&cond\_attr);
12 if (status != 0)
13 err\_abort (status, "Create attr");
14 lifdef \_POSIX\_THREAD\_PROCESS\_SHARED
15 status = pthread\_condattr\_setpshared (
16 &cond\_attr, PTHREAD\_PROCESS\_PRIVATE);
17 if (status != 0)
18 err\_abort (status, "Set pshared");
19 #endif
20 status = pthread\_cond\_init (Scond, &cond\_attr);
21 if (status != 0)
22 err\_abort (status, "Init cond");
23 return 0;
24 }
| cond\_attr.c
To make use of a pthread\_process\_shared condition variable, you must also
use a pthread\_PROCESS\_shared mutex. That's because two threads that synchro-
nize using a condition variable must also use the same mutex. Waiting for a
condition variable automatically unlocks, and then locks, the associated mutex.
So if the mutex isn't also created with pthread\_process\_SHARED, the synchroni-
zation won't work.
### 5.2.3 Thread attributes
```c
pthread\_attr\_t attr;
int pthread\_attr\_init (pthread\_attr\_t *attr);
int pthread\_attr\_destroy (pthread\_attr\_t *attr);
int pthread\_attr\_getdetachstate (
pthread\_attr\_t *attr, int *detachstate);
int pthread\_attr\_setdetachstate (
pthread\_attr\_t *attr, int detachstate);
#ifdef \_POSIX\_THREAD\_ATTR\_STACKSIZE
int pthread\_attr\_getstacksize (
pthread\_attr\_t *attr, size\_t *stacksize);
int pthread\_attr\_setstacksize (
pthread\_attr\_t *attr, size\_t stacksize);
#endif
#ifdef \_POSIX\_THREAD\_ATTR\_STACKADDR
int pthread\_attr\_getstackaddr (
pthread\_attr\_t *attr, void *stackaddr);
int pthread\_attr\_setstackaddr (
pthread\_attr\_t *attr, void **stackaddr);
#endif
```
Attributes objects 139
POSIX defines the following attributes for thread creation: detachstate, stack-
size, stackaddr, scope, inheritsched, schedpolicy, and schedparam. Some systems
won't support all of these attributes, so you need to check the system documen-
tation before using them. You initialize a thread attributes object using pthread\_
attr\_init, specifying a pointer to a variable of type pthread\_attr\_t, as in the
program thread\_attr.c, shown later. You use the attributes object you've cre-
ated by passing its address as the second argument to pthread\_create instead of
the null value we've been using so far.
All Pthreads systems support the detachstate attribute. The value of this
attribute can be either pthread\_create\_joinable or pthread\_create\_detached.
By default, threads are created joinable, which means that the thread identifica-
tion created by pthread\_create can be used to join with the thread and retrieve
its return value, or to cancel it. If you set the detachstate attribute to pthread\_
create\_detached, the identification of threads created using that attributes
object can't be used. It also means that when the thread terminates, any
resources it used can immediately be reclaimed by the system.
When you create threads that you know you won't need to cancel, or join with,
you should create them detached. Remember that, in many cases, even if you want
to know when a thread terminates, or receive some return value from it, you may
not need to use pthread\_join. If you provide your own notification mechanism,
for example, using a condition variable, you can still create your threads detached.
I Setting the size of a stack is not very portable.
If your system defines the symbol \_posix\_thread\_attr\_Stacksize, then you
can set the stacksize attribute to specify the minimum size for the stack of a
thread created using the attributes object. Most systems will support this option,
but you should use it with caution because stack size isn't portable. The amount
of stack space you'll need depends on the calling standards and data formats
used by each system.
Pthreads defines the symbol pthread\_stack\_min as the minimum stack size
required for a thread: If you really need to specify a stack size, you might be best
off calculating your requirements in terms of the minimum required by the imple-
mentation. Or, you could base your requirements on the default stacksize
attribute selected by the implementation-for example, twice the default, or half
the default. The program thread\_attr. c shows how to read the default stacksize
attribute value of an initialized attribute by calling pthread\_attr\_getstacksize.
I Setting the address of a stack is less portable!
If your system defines the symbol \_posix\_thread\_attr\_stackaddr, then you
can set the stackaddr attribute to specify a region of memory to be used as a
stack by any thread created using this attributes object. The stack must be at
least as large as pthread\_stack\_min. You may need to specify an area of memory
with an address that's aligned to some required granularity. On a machine where
the stack grows downward from higher addresses to lower addresses, the address
140 CHAPTER 5 Advanced threaded programming
you specify should be the highest address in the stack, not the lowest. If the stack
grows up, you need to specify the lowest address.
You also need to be aware of whether the machine increments (or decrements)
the stack before or after writing a new value-this determines whether the
address you specify should be "inside" or "outside" the stack you've allocated. The
system can't tell whether you allocated enough space, or specified the right
address, so it has to trust you. If you get it wrong, undesirable things will occur.
Use the stackaddr attribute only with great caution, and beware that it may
well be the least portable aspect of Pthreads. While a reasonable value for the
stacksize attribute will probably work on a wide range of machines, it is little
more than a wild coincidence if any particular value of the stackaddr attribute
works on any two machines. Also, you must remember that you can create only
one thread with any value of the stackaddr attribute. If you create two concurrent
threads with the same stackaddr attribute value, the threads will run on the
same stack. (That would be bad.)
The thread\_attr.c program that follows shows some of these attributes in
action, with proper conditionalization to avoid using the stacksize attribute if it is
not supported by your system. If stacksize is supported (and it will be on most
UNIX systems), the program will print the default and minimum stack size, and
set stacksize to a value twice the minimum. The code also creates the thread
detached, which means no thread can join with it to determine when it com-
pletes. Instead, main exits by calling pthread\_exit, which means that the
process will terminate when the last thread exits.
This example does not include the priority scheduling attributes, which are
discussed (and demonstrated) in Section 5.5.2. It also does not demonstrate use
of the stackaddr attribute-as I said, there is no way to use stackaddr in any
remotely portable way and, although I have mentioned it for completeness, I
strongly discourage use of stackaddr in any program.
| thread\_attr.c
1 #include \<limits.h\>
2 #include \<pthread.h\>
3 #include "errors.h"
4
5 /*
6 * Thread start routine that reports it ran, and then exits.
7 */
8 void *thread\_routine (void *arg)
9 {
10 printf ("The thread is here\n");
11 return NULL;
12 }
13
14 int main (int argc, char *argv[])
Attributes objects 141
15 {
16 pthread\_t thread\_id;
17 pthread\_attr\_t thread\_attr;
18 struct sched\_param thread\_param;
19 size\_t stack\_size;
20 int status;
21
22 status = pthread\_attr\_init (&thread\_attr);
23 if (status != 0)
24 err\_abort (status, "Create attr");
25
26 /*
27 * Create a detached thread.
28 */
29 status = pthread\_attr\_setdetachstate (
30 &thread\_attr, PTHREAD\_CREATE\_DETACHED);
31 if (status != 0)
32 err\_abort (status, "Set detach");
33 #ifdef \_POSIX\_THREAD\_ATTR\_STACKSIZE
34 /*
35 * If supported, determine the default stack size and report
36 * it, and then select a stack size for the new thread.
37 *
38 * Note that the standard does not specify the default stack
39 * size, and the default value in an attributes object need
40 * not be the size that will actually be used. Solaris 2.5
41 * uses a value of 0 to indicate the default.
42 */
43 status = pthread\_attr\_getstacksize (&thread\_attr, &stack\_size);
44 if (status != 0)
45 err\_abort (status, "Get stack size");
46 printf ("Default stack size is %u; minimum is %u\n",
47 stack\_size, PTHREAD\_STACK\_MIN);
48 status = pthread\_attr\_setstacksize (
49 &thread\_attr, PTHREAD\_STACK\_MIN*2);
50 if (status != 0)
51 err\_abort (status, "Set stack size");
52 #endif
53 status = pthread\_create (
54 &thread\_id, &thread\_attr, thread\_routine, NULL);
55 if (status != 0)
56 err\_abort (status, "Create thread");
57 printf ("Main exiting\n");
58 pthread\_exit (NULL);
59 return 0;
60 }
| thread attr.c
142
CHAPTER 5 Advanced threaded programming
## 5.3 Cancellation
"Now, I give you fair warning,"
shouted fhe Queen, stamping on the ground as she spoke;
"either you or your head must be off,
and that in about half no time! Take your choice!"
The Duchess took her choice, and was gone in a moment.
-Lewis Carroll, Alice's Adventures in Wonderland
```c
int pthread\_cancel (pthread\_t thread);
int pthread\_setcancelstate (int state, int *oldstate);
int pthread\_setcanceltype (int type, int *oldtype);
void pthread\_testcancel (void);
void pthread\_cleanup\_push (
void (*routine)(void *), void *arg);
void pthread\_cleanup\_pop (int execute);
```
Most of the time each thread runs independently, finishes a specific job, and
exits on its own. But sometimes a thread is created to do something that doesn't
necessarily need to be finished. The user might press a CANCEL button to stop a
long search operation. Or the thread might be part of a redundant algorithm and
is no longer useful because some other thread succeeded. What do you do when
you just want a thread to go away? That's what the Pthreads cancellation inter-
faces are for.
Cancelling a thread is a lot like telling a human to stop something they're
doing. Say that one of the bailing programmers has become maniacally obsessed
with reaching land, and refuses to stop rowing until reaching safety (Figure 5.1).
When the boat finally runs up onto the beach, he's become so fixated that he fails
to realize he's done. The other programmers must roughly shake him, and forcibly
remove the oars from his blistered hands to stop him-but clearly he must be
stopped. That's cancellation. Sort of. I can think of other analogies for cancellation
within the bailing programmer story, but I choose to ignore them. Perhaps you
can, too.
Cancellation allows you to tell a thread to shut itself down. You don't need it
often, but it can sometimes be extremely useful. Cancellation isn't an arbitrary
external termination. It is more like a polite (though not necessarily "friendly")
request. You're most likely to want to cancel a thread when you've found that
something you set it off to accomplish is no longer necessary. You should never
use cancellation unless you really want the target thread to go away. It is a termi-
nation mechanism, not a communication channel. So, why would you want to do
that to a thread that you presumably created for some reason?
An application might use threads to perform long-running operations, per-
haps in the background, while the user continues working. Such operations
Cancellation 143
; ,,(*
J ' ' -V-
| ^\ ? V
fa
\
|*| |
'"*
}. /
x
V: '^ -
//], ,'
/' ) *^"~~
V-
?/\<&*
/
/
- .v
-
FIGURE 5.1 Thread cancellation analogy
might include saving a large document, preparing to print a document, or sorting
a large list. Most such interfaces probably will need to have some way for the user
to cancel an operation, whether it is pressing the ESC key or Ctrl-C, or clicking a
stop sign icon on the screen. The thread receiving the user interface cancel
request would then determine that one or more background operations were in
progress, and use pthread\_cancel to cancel the appropriate threads.
Often, threads are deployed to "explore" a data set in parallel for some heuris-
tic solution. For example, solving an equation for a local minimum or maximum.
Once you've gotten one answer that's good enough, the remaining threads may
no longer be needed. If so, you can cancel them to avoid wasting processor time
and get on to other work.
Pthreads allows each thread to control its own termination. It can restore pro-
gram invariants and unlock mutexes. It can even defer cancellation while it
completes some important operation. For example, when two write operations
must both complete if either completes, a cancellation between the two is not
acceptable.
Pthreads supports three cancellation modes, described in Table 5.1, which
are encoded as two binarv values called "cancellation state" and "cancellation
144
CHAPTER 5 Advanced threaded programming
Mode State Type Meaning
Off
Deferred
Asynchronous
disabled
enabled
enabled
may be either
deferred
asynchronous
Cancellation remains pending
until enabled.
Cancellation occurs at next
cancellation point.
Cancellation may be processed at
any time.
TABLE 5.1 Cancellation states
type." Each essentially can be on or off. (While that technically gives four modes,
one of them is redundant.) As shown in the table, cancellation state is said to be
enabled or disabled, and cancellation type is said to be deferred or asynchronous.
By default, cancellation is deferred, and can occur only at specific points in
the program that check whether the thread has been requested to terminate,
called cancellation points. Most functions that can wait for an unbounded time
should be deferred cancellation points. Deferred cancellation points include wait-
ing on a condition variable, reading or writing a file, and other functions where
the thread may be blocked for a substantial period of time. There is also a special
function called pthread\_testcancel that is nothing but a deferred cancellation
point. It will return immediately if the thread hasn't been asked to terminate,
which allows you to turn any of your functions into cancellation points.
Some systems provide a function to terminate a thread immediately. Although
that sounds useful, it is difficult to use such a function safely. In fact, it is nearly
impossible in a normal modular programming environment. If a thread is termi-
nated with a mutex locked, for example, the next thread trying to lock that mutex
will be stuck waiting forever.
It might seem that the thread system could automatically release the mutex;
but most of the time that's no help. Threads lock mutexes because they're modi-
fying shared data. No other thread can know what data has been modified or
what the thread was trying to change, which makes it difficult to fix the data. Now
the program is broken. When the mutex is left locked, you can usually tell that
something's broken because one or more threads will hang waiting for the mutex.
The only way to recover from terminating a thread with a locked mutex is for
the application to be able to analyze all shared data and repair it to achieve a con-
sistent and correct state. That is not impossible, and it is worth substantial effort
when an application must be fail-safe. However, it is generally not practical for
anything but an embedded system where the application designers control every
bit of shared state in the process. You would have to rebuild not only your own
program or library state, but also the state affected by any library functions that
might be called by the thread (for example, the ANSI C library).
To cancel a thread, you need the thread's identifier, the pthread\_t value
returned to the creator by pthread\_create or returned to the thread itself by
pthread\_self. Cancelling a thread is asynchronous-that is, when the call to
Cancellation 145
pthread\_cancel returns, the thread has not necessarily been canceled, it may
have only been notified that a cancel request is pending against it. If you need to
know when the thread has actually terminated, you must join with it by calling
pthread\_join after cancelling it.
If the thread had asynchronous cancelability type set, or when the thread next
reaches a deferred cancellation point, the cancel request will be delivered by the
system. When that happens, the system will set the thread's cancelability type to
pthread\_cancel\_deferred and the cancelability state to pthread\_cancel\_disable.
That is, the thread can clean up and terminate without having to worry about
being canceled again.
When a function that is a cancellation point detects a pending cancel request,
the function does not return to the caller. The active cleanup handlers will be
called, if there are any, and the thread will terminate. There is no way to "handle"
cancellation and continue execution-the thread must either defer cancellation
entirely or terminate. This is analogous to C++ object destructors, rather than
C++ exceptions-the object is allowed to clean up after itself, but it is not allowed
to avoid destruction.
The following program, called cancel.c, shows how to write a thread that
responds "reasonably quickly" to deferred cancellation, by calling pthread\_
testcancel within a loop.
11-19 The thread function thread\_routine loops indefinitely, until canceled, testing
periodically for a pending cancellation request. It minimizes the overhead of call-
ing pthread\_testcancel by doing so only every 1000 iterations (line 17).
27-35 On a Solaris system, set the thread concurrency level to 2, by calling thr\_
setconcurrency. Without the call to thr\_setconcurrency, this program will
hang on Solaris because thread\_routine is "compute bound" and will not block.
The main program would never have another chance to run once thread\_routine
started, and could not call pthread\_cancel.
36-54 The main program creates a thread running thread\_routine, sleeps for two
seconds, and then cancels the thread. It joins with the thread, and checks the
return value, which should be pthread\_canceled to indicate that it was can-
celed, rather than terminated normally.
| cancel.c
1 #include \<pthread.h\>
2 #include "errors.h"
3
4 static int counter;
5
6 /*
7 * Loop until canceled. The thread can be canceled only
8 * when it calls pthread\_testcancel, which it does each 1000
9 * iterations.
10 */
146 CHAPTER 5 Advanced threaded programming
11 void *thread\_routine (void *arg)
12 {
13 DPRINTF (("thread\_routine starting\n"));
14 for (counter =0; ; counter++)
15 if ((counter % 1000) == 0) {
16 DPRINTF (("calling testcancel\n"));
17 pthread\_testcancel ( );
18 }
19 }
20
21 int main (int argc, char *argv[])
22 {
23 pthread\_t thread\_id;
24 void *result;
25 int status;
26
27 #ifdef sun
28 /*
29 * On Solaris 2.5, threads are not timesliced. To ensure
30 * that our two threads can run concurrently, we need to
31 * increase the concurrency level to 2.
32 */
33 DPRINTF (("Setting concurrency level to 2\n"));
34 thr\_setconcurrency B);
35 #endif
36 status = pthread\_create (
37 &thread\_id, NULL, thread\_routine, NULL);
38 if (status != 0)
39 err\_abort (status, "Create thread");
40 sleep B);
41
42 DPRINTF (("calling cancelNn"));
43 status = pthread\_cancel (thread\_id);
44 if (status != 0)
45 err\_abort (status, "Cancel thread");
46
47 DPRINTF (("calling join\n"));
48 status = pthread\_join (thread\_id, Sresult);
49 if (status != 0)
50 err\_abort (status, "Join thread");
51 if (result == PTHREAD\_CANCELED)
52 printf ("Thread canceled at iteration %d\n", counter);
53 else
54 printf ("Thread was not canceled\n");
55 return 0;
56 }
| cancel.c
Cancellation 147
A thread can disable cancellation around sections of code that need to com-
plete without interruption, by calling pthread\_setcancelstate. For example, if a
database update operation takes two separate write calls, you wouldn't want to
complete the first and have the second canceled. If you request that a thread be
canceled while cancellation is disabled, the thread remembers that it was can-
celed but won't do anything about it until after cancellation is enabled again.
Because enabling cancellation isn't a cancellation point, you also need to test for a
pending cancel request if you want a cancel processed immediately.
When a thread may be canceled while it holds private resources, such as a
locked mutex or heap storage that won't ever be freed by any other thread, those
resources need to be released when the thread is canceled. If the thread has a
mutex locked, it may also need to "repair" shared data to restore program invari-
ants. Cleanup handlers provide the mechanism to accomplish the cleanup,
somewhat like process atexit handlers. After acquiring a resource, and before
any cancellation points, declare a cleanup handler by calling pthread\_cleanup\_
push. Before releasing the resource, but after any cancellation points, remove the
cleanup handler by calling pthread\_cleanup\_pop.
If you don't have a thread's identifier, you can't cancel the thread. That means
that, at least using portable POSIX functions, you can't write an "idle thread
killer" that will arbitrarily terminate threads in the process. You can only cancel
threads that you created, or threads for which the creator (or the thread itself)
gave you an identifier. That generally means that cancellation is restricted to
operating within a subsystem.
### 5.3.1 Deferred cancelability
"Deferred cancelability" means that the thread's cancelability type has been set
to pthread\_cancel\_deferred and the thread's cancelability enable has been set to
pthread\_cancel\_enable. The thread will only respond to cancellation requests
when it reaches one of a set of "cancellation points."
The following functions are always cancellation points on any Pthreads
system:
pthread\_cond\_wait
pthread\_cond\_timedwait
pthread\_join
pthread\_testcancel
sigwait
aio\_suspend
close
creat
fcntl (F\_SETLCKW) \_
The following list of functions may be cancellation points. You should write
your code so that it will function correctly if any of these are cancellation points
fsync
mq receive
mq send
msync
nanosleep
open
pause
read
sem wait
sigwaitinfo
sigsuspend
sigtimedwait
sleep
system
tcdrain
wait
waitpid
write
148
CHAPTER 5 Advanced threaded programming
and also so that it will not break if any of them are not. If you depend upon any
particular behavior, you may limit the portability of your code. You'll have to look
at the conformance documentation to find out which, if any, are cancellation
points for the system you are using:
closedir
ctermid
fclose
fcntl (except F\_SETLCKW)
fflush
fgetc
fgets
fopen
fprintf
fputc
fputs
fread
getc\_unlocked
getchar
getchar\_unlocked
getcwd
getgrgid
getgrgid\_r
getrtnam
getgrnam\_r
getlogin
getlogin\_r
getpwnam
getpwnam r
printf
putc
putc\_unlocked
putchar
putchar\_unlocked
puts
readdir
remove
rename
rewind
rewinddir
scanf
tmpfile
tmpname
ttyname
ttyname\_r
ungetc
freopen getpwuid
fscanf getpwuid\_r
fseek gets
ftell lseek
fwrite opendir
getc perror
Pthreads specifies that any ANSI C or POSIX function not specified in one of
the two lists cannot be a cancellation point. However, your system probably has
many additional cancellation points. That's because few UNIX systems are
"POSIX." That is, they support other programming interfaces as well-such as
BSD 4.3, System V Release 4, UNIX95, and so forth. POSIX doesn't recognize the
existence of functions such as select or poll, and therefore it can't say whether
or not they are cancellation points. Yet clearly both are functions that may block
for an arbitrary period of time, and programmers using them with cancellation
would reasonably expect them to behave as cancellation points. X/Open is cur-
rently addressing this problem for UNIX98 [X/Open System Interfaces, Issue 5).
by extending the Pthreads list of cancellation points.
Most cancellation points involve I/O operations that may block the thread for
an "unbounded" time. They're cancelable so that the waits can be interrupted.
When a thread reaches a cancellation point the system determines whether a
cancel is pending for the current ("target") thread. A cancel will be pending if
another thread has called pthread\_cancel for the target thread since the last
time the target thread returned from a cancellation point. If a cancel is pending,
the system will immediately begin calling cleanup functions, and then the thread
will terminate.
If no cancel is currently pending, the function will proceed. If another thread
requests that the thread be canceled while the thread is waiting for something
(such as I/O) then the wait will be interrupted and the thread will begin its can-
cellation cleanup.
Cancellation 149
If you need to ensure that cancellation can't occur at a particular cancellation
point, or during some sequence of cancellation points, you can temporarily dis-
able cancellation in that region of code. The following program, called cancel\_
disable.c, is a variant of cancel.c. The "target" thread periodically calls sleep,
and does not want the call to be cancelable.
23-32 After each cycle of 755 iterations, thread\_routine will call sleep to wait a
second. (The value 755 is just an arbitrary number that popped into my head. Do
arbitrary numbers ever pop into your head?) Prior to sleeping, thread\_routine
disables cancellation by setting the cancelability state to pthread\_cancel\_
disable. After sleep returns, it restores the saved cancelability state by calling
pthread\_setcancelstate again.
33-35 Just as in cancel. c, test for a pending cancel every 1000 iterations.
| cancel\_disable.c
1 #include \<pthread.h\>
2 #include "errors.h"
3
4 static int counter;
5
6 /*
7 * Thread start routine.
8 */
9 void *thread\_routine (void *arg)
10 {
11 int state;
12 int status;
13
14 for (counter =0; ; counter++) {
15
16 /*
17 * Each 755 iterations, disable cancellation and sleep
18 * for one second.
19 *
20 * Each 1000 iterations, test for a pending cancel by
21 * calling pthread\_testcancel().
22 */
23 if ((counter % 755) == 0) {
24 status = pthread\_setcancelstate (
25 PTHREAD\_CANCEL\_DISABLE, {.State);
26 if (status != 0)
27 err\_abort (status, "Disable cancel");
28 sleep A);
29 status = pthread\_setcancelstate (
30 state, Sstate);
31 if (status != 0)
32 err abort (status, "Restore cancel");
150 CHAPTER 5 Advanced threaded programming
33 } else
34 if ((counter % 1000) == 0)
35 pthread\_testcancel ();
36 }
37 }
38
39 int main (int argc, char *argv[])
40 {
41 pthread\_t thread\_id;
42 void *result;
43 int status;
44
45 status = pthread\_create (
46 &thread\_id, NULL, thread\_routine, NULL);
47 if (status != 0)
48 err\_abort (status, "Create thread");
49 sleep B);
50 status = pthread\_cancel (thread\_id);
51 if (status != 0)
52 err\_abort (status, "Cancel thread");
53
54 status = pthread\_join (thread\_id, Sresult);
55 if (status != 0)
56 err\_abort (status, "Join thread");
57 if (result == PTHREAD\_CANCELED)
58 printf ("Thread canceled at iteration %d\n", counter);
59 else
60 printf ("Thread was not canceled\n");
61 return 0;
62 }
| cancel disable.c
### 5.3.2 Asynchronous cancelability
Asynchronous cancellation is useful because the "target thread" doesn't need
to poll for cancellation requests by using cancellation points. That can be valu-
able for a thread that runs a tight compute-bound loop (for example, searching
for a prime number factor) where the overhead of calling pthread\_testcancel
might be severe.
I Avoid asynchronous cancellation!
It is difficult to use correctly and is rarely useful.
The problem is that you're limited in what you can do with asynchronous can-
cellation enabled. You can't acquire any resources, for example, including locking
a mutex. That's because the cleanup code would have no way to determine
Cancellation 151
whether the mutex had been locked. Asynchronous cancellation can occur at any
hardware instruction. On some computers it may even be possible to interrupt
some instructions in the middle. That makes it really difficult to determine what
the canceled thread was doing.
For example, when you call ma Hoc the system allocates some heap memory
for you, stores a pointer to that memory somewhere (possibly in a hardware regis-
ter), and then returns to your code, which probably moves the return value into
some local storage for later use. There are lots of places that malloc might be
interrupted by an asynchronous cancel, with varying effects. It might be inter-
rupted before the memory was allocated. Or it might be interrupted after
allocating storage but before it stored the address for return. Or it might even
return to your code, but get interrupted before the return value could be copied
to a local variable. In any of those cases the variable where your code expects to
find a pointer to the allocated memory will be uninitialized. You can't tell whether
the memory really was allocated yet. You can't free the memory, so that memory
(if it was allocated to you) will remain allocated for the life of the program. That's a
memory leak, which is not a desirable feature.
Or when you call pthread\_mutex\_lock, the system might be interrupted
within a function call either before or after locking the mutex. Again, there's no
way for your program to find out, because the interrupt may have occurred
between any two instructions, even within the pthread\_mutex\_lock function,
which might leave the mutex unusable. If the mutex is locked, the application will
likely end up hanging because it will never be unlocked.
I Call no code with asynchronous cancellation enabled unless you
wrote it to be async-cancel safe-and even then,think twice!
You are not allowed to call any function that acquires resources while asyn-
chronous cancellation is enabled. In fact, you should never call any function
while asynchronous cancellation is enabled unless the function is documented as
"async-cancel safe." The only functions required to be async safe by Pthreads
are pthread\_cancel, pthread\_setcancelstate, and pthread\_setcanceltype. (And
there is no reason to call pthread\_cancel with asynchronous cancelability
enabled.) No other POSIX or ANSI C functions need be async-cancel safe, and you
should never call them with asynchronous cancelability enabled.
Pthreads suggests that all library functions should document whether or not
they are async-cancel safe. However if the description of a function does not spe-
cifically say it is async-cancel safe you should always assume that it is not. The
consequences of asynchronous cancellation in a function that is not async-
cancel safe can be severe. And worse, the effects are sensitive to timing-so a
function that appears to be async-cancel safe during experimentation may in fact
cause all sorts of problems later when it ends up being canceled in a slightly dif-
ferent place.
The following program. cancel\_async. c, shows the use of asynchronous can-
cellation in a compute-bound loop. Use of asynchronous cancellation makes this
152 CHAPTER 5 Advanced threaded programming
loop "more responsive" than the deferred cancellation loop in cancel.c. However,
the program would become unreliable if any function calls were made within the
loop, whereas the deferred cancellation version would continue to function cor-
rectly. In most cases, synchronous cancellation is preferable.
24-28 To keep the thread running awhile with something more interesting than an
empty loop, cancel\_async.c uses a simple matrix multiply nested loop. The
matrixa and matrixb arrays are initialized with, respectively, their major or minor
array index.
34-36 The cancellation type is changed to PTHREAD\_CANCEL\_ASYNCHRONOUS, allowing
asynchronous cancellation within the matrix multiply loops.
39-44 The thread repeats the matrix multiply until canceled, on each iteration
replacing the first source array (matrixa) with the result of the previous multipli-
cation (matrixc).
66-74 Once again, on a Solaris system, set the thread concurrency level to 2, allow-
ing the main thread and thread\_routine to run concurrently on a uniprocessor.
The program will hang without this step, since user mode threads are not
timesliced on Solaris.
| cancel\_async.c
1 #include \<pthread.h\>
2 #include "errors.h"
3
4 #define SIZE 10 /* array size */
5
6 static int matrixa[SIZE][SIZE] ;
7 static int matrixb[SIZE][SIZE];
8 static int matrixc[SIZE][SIZE];
9
10 /*
11 * Loop until canceled. The thread can be canceled at any
12 * point within the inner loop, where asynchronous cancellation
13 * is enabled. The loop multiplies the two matrices matrixa
14 * and matrixb.
15 */
16 void *thread\_routine (void *arg)
17 {
18 int cancel\_type, status;
19 int i, j, k, value = 1;
20
21 /*
22 * Initialize the matrices to something arbitrary.
23 */
24 for (i = 0; i \< SIZE; i++)
25 for (j = 0; j \< SIZE; j++) {
26 matrixa[i][j] = i;
27 matrixb[i][j] = j;
28 }
29
Cancellation 153
30 while A) {
31 /*
32 * Compute the matrix product of matrixa and matrixb.
33 */
34 status = pthread\_setcanceltype (
35 PTHREAD\_CANCEL\_ASYNCHRONOUS,
36 &cancel\_type);
37 if (status != 0)
38 err\_abort (status, "Set cancel type");
39 for (i = 0; i \< SIZE; i++)
40 for (j = 0; j \< SIZE; j++) {
41 matrixc[i][j] = 0;
42 for (k = 0; k \< SIZE; k++)
43 matrixc[i][j] += matrixa[i][k] * matrixb[k][j];
44 }
45 status = pthread\_setcanceltype (
46 cancel\_type,
47 &cancel\_type);
48 if (status != 0)
49 err\_abort (status, "Set cancel type");
50
51 /*
52 * Copy the result (matrixc) into matrixa to start again
53 */
54 for (i = 0; i \< SIZE; i++)
55 for (j = 0; j \< SIZE; j++)
56 matrixa[i][j] = matrixcfi][j];
57 }
58 }
59
60 int main (int argc, char *argv[])
61 {
62 pthread\_t thread\_id;
63 void *result;
64 int status;
65
66 #ifdef sun
67 /*
68 * On Solaris 2.5, threads are not timesliced. To ensure
69 * that our two threads can run concurrently, we need to
70 * increase the concurrency level to 2.
71 */
72 DPRINTF (("Setting concurrency level to 2\n"));
73 thr\_setconcurrency B);
74 #endif
75 status = pthread\_create (
76 &thread\_id, NULL, thread\_routine, NULL);
77 if (status != 0)
78 err\_abort (status, "Create thread");
79 sleep A);
154 CHAPTER 5 Advanced threaded programming
80 status = pthread\_cancel (thread\_id);
81 if (status != 0)
82 err\_abort (status, "Cancel thread");
83 status = pthread\_join (thread\_id, Sresult);
84 if (status != 0)
85 err\_abort (status, "Join thread");
86 if (result == PTHREAD\_CANCELED)
87 printf ("Thread canceled\n");
88 else
89 printf ("Thread was not canceled\n");
90 return 0;
91 }
| cancel\_async.c
I Warning: do not let "DCE threads'" habits carry over to Pthreads!
I'll end this section with a warning. DCE threads, a critical component of the
Open Software Foundation's Distributed Computing Environment, was designed
to be independent of the underlying UNIX kernel. Systems with no thread support
at all often emulated "thread synchronous" I/O in user mode, using nonblocking
I/O mode, so that a thread attempting I/O on a busy file was blocked on a condi-
tion variable until a later select or poll showed that the I/O could complete.
DCE listener threads might block indefinitely on a socket read, and it was impor-
tant to be able to cancel that read.
When DCE was ported to newer kernels that had thread support, but not
Pthreads support, the user mode I/O wrappers were usually omitted, resulting in
a thread blocked within a kernel that did not support deferred cancellation. Users
discovered that, in many cases, these systems implemented asynchronous can-
cellation in such a way that, quite by coincidence, a kernel wait might be
canceled "safely" if the thread switched to asynchronous cancellation immedi-
ately before the kernel call, and switched back to deferred cancellation
immediately after. This observation was publicized in DCE documentation, but it
is a very dangerous hack, even on systems where it seems to work. You should
never try this on any Pthreads system! If your system conforms to POSIX
1003. lc-1995 (or POSIX 1003.1, 1996 edition, or later), it supports deferred can-
cellation of, at minimum, kernel functions such as read and write. You do not
need asynchronous cancellation, and using it can be extremely dangerous.
### 5.3.3 Cleaning up
I When you write any library code, design it to handle deferred
cancellation gracefully. Disable cancellation where it is not
appropriate, and always use cleanup handlers at cancellation points.
Cancellation 155
If a section of code needs to restore some state when it is canceled, it must use
cleanup handlers. When a thread is canceled while waiting for a condition vari-
able, it will wake up with the mutex locked. Before the thread terminates it
usually needs to restore invariants, and it always needs to release the mutex.
Each thread may be considered to have a stack of active cleanup handlers.
Cleanup handlers are added to the stack by calling pthread\_cleanup\_push, and the
most recently added cleanup handler is removed by calling pthread\_cleanup\_
pop. When the thread is canceled or when it exits by calling pthread\_exit,
Pthreads calls each active cleanup handler in turn, beginning with the most
recently added cleanup handler. When all active cleanup handlers have returned,
the thread is terminated.
Pthreads cleanup handlers are designed so that you can often use the cleanup
handler even when the thread wasn't canceled. It is often useful to run the same
cleanup function regardless of whether your code is canceled or completes nor-
mally. When pthread\_cleanup\_pop is called with a nonzero value, the cleanup
handler is executed even if the thread was not canceled.
You cannot push a cleanup handler in one function and pop it in another func-
tion. The pthread\_cleanup\_push and pthread\_cleanup\_pop operations may be
defined as macros, such that pthread\_cleanup\_push contains the opening brace
"{" of a block, while pthread\_cleanup\_pop contains the matching closing brace "}"
of the block. You must always keep this restriction in mind while using cleanup
handlers, if you wish your code to be portable.
The following program, cancel\_cleanup.c, shows the use of a cleanup han-
dler to release a mutex when a condition variable wait is canceled.
10-17 The control structure (control) is used by all threads to maintain shared syn-
chronization objects and invariants. Each thread increases the member counter
by one when it starts, and decreases it at termination. The member busy is used
as a dummy condition wait predicate-it is initialized to 1, and never cleared,
which means that the condition wait loops will never terminate (in this example)
until the threads are canceled.
24-34 The function cleanup\_handler is installed as the cancellation cleanup han-
dler for each thread. It is called on normal termination as well as through
cancellation, to decrease the count of active threads and unlock the mutex.
47 The function thread\_routine establishes cleanup\_handler as the active can-
cellation cleanup handler.
54-58 Wait until the control structure's busy member is set to 0, which, in this
example, will never occur. The condition wait loop will exit only when the wait is
canceled.
60 Although the condition wait loop in this example will not exit, the function
cleans up by removing the active cleanup handler. The nonzero argument to
pthread\_cleanup\_pop, remember, means that the cleanup handler will be called
even though cancellation did not occur.
In some cases, you may omit "unreachable statements" like this pthread\_
cleanup\_pop call. However, in this case, your code might not compile without it.
The pthread\_cleanup\_push and pthread\_cleanup\_pop macros are special, and
may expand to form, respectively, the beginning and ending of a block. Digital
156 CHAPTER 5 Advanced threaded programming
UNIX does this, for example, to implement cancellation on top of the common
structured exception handling provided by the operating system.
| cancel\_cleanup.c
1 #include \<pthread.h\>
2 #include "errors.h"
3
4 #define THREADS 5
5
6 /*
7 * Control structure shared by the test threads, containing
8 * the synchronization and invariant data.
9 */
10 typedef struct control\_tag {
11 int counter, busy;
12 pthread\_mutex\_t mutex;
13 pthread\_cond\_t cv;
14 } control\_t;
15
16 control\_t control =
17 {0, 1, PTHREAD\_MUTEX\_INITIALIZER, PTHREAD\_COND\_INITIALIZER};
18
19 /*
20 * This routine is installed as the cancellation cleanup
21 * handler around the cancelable condition wait. It will
22 * be called by the system when the thread is canceled.
23 */
24 void cleanup\_handler (void *arg)
25 {
26 control\_t *st = (control\_t *)arg;
27 int status;
28
29 st-\>counter-;
30 printf ("cleanup\_handler: counter == %d\n", st-\>counter);
31 status = pthread\_mutex\_unlock (&st-\>mutex);
32 if (status != 0)
33 err\_abort (status, "Unlock in cleanup handler");
34 }
35
36 /*
37 * Multiple threads are created running this routine (controlled
38 * by the THREADS macro). They maintain a "counter" invariant,
39 * which expresses the number of running threads. They specify a
40 * nonzero value to pthread\_cleanup\_pop to run the same
41 * "finalization" action when cancellation does not occur.
42 */
43 void *thread\_routine (void *arg)
44 {
45 int status;
Cancellation 157
46
47 pthread\_cleanup\_push (cleanup\_handler, (void*)scontrol);
48
49 status = pthread\_mutex\_lock (Scontrol.mutex);
50 if (status != 0)
51 err\_abort (status, "Mutex lock");
52 control.counter++;
53
54 while (control.busy) {
55 status = pthread\_cond\_wait (Scontrol.cv, {.control.mutex);
56 if (status != 0)
57 err\_abort (status, "Wait on condition");
58 }
59
60 pthread\_cleanup\_pop A);
61 return NULL;
62 }
63
64 int main (int argc, char *argv[])
65 {
66 pthread\_t thread\_id[THREADS];
67 int count;
68 void *result;
69 int status;
70
71 for (count = 0; count \< THREADS; count++) {
72 status = pthread\_create (
73 &thread\_id[count], NULL, thread\_routine, NULL);
74 if (status != 0)
75 err abort (status, "Create thread");
76 }
77
78 sleep B);
79
80 for (count = 0; count \< THREADS; count++) {
81 status = pthread\_cancel (thread\_id[count]);
82 if (status != 0)
83 err\_abort (status, "Cancel thread");
84
85 status = pthread\_join (thread\_id[count], Sresult);
86 if (status != 0)
87 err\_abort (status, "Join thread");
88 if (result == PTHREAD\_CANCELED)
89 printf ("thread %d canceled\n", count);
90 else
91 printf ("thread %d was not canceled\n", count);
92 }
93 return 0;
94 }
| cancel\_cleanup.c
158 CHAPTER 5 Advanced threaded programming
If one of your threads creates a set of threads to "subcontract" some function,
say, a parallel arithmetic operation, and the "contractor" is canceled while the
function is in progress, you probably won't want to leave the subcontractor
threads running. Instead, you could "pass on" the cancellation to each subcon-
trator thread, letting them handle their own termination independently.
If you had originally intended to join with the subcontractors, remember that
they will continue to consume some resources until they have been joined or
detached. When the contractor thread cancels them, you should not delay can-
cellation by joining with the subcontractors. Instead, you can cancel each thread
and immediately detach it using pthread\_detach. The subcontractor resources
can then be recycled immediately as they finish, while the contractor can wrap
things up independently.
The following program, cancel\_subcontract.c, shows one way to propagate
cancellation to subcontractors.
9-12 The team\_t structure defines the state of the team of subcontractor threads.
The join\_i member records the index of the last subcontractor with which the
contractor had joined, so on cancellation from within pthread\_join, it can cancel
the threads it had not yet joined. The workers member is an array recording the
thread identifiers of the subcontractor threads.
18-25 The subcontractor threads are started running the worker\_routine function.
This function loops until canceled, calling pthread\_testcancel every 1000
iterations.
31-46 The cleanup function is established as the active cleanup handler within the
contractor thread. When the contractor is canceled, cleanup iterates through the
remaining (unjoined) subcontractors, cancelling and detaching each. Note that it
does not join the subcontractors-in general, it is not a good idea to wait in a
cleanup handler. The thread, after all, is expected to clean up and terminate, not to
wait around for something to happen. But if your cleanup handler really needs
to wait for something, don't be afraid, it will work just fine.
53-76 The contractor thread is started running thread\_routine. This function creates
a set of subcontractors, then joins with each subcontractor. As it joins each
thread, it records the current index within the workers array in the team\_t mem-
ber join\_i. The cleanup handler is established with a pointer to the team
structure so that it can determine the last offset and begin cancelling the remain-
ing subcontractors.
78-104 The main program creates the contractor thread, running thread\_routine,
and then sleeps for five seconds. When it wakes up, it cancels the contractor
thread, and waits for it to terminate.
| cancel\_subcontract.c
1 #include \<pthread.h\>
2 #include "errors.h"
3
4 #define THREADS 5
5
Cancellation
159
6
7
8
9
10
11
12
13
14
15
16
17
18
19
20
21
22
23
24
25
26
27
28
29
30
31
32
33
34
35
36
37
38
39
40
41
42
43
44
45
46
47
48
49
50
51
52
53
/*
* Structure that defines the threads in a "team.1
*/
typedef struct team\_tag {
int join\_i; /*
pthread\_t workers[THREADS]; /*
} team t;
join index */
thread identifiers
*/
They loop waiting for a
/*
* Start routine for worker threads.
* cancellation request.
*/
void *worker\_routine (void *arg)
int counter;
for (counter =0; ; counter++)
if ((counter % 1000) == 0)
pthread\_testcancel ();
\>
/*
* Cancellation cleanup handler for the contractor thread.
* will cancel and detach each worker in the team.
*/
void cleanup (void *arg)
team\_t *team = (team\_t *)arg;
int count, status;
for (count = team-\>join\_i; count \< THREADS; count++) {
status = pthread\_cancel (team-\>workers[count]);
if (status != 0)
err\_abort (status, "Cancel worker");
status = pthread\_detach (team-\>workers[count]);
if (status != 0)
err\_abort (status, "Detach worker");
printf ("Cleanup: canceled %d\n", count);
It
/*
* Thread start routine for the contractor. It creates a team of
* worker threads, and then joins with them. When canceled, the
* cleanup handler will cancel and detach the remaining threads.
*/
void *thread routine (void *arg)
160 CHAPTER 5 Advanced threaded programming
54 {
55 team\_t team; /* team info */
56 int count;
57 void *result; /* Return status */
58 int status;
59
60 for (count = 0; count \< THREADS; count++) {
61 status = pthread\_create (
62 Steam.workers[count], NULL, worker\_routine, NULL);
63 if (status != 0)
64 err\_abort (status, "Create worker");
65 }
66 pthread\_cleanup\_push (cleanup, (void*)Steam);
67
68 for (team.join\_i = 0; team.join\_i \< THREADS; team.join\_i++) {
69 status = pthread\_join (team.workers[team.join\_i], Sresult);
70 if (status != 0)
71 err\_abort (status, "Join worker");
72 }
73
74 pthread\_cleanup\_pop @);
75 return NULL;
76 }
77
78 int main (int argc, char *argv[])
79 {
80 pthread\_t thread\_id;
81 int status;
82
83 #ifdef sun
84 /*
85 * On Solaris 2.5, threads are not timesliced. To ensure
86 * that our threads can run concurrently, we need to
87 * increase the concurrency level to at least 2 plus THREADS
88 * (the number of workers).
89 */
90 DPRINTF (("Setting concurrency level to %d\n", THREADS+2));
91 thr\_setconcurrency (THREADS+2);
92 #endif
93 status = pthread\_create (&thread\_id, NULL, thread\_routine, NULL);
94 if (status != 0)
95 err\_abort (status, "Create team");
96 sleep E);
97 printf ("Cancelling...\n");
98 status = pthread\_cancel (thread\_id);
99 if (status != 0)
100 err abort (status, "Cancel team");
Thread-specific data 161
101 status = pthread\_join (thread\_id, NULL);
102 if (status != 0)
103 err\_abort (status, "Join team");
104 }
| cancel subcontract.c
## 5.4 Thread-specific data
No, I've made up my mind about it: if I'm Mabel, I'll stay down here. It'll be
no use their putting their heads down and saying "Come up again,
dear!" I shall only look up and say "Who am I, then? Tell me that first, and
then, if I like being that person, I'll come up: if not, I'll stay down here till
I'm somebody else."
-Lewis Carroll, Alice's Adventures in Wonderland
When a function in a single threaded program needs to create private data
that persists across calls to that function, the data can be allocated statically in
memory. The name's scope can be limited to the function or file that uses it
(static) or it can be made global (extern).
It is not quite that simple when you use threads. All threads within a process
share the same address space, which means that any variable declared as static
or extern, or in the process heap, may be read and written by all threads within
the process. That has several important implications for code that wants to store
"persistent" data between a series of function calls within a thread:
? The value in a static or extern variable, or in the heap, will be the value
last written by any thread. In some cases this may be what you want, for
example, to maintain the seed of a pseudorandom number sequence. In
other cases, it may not be what you want.
? The only storage a thread has that's truly "private" are processor registers.
Even stack addresses can be shared, although only if the "owner" deliber-
ately exposes an address to another thread. In any event, neither registers
nor "private" stack can replace uses of persistent static storage in non-
threaded code.
So when you need a private variable, you must first decide whether all threads
share the same value, or whether each thread should have its own value. If they
share, then you can use static or extern data, just as you could in a single
threaded program; however, you must synchronize access to the shared data
across multiple threads, usually by adding one or more mutexes.
If each thread needs its own value for a private variable, then you must store
all the values somewhere, and each thread must be able to locate the proper
value. In some cases you might be able to use static data, for example, a table
162 CHAPTER 5 Advanced threaded programming
where you can search for a value unique to each thread, such as the thread's
pthread\_t. In many interesting cases you cannot predict how many threads might
call the function-imagine you were implementing a thread-safe library that could
be called by arbitrary code, in any number of threads.
The most general solution is to malloc some heap in each thread and store the
values there, but your code will need to be able to find the proper private data in
any thread. You could create a linked list of all the private values, storing the cre-
ating thread's identifier (pthread\_t) so it could be found again, but that will be
slow if there are many threads. You need to search the list to find the proper
value, and it would be difficult to recover the storage that was allocated by termi-
nated threads-your function cannot know when a thread terminates.
I New interfaces should not rely on implicit persistent storage!
When you are designing new interfaces, there's a better solution. You should
require the caller to allocate the necessary persistent state, and tell you where it
is. There are many advantages to this model, including, most importantly:
? In many cases, you can avoid internal synchronization using this model,
and, in rare cases where the caller wishes to share the persistent state
between threads, the caller can supply the needed synchronization.
? The caller can instead choose to allocate more than one state buffer for use
within a single thread. The result is several independent sequences of calls
to your function within the same thread, with no conflict.
The problem is that you often need to support implicit persistent states. You may
be making an existing interface thread-safe, and cannot add an argument to the
functions, or require that the caller maintain a new data structure for your bene-
fit. That's where thread-specific data comes in.
Thread-specific data allows each thread to have a separate copy of a variable,
as if each thread has an array of thread-specific data values, which is indexed by
a common "key" value. Imagine that the bailing programmers are wearing their
corporate ID badges, clipped to their shirt pockets (Figure 5.2). While the infor-
mation is different for each programmer, you can find the information easily
without already knowing which programmer you're examining.
The program creates a key (sort of like posting a corporate regulation that
employee identification badges always be displayed clipped to the left breast
pocket of the employee's shirt or jacket) and each thread can then independently
set or get its own value for that key (although the badge is always on the left
pocket, each employee has a unique badge number, and, in most cases, a unique
name). The key is the same for all threads, but each thread can associate its own
independent value with that shared key. Each thread can change its private value
for a key at any time, without affecting the key or any value other threads may
have for the key.
Thread-specific data
163
FIGURE 5.2 Thread-specific data analogy
### 5.4.1 Creating thread-specific data
```c
pthread\_key\_t key;
int pthread\_key\_create (
pthread\_key\_t *key, void (*destructor)(void *));
int pthread\_key\_delete (pthread\_key\_t key);
```
A thread-specific data key is represented in your program by a variable of type
pthread\_key\_t. Like most Pthreads types, pthread\_key\_t is opaque and you
should never make any assumptions about the structure or content. The easiest
way to create a thread-specific data key is to call pthread\_key\_create before any
threads try to use the key, for example early in the program's main function.
If you need to create a thread-specific data key later, you have to ensure that
pthread\_key\_create is called only once for each pthread\_key\_t variable. That's
because if you create a key twice, you are really creating two different keys. The
second key will overwrite the first, which will be lost forever along with the values
any threads might have set for the first key.
When you can't add code to main, the easiest way to ensure that a thread-
specific data key is created only once is to use pthread\_once, the one-time initiali-
zation function, as shown in the following program, tsd\_once.c.
164 CHAPTER 5 Advanced threaded programming
7-10 The tsd\_t structure is used to contain per-thread data. Each thread allocates
a private tsd\_t structure, and stores a pointer to that structure as its value for
the thread-specific data key tsd\_key. The thread\_id member holds the thread's
identifier (pthread\_t), and the string member holds the pointer to a "name"
string for the thread. The variable tsd\_key holds the thread-specific data key
used to access the tsd\_t structures.
19-27 One-time initialization (pthread\_once) is used to ensure that the key tsd\_key
is created before the first access.
33-56 The threads begin in the thread start function thread\_routine. The argument
(arg) is a pointer to a character string naming the thread. Each thread calls
pthread\_once to ensure that the thread-specific data key has been created. The
thread then allocates a tsd\_t structure, initializes the thread\_id member with
the thread's identifier, and copies its argument to the string member.
The thread gets the current thread-specific data value by calling pthread\_
getspecif ic, and prints a message using the thread's name. It then sleeps for a
few seconds and prints another message to demonstrate that the thread-specific
data value remains the same, even though another thread has assigned a differ-
ent tsd\_t structure address to the same thread-specific data key.
| tsd\_once.c
1 tinclude \<pthread.h\>
2 #include "errors.h"
3
4 /*
5 * Structure used as the value for thread-specific data key.
6 */
7 typedef struct tsd\_tag {
8 pthread\_t thread\_id;
9 char *string;
10 } tsd\_t;
11
12 pthread\_key\_t tsd\_key; /* Thread-specific data key */
13 pthread\_once\_t key\_once = PTHREAD\_ONCE\_INIT;
14
15 /*
16 * One-time initialization routine used with the pthread\_once
17 * control block.
18 */
19 void once\_routine (void)
20 {
21 int status;
22
23 printf ("initializing key\n");
24 status = pthread\_key\_create (&tsd\_key, NULL);
25 if (status != 0)
26 err\_abort (status, "Create key");
27 }
Thread-specific data 165
28
29 /*
30 * Thread start routine that uses pthread\_once to dynamically
31 * create a thread-specific data key.
32 */
33 void *thread\_routine (void *arg)
34 {
35 tsd\_t *value;
36 int status;
37
38 status = pthread\_once (&key\_once, once\_routine);
39 if (status != 0)
40 err\_abort (status, "Once init");
41 value = (tsd\_t*)malloc (sizeof (tsd\_t));
42 if (value == NULL)
43 errno\_abort ("Allocate key value");
44 status = pthread\_setspecific (tsd\_key, value);
45 if (status != 0)
46 err\_abort (status, "Set tsd");
47 printf ("%s set tsd value %p\n", arg, value);
48 value-\>thread\_id = pthread\_self ();
49 value-\>string = (char*)arg;
50 value = (tsd\_t*)pthread\_getspecific (tsd\_key);
51 printf ("%s starting...\n" , value-\>string);
52 sleep B);
53 value = (tsd\_t*)pthread\_getspecific (tsd\_key);
54 printf ("%s done...\n", value-\>string);
55 return NULL;
56 }
57
58 void main (int argc, char *argv[])
59 {
60 pthread\_t threadl, thread2;
61 int status;
62
63 status = pthread\_create (
64 Srthreadl, NULL, thread\_routine, "thread 1");
65 if (status != 0)
66 err\_abort (status, "Create thread 1");
67 status = pthread\_create (
68 &thread2, NULL, thread\_routine, "thread 2");
69 if (status != 0)
70 err\_abort (status, "Create thread 2");
71 pthread\_exit (NULL);
72 }
| tsd once.c
166 CHAPTER 5 Advanced threaded programming
Pthreads allows you to destroy a thread-specific data key when your program
no longer needs it, by calling pthread\_key\_delete. The Pthreads standard guar-
antees only 128 thread-specific data keys at any one time, so it may be useful to
destroy a key that you know you aren't using and won't need again. The actual
number of keys supported by your Pthreads system is specified by the value of
the symbol PTHREAD\_KEYS\_MAX defined in \<limits.h\>.
When you destroy a thread-specific data key, it does not affect the current
value of that key in any thread, not even in the calling thread. That means your
code is completely responsible for freeing any memory that you have associated
with the thread-specific data key, in all threads. Of course, any use of the deleted
thread-specific data key (pthread\_key\_t) results in undefined behavior.
Delete thread-specific data keys only when you
are sure no thread has a value for that key!
Or... don't destroy them at all.
You should never destroy a key while some thread still has a value for that
key. Some later call to pthread\_key\_create, for example, might reuse the
pthread\_key\_t identifier that had been assigned to a deleted key. When an exist-
ing thread that had set a value for the old key requests the value of the new key, it
will receive the old value. The program will likely react badly to receiving this
incorrect data, so you should never delete a thread-specific data key until you are
sure that no existing threads have a value for that key, for example, by maintain-
ing a "reference count" for the key, as shown in the program tsd\_destructor.c.
in Section 5.4.3.
Even better, don't destroy thread-specific data keys. There's rarely any need to
do so, and if you try you will almost certainly run into difficulties. Few programs
will require even the minimum Pthreads limit of 128 thread-specific data keys.
Rarely will you use more than a few. In general, each component that uses
thread-specific data will have a small number of keys each maintaining pointers
to data structures that contain related data. It would take a lot of components to
exhaust the available keys!
### 5.4.2 Using thread-specific data
```c
int pthread\_setspecific (
pthread\_key\_t key, const void *value);
void *pthread\_getspecific (pthread\_key\_t key);
```
You can use the pthread\_getspecif ic function to determine the thread's cur-
rent value for a key, or pthread\_setspecif ic to change the current value. Take a
look at Section 7.3.1 for ideas on using thread-specific data to adapt old libraries
that rely on static data to be thread-safe.
Thread-specific data 167
I A thread-specific data value of null means something special to
Pthreads-do not set a thread-specific data value of null unless you
really mean it,
The initial value for any new key (in all threads) is null. Also, Pthreads sets
the thread-specific data value for a key to null before calling that key's destruc-
tor (passing the previous value of the key) when a thread terminates.* If your
thread-specific data value is the address of heap storage, for example, and you
want to free that storage in your destructor, you must use the argument passed
to the destructor rather than calling pthread\_getspecif ic.
Pthreads will not call the destructor for a thread-specific data key if the termi-
nating thread has a value of NULL for that key. null is special, meaning "this key
has no value." If you ever use pthread\_setspecif ic to set the value of a thread-
specific data key to null, you need to remember that you are not setting the value
null, but rather stating that the key no longer has a value in the current thread.
I Destructor functions are called only when the thread terminates,
not when the value of a thread-specific data key is changed.
Another important thing to remember is that thread-specific data key destruc-
tor functions are not called when you replace an existing value for that key. That
is, if you allocate a structure in heap and assign a pointer to that structure as the
value of a thread-specific data key, and then later allocate a new structure and
assign a pointer to that new structure to the same thread-specific data key, in the
same thread, you are responsible for freeing the old structure. Pthreads will not
free the old structure, nor will it call your destructor function with a pointer to
the old structure.
### 5.4.3 Using destructor functions
When a thread exits while it has a value defined for some thread-specific data
key, you usually need to do something about it. If your key's value is a pointer to
heap memory, you will need to free the memory to avoid a memory leak each
time a thread terminates. Pthreads allows you to define a destructor function
"That is, unfortunately, not what the standard says. This is one of the problems with formal
standards-they say what they say, not what they were intended to say. Somehow, an error crept
in, and the sentence specifying that "the implementation clears the thread-specific data value be-
fore calling the destructor" was deleted. Nobody noticed, and the standard was approved with the
error. So the standard says (by omission) that if you want to write a portable application using
thread-specific data, that will not hang on thread termination, you must call pthread\_setspe-
cif ic within your destructor function to change the value to null. This would be silly, and any
serious implementation of Pthreads will violate the standard in this respect. Of course, the stan-
dard will be fixed, probably by the 1003. In amendment (assorted corrections to 1003. lc-1995),
but that will take a while.
168 CHAPTER 5 Advanced threaded programming
when you create a thread-specific data key. When a thread terminates with a
non-NULL value for a thread-specific data key, the key's destructor (if any) is
called with the current value of the key.
I Thread-specific data destructors are called in "unspecified order."
Pthreads checks all thread-specific data keys in the process when a thread
exits, and for each thread-specific data key with a value that's not NULL, it sets
the value to null and then calls the key's destructor function. Going back to our
analogy, someone might collect the identity badges of all programmers by remov-
ing whatever is hanging from each programmer's left shirt pocket, safe in the
knowledge that it will always be the programmer's badge. Be careful, because the
order in which destructors are called is undefined. Try to make each destructor
as independent as possible.
Thread-specific data destructors can set a new value for the key for which a
value is being destroyed or for any other key. You should never do this directly,
but it can easily happen indirectly if you call other functions from your destruc-
tor. For example, the ANSI C library's destructors might be called before yours-
and calling an ANSI C function, for example, using fprintf to write a log mes-
sage to a file, could cause a new value to be assigned to a thread-specific data
key. The system must recheck the list of thread-specific data values for you after
all destructors have been called.
I If your thread-specific data destructor creates a new thread-specific
data value,you will get another chance. Maybe too many chancesl
The standard requires that a Pthreads implementation may recheck the list
some fixed number of times and then give up. When it gives up, the final thread-
specific data value is not destroyed. If the value is a pointer to heap memory,
the result may be a memory leak, so be careful. The \<limits .h\> header defines
\_pthread\_destructor\_iterations to the number of times the system will check,
and the value must be at least 4. Alternately, the system is allowed to keep
checking forever, so a destructor function that always sets thread-specific data
values may cause an infinite loop.
Usually, new thread-specific data values are set within a destructor only when
subsystem 1 uses thread-specific data that depends on another independent sub-
system 2 that also uses thread-specific data. Because the order in which
destructor functions run is unspecified, the two may be called in the wrong order.
If the subsystem 1 destructor needs to call into subsystem 2, it may inadvertently
result in allocating new thread-specific data for subsystem 2. Although the sub-
system 2 destructor will need to be called again to free the new data, the
subsystem 1 thread-specific data remains NULL, so the loop will terminate.
The following program, tsd\_destructor.c, demonstrates using thread-
specific data destructors to release memory when a thread terminates. It also
keeps track of how many threads are using the thread-specific data, and deletes
Thread-specific data 169
the thread-specific data key when the destructor is run for the final thread. This
program is similar in structure to tsd\_once.c, from Section 5.3, so only the rele-
vant differences will be annotated here.
12-14 In addition to the key value (identity\_key), the program maintains a count of
threads that are using the key (identity\_key\_counter), which is protected by a
mutex (identity\_key\_mutex).
22-42 The function identity\_key\_destructor is the thread-specific data key's
destructor function. It begins by printing a message so we can observe when it
runs in each thread. It frees the storage used to maintain thread-specific data,
the private\_t structure. Then it locks the mutex associated with the thread-
specific data key (identity\_key\_mutex) and decreases the count of threads using
the key. If the count reaches 0, it deletes the key and prints a message.
48-63 The function identity\_key\_get can be used anywhere (in this example, it is
used only once per thread) to get the value of identity\_key for the calling thread.
If there is no current value (the value is NULL), then it allocates a new private\_t
structure and assigns it to the key for future reference.
68-78 The function thread\_routine is the thread start function used by the exam-
ple. It acquires a value for the key by calling identity\_key\_get, and sets the
members of the structure. The string member is set to the thread's argument,
creating a global "name" for the thread, which can be used for printing messages.
80-114 The main program creates the thread-specific data key tsd\_key. Notice that,
unlike tsd\_once. c, this program does not bother to use pthread\_once. As I men-
tioned in the annotation for that example, in a main program it is perfectly safe,
and more efficient, to create the key inside main, before creating any threads.
101 The main program initializes the reference counter (identity\_key\_counter)
to 3. It is critical that you define in advance how many threads will reference a
key that will be deleted based on a reference count, as we intend to do. The
counter must be set before any thread using the key can possibly terminate.
You cannot, for example, code identity\_key\_get so that it dynamically
increases the counter when it first assigns a thread-specific value for identity\_
key. That is because one thread might assign a thread-specific value for
identity\_key and then terminate before another thread using the key had a
chance to start. If that happened, the first thread's destructor would find no
remaining references to the key, and it would delete the key. Later threads would
then fail when trying to set thread-specific data values.
| tsd\_destructor.c
1 #include \<pthread.h\>
2 #include "errors.h"
3
4 /*
5 * Structure used as value of thread-specific data key.
6 */
170 CHAPTER 5 Advanced threaded programming
1 typedef struct private\_tag {
8 pthread\_t thread\_id;
9 char *string;
10 } private\_t;
11
12 pthread\_key\_t identity\_key; /* Thread-specific data key */
13 pthread\_mutex\_t identity\_key\_mutex = PTHREAD\_MUTEX\_INITIALIZER;
14 long identity\_key\_counter = 0;
15
16 /*
17 * This routine is called as each thread terminates with a value
18 * for the thread-specific data key. It keeps track of how many
19 * threads still have values, and deletes the key when there are
20 * no more references.
21 */
22 void identity\_key\_destructor (void *value)
23 {
24 private\_t *private = (private\_t*)value;
25 int status;
26
27 printf ("thread \"%s\" exiting...\n", private-\>string);
28 free (value);
29 status = pthread\_mutex\_lock (&identity\_key\_mutex);
30 if (status != 0)
31 err\_abort (status, "Lock key mutex");
32 identity\_key\_counter-;
33 if (identity\_key\_counter \<= 0) {
34 status = pthread\_key\_delete (identity\_key);
35 if (status != 0)
36 err\_abort (status, "Delete key");
37 printf ("key deleted...\n");
38 }
39 status = pthread\_mutex\_unlock (&identity\_key\_mutex);
40 if (status != 0)
41 err\_abort (status, "Unlock key mutex");
42 }
43
44 /*
45 * Helper routine to allocate a new value for thread-specific
46 * data key if the thread doesn't already have one.
47 */
48 void *identity\_key\_get (void)
49 {
50 void *value;
51 int status;
52
53 value = pthread\_getspecific (identity\_key);
Thread-specific data 171
54 if (value == NULL) {
55 value = malloc (sizeof (private\_t));
56 if (value == NULL)
57 errno\_abort ("Allocate key value");
58 status = pthread\_setspecific (identity\_key, (void*)value);
59 if (status != 0)
60 err\_abort (status, "Set TSD");
61 }
62 return value;
63 }
64
65 /*
66 * Thread start routine to use thread-specific data.
67 */
68 void *thread\_routine (void *arg)
69 {
70 private\_t *value;
71
72 value = (private\_t*)identity\_key\_get ();
73 value-\>thread\_id = pthread\_self ();
74 value-\>string = (char*)arg;
75 printf ("thread \"%s\" starting...\n", value-\>string);
76 sleep B);
77 return NULL;
78 }
79
80 void main (int argc, char *argv[])
81 {
82 pthread\_t thread\_l, thread\_2;
83 private\_t *value;
84 int status;
85
86 /*
87 * Create the TSD key, and set the reference counter to
88 * the number of threads that will use it (two thread\_routine
89 * threads plus main). This must be done before creating
90 * the threads! Otherwise, if one thread runs the key's
91 * destructor before any other thread uses the key, it will
92 * be deleted.
93 *
94 * Note that there's rarely any good reason to delete a
95 * thread-specific data key.
96 */
97 status = pthread\_key\_create (
98 &identity\_key, identity\_key\_destructor);
99 if (status != 0)
100 err\_abort (status, "Create key");
172 CHAPTER 5 Advanced threaded programming
101 identity\_key\_counter = 3;
102 value = (private\_t*)identity\_key\_get ();
103 value-\>thread\_id = pthread\_self ();
104 value-\>string = "Main thread";
105 status = pthread\_create (&thread\_l, NULL,
106 thread\_routine, "Thread 1");
107 if (status != 0)
108 err\_abort (status, "Create thread 1");
109 status = pthread\_create (&thread\_2, NULL,
110 thread\_routine, "Thread 2");
111 if (status != 0)
112 err\_abort (status, "Create thread 2");
113 pthread\_exit (NULL);
114 }
| tsd destructor.c
## 5.5 Realtime scheduling
"Well, it's no use your talking about waking him," said Tweedledum,
"when you're only one of the things in his dream. You know
very well you're not real."
"I am real!" said Alice, and began to cry.
"You wo'n't make yourself a bit realler by crying," Tweedledee remarked:
"there's nothing to cry about."
-Lewis Carroll, Through the Looking-Glass
Once upon a time, realtime programming was considered an arcane and rare
art. Realtime programmers were doing unusual things, outside of the program-
ming mainstream, like controlling nuclear reactors or airplane navigational
systems. But the POSIX. lb realtime extension defines realtime as "the ability of the
operating system to provide a required level of service in a bounded response time."
What applies to the operating system also applies to your application or library.
"Bounded" response time does not necessarily mean "fast" response, but it
does mean "predictable" response. There must be some way to define a span of
time during which a sequence of operations is guaranteed to complete. A system
controlling a nuclear reactor has more strict response requirements than most
programs you will write, and certainly the consequences of failing to meet the
reactor's response requirements are more severe. But a lot of code you write will
need to provide some "required level of service" within some "bounded response
time." Realtime programming just means that the software lives in the real world.
Realtime programming covers such a vast area that it is common to divide it
into two separate categories. "Hard realtime" is the traditional sort most people
Realtime scheduling 173
think of. When your nuclear reactor will go critical if a fuel rod adjustment is
delayed by a microsecond or your airplane will crash if the navigation system
takes a half second to respond to a wind sheer, that's hard realtime. Hard realtime
is unforgiving, because the required level of service and bounded response time are
defined by physics or something equally unyielding. "Soft realtime" means that
you need to meet your schedule most of the time, but the consequences of failing
to meet the schedule are not severe.
Many systems that interact with humans should be designed according to soft
realtime principles. Although humans react slowly, in computer terms, they're
sensitive to response time. Make your users wait too often while the screen
redraws before accepting the next mouse click, and they'll be annoyed. Nobody
likes a "busy cursor"-most people expect response to be at least predictable,
even when it cannot be fast.
Threads are useful for all types of realtime programming, because coding for
predictable response is far easier when you can keep the operations separate.
Your "user input function" doesn't have to wait for your sort operation or for your
screen update operation because it executes independently.
Achieving predictability requires a lot more than just separating operations
into different threads, however. For one thing, you need to make sure that the
thread you need to run "soon" won't be left sitting on a run queue somewhere
while another thread uses the processor. Most systems, by default, will try to dis-
tribute resources more or less fairly between threads. That's nice for a lot of
things-but realtime isn't fair. Realtime means carefully giving precedence to the
parts of the program that limit external response time.
### 5.5.1 POSIX realtime options
The POSIX standards are flexible, because they're designed to be useful in a
wide range of environments. In particular, since traditional UNIX systems don't
support any form of realtime scheduling control, all of the tools for controlling
realtime response are optional. The fact that a given implementation of UNIX
"conforms to 1003.1c-1995" does not mean you can write predictable realtime
programs.
If the system defines \_posix\_thread\_priority\_scheduling, it provides sup-
port for assigning realtime scheduling priorities to threads. The POSIX priority
scheduling model is a little more complicated than the traditional UNIX priority
model, but the principle is similar. Priority scheduling allows the programmer to
give the system an idea of how important any two threads are, relative to each
other. Whenever more than one thread is ready to execute, the system will choose
the thread with the highest priority.
174
CHAPTER 5 Advanced threaded programming
### 5.5.2 Scheduling policies and priorities
```c
int sched\_get\_priority\_max (int policy);
int sched\_get\_priority\_min (int policy);
int pthread\_attr\_getinheritsched (
const pthread\_attr\_t *attr, int *inheritsched);
int pthread\_attr\_setinheritsched (
pthread\_attr\_t *attr, int inheritsched);
int pthread\_attr\_getschedparam (
const pthread\_attr\_t *attr,
struct sched\_param *param);
int pthread\_attr\_setschedparam (
pthread\_attr\_t *attr,
const struct sched\_param *param);
int pthread\_attr\_getschedpolicy (
const pthread\_attr\_t *attr, int *policy);
int pthread\_attr\_setschedpolicy (
pthread\_attr\_t *attr, int policy);
int pthread\_getschedparam (pthread\_t thread,
int *policy, struct sched\_j?aram *param);
int pthread\_setschedparam (
pthread\_t thread, int\>policy,
const struct sched\_param *param);
```
A Pthreads system that supports \_posix\_thread\_priority\_scheduling must
provide a definition of the struct sched\_param structure that includes at least
the member sched\_priority. The sched\_priority member is the only schedul-
ing parameter used by the standard Pthreads scheduling policies, SCHED\_FIFO
and SCHED\_RR. The minimum and maximum priority values (sched\_priority
member) that are allowed for each scheduling policy can be determined by calling
sched\_get\_priority\_min or sched\_get\_priority\_max, respectively, for the sched-
uling policy. Pthreads systems that support additional, nonstandard scheduling
policies may include additional members.
The sched\_fifo {first in, first out) policy allows a thread to run until another
thread with a higher priority becomes ready, or until it blocks voluntarily. When
a thread with sched\_fifo scheduling policy becomes ready, it begins executing
immediately unless a thread with equal or higher priority is already executing.
The sched\_rr [round-robin] policy is much the same, except that if a thread
with sched\_rr policy executes for more than a fixed period of time (the timeslice
interval) without blocking, and another thread with sched\_rr or sched\_fifo pol-
icy and the same priority is ready, the running thread will be preempted so the
ready thread can be executed.
When threads with sched\_fifo or sched\_rr policy wait on a condition vari-
able or wait to lock a mutex, they will be awakened in priority order. That is, if a
low-priority sched\_fifo thread and a high-priority sched\_fifo thread are both
Realtime scheduling 175
waiting to lock the same mutex, the high-priority thread will always be unblocked
first when the mutex is unlocked.
Pthreads defines the name of an additional scheduling policy, called sched\_
other. Pthreads, however, says nothing at all regarding what this scheduling pol-
icy does. This is an illustration of an unofficial POSIX philosophy that has been
termed "a standard way to be nonstandard" (or, alternately, "a portable way to be
nonportable"). That is, when you use any implementation of Pthreads that sup-
ports the priority scheduling option, you can write a portable program that
creates threads running in SCHED\_other policy, but the behavior of that program
is nonportable. (The official explanation of sched\_other is that it provides a por-
table way for a program to declare that it does not need a realtime scheduling
policy.)
The schedjdther policy may be an alias for sched\_fifo, or it may be sched\_rr,
or it may be something entirely different. The real problem with this ambiguity is
not that you don't know what sched\_other does, but that you have no way of
knowing what scheduling parameters it might require. Because the meaning of
SCHED\_OTHER is undefined, it does not necessarily use the sched\_priority member
of the struct sched\_param structure, and it may require additional, nonstandard
members that an implementation may add to the structure. If there's any point to
this, it is simply that sched\_other is not portable. If you write any code that uses
SCHED\_OTHER you should be aware that the code is not portable-you are, by defini-
tion, depending on the sched\_other of the particular Pthreads implementation for
which you wrote the code.
The schedpolicy and schedparam attributes, set respectively by pthread\_
attr\_setschedpolicy and pthread\_attr setschedparam, specify the explicit
scheduling policy and parameters for the attributes object. Pthreads does not
specify a default value for either of these attributes, which means that each
implementation may choose some "appropriate" value. A realtime operating sys-
tem intended for embedded controller applications, for example, might choose to
create threads by default with SCHED\_fifo policy, and, perhaps, some medium-
range priority.
Most multiuser operating systems are more likely to use a nonstandard "time-
share" scheduling policy by default, causing threads to be scheduled more or less
as processes have always been scheduled. The system may, for example, tempo-
rarily reduce the priority of "CPU hogs" so that they cannot prevent other threads
from making progress.
One example of a multiuser operating system is Digital UNIX, which supports
two nonstandard timeshare scheduling policies. The foreground policy (sched\_
fg\_np), which is the default, is used for normal interactive activity, and corre-
sponds to the way nonthreaded processes are scheduled. The background policy
(SCHED\_BG\_NP) can be used for less important support activities.
When you set the scheduling policy or priority attributes in an
attributes object,you must also set the inheritsched attribute!
176 CHAPTER 5 Advanced threaded programming
The inheritsched attribute, which you can set by calling pthread\_attr\_
setinheritsched, controls whether a thread you create inherits scheduling
information from the creating thread, or uses the explicit scheduling information
in the schedpolicy and schedparam attributes. Pthreads does not specify a
default value for inheritsched, either, so if you care about the policy and schedul-
ing parameters of your thread, you must always set this attribute.
Set the inheritsched attribute to pthread\_inherit\_sched to cause a new
thread to inherit the scheduling policy and parameters of the creating thread.
Scheduling inheritance is useful when you're creating "helper" threads that are
working on behalf of the creator-it generally makes sense for them to run at the
same policy and priority. Whenever you need to control the scheduling policy or
parameters of a thread you create, you must set the inheritsched attribute to
PTHREAD\_EXPLICIT\_SCHED.
58-118 The following program, sched\_attr.c, shows how to use an attributes object to
create a thread with an explicit scheduling policy and priority. Notice that it uses
conditional code to determine whether the priority scheduling feature of Pthreads
is supported at compilation time. It will print a message if the option is not sup-
ported and continue, although the program in that case will not do much. (It
creates a thread with default scheduling behavior, which can only say that it ran.)
Although Solaris 2.5 defines \_posix\_thread\_priority\_scheduling, it does
not support the POSIX realtime scheduling policies, and attempting to set the
policy attribute to sched\_rr would fail. This program treats Solaris as if it did not
define the\_POSlx\_THREAD\_PRiORiTY\_SCHEDULlNG option.
| sched\_attr.c
1 #include \<unistd.h\>
2 #include \<pthread.h\>
3 #include \<sched.h\>
4 #include "errors.h"
5
6 /*
7 * Thread start routine. If priority scheduling is supported,
8 * report the thread's scheduling attributes.
9 */
10 void *thread\_routine (void *arg)
11 {
12 int my\_policy;
13 struct sched\_param my\_param;
14 int status;
15
16 /*
17 * If the priority scheduling option is not defined, then we
18 * can do nothing with the output of pthread\_getschedparam,
19 * so just report that the thread ran, and exit.
20 */
Realtime scheduling 177
21 #if defined (\_POSIX\_THREAD\_PRIORITY\_SCHEDULING) && Idefined (sun)
22 status = pthread\_getschedparam (
23 pthread\_self (), &my\_policy, &my\_param);
24 if (status != 0)
25 err\_abort (status, "Get sched");
26 printf ("thread\_routine running at %s/%d\n",
27 (my\_policy == SCHED\_FIFO ? "FIFO"
28 : (my\_policy == SCHED\_RR ? "RR"
29 : (my\_policy == SCHED\_OTHER ? "OTHER"
30 : "unknown"))),
31 my\_param.sched\_priority);
32 #else
33 printf ("thread\_routine running\n");
34 #endif
35 return NULL;
36 }
37
38 int main (int argc, char *argv[])
39 {
40 pthread\_t thread\_id;
41 pthread\_attr\_t thread\_attr;
42 int thread\_policy;
43 struct sched\_param thread\_param;
44 int status, rr\_min\_priority, rr\_max\_priority;
45
46 status = pthread\_attr\_init (&thread\_attr); '
47 if (status != 0)
4 8 err\_abort (status, "Init attr");
49
50 /*
51 * If the priority scheduling option is defined, set various
52 * scheduling parameters. Note that it is particularly important
53 * that you remember to set the inheritsched attribute to
54 * PTHREAD\_EXPLICIT\_SCHED, or the policy and priority that you've
55 * set will be ignored! The default behavior is to inherit
56 * scheduling information from the creating thread.
57 */
58 #if defined (\_POSIX\_THREAD\_PRIORITY\_SCHEDULING) && Idefined (sun)
59 status = pthread\_attr\_getschedpolicy (
60 &thread\_attr, &thread\_policy);
61 if (status != 0)
62 err\_abort (status, "Get policy");
63 status = pthread\_attr\_getschedparam (
64 &thread\_attr, &thread\_param);
65 if (status != 0)
66 err\_abort (status, "Get sched param");
67 printf (
68 "Default policy is %s, priority is %d\n",
69 (thread\_policy == SCHED FIFO ? "FIFO"
178 CHAPTER 5 Advanced threaded programming
70 : (thread\_policy == SCHED\_RR ? "RR"
71 : (thread\_policy == SCHED\_OTHER ? "OTHER"
72 : "unknown"))),
73 thread\_param.sched\_priority);
74
75 V status = pthread\_attr\_setschedpolicy (
76 &thread\_attr, SCHED\_RR);
77 if (status != 0)
78 printf ("Unable to set SCHED\_RR policy.\n");
79 else {
80 /*
81 * Just for the sake of the exercise, we'll use the
82 * middle of the priority range allowed for
83 * SCHED\_RR. This should ensure that the thread will be
84 * run, without blocking everything else. Because any
85 * assumptions about how a thread's priority interacts
86 * with other threads (even in other processes) are
87 * nonportable, especially on an implementation that
88 * defaults to System contention scope, you may have to
89 * adjust this code before it will work on some systems.
90 */
91 rr\_min\_priority = sched\_get\_priority\_min (SCHED\_RR);
92 if (rr\_min\_priority == -1)
93 errno\_abort ("Get SCHED\_RR min priority");
94 rr\_max\_priority = sched\_get\_priority\_max (SCHED\_RR);
95 if (rr\_max\_priority == -1)
96 errno\_abort ("Get SCHED\_RR max priority");
97 thread\_param.sched\_priority =
98 (rr\_min\_priority + rr\_max\_priorityI2;
99 printf (
100 "SCHED\_RR priority range is %d to %d: using %d\n",
101 rr\_min\_priority,
102 rr\_max\_priority,
103 thread\_param.sched\_priority);
104 y status = pthread\_attr\_setschedparam (
105 &thread\_attr, &thread\_param);
106 if (status != 0)
107 err\_abort (status, "Set params");
108 printf (
109 "Creating thread at RR/%d\n",
110 thread\_param.sched\_priority);
111 status = pthread\_attr\_setinheritsched (
112 &thread\_attr, PTHREAD\_EXPLICIT\_SCHED);
113 if (status != 0)
114 err\_abort (status, "Set inherit");
115 }
116 #else
117 printf ("Priority scheduling not supported\n");
Realtime scheduling 179
118 #endif
119 status = pthread\_create (
120 &thread\_id, &thread\_attr, thread\_routine, NULL);
121 if (status != 0)
122 err\_abort (status, "Create thread");
123 status = pthread\_join (thread\_id, NULL);
124 if (status != 0)
125 err\_abort (status, "Join thread");
126 printf ("Main exitingW);
127 return 0;
128 }
| sched\_attr.c
The next program, sched\_thread.c, shows how to modify the realtime sched-
uling policy and parameters for a running thread. When changing the scheduling
policy and parameters in a thread attributes object, remember, you use two sepa-
rate operations: one to modify the scheduling policy and the other to modify the
scheduling parameters.
You cannot modify the scheduling policy of a running thread separately from
the thread's parameters, because the policy and parameters must always be con-
sistent for scheduling to operate correctly. Each scheduling policy may have a
unique range of valid scheduling priorities, and a thread cannot operate at a pri-
ority that isn't valid for its current policy. To ensure consistency of the policy and
parameters, they are set with a single call.
55 Unlike sched\_attr.c, sched\_thread.c does not check the compile-time fea-
ture macro \_posix\_thread\_priority\_scheduling. That means it will probably
not compile, and almost certainly won't run correctly, on a system that does not
support the option. There's nothing wrong with writing a program that way-in
fact, that's what you are likely to do most of the time. If you need priority sched-
uling, you would document that your application requires the \_POSIX\_thread\_
priority\_scheduling option, and use it.
57-62 Solaris 2.5, despite denning \_posix\_thread\_priority\_scheduling, does not
support realtime scheduling policies. For this reason, the ENOSYS from sched\_
get\_priority\_min is handled as a special case.
| sched\_thread.c
1 #include \<unistd.h\>
2 #include \<pthread.h\>
3 #include \<sched.h\>
4 #include "errors.h"
5
6 #define THREADS 5
7
8 /*
9 * Structure describing each thread.
10 */
180 CHAPTER 5 Advanced threaded programming
11 typedef struct thread\_tag {
12 int index;
13 pthread\_t id;
14 } thread\_t;
15
16 thread\_t threads[THREADS];
17 int rr\_min\_priority;
18
19 /*
20 * Thread start routine that will set its own priority.
21 */
22 void *thread\_routine (void *arg)
23 {
24 thread\_t *self = (thread\_t*)arg;
25 int my\_policy;
26 struct sched\_param my\_param;
27 int status;
28
29 my\_param.sched\_priority = rr\_min\_priority + self-\>index;
30 DPRINTF ((
31 "Thread %d will set SCHED\_FIFO, priority %d\n",
32 self-\>index, my\_param.sched\_priority));
33 status = pthread\_setschedparam (
34 self-\>id, SCHED\_RR, &my\_param); o'-, ;|' - |- .
35 if (status != 0) V ?--"|' (
36 err\_abort (status, "Set sched");
37 status = pthread\_getschedparam (
38 self-\>id, &my\_policy, &my\_param);
39 if (status != 0)
40 err\_abort (status, "Get sched");
41 'printf ("thread\_routine %d running at %s/%d\n",
42 self-\>index,
43 (my\_policy == SCHED\_FIFO ? "FIFO"
44 : (my\_policy == SCHED\_RR ? "RR"
45 : (my\_policy == SCHED\_OTHER ? "OTHER"
4 6 : "unknown"))),
47 my\_param.sched\_priority);
48 return NULL;
49 }
50
51 int main (int argc, char *argv[])
52 {
53 int count, status;
54
55 rr\_min\_priority = sched\_get\_priority\_min (SCHED\_RR);
56 if (rr\_min\_priority == -1) {
57 #ifdef sun
58 if (errno == ENOSYS) {
59 fprintf (stderr, "SCHED\_RR is not supported.\n");
Realtime scheduling 181
60 exit @);
61 }
62 #endif
63 errno\_abort ("Get SCHED\_RR min priority");
64 }
65 for (count = 0; count \< THREADS; count++) {
66 threads[count].index = count;
67 status = pthread\_create (
68 Sthreadsfcount].id, NULL,
69 thread\_routine, (void*)&threads[count]);
70 if (status != 0)
71 err\_abort (status, "Create thread");
72 }
73 for (count = 0; count \< THREADS; count++) {
74 status = pthread\_join (threads[count].id, NULL);
75 if (status != 0)
76 err\_abort (status, "Join thread");
77 }
78 printf ("Main exiting\n");
79 return 0;
80 }
| sched thread.c
### 5.5.3 Contention scope and allocation domain
```c
int pthread\_attr\_getscope (
const pthread\_attr\_t *attr, int *contentionscope);
int pthread\_attr\_setscope (
pthread attr t *attr, int contentionscope);
```
Besides scheduling policy and parameters, two other controls are important in
realtime scheduling. Unless you are writing a realtime application, they probably
don't matter. If you are writing a realtime application, you will need to find out
which settings of these controls are supported by a system.
The first control is called contention scope. It is a description of how your
threads compete for processor resources. System contention scope means that
your thread competes for processor resources against threads outside your pro-
cess. A high-priority system contention scope thread in your process can keep
system contention scope threads in other processes from running (or vice versa).
Process contention scope means that your threads compete only among them-
selves. Usually, process contention scope means that the operating system
chooses a process to execute, possibly using only the traditional UNIX priority,
and some additional scheduler within the process applies the POSIX scheduling
rules to determine which thread to execute.
182 CHAPTER 5 Advanced threaded programming
Pthreads provides the thread scope attribute so that you can specify whether
each thread you create should have process or system contention scope. A
Pthreads system may choose to support pthread\_scope\_process, pthread\_
scope\_system, or both. If you try to create a thread with a scope that is not sup-
ported by the system, pthread\_attr\_setscope will return ENOTSUP.
The second control is allocation domain. An allocation domain is the set of pro-
cessors within the system for which threads may compete. A system may have
one or more allocation domains, each containing one or more processors. In a
uniprocessor system, an allocation domain will contain only one processor, but
you may still have more than one allocation domain. On a multiprocessor, each
allocation domain may contain from one processor to the number of processors
in the system.
There is no Pthreads interface to set a thread's allocation domain. The
POSIX. 14 (Multiprocessor Profile) working group considered proposing standard
interfaces, but the effort was halted by the prospect of dealing with the wide
range of hardware architectures and existing software interfaces. Despite the
lack of a standard, any system supporting multiprocessors will have interfaces to
affect the allocation domain of a thread.
Because there is no standard interface to control allocation domain, there is
no way to describe precisely all the effects of any particular hypothetical situa-
tion. Still, you may need to be concerned about these things if you use a system
that supports multiprocessors. A few things to think about:
1. How do system contention scope threads and process contention scope
threads, within the same allocation domain, interact with each other? They
are competing for resources in some manner, but the behavior is not
denned by the standard.
2. If the system supports "overlapping" allocation domains, in other words, if
a processor can appear in more than one allocation domain within the sys-
tem, and you have one system contention scope thread in each of two
overlapping allocation domains, what happens?
I System contention scope is predictable.
Process contention scope is cheap.
On most systems, you will get better performance, and lower cost, by using
only process contention scope. Context switches between system contention
scope threads usually require at least one call into the kernel, and those calls are
relatively expensive compared to the cost of saving and restoring thread state in
user mode. Each system contention scope thread will be permanently associated
with one "kernel entity," and the number of kernel entities is usually more limited
than the number of Pthreads threads. Process contention scope threads may
share one kernel entity, or some small number of kernel entities. On a given sys-
tem configuration, for example, you may be able to create thousands of process
contention scope threads, but only hundreds of system contention scope threads.
Realtime scheduling 183
On the other hand, process contention scope gives you no real control over the
scheduling priority of your thread-while a high priority may give it precedence
over other threads in the process, it has no advantage over threads in other pro-
cesses with lower priority. System contention scope gives you better predictability
by allowing control, often to the extent of being able to make your thread "more
important" than threads running within the operating system kernel.
I System contention scope is less predictable with an allocation domain
greater than one.
When a thread is assigned to an allocation domain with more than a single
processor, the application can no longer rely on completely predictable schedul-
ing behavior. Both high- and low-priority threads may run at the same time, for
example, because the scheduler will not allow processors to be idle just because a
high-priority thread is running. The uniprocessor behavior would make little
sense on a multiprocessor.
When thread 1 awakens thread 2 by unlocking a mutex, and thread 2 has a
higher priority than thread 1, thread 2 will preempt thread 1 and begin running
immediately. However, if thread 1 and thread 2 are running simultaneously in an
allocation domain greater than one, and thread 1 awakens thread 3, which has
lower priority than thread 1 but higher priority than thread 2, thread 3 may not
immediately preempt thread 2. Thread 3 may remain ready until thread 2 blocks.
For some applications, the predictability afforded by guaranteed preemption
in the case outlined in the previous paragraph may be important. In most cases,
it is not that important as long as thread 3 will eventually run. Although POSIX
does not require any Pthreads system to implement this type of "cross processor
preemption," you are more likely to find it when you use system contention scope
threads. If predictability is critical, of course, you should be using system conten-
tion scope anyway.
### 5.5.4 Problems with realtime scheduling
One of the problems of relying on realtime scheduling is that it is not modu-
lar. In real applications you will generally be working with libraries from a variety
of sources, and those libraries may rely on threads for important functions like
network communication and resource management. Now, it may seem reason-
able to make "the most important thread" in your library run with sched\_fifo
policy and maximum priority. The resulting thread, however, isn't just the most
important thread for your library-it is (or, at least, behaves as) the most impor-
tant thread in the entire process, including the main program and any other
libraries. Your high-priority thread may prevent all other libraries, and in some
cases even the operating system, from performing work on which the application
relies.
184 CHAPTER 5 Advanced threaded programming
Another problem, which really isn't a problem with priority scheduling, but
with the way many people think about priority scheduling, is that it doesn't do
what many people expect. Many people think that "realtime priority" threads
somehow "go faster" than other threads, and that's not true. Realtime priority
threads may actually go slower, because there is more overhead involved in mak-
ing all of the required preemption checks at all the right times-especially on a
multiprocessor.
A more severe problem with fixed priority scheduling is called priority inver-
sion. Priority inversion is when a low-priority thread can prevent a high-priority
thread from running-a nasty interaction between scheduling and synchroniza-
tion. Scheduling rules state that one thread should run, but synchronization
requires that another thread run, so that the priorities of the two threads appear
to be reversed.
Priority inversion occurs when low-priority thread acquires a shared resource
(such as a mutex), and is preempted by a high-priority thread that then blocks on
that same resource. With only two threads, the low-priority thread would then be
allowed to run, eventually (we assume) releasing the mutex. However, if a third
thread with a priority between those two is ready to run, it can prevent the low-
priority thread from running. Because the low-priority thread holds the mutex
that the high-priority thread needs, the middle-priority thread is also keeping the
higher-priority thread from running.
There are a number of ways to prevent priority inversion. The simplest is to
avoid using realtime scheduling, but that's not always practical. Pthreads pro-
vides several mutex locking protocols that help avoid priority inversion, priority
ceiling and priority inheritance. These are discussed in Section 5.5.5.
I Most threaded programs do not need realtime scheduling.
A final problem is that priority scheduling isn't completely portable. Pthreads
defines the priority scheduling features under an option, and many implementa-
tions that are not primarily intended for realtime programming may choose not to
support the option. Even if the option is supported, there are many important
aspects of priority scheduling that are not covered by the standard. When you
use system contention scope, for example, where your threads may compete
directly against threads within the operating system, setting a high priority on
your threads might prevent kernel I/O drivers from functioning on some systems.
Pthreads does not specify a thread's default scheduling policy or priority, or
how the standard scheduling policies interact with nonstandard policies. So
when you set the scheduling policy and priority of your thread, using "portable"
interfaces, the standard provides no way to predict how that setting will affect
any other threads in the process or the system itself.
If you really need priority scheduling, then use it-and be aware that it has
special requirements beyond simply Pthreads. If you need priority scheduling,
keep the following in mind:
Realtime scheduling
185
1. Process contention scope is "nicer" than system contention scope, because
you will not prevent a thread in another process, or in the kernel, from
running.
2. sched\_rr is "nicer" than sched\_fifo, and slightly more portable, because
SCHED\_rr threads will be preempted at intervals to share the available pro-
cessor time with other threads at the same priority.
3. Lower priorities for sched\_fifo and sched\_rr policies are nicer than
higher priorities, because you are less likely to interfere with something
else that's important.
Unless your code really needs priority scheduling, avoid it. In most cases,
introducing priority scheduling will cause more problems than it will solve.
### 5.5.5 Priority-aware mutexes
```c
#if defined (\_POSIX\_THREAD\_PRIO\_PROTECT) \
|| defined (\_POSIX\_THREAD\_PRIO\_INHERIT)
int pthread\_mutexattr\_getprotocol (
const pthread\_mutexattr\_t *attr, int *protocol);
int pthread\_mutexattr\_setprotocol (
pthread\_mutexattr\_t *attr, int protocol);
#endif
#ifdef \_POSIX\_THREAD\_PRIO\_PROTECT
int pthread\_mutexattr\_getprioceiling (
const pthread\_attr\_t *attr, int *prioceiling);
int pthread\_rautexattr\_setprioceiling (
pthread\_mutexattr\_t *attr, int prioceiling);
int pthread\_mutex\_getprioceiling (
const pthread\_mutex\_t *mutex, int *prioceiling);
int pthread\_mutex\_setprioceiling (
pthread\_mutex\_t *mutex,
int prioceiling, int *old\_ceiling);
#endif
```
Pthreads provides several special mutex attributes that can help to avoid pri-
ority inversion deadlocks. Locking, or waiting for, a mutex with one of these
attributes may change the priority of the thread-or the priority of other threads-
to ensure that the thread that owns the mutex cannot be preempted by another
thread that needs to lock the same mutex.
These mutex attributes may not be supported by your implementation of
Pthreads, because they are optional features. If your code needs to function with
or without these options, you can conditionally compile references based on the
feature test macros \_posix\_thread\_prio\_protect or \_posix\_thread\_prio\_
inherit, defined in \<unistd.h\>, or you can call sysconf during program execu-
tion to check for SC THREAD PRIO PROTECT or SC THREAD PRIO INHERIT.
186 CHAPTER 5 Advanced threaded programming
Once you've created a mutex using one of these attributes, you can lock and
unlock the mutex exactly like any other mutex. As a consequence, you can easily
convert any mutex you create by changing the code that initializes the mutex.
(You must call pthread\_mutex\_init, however, because you cannot statically ini-
tialize a mutex with nondefault attributes.)
I "Priority ceiling" protocol means that while a thread owns the mutex, it
runs at the specified priority.
If your system defines \_posix\_thread\_prio\_protect then it supports the
protocol and prioceiling attributes. You set the protocol attribute by calling
pthread\_mutexattr\_setprotocol. If you set the protocol attribute to the value
pthread\_prio\_protect, then you can also specify the priority ceiling for mutexes
created using the attributes object by setting the prioceiling attribute.
You set the prioceiling attribute by calling the function pthread\_mutexattr\_
setprioceiling. When any thread locks a mutex defined with such an attributes
object, the thread's priority will be set to the priority ceiling of the mutex, unless
the thread's priority is already the same or higher. Note that locking the mutex in
a thread running at a priority above the priority ceiling of the mutex breaks the
protocol, removing the protection against priority inversion.
I "Priority inheritance" means that when a thread waits on a mutex
owned by a lower-priority thread, the priority of the owner is increased
to that of the waiter.
If your system defines \_posix\_thread\_prio\_inherit then it supports the pro-
tocol attribute. If you set the protocol attribute to the value pthread\_prio\_
inherit, then no thread holding the mutex can be preempted by another thread
with a priority lower than that of any thread waiting for the mutex. When any
thread attempts to lock the mutex while a lower-priority thread holds the mutex,
the priority of the thread currently holding the mutex will be raised to the priority
of the waiter as long as it owns the mutex.
If your system does not define either \_posix\_thread\_prio\_protect or \_posix\_
thread\_prio\_inherit then the protocol attribute may not be defined. The default
value of the protocol attribute (or the effective value if the attribute isn't defined) is
posix\_prio\_none, which means that thread priorities are not modified by the act
of locking (or waiting for) a mutex.
#### 5.5.5.1 Priority ceiling mutexes
The simplest of the two types of "priority aware" mutexes is the priority ceiling
(or "priority protection") protocol (Figure 5.3). When you create a mutex using a
priority ceiling, you specify the highest priority at which a thread will ever be
running when it locks the mutex. Any thread locking that mutex will have its
Realtime scheduling
187
priority
time
|- lock mutex
- unlock mutex
priority ceiling
FIGURE 5.3 Priority ceiling mutex operation
priority automatically raised to that value, which will allow it to finish with the
mutex before it can be preempted by any other thread that might try to lock the
mutex. You can also examine or modify the priority ceiling of a mutex that was
created with the priority ceiling [protect) protocol.
A priority ceiling mutex is not useful within a library that can be called by
threads you don't control. If any thread that is running at a priority above the
ceiling locks the priority ceiling mutex, the protocol is broken. This doesn't neces-
sarily guarantee a priority inversion, but it removes all protection against priority
inversion. Since the priority ceiling protocol adds overhead to each mutex opera-
tion compared to a normal "unprotected" mutex, you may have wasted processor
time accomplishing nothing.
Priority ceiling is perfect for an embedded realtime application where the devel-
opers control all synchronization within the system. The priority ceiling can be
safely determined when the code is designed, and you can avoid priority inversion
with a relatively small cost in performance compared to more general solutions. Of
course it is always most efficient to avoid priority inversion, either by avoiding pri-
ority scheduling or by using any given mutex only within threads of equal priority.
Equally, of course, these alternatives rarely prove practical when you need them
most.
You can use priority ceiling within almost any main program, even when you
don't control the code in libraries you use. That's because while it is common for
threads that call into library functions to lock library mutexes, it is not common
for threads created by a library to call into application code and lock application
mutexes. If you use a library that has "callbacks" into your code, you must either
ensure that those callbacks (and any functions they call) don't use the priority
ceiling mutexes or that no thread in which the callback might be invoked will run
at a priority above the ceiling priority of the mutex.
188
CHAPTER 5 Advanced threaded programming
#### 5.5.5.2 Priority inheritance mutexes
The other Pthreads mutex protocol is priority inheritance. In the priority inher-
itance protocol, when a thread locks a mutex the thread's priority is controlled
through the mutex (Figure 5.4). When another thread needs to block on thai
mutex, it looks at the priority of the thread that owns the mutex. If the thread
that owns the mutex has a lower priority than the thread attempting to block on
the mutex, the priority of the owner is raised to the priority of the blocking
thread.
The priority increase ensures that the thread that has the mutex locked can-
not be preempted unless the waiting thread would also have been preempted-in
a sense, the thread owning the mutex is working on behalf of the higher-priority
thread. When the thread unlocks the mutex, the thread's priority is automatically
lowered to its normal priority and the highest-priority waiter is awakened. If a
second thread of even higher priority blocks on the mutex, the thread that has
the mutex blocked will again have its priority increased. The thread will still be
returned to its original priority when the mutex is unlocked.
The priority inheritance protocol is more general and powerful than priority
celling, but also more complicated and expensive. If a library package must make
use of priority scheduling, and cannot avoid use of a mutex from threads of differ-
ent priority, then priority inheritance is the only currently available solution. If
you are writing a main program, and know that none of your mutexes can be
locked by threads created within a library, then priority ceiling will accomplish
the same result as priority inheritance, and with less overhead.
thread 1
locks mi
i
priority
atex
r
time
thread 2 b
f~ waiting foi
locks
mutex
thread 1
unlocks mutex
thread 1 priority |
thread 2 priority |
FIGURE 5.4 Priority inheritance mutex operation
Threads and kernel entities 189
## 5.6 Threads and kernel entities
"Two lines!" cried the Mock Turtle. "Seals, turtles, salmon, and so on:
then, when you've cleared all the jelly-fish out of the way-"
'That generally takes some time," interrupted the Gryphon.
"-you advance twice-"
"Each with a lobster as a partner!" cried the Gryphon.
-Lewis Carroll, Alice's Adventures in Wonderland
Pthreads deliberately says very little about implementation details. This leaves
each vendor free to make decisions based on the needs of their users and to allow
the state of the art to advance by permitting innovation. The standard places a
few essential requirements on the implementation-enough that you can write
strictly conforming POSIX applications that do useful work with threads and will
be able to run correctly on all conforming implementations of the standard.
Any Pthreads implementation must ensure that "system services invoked by
one thread do not suspend other threads" so that you do not need to worry that
calling read might block all threads in the process on some systems. On the other
hand, this does not mean that your process will always have the maximum possi-
ble level of concurrency.
Nevertheless, when using a system it is often useful to understand the ways in
which the system may be implemented. When writing ANSI C expressions, for
example, it is often helpful to understand what the code generator, and even the
hardware, will do with those expressions. With that in mind, the following sec-
tions describe, briefly, a few of the main variations you're likely to encounter.
The important terms used in these sections are "Pthreads thread," "kernel
entity," and "processor." "Pthreads thread" means a thread that you created by
calling pthread\_create, represented by an identifier of type pthread\_t. These
are the threads that you control using Pthreads interfaces. By "processor," I refer
to the physical hardware, the particular thing of which a "multiprocessor" has
more than one.
Most operating systems have at least one additional level of abstraction
between "Pthreads thread" and "processor" and I refer to that as a "kernel entity,"
because that is the term used by Pthreads. In some systems, "kernel entity" may
be a traditional UNIX process. It may be a Digital UNIX Mach thread, or a Solaris
2.x LWP, or an IRIX sproc process. The exact meaning of "kernel entity," and how
it interacts with the Pthreads thread, is the crucial difference between the three
models described in the following sections.
* Strictly conforming is used by POSIX to mean something quite specific: a strictly conforming
application is one that does not rely on any options or extensions to the standard and requires
only the specified minimum value for all implementation limits (but will work correctly with any
allowed value).
190 CHAPTER 5 Advanced threaded programming
### 5.6.1 Many-to-one (user level)
The many-to-one method is also sometimes called a "library implementation."
In general, "many-to-one" implementations are designed for operating systems
with no support for threads. Pthreads implementations that run on generic UNIX
kernels usually fall into this category-for example, the classic DCE threads ref-
erence implementation, or the SunOS 4.x LWP package (no relation to the Solaris
2.x LWP, which is a kernel entity).
Many-to-one implementations cannot take advantage of parallelism on a mul-
tiprocessor, and any blocking system service, for example, a call to read, will
block all threads in the process. Some implementations may help you avoid this
problem by using features such as UNIX nonblocking I/O, or POSIX. lb asyn-
chronous I/O, where available. However, these features have limitations; for
example, not all device drivers support nonblocking I/O, and traditional UNIX
disk file system response is usually considered "instantaneous" and will ignore
the nonblocking I/O mode.
Some many-to-one implementations may not be tightly integrated with the
ANSI C library's support functions, and that can cause serious trouble. The stdio
functions, for example, might block the entire process (and all threads) while one
thread waits for you to enter a command. Any many-to-one implementation that
conforms to the Pthreads standard, however, has gotten around these problems,
perhaps by including a special version of stdio and other functions.
When you require concurrency but do not need parallelism, a many-to-one
implementation may provide the best thread creation performance, as well as the
best context switch performance for voluntary blocking using mutexes and con-
dition variables. It is fast because the Pthreads library saves and restores thread
context entirely in user mode. You can, for example, create a lot of threads and
block most of them on condition variables (waiting for some external event) very
quickly, without involving the kernel at all.
Figure 5.5 shows the mapping of Pthreads threads (left column) to the kernel
entity (middle column), which is a process, to physical processors (right column).
In this case, the process has four Pthreads threads, labeled "Pthread 1" through
"Pthread 4." The Pthreads library schedules the four threads onto the single pro-
cess in user mode by swapping register state (SP, general registers, and so forth).
The library may use a timer to preempt a Pthreads thread that runs too long. The
kernel schedules the process onto one of the two physical processors, labeled
"processor 1" and "processor 2." The important characteristics of this model are
shown in Table 5.2.
Threads and kernel entities
191
Pthread 1
Pthread 2
Pthread 3
kernel entity 1
processor 1
Pthread 4
processor 2
FIGURE 5.5 Many-to-one thread mapping
Advantages
Fastest context switch time.
Simple; the implementation
may even be (mostly) portable.*
Disadvantages
Potentially long latency during system service blocking.
Single-process applications cannot take advantage of
multiprocessor hardware.
* The DCE threads user-mode scheduler can usually be ported to new operating systems in a few
days, involving primarily new assembly language for the register context switching routines. We use
the motto "Some Assembly Required."
TABLE 5.2 Many-to-one thread scheduling
### 5.6.2 One-to-one (kernel level)
One-to-one thread mapping is also sometimes called a "kernel thread" imple-
mentation. The Pthreads library assigns each thread to a kernel entity. It
generally must use blocking kernel functions to wait on mutexes and condition
variables. While synchronization may occur either within the kernel or in user
mode, thread scheduling occurs within the kernel.
Pthreads threads can take full advantage of multiprocessor hardware in a
one-to-one implementation without any extra effort on your part, for example,
separating your code into multiple processes. When a thread blocks in the kernel,
192 CHAPTER 5 Advanced threaded programming
it does not affect other threads any more than the blocking of a normal UNIX pro-
cess affects other processes. One thread can even process a page fault without
affecting other threads.
One-to-one implemenations suffer from two main problems. The first is that
they do not scale well. That is, each thread in your application is a kernel entity.
Because kernel memory is precious, kernel objects such as processes and
threads are often limited by preallocated arrays, and most implementations will
limit the number of threads you can create. It will also limit the number of
threads that can be created on the entire system-so depending on what other
processes are doing, your process may not be able to reach its own limit.
The second problem is that blocking on a mutex and waiting on a condition
variable, which happen frequently in many applications, are substantially more
expensive on most one-to-one implementations, because they require entering
the machine's protected kernel mode. Note that locking a mutex, when it was not
already locked, or unlocking a mutex, when there are no waiting threads, may be
no more expensive than on a many-to-one implementation, because on most sys-
tems those functions can be completed in user mode.
A one-to-one implementation can be a good choice for CPU-bound applica-
tions, which don't block very often. Many high-performance parallel applications
begin by creating a worker thread for each physical processor in the system, and.
once started, the threads run independently for a substantial time period. Such
applications will work well because they do not strain the kernel by creating a lot
of threads, and they don't require a lot of calls into the kernel to block and
unblock their threads.
Figure 5.6 shows the mapping of Pthreads threads (left column) to kernel enti-
ties (middle column) to physical processors (right column). In this case, the
process has four Pthreads threads, labeled "Pthread 1" through "Pthread 4."
Each Pthreads thread is permanently bound to the corresponding kernel entity.
The kernel schedules the four kernel entities (along with those from other pro-
cesses) onto the two physical processors, labeled "processor 1" and "processor 2."
The important characteristics of this model are shown in Table 5.3.
Threads and kernel entities
193
Pthread 1
Pthread 2
Pthread 3
Pthread 4
P \>
I \>
jg \>
i \>|
kernel entity 1
kernel entity 2
kernel entity 3
kernel entity 4
1
processor 1 |
processor 2 m
FIGURE 5.6 One-to-one thread mapping
Advantages
Can take advantage of
multiprocessor hard-
ware within a single
process.
No latency during sys-
tem service blocking.
Disadvantages
Relatively slow thread context switch (calls into
kernel).
Poor scaling when many threads are used, because
each Pthreads thread takes kernel resources from
the system.
TABLE 5.3 One-to-one thread scheduling
### 5.6.3 Many-to-few (two level)
The many-to-few model tries to merge the advantages of both the many-to-
one and one-to-one models, while avoiding their disadvantages. This model
requires cooperation between the user-level Pthreads library and the kernel.
They share scheduling responsibilities and may communicate information about
the threads between each other.
194
CHAPTER 5 Advanced threaded programming
When the Pthreads library needs to switch between two threads, it can do so
directly, in user mode. The new Pthreads thread runs on the same kernel entity
without intervention from the kernel. This gains the performance benefit of
many-to-one implementations for the most common cases, when a thread blocks
on a mutex or condition variable, and when a thread terminates.
When the kernel needs to block a thread, to wait for an I/O or other resource,
it does so. The kernel may inform the Pthreads library, as in Digital UNIX 4.0, so
that the library can preserve process concurrency by immediately scheduling a
new Pthreads thread, somewhat like the original "scheduler activations" model
proposed by the famous University of Washington research [Anderson, 1991]. Or.
the kernel may simply block the kernel entity, in which case it may allow pro-
grammers to increase the number of kernel entities that are allocated to the
process, as in Solaris 2.5-otherwise the process could be stalled when all kernel
entities have blocked, even though other user threads are ready to run.
Many-to-few implementations excel in most real-world applications, because
in most applications, threads perform a mixture of CPU-bound and I/O-bound
operations, and block both in I/O and in Pthreads synchronization. Most applica-
tions also create more threads than there are physical processors, either directly
or because an application that creates a few threads also uses a parallel library
that creates a few threads, and so forth.
Figure 5.7 shows the mapping of Pthreads threads (left column) to kernel enti-
ties (middle column) to physical processors (right column). In this case, the
process has four Pthreads threads, labeled "Pthread 1" through "Pthread 4." The
Pthreads library creates some number of kernel entities at initialization (and may
create more later). Typically, the library will start with one kernel entity (labeled
"kernel entity 1" and "kernel entity 2") for each physical processor. The kernel
schedules these kernel entities (along with those from other processes) onto the
Pthread 1
Pthread 2
Pthread 3
Pthread 4
/
kernel entity 1
kernel entity 2
processor 1
processor 2
FIGURE 5.7 Many-to-few thread mapping
Threads and kernel entities
195
two physical processors, labeled "processor 1" and "processor 2." The important
characteristics of this model are shown in Table 5.4.
Advantages
Can take advantage of multiprocessor
hardware within a process.
Most context switches are in user mode
(fast).
Scales well; a process may use one ker-
nel entity per physical processor, or "a
few" more.
Little latency during system service
blocking.
Disadvantages
More complicated than other models.
Programmers lose direct control over
kernel entities, since the thread's pri-
ority may be meaningful only in user
mode.
TABLE 5.4 Many-to-jew thread scheduling
# 6 POSIX adjusts to threads
"Who are you ?" said the Caterpillar.
This was not an encouraging opening for a conversation.
Alice replied, rather shyly, "I-/ hardly know, Sir,
lust at present-at least I know who I was when / got up this morning, but
I think I must have been changed several times since then."
-Lewis Carroll, Alice's Adventures in Wonderland
Pthreads changes the meaning of a number of traditional POSIX process func-
tions. Most of the changes are obvious, and you'd probably expect them even if
the standard hadn't added specific wording. When a thread blocks for I/O, for
example, only the calling thread blocks, while other threads in the process can
continue to run.
But there's another class of POSIX functions that doesn't extend into the
threaded world quite so unambiguously. For example, when you fork a threaded
process, what happens to the threads? What does exec do in a threaded process?
What happens when one of the threads in a threaded process calls exit?
## 6.1 fork
I Avoid using fork in a threaded program (if you can)
unless you intend to exec a new program immediately.
When a threaded process calls fork to create a child process, Pthreads speci-
fies that only the thread calling fork exists in the child. Although only the calling
thread exists on return from fork in the child process, all other Pthreads states
remain as they were at the time of the call to fork. In the child process, the
thread has the same thread state as in the parent. It owns the same mutexes, has
the same value for all thread-specific data keys, and so forth. All mutexes and
condition variables exist, although any threads that were waiting on a synchroni-
zation object at the time of the fork are no longer waiting. (They don't exist in the
child process, so how could they be waiting?)
Pthreads does not "terminate" the other threads in a forked process, as if they
exited with pthread\_exit or even as if they were canceled. They simply cease to
exist. That is, the threads do not run thread-specific data destructors or cleanup
handlers. This is not a problem if the child process is about to call exec to run a
197
198 CHAPTER 6 POSIX adjusts to threads
new program, but if you use fork to clone a threaded program, beware that you
may lose access to memory, especially heap memory stored only as thread-
specific data values.
I The state of mutexes is not affected by a fork. If it was locked in the
parent it is locked in the child!
If a mutex was locked at the time of the call to fork, then it is still locked in
the child. Because a locked mutex is owned by the thread that locked it, the
mutex can be unlocked in the child only if the thread that locked the mutex was
the one that called fork. This is important to remember-if another thread has a
mutex locked when you call fork, you will lose access to that mutex and any data
controlled by that mutex.
Despite the complications, you can fork a child that continues running and
even continues to use Pthreads. You must use fork handlers carefully to protect
your mutexes and the shared data that the mutexes are protecting. Fork han-
dlers are described in Section 6.1.1.
Because thread-specific data destructors and cleanup handlers are not called,
you may need to worry about memory leaks. One possible solution would be to
cancel threads created by your subsystem in the prepare fork handler, and wait
for them to terminate before allowing the fork to continue (by returning), and
then create new threads in the parent handler that is called after fork completes.
This could easily become messy, and I am not recommending it as a solution.
Instead, take another look at the warning back at the beginning of this section:
Avoid using fork in threaded code except where the child process will immedi-
ately exec a new program.
POSIX specifies a small set of functions that may be called safely from within
signal-catching functions ("async-signal safe" functions), and fork is one of them.
However, none of the POSIX threads functions is async-signal safe (and there are
good reasons for this, because being async-signal safe generally makes a function
substantially more expensive). With the introduction of fork handlers, however, a
call to fork is also a call to some set of fork handlers.
The purpose of a fork handler is to allow threaded code to protect synchroni-
zation state and data invariants across a fork, and in most cases that requires
locking mutexes. But you cannot lock mutexes from a signal-catching function.
So while it is legal to call fork from within a signal-catching function, doing so
may (beyond the control or knowledge of the caller) require performing other
operations that cannot be performed within a signal-catching function.
This is an inconsistency in the POSIX standard that will need to be fixed.
Nobody yet knows what the eventual solution will be. My advice is to avoid using
fork in a signal-catching function.
fork 199
### 6.1.1 Fork handlers
```c
int pthread\_atfork (void (*prepare)(void), I
void (*parent)(void), void (*child)(void)); I
```
Pthreads added the pthread\_atf ork "fork handler" mechanism to allow your
code to protect data invariants across fork. This is somewhat analogous to
atexit, which allows a program to perform cleanup when a process terminates.
With pthread\_atf ork you supply three separate handler addresses. The prepare
fork handler is called before the fork takes place in the parent process. The parent
fork handler is called after the fork in the parent process, and the child fork han-
dler is called after the fork in the child process.
I If you write a subsystem that uses mutexes and does not establish
I fork handlers, then that subsystem will not function correctly in a
I child process after a fork.
Normally a prepare fork handler locks all mutexes used by the associated code
(for a library or an application) in the correct order to prevent deadlocks. The
thread calling fork will block in the prepare fork handler until it has locked all the
mutexes. That ensures that no other threads can have the mutexes locked or be
modifying data that the child might need. The parent fork handler need only
unlock all of those mutexes, allowing the parent process and all threads to con-
tinue normally.
The child fork handler may often be the same as the parent fork handler; but
sometimes you'll need to reset the program or library state. For example, if you
use "daemon" threads to perform functions in the background you'll need to either
record the fact that those threads no longer exist or create new threads to per-
form the same function in the child. You may need to reset counters, free heap
memory, and so forth.
I Your fork handlers are only as good as everyone else's fork handlers.
The system will run all prepare fork handlers declared in the process when
any thread calls fork. If you code your prepare and child fork handlers correctly
then, in principle, you will be able to continue operating in the child process. But
what if someone else didn't supply fork handlers or didn't do it right? The ANSI C
library on a threaded system, for example, must use a set of mutexes to synchro-
nize internal data, such as stdio file streams.
If you use an ANSI C library that doesn't supply fork handlers to prepare those
mutexes properly for a fork, for example, then, sometimes, you may find that
200 CHAPTER 6 POSIX adjusts to threads
your child process hangs when it calls printf, because another thread in the
parent process had the mutex locked when your thread called fork. There's often
nothing you can do about this type of problem except to file a problem report
against the system. These mutexes are usually private to the library, and aren't
visible to your code-you can't lock them in your prepare handler or before calling
fork.
The program atfork.c shows the use of fork handlers. When run with no
argument, or with a nonzero argument, the program will install fork handlers.
When run with a zero argument, such as atf ork 0, it will not.
With fork handlers installed, the result will be two output lines reporting the
result of the fork call and, in parentheses, the pid of the current process. With-
out fork handlers, the child process will be created while the initial thread owns
the mutex. Because the initial thread does not exist in the child, the mutex can-
not be unlocked, and the child process will hang-only the parent process will
print its message.
13-25 Function fork\_prepare is the prepare handler. This will be called by fork, in
the parent process, before creating the child process. Any state changed by this
function, in particular, mutexes that are locked, will be copied into the child pro-
cess. The f ork\_prepare function locks the program's mutex.
31-42 Function f ork\_parent is the parent handler. This will be called by fork, in the
parent process, after creating the child process. In general, a parent handler
should undo whatever was done in the prepare handler, so that the parent pro-
cess can continue normally. The f ork\_parent function unlocks the mutex that
was locked by f ork\_prepare.
48-60 Function fork\_child is the child handler. This will be called by fork, in the
child process. In most cases, the child handler will need to do whatever was done
in the f ork\_parent handler to "unlock" the state so that the child can continue.
It may also need to perform additional cleanup, for example, f ork\_child sets the
self\_pid variable to the child process's pid as well as unlocking the process
mutex.
65-91 After creating a child process, which will continue executing the thread\_
routine code, the thread\_routine function locks the mutex. When run with fork
handlers, the fork call will be blocked (when the prepare handler locks the mutex)
until the mutex is available. Without fork handlers, the thread will fork before
main unlocks the mutex, and the thread will hang in the child at this point.
99-106 The main program declares fork handlers unless the program is run with an
argument of 0.
108-123 The main program locks the mutex before creating the thread that will fork. It
then sleeps for several seconds, to ensure that the thread will be able to call fork
while the mutex is locked, and then unlocks the mutex. The thread running
thread\_routine will always succeed in the parent process, because it will simply
block until main releases the lock.
However, without the fork handlers, the child process will be created while the
mutex is locked. The thread (main) that locked the mutex does not exist in the
child, and cannot unlock the mutex in the child process. Mutexes can be unlocked
fork 201
in the child only if they were locked by the thread that called fork-and fork han-
dlers provide the best way to ensure that.
| atfork.c
1 #include \<sys/types.h\>
2 #include \<pthread.h\>
3 #include \<sys/wait.h\>
4 #include "errors.h"
5
6 pid\_t self\_pid; /* pid of current process */
7 pthread\_mutex\_t mutex = PTHREAD\_MUTEX\_INITIALIZER;
8
9 /*
10 * This routine will be called prior to executing the fork,
11 * within the parent process.
12 */
13 void fork\_prepare (void)
14 {
15 int status;
16
17 /*
18 * Lock the mutex in the parent before creating the child,
19 * to ensure that no other thread can lock it (or change any
20 * associated shared state) until after the fork completes.
21 */
22 status = pthread\_mutex\_lock (Smutex);
23 if (status != 0)
24 err\_abort (status, "Lock in prepare handler");
25 }
26
27 /*
28 * This routine will be called after executing the fork, within
29 * the parent process
30 */
31 void fork\_parent (void)
32 {
33 int status;
34
35 /*
36 * Unlock the mutex in the parent after the child has been
37 * created.
38 */
39 status = pthread\_mutex\_unlock (Smutex);
40 if (status != 0)
41 err\_abort (status, "Unlock in parent handler");
42 }
43
202 CHAPTER 6 POSDC adjusts to threads
44 /*
45 * This routine will be called after executing the fork, within
46 * the child process.
47 */
48 void fork\_child (void)
49 {
50 int status;
51
52 /*
53 * Update the file scope "self\_pid" within the child process, and
54 * unlock the mutex.
55 */
56 self\_pid = getpid ();
57 status = pthread\_mutex\_unlock (Smutex);
58 if (status != 0)
59 err\_abort (status, "Unlock in child handler");
60 }
61
62 /*
63 * Thread start routine, which will fork a new child process.
64 */
65 void *thread\_routine (void *arg)
66 {
67 pid\_t child\_pid;
68 int status;
69
70 child\_pid = fork ();
71 if (child\_pid == (pid\_t)-l)
72 errno\_abort ("Fork");
73
74 /*
75 * Lock the mutex - without the atfork handlers, the mutex will
76 * remain locked in the child process and this lock attempt will
77 * hang (or fail with EDEADLK) in the child.
78 */
79 status = pthread\_mutex\_lock (Smutex);
80 if (status != 0)
81 err\_abort (status, "Lock in child");
82 status = pthread\_mutex\_unlock (Smutex);
83 if (status != 0)
84 err\_abort (status, "Unlock in child");
85 printf ("After fork: %d (%d)\n", child\_pid, selfjpid);
86 if (child\_pid != 0) {
87 if ((pid\_t)-l == waitpid (child\_pid, (int*H, 0))
88 errno\_abort ("Wait for child");
89 }
90 return NULL;
91 }
92
93 int main (int argc, char *argv[])
fork 203
94 {
95 pthread\_t fork\_thread;
96 int atfork\_flag = 1;
97 int status;
98
99 if (argc \> 1)
100 atfork\_flag = atoi (argv[l]);
101 if (atfork\_flag) {
102 status = pthread\_atfork (
103 fork\_prepare, fork\_parent, fork\_child);
104 if (status != 0)
105 err\_abort (status, "Register fork handlers");
106 }
107 self\_pid = getpid ( );
108 status = pthread\_mutex\_lock (Smutex);
109 if (status != 0)
110 err\_abort (status, "Lock mutex");
111 /*
112 * Create a thread while the mutex is locked. It will fork a
113 * process, which (without atfork handlers) will run with the
114 * mutex locked.
115 */
116 status = pthread\_create (
117 &fork\_thread, NULL, thread\_routine, NULL);
118 if (status != 0)
119 err\_abort (status, "Create thread");
120 sleep E);
121 status = pthread\_mutex\_unlock (Smutex);
122 if (status != 0)
123 err\_abort (status, "Unlock mutex");
124 status = pthread\_join (fork\_thread, NULL);
125 if (status != 0)
126 err\_abort (status, "Join thread");
127 return 0;
128 }
| atfork.c
Now, imagine you are writing a library that manages network server connec-
tions, and you create a thread for each network connection that listens for service
requests. In your prepare fork handler you lock all of the library's mutexes to make
sure the child's state is consistent and recoverable. In your parent fork handler
you unlock those mutexes and return. When designing the child fork handler, you
need to decide exactly what a fork means to your library. If you want to retain all
network connections in the child, then you would create a new listener thread for
each connection and record their identifiers in the appropriate data structures
before releasing the mutexes. If you want the child to begin with no open connec-
tions, then you would locate the existing parent connection data structures and
free them, closing the associated files that were propagated by fork.
204 CHAPTER 6 POSIX adjusts to threads
## 6.2 exec
The exec function isn't affected much by the presence of threads. The func-
tion of exec is to wipe out the current program context and replace it with a new
program. A call to exec immediately terminates all threads in the process except
the thread calling exec. They do not execute cleanup handlers or thread-specific
data destructors-the threads simply cease to exist.
All synchronization objects also vanish, except for pshared mutexes (mutexes
created using the pthread\_process\_shared attribute value) and pshared condi-
tion variables, which remain usable as long as the shared memory is mapped by
some process. You should, however, unlock any pshared mutexes that the cur-
rent process may have locked-the system will not unlock them for you.
## 6.3 Process exit
In a nonthreaded program, an explicit call to the exit function has the same
effect as returning from the program's main function. The process exits. Pthreads
adds the pthread\_exit function, which can be used to cause a single thread to
exit while the process continues. In a threaded program, therefore, you call exit
when you want the process to exit, or pthread\_exit when you want only the call-
ing thread to exit.
In a threaded program, main is effectively the "thread start function" for the
process's initial thread. Although returning from the start function of any other
thread terminates that thread just as if it had called pthread\_exit, returning
from main terminates the process. All memory (and threads) associated with the
process evaporate. Threads do not run cleanup handlers or thread-specific data
destructors. Calling exit has the same effect.
When you don't want to make use of the initial thread or make it wait for other
threads to complete, you can exit from main by calling pthread\_exit rather than
by returning or calling exit. Calling pthread\_exit from main will terminate the
initial thread without affecting the other threads in the process, allowing them to
continue and complete normally.
The exit function provides a simple way to shut down the entire process. For
example, if a thread determines that data has been severely corrupted by some
error, it may be dangerous to allow the program to continue to operate on the
data. When the program is somehow broken, it might be dangerous to attempt to
shut down the application threads cleanly. In that case, you might call exit to stop
all processing immediately.
## 6.4 Stdio
Pthreads specifies that the ANSI C standard I/O [stdio) functions are thread-
safe. Because the stdio package requires static storage for output buffers and file
Stdio 205
state, stdio implementations will use synchronization, such as mutexes or
semaphores.
### 6.4.1 f lockf ile and f unlockf ile
```c
void flockfile (FILE *file);
int ftrylockfile (FILE *file);
void funlockfile (FILE *file);
```
In some cases, it is important that a sequence of stdio operations occur in
uninterrupted sequence; for example, a prompt followed by a read from the ter-
minal, or two writes that need to appear together in the output file even if another
thread attempts to write data between the two stdio calls. Therefore, Pthreads
adds a mechanism to lock a file and specifies how file locking interacts with inter-
nal stdio locking. To write a prompt string to stdin and read a response from
stdout without allowing another thread to read from stdin or write to stdout
between the two, you would need to lock both stdin and stdout around the two
calls as shown in the following program, f lock.c.
19-20 This is the important part: Two separate calls to f lockf ile are made, one for
each of the two file streams. To avoid possible deadlock problems within stdio,
Pthreads recommends always locking input streams before output streams,
when you must lock both. That's good advice, and I've taken it by locking stdin
before stdout.
29-30 The two calls to funlockfile must, of course, be made in the opposite order.
Despite the specialized call, you are effectively locking mutexes within the stdio
library, and you should respect a consistent lock hierarchy.
| flock.c
1 #include \<pthread.h\>
2 #include "errors.h"
3
4 /*
5 * This routine writes a prompt to stdout (passed as the thread's
6 * "arg"), and reads a response. All other I/O to stdin and stdout
7 * is prevented by the file locks until both prompt and fgets are
8 * complete.
9 */
10 void *prompt\_routine (void *arg)
11 {
12 char *prompt = (char*)arg;
13 char *string;
14 int len;
15
16 string = (char*)malloc A28);
206 CHAPTER 6 POSIX adjusts to threads
17 if (string == NULL)
18 errno\_abort ("Alloc string");
19 flockfile (stdin);
20 flockfile (stdout);
21 printf (prompt);
22 if (fgets (string, 128, stdin) == NULL)
23 string[0] = '\0';
24 else {
25 len = strlen (string);
26 if (len \> 0 && string[len-l] == '\n')
27 string!len-1] = '\0';
28 }
29 funlockfile (stdout);
30 funlockfile (stdin);
31 return (void*)string;
32 }
33
34 int main (int argc, char *argv[])
35 {
36 pthread\_t threadl, thread2, thread3;
37 char *string;
38 int status;
39
40 #ifdef sun
41 /*
42 * On Solaris 2.5, threads are not timesliced. To ensure
43 * that our threads can run concurrently, we need to
44 * increase the concurrency level.
45 */
46 DPRINTF (("Setting concurrency level to 4\n"));
47 thr\_setconcurrency D);
48 #endif
49 status = pthread\_create (
50 Sthreadl, NULL, prompt\_routine, "Thread 1\> ");
51 if (status != 0)
52 err\_abort (status, "Create thread");
53 status = pthread\_create (
54 &thread2, NULL, prompt\_routine, "Thread 2\> ");
55 if (status != 0)
56 err\_abort (status, "Create thread");
57 status = pthread\_create (
58 &thread3, NULL, prompt\_routine, "Thread 3\> ");
59 if (status != 0)
60 err\_abort (status, "Create thread");
61 status = pthread\_join (threadl, (void**)Sstring);
62 if (status != 0)
63 err\_abort (status, "Join thread");
64 printf ("Thread 1: \"%s\"\n", string);
65 free (string);
66 status = pthread join (thread2, (void**)&string);
Stdio 207
67 if (status != 0)
68 err\_abort (status, "Join thread");
69 printf ("Thread 1: \"%s\"\n", string);
70 free (string);
71 status = pthread\_join (thread3, (void**)sstring);
72 if (status != 0)
73 err\_abort (status, "Join thread");
74 printf ("Thread 1: \"%s\"\n", string);
75 free (string);
76 return 0;
77 }
| flock.c
You can also use the flockfile and funlockfile functions to ensure that a
series of writes is not interrupted by a file access from some other thread. The
f trylockf ile function works like pthread\_mutex\_trylock in that it attempts to
lock the file and, if the file is already locked, returns an error status instead of
blocking.
### 6.4.2 getchar\_unlocked and putchar\_unlocked
```c
int getc\_unlocked (FILE *stream);
int getchar\_unlocked (void);
int putc\_unlocked (int c, FILE *stream);
int putchar\_unlocked (int c);
```
ANSI C provides functions to get and put single characters efficiently into
stdio buffers. The functions getchar and putchar operate on stdin and stdout,
respectively, and getc and putc can be used on any stdio file stream. These are
traditionally implemented as macros for maximum performance, directly reading
or writing the file stream's data buffer. Pthreads, however, requires these func-
tions to lock the stdio stream data, to prevent code from accidentally corrupting
the stdio buffers.
The overhead of locking and unlocking mutexes will probably vastly exceed
the time spent performing the character copy, so these functions are no longer
high performance. Pthreads could have denned new functions that provided the
locked variety rather than redefining the existing functions; however, the result
would be that existing code would be unsafe for use in threads. The working
group decided that it was preferable to make existing code slower, rather than to
make it incorrect.
Pthreads adds new functions that replace the old high-performance macros with
essentially the same implementation as the traditional macros. The functions getc\_
unlocked, putc\_unlocked, getchar\_unlocked, and putchar\_unlocked do not per-
form any locking, so you must use flockfile and funlockfile around any
208 CHAPTER 6 POSIX adjusts to threads
sequence of these operations. If you want to read or write a single character, you
should usually use the locked variety rather than locking the file stream, calling the
new unlocked get or put function, and then unlocking the file stream.
If you want to perform a sequence of fast character accesses, where you would
have previously used getchar and putchar, you can now use getchar\_unlocked
and putchar\_unlocked. The following program, putchar.c, shows the difference
between using putchar and using a sequence of putchar\_unlocked calls within a
file lock.
9-20 When the program is run with a nonzero argument or no argument at all, it
creates threads running the lock\_routine function. This function locks the std-
out file stream, and then writes its argument (a string) to stdout one character at
a time using putchar\_unlocked.
28-37 When the program is run with a zero argument, it creates threads running the
unlock\_routine function. This function writes its argument to stdout one char-
acter at a time using putchar. Although putchar is internally synchronized to
ensure that the stdio buffer is not corrupted, the individual characters may
appear in any order.
| putchar.c
1 #include \<pthread.h\>
2 #include "errors.h"
3
4 /*
5 * This function writes a string (the function's arg) to stdout,
6 * by locking the file stream and using putchar\_unlocked to write
7 * each character individually.
8 */
9 void *lock\_routine (void *arg)
10 {
11 char *pointer;
12
13 flockfile (stdout);
' 14 for (pointer = arg; *pointer != '\0'; pointer++) {
15 putchar\_unlocked (*pointer);
16 sleep A);
17 }
18 funlockfile (stdout);
19 return NULL;
20 }
21
22 /*
23 * This function writes a string (the function's arg) to stdout,
24 * by using putchar to write each character individually.
25 * Although the internal locking of putchar prevents file stream
26 * corruption, the writes of various threads may be interleaved.
27 */
28 void *unlock routine (void *arg)
Thread-safe functions 209
29 {
30 char *pointer;
31
32 for (pointer = arg; *pointer != '\0'; pointer++) {
33 putchar (*pointer);
34 sleep A);
35 }
36 return NULL;
37 }
38
39 int main (int argc, char *argv[])
40 {
41 pthread\_t threadl, thread2, thread3;
42 int flock\_flag = 1;
43 void *(*thread\_func)(void *);
44 int status;
45
46 if (argc \> 1)
47 flock\_flag = atoi (argv[l]);
48 if (flock\_flag)
49 thread\_func = lock\_routine;
50 else
51 thread\_func = unlock\_routine;
52 status = pthread\_create (
53 Sthreadl, NULL, thread\_func, "this is thread l\n");
54 if (status != 0)
55 err\_abort (status, "Create thread");
56 status = pthread\_create (
57 &thread2, NULL, thread\_func, "this is thread 2\n");
58 if (status != 0)
59 err\_abort (status, "Create thread");
60 status = pthread\_create (
61 &thread3, NULL, thread\_func, "this is thread 3\n");
62 if (status != 0)
63 err\_abort (status, "Create thread");
64 pthread\_exit (NULL);
65 }
| putchar.c
## 6.5 Thread-safe functions
Although ANSI C and POSIX 1003.1-1990 were not developed with threads in
mind, most of the functions they define can be made thread-safe without chang-
ing the external interface. For example, although malloc and free must be
changed to support threads, code calling these functions need not be aware of the
changes. When you call malloc, it locks a mutex (or perhaps several mutexes) to
210 CHAPTER 6 POSIX adjusts to threads
perform the operation, or may use other equivalent synchronization mecha-
nisms. But your code just calls malloc as it always has, and it does the same
thing as always.
In two main classes of functions, this is not true:
? Functions that traditionally return pointers to internal static buffers, for
example, asctime. An internal mutex wouldn't help, since the caller will
read the formatted time string some time after the function returns and,
therefore, after the mutex has been unlocked.
? Functions that require static context between a series of calls, for example,
strtok, which stores the current position within the token string in a local
static variable. Again, using a mutex within strtok wouldn't help, because
other threads would be able to overwrite the current location between two
calls.
In these cases, Pthreads has defined variants of the existing functions that are
thread-safe, which are designated by the suffix "\_r" at the end of the function
name. These variants move context outside the library, under the caller's control.
When each thread uses a private buffer or context, the functions are thread-safe.
You can also share context between threads if you want-but the caller must pro-
vide synchronization between the threads. If you want two threads to search a
directory in parallel, you must synchronize their use of the shared struct
dirent passed to readdir\_r.
A few existing functions, such as ctermid, are already thread-safe as long as
certain restrictions are placed on parameters. These restrictions are noted in the
following sections.
### 6.5.1 User and terminal identification
```c
int getlogin\_r (char *name, size\_t namesize);
char *ctermid (char *s);
int ttyname\_r (int fildes,
char *name, size t namesize);
```
These functions return data to a caller-specified buffer. For getlogin\_r,
namesize must be at least LOGIN\_NAME\_MAX characters. For ttyname\_r, namesize
must be at least tty\_name\_max characters. Either function returns a value of 0 on
success, or an error number on failure. In addition to errors that might be
returned by getlogin or ttyname, getlogin\_r and ttyname\_r may return ERANGE
to indicate that the name buffer is too small.
Pthreads requires that when ctermid (which has not changed) is used in a
threaded environment, the s return argument must be specified as a pointer to a
Thread-safe Junctions 211
character buffer having at least L\_ctermid bytes. It was felt that this restriction
was sufficient, without defining a new variant to also specify the size of the buffer.
Program getlogin.c shows how to call these functions. Notice that these func-
tions do not depend on threads, or \<pthread.h\>, in any way, and may even be
provided on systems that don't support threads.
| getlogin.c
1 #include \<limits.h\>
2 #include "errors.h"
3
4 /*
5 * If either TTY\_NAME\_MAX or LOGIN\_NAME\_MAX are undefined
6 * (this means they are "indeterminate" values), assume a
7 * reasonable size (for simplicity) rather than using sysconf
8 * and dynamically allocating the buffers.
9 */
10 #ifndef TTY\_NAME\_MAX
11 # define TTY\_NAME\_MAX 128
12 #endif
13 #ifndef LOGIN\_NAME\_MAX
14 # define LOGIN\_NAME\_MAX 32
15 #endif
16
17 int main (int argc, char *argv[])
18 {
19 char login\_str[LOGIN\_NAME\_MAX];
20 char stdin\_str[TTY\_NAME\_MAX];
21 char cterm\_str[L\_ctermid], *cterm\_str\_ptr;
22 int status;
23
24 status = getlogin\_r (login\_str, sizeof (login\_str));
25 if (status != 0)
26 err\_abort (status, "Get login");
27 cterm\_str\_ptr = ctermid (cterm\_str);
28 if (cterm\_str\_ptr == NULL)
29 errno\_abort ("Get cterm");
30 status = ttyname\_r @, stdin\_str, sizeof (stdin\_str));
31 if (status != 0)
32 err\_abort (status, "Get stdin");
33 printf ("User: %s, cterm: %s, fd 0: %s\n",
34 login\_str, cterm\_str, stdin\_str);
35 return 0;
36 }
| getlogin.c
212 CHAPTER 6 POSIX adjusts to threads
### 6.5.2 Directory searching
```c
int readdir\_r (DIR *dirp, struct dirent *entry, I
struct dirent **result); I
```
This function performs essentially the same action as readdir. That is, it
returns the next directory entry in the directory stream specified by dirp. The dif-
ference is that instead of returning a pointer to that entry, it copies the entry into
the buffer specified by entry. On success, it returns 0 and sets the pointer speci-
fied by result to the buffer entry. On reaching the end of the directory stream, it
returns 0 and sets result to null. On failure, it returns an error number such as
EBADF.
Refer to program pipe. c, in Section 4.1, for a demonstration of using readdir\_r
to allow your threads to search multiple directories concurrently.
### 6.5.3 String token
```c
char *strtok\_r ( I
char *s, const char *sep, char **lasts); I
```
This function returns the next token in the string s. Unlike strtok, the con-
text (the current pointer within the original string) is maintained in lasts, which
is specified by the caller, rather than in a static pointer internal to the function.
In the first call of a series, the argument s gives a pointer to the string. In sub-
sequent calls to return successive tokens of that string, s must be specified as
NULL. The value lasts is set by strtok\_r to maintain the function's position
within the string, and on each subsequent call you must return that same value
of lasts. The strtok\_r function returns a pointer to the next token, or NULL
when there are no more tokens to be found in the original string.
### 6.5.4 Time representation
```c
char *asctime\_r (const struct tm *tm, char *buf);
char *ctime\_r (const time\_t *clock, char *buf);
struct tm *gmtime\_r (
const time\_t *clock, struct tm *result);
struct tm *localtime\_r (
const time t *clock, struct tm *result);
```
Thread-safe functions 213
The output buffers (buf and result) are supplied by the caller, instead of
returning a pointer to static storage internal to the functions. Otherwise, they are
identical to the traditional variants. The asctime\_r and ctime\_r routines, which
return ASCII character representations of a system time, both require that their
buf argument point to a character string of at least 26 bytes.
### 6.5.5 Random number generation
```c
int rand\_r (unsigned int *seed); I
```
The seed is maintained in caller-supplied storage (seed) rather than using
static storage internal to the function. The main problem with this interface is
that it is not usually practical to have a single seed shared by all application and
library code within a program. As a result, the application and each library will
generally have a separate "stream" of random numbers. Thus, a program con-
verted to use rand\_r instead of rand is likely to generate different results, even if
no threads are created. (Creating threads would probably change the order of
calls to rand, and therefore change results anyway.)
### 6.5.6 Group and user database
Group database:
```c
int getgrgid\_r (
gid\_t gid, struct group *grp, char *buffer,
size\_t bufsize, struct group **result);
int getgrnam\_r (
const char *name, struct group *grp,
char *buffer, size\_t bufsize,
struct group **result);
User database:
int getpwuid\_r (
uid\_t uid, struct passwd *pwd, char *buffer,
size\_t bufsize, struct passwd **result);
int getpwnam\_r (
const char *name, struct passwd *pwd,
char *buffer, size\_t bufsize,
struct passwd **result);
```
These functions store a copy of the group or user record (grp or pwd, respec-
tively) for the specified group or user (gid, uid, or name) in a buffer designated by
214 CHAPTER 6 POSIX adjusts to threads
the arguments buffer and bufsize. The function return value is in each case
either 0 for success, or an error number (such as erange when the buffer is too
small) to designate an error. If the requested record is not present in the group or
passwd database, the functions may return success but store the value null into
the result pointer. If the record is found and the buffer is large enough, result
becomes a pointer to the struct group or struct passwd record within buffer.
The maximum required size for buffer can be determined by calling sysconf
with the argument \_SC\_GETGR\_R\_SIZE\_MAX (for group data) or with the argument
SC GETPW R SIZE max (for user data).
## 6.6 Signals
Beware the Jabberwock, my son!
The jaws that bite, the claws that catch!
Beware the Jubjub bird, and shun
The frumious Bandersnatch!
-Lewis Carroll, Through the Looking-Glass
The history of the Pthreads signal-handling model is the most convoluted and
confusing part of the standard. There were several different viewpoints, and it
was difficult to devise a compromise that would satisfy everyone in the working
group (much less the larger and more diverse balloting group). This isn't surpris-
ing, since signals are complicated anyway, and have a widely divergent history in
the industry.
There were two primary conflicting goals:
? First, "signals should be completely compatible with traditional UNIX."
That means signal handlers and masks should remain associated with the
process. That makes them virtually useless with multiple threads, which is
as it should be since signals have complicating semantics that make it
difficult for signals and threads to coexist peacefully. Tasks should be
accomplished synchronously using threads rather than asynchronously
using signals.
? Second, "signals should be completely compatible with traditional UNIX."
This time, "compatible" means signal handlers and masks should be com-
pletely thread-private. Most existing UNIX code would then function
essentially the same running within a thread as it had within a process.
Code migration would be simplified.
The problem is that the definitions of "compatible" were incompatible. Although
many people involved in the negotiation may not agree with the final result, nearly
everyone would agree that those who devised the compromise did an extraordinar-
ily good job, and that they were quite courageous to attempt the feat.
Signals
215
I When writing threaded code, treat signals as Jabberwocks-
curious and potentially dangerous creatures to be
approached with caution, if at all.
It is always best to avoid using signals in conjunction with threads. At the same
time, it is often not possible or practical to keep them separate. When signals and
threads meet, beware. If at all possible, use only pthread\_sigmask to mask signals
in the main thread, and sigwait to handle signals synchronously within a single
thread dedicated to that purpose. If you must use sigaction (or equivalent) to
handle synchronous signals (such as sigsegv) within threads, be especially cau-
tious. Do as little work as possible within the signal-catching function.
### 6.6.1 Signal actions
All signal actions are process-wide. A program must coordinate any use of
sigaction between threads. This is nonmodular, but also relatively simple, and
signals have never been modular. A function that dynamically modifies signal
actions, for example, to catch or ignore sigfpe while it performs floating-point
operations, or sigpipe while it performs network I/O, will be tricky to code on a
threaded system.
While modifying the process signal action for a signal number is itself thread-
safe, there is no protection against some other thread setting a new signal action
immediately afterward. Even if the code tries to be "good" by saving the original
signal action and restoring it, it may be foiled by another thread, as shown in
Figure 6.1.
Signals that are not "tied" to a specific hardware execution context are deliv-
ered to one arbitrary thread within the process. That means a sigchld raised by
a child process termination, for example, may not be delivered to the thread that
created the child. Similarly, a call to kill results in a signal that may be delivered
to any thread.
Thread 1
sigaction(SIGFPE)
Generate sigfpe
Restore action
Thread 2
sigaction(SIGFPE)
restore action
Comments
Thread
Thread
l's signal action active.
2's signal action active.
Thread 1 signal is handled by the thread 2
signal action (but still in the context of
thread 1).
Thread
Thread
origina
1 restores original signal action.
2 restores thread 1 's signal action-
action is lost.
FIGURE 6.1 Nonmodularity of signal actions
216 CHAPTER 6 POSIX adjusts to threads
The synchronous "hardware context" signals, including sigfpe, sigsegv, and
SIGTRAP, are delivered to the thread that caused the hardware condition, never to
another thread.
I You cannot kill a thread by sending it a SIGKILL or stop a thread by
sending it a SIGSTOR
Any signal that affected a process still affects the process when multiple
threads are active, which means that sending a SIGKILL to a process or to any spe-
cific thread in the process (using pthread\_kill, which we'll get to in Section 6.6.3)
will terminate the process. Sending a SIGSTOP will cause all threads to stop until
a sigcont is received. This ensures that existing process control functions con-
tinue to work-otherwise most threads in a process could continue running when
you stopped a command by sending a sigstop. This also applies to the default
action of the other signals, for example, SIGSEGV, if not handled, will terminate
the process and generate a core file-it will not terminate only the thread that
generated the SIGSEGV.
What does this mean to a programmer? It has always been common wisdom
that library code should not change signal actions-that this is exclusively the
province of the main program. This philosophy becomes even more wise when
you are programming with threads. Signal actions must always be under the con-
trol of a single component, at least, and to assign that responsibility to the main
program makes the most sense in nearly all situations.
### 6.6.2 Signal masks
```c
int pthread\_sigmask (int how, I
const sigset\_t *set, sigset\_t *oset); I
```
Each thread has its own private signal mask, which is modified by calling
pthread\_sigmask. Pthreads does not specify what sigprocmask does within a
threaded process-it may do nothing. Portable threaded code does not call sig-
procmask. A thread can block or unblock signals without affecting the ability of
other threads to handle the signal. This is particularly important for synchronous
signals. It would be awkward if thread A were unable to process a sigfpe because
thread B was currently processing its own sigfpe or, even worse, because thread C
had blocked sigfpe. When a thread is created, it inherits the signal mask of the
thread that created it-if you want a signal to be masked everywhere, mask it first
thing in main.
Signals 217
### 6.6.3 pthreadjdll
```c
int pthread\_kill (pthread\_t thread, int sig); I
```
Within a process, one thread can send a signal to a specific thread (including
itself) by calling pthread\_kill. When calling pthread\_kill, you specify not only
the signal number to be delivered, but also the pthread\_t identifier for the thread
to which you want the signal sent. You cannot use pthread\_kill to send a signal to
a thread in another process, however, because a thread identifier (pthread\_t) is
meaningful only within the process that created it.
The signal sent by pthread\_kill is handled like any other signal. If the "tar-
get" thread has the signal masked, it will be marked pending against that thread.
If the thread is waiting for the signal in sigwait (covered in Section 6.6.4), the
thread will receive the signal. If the thread does not have the signal masked, and
is not blocked in sigwait, the current signal action will be taken.
Remember that, aside from signal-catching functions, signal actions affect the
process. Sending the SIGKILL signal to a specific thread using pthread\_kill will
kill the process, not just the specified thread. Use pthread\_cancel to get rid of a
particular thread (see Section 5.3). Sending sigstop to a thread will stop all
threads in the process until a SIGCONT is sent by some other process.
The raise function specified by ANSI C has traditionally been mapped to a kill
for the current process. That is, raise (SIGABRT) is usually the same as kill
(getpid ( ), SIGABRT).
With multiple threads, code calling raise is most likely to intend that the sig-
nal be sent to the calling thread, rather than to some arbitrary thread within the
process. Pthreads specifies that raise (SIGABRT) is the same as pthread\_kill
(pthread\_self (), SIGABRT).
The following program, susp.c, uses pthread\_kill to implement a portable
"suspend and resume" (or, equivalently, "suspend and continue") capability
much like that provided by the Solaris "UI threads" interfaces thr\_suspend and
thr\_continue. You call the thd\_suspend function with the pthread\_t of a thread,
and when the function returns, the specified thread has been suspended from
execution. The thread cannot execute until a later call to thd\_continue is made
with the same pthread\_t.
A request to suspend a thread that is already suspended has no effect. Calling
thd\_continue a single time for a suspended thread will cause it to resume execu-
tion, even if it had been suspended by multiple calls to thd\_suspend. Calling thd\_
continue for a thread that is not currently suspended has no effect.
"The algorithm (and most of the code) for susp.c was developed by a coworker of mine, Brian
Silver. The code shown here is a simplified version for demonstration purposes.
218
CHAPTER 6 POSIX adjusts to threads
Suspend and resume are commonly used to solve some problems, for example,
multithread garbage collectors, and may even work sometimes if the programmer
is very careful. This emulation of suspend and resume may therefore be valuable
to the few programmers who really need these functions. Beware, however, that
should you suspend a thread while it holds some resource (such as a mutex),
application deadlock can easily result.
6 The symbol ITERATIONS defines how many times the "target" threads will loop.
If this value is set too small, some or all of the threads will terminate before the
main thread has been able to suspend and continue them as it desires. If that
happens, the program will fail with an error message-increase the value of itera-
tions until the problem goes away.
12 The variable sentinel is used to synchronize between a signal-catching func-
tion and another thread. "Oh?" you may ask, incredulously. This mechanism is
not perfect-the suspending thread (the one calling thd\_suspend) waits in a loop,
yielding the processor until this sentinel changes state. The volatile storage
attribute ensures that the signal-catching function will write the value to mem-
ory.* Remember, you cannot use a mutex within a signal-catching function.
22-40 The suspend\_signal\_handler function will be established as the signal-
catching function for the "suspend" signal, SIGUSR1. It initializes a signal mask to
block all signals except SIGUSR2, which is the "resume" signal, and then waits for
that signal by calling sigsuspend. Just before suspending itself, it sets the senti-
nel variable to inform the suspending thread that it is no longer executing user
code-for most practical purposes, it is already suspended.
The purpose for this synchronization between the signal-catching function
and thd\_suspend is that, to be most useful, the thread calling thd\_suspend must
be able to know when the target thread has been successfully suspended. Simply
calling pthread\_kill is not enough, because the system might not deliver the
signal for a substantial period of time; we need to know when the signal has been
received.
47-51 The resume\_signal\_handler function will be established as the signal-catching
function for the "resume" signal, sigusri. The function isn't important, since the
signal is sent only to interrupt the call to sigsuspend in suspend\_signal\_handler.
susp.c
parti signal-catchingfunctions
#include \<pthread.h\>
#include \<signal.h\>
#include "errors.h"
#define THREAD\_COUNT
#define ITERATIONS
20
40000
* A semaphore, as described later in Section 6.6.6, would provide cleaner, and somewhat saf-
er, synchronization. The thd\_suspend would call sem\_wait on a semaphore with an initial value
of 0, and the signal-catching function would call sem\_post to wake it.
Signals 219
8 unsigned long thread\_count = THREAD\_COUNT;
9 unsigned long iterations = ITERATIONS;
10 pthread\_mutex\_t the\_mutex = PTHREAD\_MUTEX\_INITIALIZER;
11 pthread\_mutex\_t mut = PTHREAD\_MUTEX\_INITIALIZER;
12 volatile int sentinel = 0;
13 pthread\_once\_t once = PTHREAD\_ONCE\_INIT;
14 pthread\_t *array = NULL, null\_pthread = {0};
15 int bottom = 0;
16 int inited = 0;
17
18 /*
19 * Handle SIGUSR1 in the target thread, to suspend it until
20 * receiving SIGUSR2 (resume).
21 */
22 void
23 suspend\_signal\_handler (int sig)
24 {
25 sigset\_t signal\_set;
26
27 /*
28 * Block all signals except SIGUSR2 while suspended.
29 */
30 sigfillset (&signal\_set);
31 sigdelset (&signal\_set, SIGUSR2);
32 sentinel = 1;
33 sigsuspend (&signal\_set);
34
35 /*
36 * Once I'm here, I've been resumed, and the resume signal
37 * handler has been run to completion.
38 */
39 return;
40 }
41
42 /*
43 * Handle SIGUSR2 in the target thread, to resume it. Note that
44 * the signal handler does nothing. It exists only because we need
45 * to cause sigsuspend() to return.
46 */
47 void
48 resume\_signal\_handler (int sig)
49 {
50 return;
51 }
| susp.c part 1 signal-catchingfunctions
220 CHAPTER 6 POSIX adjusts to threads
The suspend\_init\_routine function dynamically initializes the suspend/
resume package when the first call to thd\_suspend is made. It is actually called
indirectly by pthread\_once.
15-16 It allocates an initial array of thread identifiers, which is used to record the
identifiers of all threads that have been suspended. This array is used to ensure
that multiple calls to thd\_suspend have no additional effect on the target thread,
and that calling thd\_continue for a thread that is not suspended has no effect.
21-35 It sets up signal actions for the SIGUSRI and SIGUSR2 signals, which will be
used, respectively, to suspend and resume threads.
| susp.c part 2 initialization
1 /*
2 * Dynamically initialize the "suspend package" when first used
3 * (called by pthread\_once).
4 */
5 void
6 suspend\_init\_routine (void)
7 {
8 int status;
9 struct sigaction sigusrl, sigusr2;
10
11 /*
12 * Allocate the suspended threads array. This array is used
13 * to guarentee idempotency
14 */
15 bottom = 10;
16 array = (pthread\_t*) calloc (bottom, sizeof (pthread\_t));
17
18 /*
19 * Install the signal handlers for suspend/resume.
20 */
21 sigusrl.sa\_flags = 0;
22 sigusrl.sa\_handler = suspend\_signal\_handler;
23
24 sigemptyset (Ssigusrl.sa\_mask);
25 sigusr2.sa\_flags = 0;
26 sigusr2.sa\_handler = resume\_signal\_handler;
27 sigusr2.sa\_mask = sigusrl.sa\_mask;
28
29 status = sigaction (SIGUSRI, Ssigusrl, NULL);
30 if (status == -1)
31 errno\_abort ("Installing suspend handler");
32
33 status = sigaction (SIGUSR2, &sigusr2, NULL);
34 if (status == -1)
35 errno abort ("Installing resume handler");
Signals 221
36
37 inited = 1;
38 return;
39 }
| susp.c part 2 initialization
9-40 The thd\_suspend function suspends a thread, and returns when that thread
has ceased to execute user code. It first ensures that the suspend/resume pack-
age is initialized by calling pthread\_once. Under protection of a mutex, it
searches for the target thread's identifier in the array of suspended thread identi-
fiers. If the thread is already suspended, thd\_suspend returns successfully.
47-60 Determine whether there is an empty entry in the array of suspended threads
and, if not, realloc the array with an extra entry.
65-78 The sentinel variable is initialized to 0, to detect when the target thread sus-
pension occurs. The thread is sent a SIGUSRI signal by calling pthread\_kill, and
thd\_suspend loops, calling sched\_yield to avoid monopolizing a processor, until
the target thread responds by setting sentinel. Finally, the suspended thread's
identifier is stored in the array.
| susp.c part 3 thd\_suspend
1 /*
2 * Suspend a thread by sending it a signal (SIGUSRI), which will
3 * block the thread until another signal (SIGUSR2) arrives.
4 *
5 * Multiple calls to thd\_suspend for a single thread have no
6 * additional effect on the thread - a single thd\_continue
7 * call will cause it to resume execution.
8 */
9 int
10 thd\_suspend (pthread\_t target\_thread)
11 {
12 int status;
13 int i = 0;
14
15 /*
16 * The first call to thd\_suspend will initialize the
17 * package.
18 */
19 status = pthread\_once (Sonce, suspend\_init\_routine);
20 if (status != 0)
21 return status;
22
23 /*
24 * Serialize access to suspend, makes life easier.
25 */
222 CHAPTER 6 POSIX adjusts to threads
26 status = pthread\_mutex\_lock (smut);
27 if (status != 0)
28 return status;
29
30 /*
31 * Threads that are suspended are added to the target\_array;
32 * a request to suspend a thread already listed in the array
33 * is ignored. Sending a second SIGUSR1 would cause the
34 * thread to resuspend itself as soon as it is resumed.
35 */
36 while (i \< bottom)
37 if (array[i++] == target\_thread) {
38 status = pthread\_mutex\_unlock (Smut);
39 return status;
40 }
41
42 /*
43 * Ok, we really need to suspend this thread. So, let's find
44 * the location in the array that we'll use. If we run off
45 * the end, realloc the array for more space.
46 */
47 i = 0;
48 while (array[i] != 0)
49 i++;
50
51 if (i == bottom) {
52 array = (pthread\_t*) realloc (
53 array, (++bottom * sizeof (pthread\_t)));
54 if (array == NULL) {
55 pthread\_mutex\_unlock (Smut);
56 return errno;
57 }
58
59 arrayfbottom] = null\_pthread; /* Clear new entry */
60 }
61
62 /*
63 * Clear the sentinel and signal the thread to suspend.
64 */
65 sentinel = 0;
66 status = pthread\_kill (target\_thread, SIGUSR1);
67 if (status != 0) {
68 pthread\_mutex\_unlock (Smut);
69 return status;
70 \>
71
72 /*
73 * Wait for the sentinel to change.
74 */
Signals 223
75 while (sentinel == 0)
76 sched\_yield ();
77
78 array[i] = target\_thread;
79
80 status = pthread\_mutex\_unlock (Smut);
81 return status;
82 }
| susp.c part 3 thd\_suspend
23-26 The thd\_continue function first checks whether the suspend/resume pack-
age has been initialized (inited is not 0). If it has not been initialized, then no
threads are suspended, and thd\_continue returns with success.
33-39 If the specified thread identifier is not found in the array of suspended
threads, then it is not suspended-again, return with success.
45-51 Send the resume signal, SIGUSR2. There's no need to wait-the thread will
resume whenever it can, and the thread calling thd\_continue doesn't need to
know.
| susp.c part 4 thd\_continue
1 /*
2 * Resume a suspended thread by sending it SIGUSR2 to break
3 * it out of the sigsuspend() in which it's waiting. If the
4 * target thread isn't suspended, return with success.
5 */
6 int
7 thd\_continue (pthread\_t target\_thread)
8 {
9 int status;
10 int i = 0;
11
12 /*
13 * Serialize access to suspend, makes life easier.
14 */
15 status = pthread\_mutex\_lock (Smut);
16 if (status != 0)
17 return status;
18
19 /*
20 * If we haven't been initialized, then the thread must be
21 * "resumed"; it couldn't have been suspended!
22 */
23 if (!inited) {
24 status = pthread\_mutex\_unlock (Smut);
25 return status;
26 }
27
224 CHAPTER 6 POSIX adjusts to threads
28 /*
29 * Make sure the thread is in the suspend array. If not, it
30 * hasn't been suspended (or it has already been resumed) and
31 * we can just carry on.
32 */
33 while (array[i] != target\_thread && i \< bottom)
34 i++;
35
36 if (i \>= bottom) {
37 pthread\_mutex\_unlock (Smut);
38 return 0;
39 }
40
41 /*
42 * Signal the thread to continue, and remove the thread from
43 * the suspended array.
44 */
45 status = pthread\_kill (target\_thread, SIGUSR2);
46 if (status != 0) {
47 pthread\_mutex\_unlock (Smut);
48 return status;
49 }
50
51 array[i] = 0; /* Clear array element */
52 status = pthread\_mutex\_unlock (smut);
53 return status;
54 }
| susp.c part 4 thd\_continue
2-25 The thread\_routine function is the thread start routine for each of the "target"
threads created by the program. It simply loops for a substantial period of time,
periodically printing a status message. On each iteration, it yields to other threads
to ensure that the processor time is apportioned "fairly" across all the threads.
Notice that instead of calling printf, the function formats a message with
sprintf and then displays it on stdout (file descriptor 1) by calling write. This
illustrates one of the problems with using suspend and resume (thd\_suspend and
thd\_continue) for synchronization. Suspend and resume are scheduling func-
tions, not synchronization functions, and using scheduling and synchronization
controls together can have severe consequences.
I Incautious use of suspend and resume can deadlock your application.
In this case, if a thread were suspended while modifying a stdio stream, all other
threads that tried to modify that stdio stream might block, waiting for a mutex that
is locked by the suspended thread. The write function, on the other hand, is usu-
ally a call to the kernel-the kernel is atomic with respect to signals, and therefore
can't be suspended. Use of write, therefore, cannot cause a deadlock.
Signals 225
In general, you cannot suspend a thread that may possibly hold any resource,
if that resource may be required by some other thread before the suspended
thread is resumed. In particular, the result is a deadlock if the thread that would
resume the suspended thread first needs to acquire the resource. This prohibi-
tion includes, especially, mutexes used by libraries you call-such as the
mutexes used by malloc and free, or the mutexes used by stdio.
36-42 Threads are created with an attributes object set to create threads detached,
rather than joinable. The result is that threads will cease to exist as soon as they
terminate, rather than remaining until main calls pthread\_join. The pthread\_
kill function does not necessarily fail if you attempt to send a signal to a termi-
nated thread (the standard is silent on this point), and you may be merely setting
a pending signal in a thread that will never be able to act on it. If this were to
occur, the thd\_suspend routine would hang waiting for the thread to respond.
Although pthread\_kill may not fail when sending to a terminated thread, it will
fail when sending to a thread that doesn't exist-so this attribute converts a pos-
sible hang, when the program is run with iterations set too low, into an abort
with an error message.
51-85 The main thread sleeps for two seconds after creating the threads to allow
them to reach a "steady state." It then loops through the first half of the threads,
suspending each of them. It waits an additional two seconds and then resumes
each of the threads it had suspended. It waits another two seconds, suspends each
of the remaining threads (the second half), and then after another two seconds
resumes them.
By watching the status messages printed by the individual threads, you can
see the pattern of output change as the threads are suspended and resumed.
| susp.c part 5 sampleprogram
1 static void *
2 thread\_routine (void *arg)
3 {
4 int number = (int)arg;
5 int status;
6 int i;
7 char buffer[128] ;
8
9 for (i = 1; i \<= iterations; i++) {
10 /*
11 * Every time each thread does 5000 interations, print
12 * a progress report.
13 */
14 if (i % 2000 == 0) {
15 sprintf (
16 buffer, "Thread %02d: %d\n",
17 number, i);
18 write A, buffer, strlen (buffer));
19 }
20
226 CHAPTER 6 POSIX adjusts to threads
21 sched\_yield ();
22 }
23
24 return (void *H;
25 }
26
27 int
28 main (int argc, char *argv[])
29 {
30 pthread\_t threads[THREAD\_COUNT];
31 pthread\_attr\_t detach;
32 int status;
33 void *result;
34 int i;
35
36 status = pthread\_attr\_init (Sdetach);
37 if (status != 0)
38 err\_abort (status, "Init attributes object");
39 status = pthread\_attr\_setdetachstate (
40 Sdetach, PTHREAD\_CREATE\_DETACHED);
41 if (status != 0)
42 err\_abort (status, "Set create-detached");
43
44 for (i = 0; i\< THREAD\_COUNT; i++) {
45 status = pthread\_create (
46 &threads[i], Sdetach, thread\_routine, (void *)i);
47 if (status != 0)
48 err\_abort (status, "Create thread");
49 }
50
51 sleep B);
52
53 for (i = 0; i \< THREAD\_COUNT/2; i++) {
54 printf ("Suspending thread %d.\n", i);
55 status = thd\_suspend (threads[i]);
56 if (status != 0)
57 err\_abort (status, "Suspend thread");
58 }
59
60 printf ("Sleeping ...\n");
61 sleep B);
62
63 for (i = 0; i \< THREAD\_COUNT/2; i++) {
64 printf ("Continuing thread %d.\n", i);
65 status = thd\_continue (threads[i]);
66 if (status != 0)
67 err\_abort (status, "Suspend thread");
68 }
69
Signals 227
70 for (i = THREAD\_C0UNT/2; i \< THREAD\_COUNT; i
71 printf ("Suspending thread %d.\n", i);
72 status = thd\_suspend (threads[i]);
73 if (status != 0)
74 err\_abort (status, "Suspend thread");
75 }
76
77 printf ("Sleeping ...\n");
78 sleep B);
79
80 for (i = THREAD\_COUNT/2; i \< THREAD\_COUNT; i
81 printf ("Continuing thread %d.\n", i);
82 status = thd\_continue (threads[i]);
83 if (status != 0)
84 err\_abort (status, "Continue thread");
85 }
86
87 pthread\_exit (NULL); /* Let threads finish */
88 }
| susp.c part 5 sampleprogram
### 6.6.4 sigwait and sigwaitinfo
```c
int sigwait (const sigset
t *set, int *sig);
fifdef \_POSIX\_REALTIME\_SIGNALS
int sigwaitinfo (
const sigset t *set,
int sigtimedwait (
const sigset t *set,
const struct timespec
#endif
siginfo t
siginfo\_t
|timeout)
*info);
*info,
r
```
I Always use sigwait to work with asynchronous signals within threaded
code.
Pthreads adds a function to allow threaded programs to deal with "asynchro-
nous" signals synchronously. That is, instead of allowing a signal to interrupt a
thread at some arbitrary point, a thread can choose to receive a signal synchro-
nously. It does this by calling sigwait, or one of sigwait's siblings.
I The signals for which you sigwait must be masked in the sigwaiting
thread, and should usually be masked in all threads.
The sigwait function takes a signal set as its argument, and returns a signal
number when any signal in that set occurs. You can create a thread that waits for
228 CHAPTER 6 POSIX adjusts to threads
some signal, for example, sigint, and causes some application activity when it
occurs. The nonobvious rule is that the signals for which you wait must be
masked before calling sigwait. In fact, you should ideally mask these signals in
main, at the start of the program. Because signal masks are inherited by threads
you create, all threads will (by default) have the signal masked. This ensures that
the signal will never be delivered to any thread except the one that calls sigwait.
Signals are delivered only once. If two threads are blocked in sigwait, only
one of them will receive a signal that's sent to the process. This means you can't,
for example, have two independent subsystems using sigwait that catch SIGINT.
It also means that the signal will not be caught by sigwait in one thread and also
delivered to some signal-catching function in another thread. That's not so bad,
since you couldn't do that in the old nonthreaded model either-only one signal
action can be active at a time.
While sigwait, a Pthreads function, reports errors by returning an error num-
ber, its siblings, sigwaitinfo and sigtimedwait, were added to POSIX prior to
Pthreads, and use the older errno mechanism. This is confusing and awkward,
and that is unfortunate. The problem is that they deal with the additional informa-
tion supplied by the POSIX realtime signals option (\<unistd. h\> defines the symbol
\_P0SIX\_realtime\_SIGNALS), and the POSIX realtime amendment, POSIX. 1b, was
completed before the Pthreads amendment.
Both sigwaitinfo and sigtimedwait return the realtime signal information,
siginfo\_t, for signals received. In addition, sigtimedwait allows the caller to
specify that sigtimedwait should return with the error EAGAIN in the event that
none of the selected signals is received within the specified interval.
The sigwait.c program creates a "sigwait thread" that handles SIGINT.
23-41 The signal\_waiter thread repeatedly calls sigwait, waiting for a SIGINT sig-
nal. It counts five occurrences of sigint (printing a message each time), and then
signals a condition variable on which main is waiting. At that time, main will exit.
61-65 The main program begins by masking sigint. Because all threads inherit
their initial signal mask from their creator, SIGINT will be masked in all threads.
This prevents SIGINT from being delivered at any time except when the signal\_
waiter thread is blocked in sigwait and ready to receive the signal.
| sigwait.c
1 #include \<sys/types.h\>
2 #include \<unistd.h\>
3 #include \<pthread.h\>
4 #include \<signal.h\>
5 #include "errors.h"
6
7 pthread\_mutex\_t mutex = PTHREAD\_MUTEX\_INITIALIZER;
8 pthread\_cond\_t cond = PTHREAD\_COND\_INITIALIZER;
9 int interrupted = 0;
10 sigset\_t signal\_set;
11
Signals 229
12 /*
13 * Wait for the SIGINT signal. When it has occurred 5 times, set the
14 * "interrupted" flag (the main thread's wait predicate) and signal a
15 * condition variable. The main thread will exit.
16 */
17 void *signal\_waiter (void *arg)
18 {
19 int sig\_number;
20 int signal\_count = 0;
21 int status;
22
23 while A) {
24 sigwait (&signal\_set, &sig\_number);
25 if (sig\_number == SIGINT) {
26 printf ("Got SIGINT (%d of 5)\n", signal\_count+l);
27 if (++signal\_count \>= 5) {
28 status = pthread\_mutex\_lock (Smutex);
29 if (status != 0)
30 err\_abort (status, "Lock mutex");
31 interrupted = 1;
32 status = pthread\_cond\_signal (Scond);
33 if (status != 0)
34 err\_abort (status, "Signal condition");
35 status = pthread\_mutex\_unlock (Smutex);
36 if (status != 0)
37 err\_abort (status, "Unlock mutex");
38 break;
39 }
40 }
41 }
42 return NULL;
43 }
44
45 int main (int argc, char *argv[])
46 {
47 pthread\_t signal\_thread\_id;
48 int status;
49
50 /*
51 * Start by masking the "interesting" signal, SIGINT in the
52 * initial thread. Because all threads inherit the signal mask
53 * from their creator, all threads in the process will have
54 * SIGINT masked unless one explicitly unmasks it. The
55 * semantics of sigwait requires that all threads (including
56 * the thread calling sigwait) have the signal masked, for
57 * reliable operation. Otherwise, a signal that arrives
58 * while the sigwaiter is not blocked in sigwait might be
59 * delivered to another thread.
60 */
230 CHAPTERS POSIXadjusts to threads
61 sigemptyset (&signal\_set);
62 sigaddset (&signal\_set, SIGINT);
63 status = pthread\_sigmask (SIG\_BL0CK, &signal\_set, NULL);
64 if (status != 0)
65 err\_abort (status, "Set signal mask");
66
67 /*
68 * Create the sigwait thread.
69 */
70 status = pthread\_create (&signal\_thread\_id, NULL,
71 signal\_waiter, NULL);
72 if (status != 0)
73 err\_abort (status, "Create sigwaiter");
74
75 /*
76 * Wait for the sigwait thread to receive SIGINT and signal
77 * the condition variable.
78 */
79 status = pthread\_mutex\_lock (Smutex);
80 if (status != 0)
81 err\_abort (status, "Lock mutex");
82 while (!interrupted) {
83 status = pthread\_cond\_wait (Scond, Smutex);
84 if (status != 0)
85 err\_abort (status, "Wait for interrupt");
86 }
87 status = pthread\_mutex\_unlock (Smutex);
88 if (status != 0)
89 err\_abort (status, "Unlock mutex");
90 printf ("Main terminating with SIGINT\n");
91 return 0;
92 }
| sigwait.c
### 6.6.5 SIGEVJHREAD
Some of the functions in the POSIX. lb realtime standard, which provide for
asynchronous notification, allow the programmer to give specific instructions
about how that notification is to be accomplished. For example, when initiating
an asynchronous device read or write using aio\_read or aio\_write, the pro-
grammer specifies a struct aiocb, which contains, among other members, a
struct sigevent. Other functions that accept a struct sigevent include timer\_
create (which creates a per-process timer) and sigqueue (which queues a signal
to a process).
Signals 231
The struct sigevent structure in POSIX. lb provides a "notification mecha-
nism" that allows the programmer to specify whether a signal is to be generated,
and, if so, what signal number should be used. Pthreads adds a new notification
mechanism called SIGEV\_THREAD. This new notification mechanism causes the
signal notification function to be run as if it were the start routine of a thread.
Pthreads adds several members to the POSIX. lb struct sigevent structure.
The new members are sigev\_notify\_function, a pointer to a thread start func-
tion; and sigev\_notify\_attributes, a pointer to a thread attributes object
(pthread\_attr\_t) containing the desired thread creation attributes. If sigev\_
notify\_attributes is NULL, the notify thread is created as if the detachstate
attribute was set to pthread\_create\_detached. This avoids a memory leak-in
general, the notify thread's identifier won't be available to any other thread. Fur-
thermore, Pthreads says that the result of specifying an attributes object that has
the detachstate attribute set to pthread\_create\_joinable is "undefined." (Most
likely, the result will be a memory leak because the thread cannot be joined-if
you are lucky, the system may override your choice and create it detached
anyway.)
The sigevjthread notification function may not actually be run in a new
thread-Pthreads carefully specifies that it behaves as if it were run in a new thread,
just as I did a few paragraphs ago. The system may, for example, queue sigev\_
THREAD events and call the start routines, serially, in some internal "server
thread." The difference is effectively indistinguishable to the application. A sys-
tem that uses a server thread must be very careful about the attributes specified
for the notification thread-for example, scheduling policy and priority, conten-
tion scope, and minimum stack size must all be taken into consideration.
The SIGEV\_THREAD feature is not available to any of the "traditional" signal
generation mechanisms, such as setitimer, or for SIGCHLD, SIGINT, and so forth.
Those who are programming using the POSIX. lb "realtime signal" interfaces,
including timers and asynchronous I/O, may find this new capability useful.
The following program, sigev\_thread.c, shows how to use the SIGEV\_THREAD
notification mechanism for a POSIX. lb timer.
20-37 The function timer\_thread is specified as the "notification function" (thread
start routine) for the sigevjthread timer. The function will be called each time
the timer expires. It counts expirations, and wakes the main thread after five.
Notice that, unlike a signal-catching function, the sigev\_thread notification
function can make full use of Pthreads synchronization operations. This can be a
substantial advantage in many situations.
45-51 Unfortunately, neither Solaris 2.5 nor Digital UNIX 4.0 correctly implemented
SIGEV\_THREAD. Thus, unlike all other examples in this book, this code will not
compile on Solaris 2.5. This #ifdef block allows the code to compile, and to fail
gracefully if the resulting program is run, with an error message. Although the
program will compile on Digital UNIX 4.0, it will not run. The implementation of
sigev\_thread has been fixed in Digital UNIX 4.0D, which should be available by
the time you read this, and it should also be fixed in Solaris 2.6.
232 CHAPTER 6 POSIX adjusts to threads
56-59 These statements initialize the sigevent structure, which describes how the
system should notify the application when an event occurs. In this case, we are
telling it to call timer\_thread when the timer expires, and to use default
attributes.
| sigev\_thread.c
1 #include \<pthread.h\>
2 #include \<sys/signal.h\>
3 #include \<sys/time.h\>
4 #include "errors.h"
5
6 timer\_t timer\_id;
7 pthread\_mutex\_t mutex = PTHREAD\_MUTEX\_INITIALIZER;
8 pthread\_cond\_t cond = PTHREAD\_COND\_INITIALIZER;
9 int counter = 0;
10
11 /*
12 * Thread start routine to notify the application when the
13 * timer expires. This routine is run "as if" it were a new
14 * thread, each time the timer expires.
15 *
16 * When the timer has expired 5 times, the main thread will
17 * be awakened, and will terminate the program.
18 */
19 void
20 timer\_thread (void *arg)
21 {
22 int status;
23
24 status = pthread\_mutex\_lock (smutex);
25 if (status != 0)
26 err\_abort (status, "Lock mutex");
27 if (++counter \>= 5) {
28 status = pthread\_cond\_signal (Scond);
29 if (status != 0)
30 err\_abort (status, "Signal condition");
31 }
32 status = pthread\_mutex\_unlock (Smutex);
33 if (status != 0)
34 err\_abort (status, "Unlock mutex");
35
36 printf ("Timer %d\n", counter);
37 }
38
39 main( )
Signals 233
40 {
41 int status;
42 struct itimerspec ts;
43 struct sigevent se;
44
45 #ifdef sun
46 fprintf (
47 stderr,
48 "This program cannot compile on Solaris 2.5.\n"
49 "To build and run on Solaris 2.6, remove the\n"
50 "\"#ifdef sun\" block in main().\n");
51 #else
52 /*
53 * Set the sigevent structure to cause the signal to be
54 * delivered by creating a new thread.
55 */
56 se.sigev\_notify = SIGEV\_THREAD;
57 se.sigev\_value.sival\_ptr = &timer\_id;
58 se.sigev\_notify\_function = timer\_thread;
59 se.sigev\_notify\_attributes = NULL;
60
61 /*
62 * Specify a repeating timer that fires every 5 seconds.
63 */
64 ts.it\_value.tv\_sec = 5;
65 ts.it\_value.tv\_nsec = 0;
66 ts.it\_interval.tv\_sec =5;
67 ts.it\_interval.tv\_nsec = 0;
68
69 DPRINTF (("Creating timer\n"));
70 status = timer\_create(CLOCK\_REALTIME, &se, &timer\_id);
71 if (status == -1)
72 errno\_abort ("Create timer");
73
74 DPRINTF ((
75 "Setting timer %d for 5-second expiration...\n", timer\_id));
76 status = timer\_settime(timer\_id, 0, &ts, 0);
77 if (status == -1)
78 errno\_abort ("Set timer");
79
80 status = pthread\_mutex\_lock (Smutex);
81 if (status != 0)
82 err\_abort (status, "Lock mutex");
83 while (counter \< 5) {
84 status = pthread\_cond\_wait (Scond, Smutex);
85 if (status != 0)
86 err\_abort (status, "Wait on condition");
87 }
234 CHAPTER 6 POSIX adjusts to threads
88 status = pthread\_mutex\_unlock (Smutex);
89 if (status != 0)
90 err\_abort (status, "Unlock mutex");
91
92 #endif /* Sun */
93 return 0;
94 }
| sigev thread.c
### 6.6.6 Semaphores: synchronizing with a signal-catching function
#ifdef
int
int
int
int
int
int
sem
int
sem
sem
\_POSIX\_SEMAPHORES
init (sem t *sem,
pshared, unsigned int value);
destroy (sem t *sem);
wait (sem t *sem);
sem trywake (sem t *sem);
sem post (sem\_t *sem);
sem
#endif
getvalue (sem t *sem, int *sval);
Although mutexes and condition variables provide an ideal solution to most
synchronization needs, they cannot meet all needs. One example of this is a need
to communicate between a POSIX signal-catching function and a thread waiting
for some asynchronous event. In new code, it is best to use sigwait or sigwait-
inf o rather than relying on a signal-catching function, and this neatly avoids this
problem. However, the use of asynchronous POSIX signal-catching functions is
well established and widespread, and most programmers working with threads
and existing code will probably encounter this situation.
To awaken a thread from a POSIX signal-catching function, you need a mech-
anism that's reentrant with respect to POSIX signals (async-signal safe). POSIX
provides relatively few of these functions, and none of the Pthreads functions is
included. That's primarily because an async-signal safe mutex lock operation
would be many times slower than one that isn't async-signal safe. Outside of the
kernel, making a function async-signal safe usually requires that the function
mask (block) signals while it runs-and that is expensive.
In case you're curious, here is the full list of POSIX 1003.1-1996 functions
that are async-signal safe (some of these functions exist only when certain POSIX
options are denned, such as \_posix\_asynchronous\_io or \_posix\_timers):
Signals
235
access
aio error
aio return
aio suspend
alarm
cfgetispeed
cfgetospeed
cfsetispeed
cfsetospeed
chdir
chmod
chown
clock gettime
close
creat
dup2
dup
execle
execve
exit
fcntl
fdatasync
fork
fstat
f sync
getegid
geteuid
getgid
getoverrun
getgroups
getpgrp
getpid
getppid
getuid
kill
link
lseek
mkdir
mkfifo
open
pathconf
pause
pipe
read
rename
rmdir
sem post
setgid
setpgid
setsid
setuid
sigaction
sigaddset
sigdelset
sigemptyset
sigfillset
sigismember
sigpending
sigprocmask
sigqueue
sigsuspend
sleep
stat
sysconf
tcdrain
tcflow
tcflush
tcgetattr
tcgetpgrp
tcsendbreak
tcsetattr
tcsetpgrp
time
timer getoverrun
timer gettime
timer settime
times
umask
uname
unlink
utime
wait
waitpid
write
POSIX. lb provides counting semaphores, and most systems that support
Pthreads also support POSIX. lb semaphores. You may notice that the sem\_post
function, which wakes threads waiting on a semaphore, appears in the list of
async-signal safe functions. If your system supports POSIX semaphores
(\<unistd.h\> defines the \_posix\_semaphores option), then Pthreads adds the
ability to use semaphores between threads within a process. That means you can
post a semaphore, from within a POSIX signal-catching function, to wake a
thread in the same process or in another process.
A semaphore is a different kind of synchronization object-it is a little like a
mutex, a little like a condition variable. The differences can make semaphores a
little harder to use for many common tasks, but they make semaphores substan-
tially easier to use for certain specialized purposes. In particular, semaphores can
be posted (unlocked or signaled) from a POSIX signal-catching function.
236 CHAPTER 6 POSIX adjusts to threads
Semaphores are a general synchronization mechanism.
We just have no reason to use them that way.
I am emphasizing the use of semaphores to pass information from a signal-
catching function, rather than for general use, for a couple of reasons. One rea-
son is that semaphores are part of a different standard. As I said, most systems
that support Pthreads will also support POSIX. lb, but there is no such require-
ment anywhere in the standard. So you may well find yourself without access to
semaphores, and you shouldn't feel dependent on them. {Of course, you may also
find yourself with semaphores and without threads-but in that case, you should
be reading a different book.)
Another reason for keeping semaphores here with signals is that, although
semaphores are a completely general synchronization mechanism, it can be more
difficult to solve many problems using semaphores-mutexes and condition vari-
ables are simpler. If you've got Pthreads, you only need semaphores to handle
this one specialized function-waking a waiting thread from a signal-catching
function. Just remember that you can use them for other things when they're
convenient and available.
POSIX semaphores contain a count, but no "owner," so although they can be
used essentially as a lock, they can also be used to wait for events. The terminol-
ogy used in the POSIX semaphore operations stresses the "wait" behavior rather
than the "lock" behavior. Don't be confused by the names, though; there's no dif-
ference between "waiting" on a semaphore and "locking" the semaphore.
A thread waits on a semaphore (to lock a resource, or wait for an event) by
calling sem\_wait. If the semaphore counter is greater than zero, sera\_wait decre-
ments the counter and returns immediately. Otherwise, the thread blocks. A
thread can post a semaphore (to unlock a resource, or awaken a waiter) by calling
sem\_post. If one or more threads are waiting on the semaphore, sem\_post will
wake one waiter (the highest priority, or earliest, waiter). If no threads are wait-
ing, the semaphore counter is incremented.
The initial value of the semaphore counter is the distinction between a "lock"
semaphore and a "wait" semaphore. By creating a semaphore with an initial count
of 1, you allow one thread to complete a sem\_wait operation without blocking-
this "locks" the semaphore. By creating a semaphore with an initial count of 0, you
force all threads that call sem\_wait to block until some thread calls sem\_post.
The differences in how semaphores work give the semaphore two important
advantages over mutexes and condition variables that may be of use in threaded
programs:
1. Unlike mutexes, semaphores have no concept of an "owner." This means
that any thread may release threads blocked on a semaphore, much as if
any thread could unlock a mutex that some thread had locked. (Although
this is usually not a good programming model, there are times when it is
handy.)
Signals 237
2. Unlike condition variables, semaphores can be independent of any exter-
nal state. Condition variables depend on a shared predicate and a mutex
for waiting-semaphores do not.
A semaphore is represented in your program by a variable of type sem\_t. You
should never make a copy of a sem\_t variable-the result of using a copy of a
sem\_t variable in the sem\_wait, sem\_trywait, sem\_post, and sem\_destroy func-
tions is undefined. For our purposes, a sem\_t variable is initialized by calling the
sem\_init function. POSIX. lb provides other ways to create a "named" semaphore
that can be shared between processes without sharing memory, but there is no
need for this capability when using a semaphore within a single process.
Unlike Pthreads functions, the POSIX semaphore functions use errno to
report errors. That is, success is designated by returning the value 0, and errors
are designated by returning the value -1 and setting the variable errno to an
error code.
If you have a section of code in which you want up to two threads to execute
simultaneously while others wait, you can use a semaphore without any addi-
tional state. Initialize the semaphore to the value 2; then put a sem\_wait at the
beginning of the code and a sem\_post at the end. Two threads can then wait on
the semaphore without blocking, but a third thread will find the semaphore's
counter at 0, and block. As each thread exits the region of code it posts the sema-
phore, releasing one waiter (if any) or restoring the counter.
The sem\_getvalue function returns the current value of the semaphore
counter if there are no threads waiting. If threads are waiting, sem\_getvalue
returns a negative number. The absolute value of that number tells how many
threads are waiting on the semaphore. Keep in mind that the value it returns
may already be incorrect-it can change at any time due to the action of some
other thread.
The best use for sem\_getvalue is as a way to wake multiple waiters, somewhat
like a condition variable broadcast. Without sem\_getvalue, you have no way of
knowing how many threads might be blocked on a semaphore. To "broadcast" a
semaphore, you could call sem\_getvalue and sem\_jpost in a loop until sem\_getvalue
reports that there are no more waiters.
But remember that other threads can call sem\_post during this loop, and
there is no synchronization between the various concurrent calls to sem\_post and
sem\_getvalue. You can easily end up issuing one or more extra calls to sem\_post,
which will cause the next thread that calls sem\_wait to find a value greater than 0,
and return immediately without blocking.
The program below, semaphore\_signal.c, uses a semaphore to awaken
threads from within a POSIX signal-catching function. Notice that the sem\_init
call sets the initial value to 0 so that each thread calling sem\_wait will block. The
main program then requests an interval timer, with a POSIX signal-catching
function that will wake one waiting thread by calling sem\_post. Each occurrence
of the POSIX timer signal will awaken one waiting thread. The program will exit
when each thread has been awakened five times.
238 CHAPTER 6 POSIX adjusts to threads
32-35 Notice the code to check for eintr return status from the sem\_wait call. The
POSIX timer signal in this program will always occur while one or more threads
are blocked in sem\_wait. When a signal occurs for a process (such as a timer sig-
nal), the system may deliver that signal within the context of any thread within
the process. Likely "victims" include threads that the kernel knows to be waiting,
for example, on a semaphore. So there is a fairly good chance that the sem\_wait
thread will be chosen, at least sometimes. If that occurs, the call to sem\_wait will
return with eintr. The thread must then retry the call. Treating an eintr return
as "success" would make it appear that two threads had been awakened by each
call to sem\_post: the thread that was interrupted, and the thread that was awak-
ened by the sem\_post call.
| semaphore\_signal.c
1 #include \<sys/types.h\>
2 tinclude \<unistd.h\>
3 #include \<pthread.h\>
4 linclude \<semaphore.h\>
5 #include \<signal.h\>
6 #include \<time.h\>
7 #include "errors.h"
8
9 sem\_t semaphore;
10
11 /*
12 * Signal-catching function.
13 */
14 void signal\_catcher (int sig)
15 {
16 if (sem\_post (Ssemaphore) == -1)
17 errno\_abort ("Post semaphore");
18 }
19
20 /*
21 * Thread start routine which waits on the semaphore.
22 */
23 void *sem\_waiter (void *arg)
24 {
25 int number = (int)arg;
26 int counter;
27
28 /*
29 * Each thread waits 5 times.
30 */
31 for (counter = 1; counter \<= 5; counter++) {
32 while (sem wait (Ssemaphore) == -1) {
Signals 239
33 if (errno != EINTR)
34 errno\_abort ("Wait on semaphore");
35 }
36 printf ("%d waking (%d)...\n", number, counter);
37 }
38 return NULL;
39 }
40
41 int main (int argc, char *argv[])
42 {
43 int thread\_count, status;
44 struct sigevent sig\_event;
45 struct sigaction sig\_action;
46 sigset\_t sig\_mask;
47 timer\_t timer\_id;
48 struct itimerspec timer\_val;
49 pthread\_t sem\_waiters[5];
50
51 #if !defined(\_POSIX\_SEMAPHORES) || !defined(\_POSIX\_TIMERS)
52 # if !defined(\_POSIX\_SEMAPHORES)
53 printf ("This system does not support POSIX semaphores\n");
54 # endif
55 # if !defined(\_POSIX\_TIMERS)
56 printf ("This system does not support POSIX timers\n");
57 # endif
58 return -1;
59 #else
60 sem\_init (^semaphore, 0, 0);
61
62 /*
63 * Create 5 threads to wait on a semaphore.
64 */
65 for (thread\_count = 0; thread\_count \< 5; thread\_count++) {
66 status = pthread\_create (
67 &sem\_waiters[thread\_count], NULL,
68 sem\_waiter, (void*)thread\_count);
69 if (status != 0)
70 err\_abort (status, "Create thread");
71 }
72
73 /*
74 * Set up a repeating timer using signal number SIGRTMIN,
75 * set to occur every 2 seconds.
76 */
77 sig\_event.sigev\_value.sival\_int = 0;
78 sig\_event.sigev\_signo = SIGRTMIN;
79 sig\_event.sigev\_notify = SIGEV\_SIGNAL;
240 CHAPTER 6 POSIX adjusts to threads
80 if (timer\_create (CLOCK\_REALTIME, &sig\_event, &timer\_id) == -1)
81 errno\_abort ("Create timer");
82 sigemptyset (&sig\_mask);
83 sigaddset (&sig\_mask, SIGRTMIN);
84 sig\_action.sa\_handler = signal\_catcher;
85 sig\_action.sa\_mask = sig\_mask;
86 sig\_action.sa\_flags = 0;
87 if (sigaction (SIGRTMIN, &sig\_action, NULL) == -1)
88 errno\_abort ("Set signal action");
89 timer\_val.it\_interval.tv\_sec = 2;
90 timer\_val.it\_interval.tv\_nsec = 0;
91 timer\_val.it\_value.tv\_sec = 2 ;
92 timer\_val.it\_value.tv\_nsec = 0;
93 if (timer\_settime (timer\_id, 0, &timer\_val, NULL) == -1)
94 errno\_abort ("Set timer");
95
96 /*
97 * Wait for all threads to complete.
98 */
99 for (thread\_count = 0; thread\_count \< 5; thread\_count++) {
100 status = pthread\_join (sem\_waiters[thread\_count], NULL);
101 if (status != 0)
102 err\_abort (status, "Join thread");
103 }
104 return 0;
105 #endif
106 }
| semaphore signal.c
# 7 "Real code"
"When we were still little," the Mock Turtle went on at last, more calmly,
though still sobbing a little now and then, "we went to school in the sea.
The master was an old Turtle-we used to call him Tortoise-"
"Why did you call him Tortoise, if he wasn't one?" Alice asked.
"We called him Tortoise because he taught us," said the
Mock Turtle angrily.
-Lewis Carroll, Through the Looking-Glass
This section builds on most of the earlier sections of the book, but principal!;.
on the mutex and condition variable sections. You should already understar.c
how to create both types of synchronization objects and how they work. I wl!
demonstrate the design and construction of barrier and read/ write lock synchr:
nization mechanisms that are built from mutexes, condition variables, arc ;
dash of data. Both barriers and read/write locks are in common use. and ha.-;
been proposed for standardization in the near future. I will follow up with a -.j-:~-
queue server that lets you parcel out tasks to a pool of threads.
The purpose of all this is to teach you more about the subtleties of usir.i. a.
these new threaded programming tools (that is, mutexes, condition variables ar.:
threads). The library packages may be useful to you as is or as templates. Pnrr.ar
ily, though, they are here to give me something to talk about in this section, ar. z
have omitted some complication that may be valuable in real code. The err::
detection and recovery code, for example, is fairly primitive.
## 7.1 Extended synchronization
Mutexes and condition variables are flexible and efficient synchronization
tools. You can build just about any form of synchronization you need using those
two things. But you shouldn't build them from scratch every time you need them.
It is nice to start with a general, modular implementation that doesn't need to be
debugged every time. This section shows some common and useful tools that you
won't have to redesign every time you write an application that needs them.
First we'll build a barrier. The function of a barrier is about what you might
guess-it stops threads. A barrier is initialized to stop a certain number of
threads-when the required number of threads have reached the barrier, all are
allowed to continue.
241
242
CHAPTER 7 "Real code"
Then we'll build something called a read/write lock. A read/write lock allows
multiple threads to read data simultaneously, but prevents any thread from mod-
ifying data that is being read or modified by another thread.
### 7.1.1 Barriers
A barrier is a way to keep the members of a group together. If our intrepid
"bailing programmers" washed up on a deserted island, for example, and they
ventured into the jungle to explore, they would want to remain together, for the
illusion of safety in numbers, if for no other reason (Figure 7.1). Any exploring
programmer finding himself very far in front of the others would therefore wait
for them before continuing.
A barrier is usually employed to ensure that all threads cooperating in some
parallel algorithm reach a specific point in that algorithm before any can pass.
This is especially common in code that has been decomposed automatically by
creating fine-grained parallelism within compiled source code. All threads may
execute the same code, with threads processing separate portions of a shared
data set (such as an array) in some areas and processing private data in parallel
FIGURE 7.1 Barrier analogy
Extended synchronization
243
in other areas. Still other areas must be executed by only one thread, such as
setup or cleanup for the parallel regions. The boundaries between these areas are
often implemented using barriers. Thus, threads completing a matrix computa-
tion may wait at a barrier until all have finished. One may then perform setup for
the next parallel segment while the others skip ahead to another barrier. When the
setup thread reaches that barrier, all threads begin the next parallel region.
Figure 7.2 shows the operation of a barrier being used to synchronize three
threads, called thread 1, thread 2, and thread 3. The figure is a sort of timing dia-
gram, with time increasing from left to right. Each of the lines beginning at the
labels in the upper left designates the behavior of a specific thread-solid for
thread 1, dotted for thread 2, and dashed for thread 3. When the lines drop
within the rounded rectangle, they are interacting with the barrier. If the line
drops below the center line, it shows that the thread is blocked waiting for other
threads to reach the barrier. The line that stops above the center line represents
the final thread to reach the barrier, awakening all waiters.
In this example, thread 1 and then thread 2 wait on the barrier. At a later
time, thread 3 waits on the barrier, finds that the barrier is now full, and awakens
all the waiters. All three threads then return from the barrier wait.
The core of a barrier is a counter. The counter is initialized to the number of
threads in the "tour group," the number of threads that must wait on a barrier
before all the waiters return. I'll call that the "threshold," to give it a simple one-
word name. When each thread reaches the barrier, it decreases the counter. If the
value hasn't reached 0, it waits. If the value has reached 0, it wakes up the wait-
ing threads.
thread 1 waits -?
thread 1
thread 2
thread 3
time:
'barrier
thread 3 waits,
releasing all three
2
thread 2 and /
thread 3 wake
FIGURE 7.2 Barrier operation
244 CHAPTER 7 "Real code'
Because the counter will be modified by multiple threads, it has to be pro-
tected by a mutex. Because threads will be waiting for some event (a counter value
of 0), the barrier needs to have a condition variable and a predicate expression.
When the counter reaches 0 and the barrier drops open, we need to reset the
counter, which means the barrier must store the original threshold.
The obvious predicate is to simply wait for the counter to reach 0, but that
complicates the process of resetting the barrier. When can we reset the count to
the original value? We can't reset it when the counter reaches 0, because at that
point most of the threads are waiting on the condition variable. The counter must
be 0 when they wake up, or they'll continue waiting. Remember that condition
variable waits occur in loops that retest the predicate.
The best solution is to add a separate variable for the predicate. We will use a
"cycle" variable that is logically inverted each time some thread determines that
one cycle of the barrier is complete. That is, whenever the counter value is reset,
before broadcasting the condition variable, the thread inverts the cycle flag.
Threads wait in a loop as long as the cycle flag remains the same as the value
seen on entry, which means that each thread must save the initial value.
The header file barrier.h and the C source file barrier.c demonstrate an
implementation of barriers using standard Pthreads mutexes and condition vari-
ables. This is a portable implementation that is relatively easy to understand.
One could, of course, create a much more efficient implementation for any spe-
cific system based on knowledge of nonportable hardware and operating system
characteristics.
6-13 Part 1 shows the structure of a barrier, represented by the type barrier\_t.
You can see the mutex (mutex) and the condition variable (cv). The threshhold
member is the number of threads in the group, whereas counter is the number of
threads that have yet to join the group at the barrier. And cycle is the flag dis-
cussed in the previous paragraph. It is used to ensure that a thread awakened
from a barrier wait will immediately return to the caller, but will block in the bar-
rier if it calls the wait operation again before all threads have resumed execution.
15 The barrier\_valid macro defines a "magic number," which we store into the
valid member and then check to determine whether an address passed to other
barrier interfaces is "reasonably likely to be" a barrier. This is an easy, quick
check that will catch the most common errors.
* I always like to define magic numbers using hexadecimal constants that can be pronounced
as English words. For barriers, I invented my own restaurant called the "DB cafe," or, in C syntax,
Oxdbcaf e. Many interesting (or at least mildly amusing) English words can be spelled using only
the letters a through/ There are even more possibilities if you allow the digit 1 to stand in for the
letter 1. and the digit 0 to stand in for the letter o. (Whether you like the results will depend a lot
on the typeface in which you commonly read your code.)
Extended synchronization
245
barrier.h
part 1 barrier\_t
1
2
3
4
5
6
7
8
9
10
11
12
13
14
15
#include \<pthread.h\>
/*
* Structure describing a barrier.
*/
typedef struct barrier tag {
pthread\_mutex\_t
pthread\_cond\_t
int
int
int
int
barrier t;
mutex;
cv;
valid;
threshold;
counter;
cycle;
#define BARRIER VALID Oxdbcafe
/* Control access to barrier */
/* wait for barrier */
/* set when valid */
/* number of threads required */
/* current number of threads */
/* alternate cycles @ or 1) */
barrier.h
part 1 barrier\_t
Part 2 shows definitions and prototypes that allow you to do something with
the barrier\_t structure. First, you will want to initialize a new barrier.
4-6 You can initialize a static barrier at compile time by using the macro barrier\_
INITIALIZER. You can instead dynamically initialize a barrier by calling the func-
tion barrier\_init.
11-13 Once you have initialized a barrier, you will want to be able to use it. and the
main thing to be done with a barrier is to wait on it. When we're done with a bar-
rier, it would be nice to be able to destroy the barrier and reclaim the resources it
used. We'll call these operations barrier\_init, barrier\_wait, and barrier\_
destroy. All the operations need to specify upon which barrier they will operate.
Because barriers are synchronization objects, and contain both a mutex and a
condition variable (neither of which can be copied), we always pass a pointer to a
barrier. Only the initialization operation requires a second parameter, the num-
ber of waiters required before the barrier opens.
To be consistent with Pthreads conventions, the functions all return an inte-
ger value, representing an error number defined in \<errno.h\>. The value 0
represents success.
barrier.h
part 2 interfaces
/*
* Support static initialization of barriers.
*/
#define BARRIER\_INITIALIZER(cnt) \
{PTHREAD\_MUTEX\_INITIALIZER, PTHREAD\_COND\_INITIALIZER, \
BARRIER\_VALID, cnt, cnt, 0}
246 CHAPTER 7 "Real code"
8 /*
9 * Define barrier functions
10 */
11 extern int barrier\_init (barrier\_t *barrier, int count);
12 extern int barrier\_destroy (barrier\_t *barrier);
13 extern int barrier\_wait (barrier\_t *barrier);
| barrier.h part 2 interfaces
Now that you know the interface definition, you could write a program using
barriers. But then, the point of this section is not to tell you how to use barriers,
but to help improve your understanding of threaded programming by showing
how to build a barrier. The following examples show the functions provided by
barrier. c, to implement the interfaces we've just seen in barrier. h.
Part 1 shows barrier\_init, which you would call to dynamically initialize a
barrier, for example, if you allocate a barrier with malloc.
12 Both the counter and threshold are set to the same value. The counter is the
"working counter" and will be reset to threshold for each barrier cycle.
14-16 If mutex initialization fails, barrier\_init returns the failing status to the
caller.
17-21 If condition variable (cv) initialization fails, barrier\_init destroys the mutex
it had already created and returns the failure status-the status of pthread\_
mutex\_destroy is ignored because the failure to create the condition variable is
more important than the failure to destroy the mutex.
22 The barrier is marked valid only after all initialization is complete. This does
not completely guarantee that another thread erroneously trying to wait on that
barrier will detect the invalid barrier rather than failing in some less easily diag-
nosable manner, but at least it is a token attempt.
| barrier.c part 1 barrier\_init
1 #include \<pthread.h\>
2 #include "errors.h"
3 #include "barrier.h"
4
5 /*
6 * Initialize a barrier for use.
7 */
8 int barrier\_init (barrier\_t *barrier, int count)
9 {
10 int status;
11
12 barrier-\>threshold = barrier-\>counter = count;
13 barrier-\>cycle = 0;
14 status = pthread\_mutex\_init (&barrier-\>mutex, NULL);
15 if (status != 0)
16 return status;
17 status = pthread cond init (&barrier-\>cv, NULL);
Extended synchronization 247
18 if (status != 0) {
19 pthread\_mutex\_destroy (&barrier-\>mutex);
20 return status;
21 }
22 barrier-\>valid = BARRIER\_VALID;
23 return 0 ;
24 }
| barrier.c part 1 barrier\_init
Part 2 shows the barrier\_destroy function, which destroys the mutex and
condition variable (cv) in the barrier\_t structure. If we had allocated any addi-
tional resources for the barrier, we would need to release those resources also.
8-9 First check that the barrier appears to be valid, and initialized, by looking at
the valid member. We don't lock the mutex first, because that will fail, possibly
with something nasty like a segmentation fault, if the mutex has been destroyed
or hasn't been initialized. Because we do not lock the mutex first, the validation
check is not entirely reliable, but it is better than nothing, and will only fail to
detect some race conditions where one thread attempts to destroy a barrier while
another is initializing it, or where two threads attempt to destroy a barrier at
nearly the same time.
19-22 If any thread is currently waiting on the barrier, return ebusy.
24-27 At this point, the barrier is "destroyed"-all that's left is cleanup. To minimize
the chances of confusing errors should another thread try to wait on the barrier
before we're done, mark the barrier "not valid" by clearing valid, before changing
any other state. Then, unlock the mutex, since we cannot destroy it while it :=?
locked.
33-35 Destroy the mutex and condition variable. If the mutex destruction fads
return the status; otherwise, return the status of the condition variable destruc -
tion. Or, to put it another way, return an error status if either destruction failed
otherwise, return success.
| barrier.c part 2 barrier\_destrcy
1 /*
2 * Destroy a barrier when done using it.
3 */
4 int barrier\_destroy (barrier\_t *barrier)
5 {
6 int status, status2;
7
8 if (barrier-\>valid != BARRIER\_VALID)
9 return EINVAL;
10
11 status = pthread\_mutex\_lock (&barrier-\>mutex);
12 if (status != 0)
13 return status;
14
248 CHAPTER 7 "Real code"
15 /*
16 * Check whether any threads are known to be waiting; report
17 * "BUSY" if so.
18 */
19 if (barrier-\>counter != barrier-\>threshold) {
20 pthread\_mutex\_unlock (&barrier-\>mutex);
21 return EBUSY;
22 }
23
24 barrier-\>valid = 0;
25 status = pthread\_mutex\_unlock (&barrier-\>mutex);
26 if (status != 0)
27 return status;
28
29 /*
30 * If unable to destroy either 1003.1c synchronization
31 * object, return the error status.
32 */
33 status = pthread\_mutex\_destroy (&barrier-\>mutex);
34 status2 = pthread\_cond\_destroy (&barrier-\>cv);
35 return (status == 0 ? status : status2);
36 }
| barrier.c part 2 barrier\_destroy
Finally, part 3 shows the implementation of barrier\_wait.
10-n First we verify that the argument barrier appears to be a valid barrier\_t. We
perform this check before locking the mutex, so that barrier\_destroy can safely
destroy the mutex once it has cleared the valid member. This is a simple attempt
to minimize the damage if one thread attempts to wait on a barrier while another
thread is simultaneously either initializing or destroying that barrier.
We cannot entirely avoid problems, since without the mutex, barrier\_wait
has no guarantee that it will see the correct (up-to-date) value of valid. The valid
check may succeed when the barrier is being made invalid, or fail when the bar-
rier is being made valid. Locking the mutex first would do no good, because the
mutex may not exist if the barrier is not fully initialized, or if it is being destroyed.
This isn't a problem as long as you use the barrier correctly-that is, you initialize
the barrier before any thread can possibly try to use it, and do not destroy the
barrier until you are sure no thread will try to use it again.
17 Copy the current value of the barrier's cycle into a local variable. The com-
parison of our local cycle against the barrier\_t structure's cycle member
becomes our condition wait predicate. The predicate ensures that all currently
waiting threads will return from barrier\_wait when the last waiter broadcasts
the condition variable, but that any thread that calls barrier\_wait again will
wait for the next broadcast. (This is the "tricky part" of correctly implementing a
barrier.)
Extended synchronization 249
19-22 Now we decrease counter, which is the number of threads that are required
but haven't yet waited on the barrier. When counter reaches 0, no more threads
are needed-they're all here and waiting anxiously to continue to the next attrac-
tion. Now all we need to do is tell them to wake up. We advance to the next cycle,
reset the counter, and broadcast the barrier's condition variable.
28-29 Earlier, I mentioned that a program often needs one thread to perform some
cleanup or setup between parallel regions. Each thread could lock a mutex and
check a flag so that only one thread would perform the setup. However, the setup
may not need additional synchronization, for example, because the other threads
will wait at a barrier for the next parallel region, and, in that case, it would be
nice to avoid locking an extra mutex.
The barrier\_wait function has this capability built into it. One and only one
thread will return with the special value of -1 while the others return 0. In this
particular implementation, the one that waits last and wakes the others will take
the honor, but in principle it is "unspecified" which thread returns -1. The thread
that receives -1 can perform the setup, while others race ahead. If you do not
need the special return status, treat -1 as another form of success. The proposed
POSIX. lj standard has a similar capability-one (unspecified) thread completing
a barrier will return the status barrier\_serial\_thread.
3 5 Any threaded code that uses condition variables should always either support
deferred cancellation or disable cancellation. Remember that there are two dis-
tinct types of cancellation: deferred and asynchronous. Code that deals with
asynchronous cancellation is rare. In general it is difficult or impossible to sup-
port asynchronous cancellation in any code that acquires resources (including
locking a mutex). Programmers can't assume any function supports asynchro-
nous cancellation unless its documentation specifically says so. Therefore we do
not need to worry about asynchronous cancellation.
We could code barrier\_wait to deal with deferred cancellation, but that
raises difficult questions. How, for example, will the barrier wait ever be satisfied
if one of the threads has been canceled? And if it won't be satisfied, what happens
to all the other threads that have already waited (or are about to wait) on that
barrier? There are various ways to answer these questions. One would be for
barrier\_wait to record the thread identifiers of all threads waiting on the bar-
rier, and for any thread that's canceled within the wait to cancel all other waiters.
Or we might handle cancellation by setting some special error flag and broad-
casting the condition variable, and modifying barrier\_wait to return a special
error when awakened in that way. However, it makes little sense to cancel one
thread that's using a barrier. We're going to disallow it, by disabling cancellation
prior to the wait, and restoring the previous state of cancellation afterward. This
is the same approach taken by the proposed POSIX. lj standard, by the way-bar-
rier waits are not cancellation points.
42-46 If there are more threads that haven't reached the barrier, we need to wait for
them. We do that by waiting on the condition variable until the barrier has
advanced to the next cycle-that is, the barrier's cycle no longer matches the
local copy.
250 CHAPTER 7 "Real code"
| barrier.c part 3 barrier\_wait
1 /*
2 * Wait for all members of a barrier to reach the barrier. When
3 * the count (of remaining members) reaches 0, broadcast to wake
4 * all threads waiting.
5 */
6 int barrier\_wait (barrier\_t *barrier)
7 {
8 int status, cancel, tmp, cycle;
9
10 if (barrier-\>valid != BARRIER\_VALID)
11 return EINVAL;
12
13 status = pthread\_mutex\_lock (&barrier-\>mutex);
14 if (status != 0)
15 return status;
16
17 cycle = barrier-\>cycle; /* Remember which cycle we're on */
18
19 if (-barrier-\>counter ==0) {
20 barrier-\>cycle = !barrier-\>cycle;
21 barrier-\>counter = barrier-\>threshold;
22 status = pthread\_cond\_broadcast (&barrier-\>cv);
23 /*
24 * The last thread into the barrier will return status
25 * -1 rather than 0, so that it can be used to perform
26 * some special serial code following the barrier.
27 */
28 if (status == 0)
29 status = -1;
30 } else {
31 /*
32 * Wait with cancellation disabled, because barrier\_wait
33 * should not be a cancellation point.
34 */
35 pthread\_setcancelstate (PTHREAD\_CANCEL\_DISABLE, scancel);
36
37 /*
38 * Wait until the barrier's cycle changes, which means
39 * that it has been broadcast, and we don't want to wait
40 * anymore.
41 */
42 while (cycle == barrier-\>cycle) {
43 status = pthread\_cond\_wait (
44 &barrier-\>cv, &barrier-\>mutex);
45 if (status != 0) break;
46 }
47
Extended synchronization 251
48 pthread\_setcancelstate (cancel, &tmp);
49 }
50 /*
51 * Ignore an error in unlocking. It shouldn't happen, and
52 * reporting it here would be misleading - the barrier wait
53 * completed, after all, whereas returning, for example,
54 * EINVAL would imply the wait had failed. The next attempt
55 * to use the barrier *will* return an error, or hang, due
56 * to whatever happened to the mutex.
57 */
58 pthread\_mutex\_unlock (&barrier-\>mutex);
59 return status; /* error, -1 for waker, or 0 */
60 }
| barrier.c part 3 barrier\_wait
Finally, barrier\_main.c is a simple program that uses barriers. Each thread
loops on calculations within a private array.
35,47 At the beginning and end of each iteration, the threads, running function
thread\_routine, all wait on a barrier to synchronize the operation.
56-61 At the end of each iteration, the "lead thread" (the one receiving a -1 result
from barrier\_wait) will modify the data of all threads, preparing them for the
next iteration. The others go directly to the top of the loop and wait on the barrier
at line 35.
| barrier\_main.c
1 #include \<pthread.h\>
2 #include "barrier.h"
3 #include "errors.h"
4
5 #define THREADS 5
6 #define ARRAY 6
7 #define INLOOPS 1000
8 #define OUTLOOPS 10
9
10 /*
11 * Keep track of each thread.
12 */
13 typedef struct thread\_tag {
14 pthread\_t thread\_id;
15 int number;
16 int increment;
17 int array[ARRAY];
18 } thread\_t;
19
20 barrier\_t barrier;
21 thread\_t thread[THREADS];
22
252 CHAPTER 7 "Real code"
23 /*
24 * Start routine for threads.
25 */
26 void *thread\_routine (void *arg)
27 {
28 thread\_t *self = (thread\_t*)arg; /* Thread's thread\_t */
29 int in\_loop, out\_loop, count, status;
30
31 /*
32 * Loop through OUTLOOPS barrier cycles.
33 */
34 for (out\_loop = 0; out\_loop \< OUTLOOPS; out\_loop++) {
35 status = barrier\_wait (Sbarrier);
36 if (status \> 0)
37 err\_abort (status, "Wait on barrier");
38
39 /*
40 * This inner loop just adds a value to each element in
41 * the working array.
42 */
43 for (in\_loop = 0; in\_loop \< INLOOPS; in\_loop++)
44 for (count = 0; count \< ARRAY; count++)
45 self-\>array[count] += self-\>increment;
46
47 status = barrier\_wait (Sbarrier);
48 if (status \> 0)
49 err\_abort (status, "Wait on barrier");
50
51 /*
52 * The barrier causes one thread to return with the
53 * special return status -1. The thread receiving this
54 * value increments each element in the shared array.
55 */
56 if (status == -1) {
57 int thread\_num;
58
59 for (thread\_num = 0; thread\_num \< THREADS; thread\_num++)
60 thread[thread\_num].increment += 1;
61 }
62 \>
63 return NULL;
64 }
65
66 int main (int arg, char *argv[])
67 {
68 int thread\_count, array\_count;
69 int status;
70
71 barrier\_init (Sbarrier, THREADS);
72
Extended synchronization 253
73 /*
74 * Create a set of threads that will use the barrier.
75 */
76 for (thread\_count = 0; thread\_count \< THREADS; thread\_count++) {
77 thread[thread\_count].increment = thread\_count;
78 thread[thread\_count].number = thread\_count;
79
80 for (array\_count = 0; array\_count \< ARRAY; array\_count++)
81 thread[thread\_count].array[array\_count] = array\_count + 1;
82
83 status = pthread\_create (&thread[thread\_count].thread\_id,
84 NULL, thread\_routine, (void*)&thread[thread\_count]);
85 if (status != 0)
86 err\_abort (status, "Create thread");
87 }
88
89 /*
90 * Now join with each of the threads.
91 */
92 for (thread\_count = 0; thread\_count \< THREADS; thread\_count++) {
93 status = pthread\_join (thread[thread\_count].thread\_id, NULL);
94 if (status != 0)
95 err\_abort (status, "Join thread");
96
97 printf ("%02d: (%d) ",
98 thread\_count, thread[thread\_count].increment);
99 ~
100 for (array\_count = 0; array\_count \< ARRAY; array\_count++)
101 printf ("%010u ",
102 thread[thread\_count].array[array\_count]);
103 printf ("\n");
104 }
105
106 /*
107 * To be thorough, destroy the barrier.
108 */
109 barrier\_destroy (sbarrier);
110 return 0;
111 }
| barrier main.c
### 7.1.2 Read/write locks
A read/write lock is a lot like a mutex. It is another way to prevent more than
one thread from modifying shared data at the same time. But unlike a mutex it
distinguishes between reading data and writing data. A mutex excludes all other
threads, while a read/write lock allows more than one thread to read the data, as
long as none of them needs to change it.
254
CHAPTER 7 "Real code"
Read/write locks are used to protect information that you need to read fre-
quently but usually don't need to modify. For example, when you build a cache of
recently accessed information, many threads may simultaneously examine the
cache without conflict. When a thread needs to update the cache, it must have
exclusive access.
When a thread locks a read/write lock, it chooses shared read access or exclu-
sive write access. A thread that wants read access can't continue while any thread
currently has write access. A thread trying to gain write access can't continue
when another thread currently has either write access or read access.
When both readers and writers are waiting for access at the same time, the
readers are given precedence when the write lock is released. Read precedence
favors concurrency because it potentially allows many threads to accomplish
work simultaneously. Write precedence on the other hand would ensure that
pending modifications to the shared data are completed before the data is used.
There's no absolute right or wrong policy, and if you don't find the implementa-
tion here appropriate for you, it is easy to change.
Figure 7.3 shows the operation of a read/write lock being used to synchronize
three threads, called thread 1, thread 2, and thread 3. The figure is a sort of tim-
ing diagram, with time increasing from left to right. Each of the lines beginning at
the labels in the upper left designates the behavior of a specific thread-solid for
thread 1, dotted for thread 2, and dashed for thread 3. When the lines drop
within the rounded rectangle, they are interacting with the read/write lock. If the
thread 2 waits
for read lock
thread 1 locks
for write
r thread 1 unlocks
thread 3 unlocks
thread 1 (writer)
thread 2 (reader)
thread 3 (reader)
time:
/render/writer
lock
thread 2 locks
for read
thread 1 waits
for write lock
thread 3 locks
for read
thread 1 locks
for write
FIGURE 7.3 Read/write lock operation
Extended synchronization 255
line drops below the center line, it shows that the thread has the read/write lock
locked, either for exclusive write or for shared read. Lines that hover above the
center line represent threads waiting for the lock.
In this example, thread 1 locks the read/write lock for exclusive write. Thread 2
tries to lock the read/write lock for shared read and, finding it already locked for
exclusive write, blocks. When thread 1 releases the lock, it awakens thread 2,
which then succeeds in locking the read/write lock for shared read. Thread 3
then tries to lock the read/write lock for shared read and, because the read/write
lock is already locked for shared read, it succeeds immediately. Thread 1 then
tries to lock the read/write lock again for exclusive write access, and blocks
because the read/write lock is already locked for read access. When thread 3
unlocks the read/write lock, it cannot awaken thread 1, because there is another
reader. Only when thread 2 also unlocks the read/write lock, and the lock
becomes unlocked, can thread 1 be awakened to lock the read/write lock for
exclusive write access.
The header file rwlock. h and the C source file rwlock. c demonstrate an imple-
mentation of read/write locks using standard Pthreads mutexes and condition
variables. This is a portable implementation that is relatively easy to understand.
One could, of course, create a much more efficient implementation for any specific
system based on knowledge of nonportable hardware and operating system
characteristics.
The rest of this section shows the details of a read/write lock package. First,
rwlock. h describes the interfaces, and then rwlock. c provides the implementa-
tion. Part 1 shows the structure of a read/write lock, represented by the type
rwlock\_t.
7-9 Of course, there's a mutex to serialize access to the structure. We'll use two
separate condition variables, one to wait for read access (called read) and one to
wait for write access (called, surprisingly, write).
10 The rwlock\_t structure has a valid member to easily detect common usage
errors, such as trying to lock a read/write lock that hasn't been initialized. The
member is set to a magic number when the read/write lock is initialized, just as
in barrier\_init.
11-12 To enable us to determine whether either condition variable has waiters, we'll
keep a count of active readers (r\_active) and a flag to indicate an active writer
(w\_active).
13-14 We also keep a count of the number of threads waiting for read access (r\_wait)
and for write access (wwait).
17 Finally, we need a "magic number" for our valid member. (See the footnote in
Section 7.1.1 if you missed this part of the barrier example.)
| rwlock.h part 1 rwlock\_t
1 #include \<pthread.h\>
2
3 /*
4 * Structure describing a read/write lock.
5 */
256 CHAPTER 7 "Real code"
6
7
8
9
10
11
12
13
14
15
16
17
typedef struct rwlock
pthread mutex t
pthread cond t
pthread cond t
int
int
int
int
int
} rwlock t;
#define RWLOCK\_VALID
\_tag {
mutex;
read;
write;
valid;
r active;
w active;
r wait;
w wait;
Oxfacade
/*
/*
/*
/*
/*
/*
/*
wait for read */
wait for write */
set when valid */
readers active */
writer active */
readers waiting */
writers waiting */
rwlock.h part 1 rwlock\_t
We could have saved some space and simplified the code by using a single
condition variable, with readers and writers waiting using separate predicate
expressions. We will use one condition variable for each predicate, because it is
more efficient. This is a common trade-off. The main consideration is that when
two predicates share a condition variable, you must always wake them using
pthread\_cond\_broadcast, which would mean waking all waiters each time the
read/write lock is unlocked.
We keep track of a boolean variable for "writer active," since there can only be
one. There are also counters for "readers active," "readers waiting," and "writers
waiting." We could get by without counters for readers and writers waiting. All
readers are awakened simultaneously using a broadcast, so it doesn't matter how
many there are. Writers are awakened only if there are no readers, so we could
dispense with keeping track of whether there are any threads waiting to write (at
the cost of an occasional wasted condition variable signal when there are no
waiters).
We count the number of threads waiting for read access because the condition
variable waits might be canceled. Without cancellation, we could use a simple
flag-"threads are waiting for read" or "no threads are waiting for read." Each
thread could set it before waiting, and we could clear it before broadcasting to
wake all waiting readers. However, because we can't count the threads waiting on
a condition variable, we wouldn't know whether to clear that flag when a waiting
reader was canceled. This information is critical, because if there are no readers
waiting when the read/write lock is unlocked, we must wake a writer-but we
cannot wake a writer if there are waiting readers. A count of waiting readers,
which we can decrease when a waiter is canceled, solves the problem.
The consequences of "getting it wrong" are less important for writers than for
readers. Because we check for readers first, we don't really need to know whether
there are writers. We could signal a "potential writer" anytime the read/write lock
was released with no waiting readers. But counting waiting writers allows us to
avoid a condition variable signal when no threads are waiting.
Part 2 shows the rest of the definitions and the function prototypes.
Extended synchronization 257
4-6 The rwlock\_initializer macro allows you to statically initialize a read/write
lock.
n-18 Of course, you must also be able to initialize a read/write lock that you cannot
allocate statically, so we provide rwl\_init to initialize dynamically, and rwl\_
destroy to destroy a read/write lock once you're done with it. In addition, there
are functions to lock and unlock the read/write lock for either read or write
access. You can "try to lock" a read/write lock, either for read or write access, by
calling rwl\_readtrylock or rwl\_writetrylock., just as you can try to lock a
mutex by calling pthread\_mutex\_trylock.
| rwlock.h part 2 interfaces
1 /*
2 * Support static initialization of barriers.
3 */
4 #define RWL\_INITIALIZER \
5 {PTHREAD\_MUTEX\_INITIALIZER, PTHREAD\_COND\_INITIALIZER, \
6 PTHREAD\_COND\_INITIALIZER, RWLOCK\_VALID, 0, 0, 0, 0}
7
8 /*
9 * Define read/write lock functions.
10 */
11 extern int rwl\_init (rwlock\_t *rwlock);
12 extern int rwl\_destroy (rwlock\_t *rwlock);
13 extern int rwl\_readlock (rwlock\_t *rwlock);
14 extern int rwl\_readtrylock (rwlock\_t *rwlock);
15 extern int rwl\_readunlock (rwlock\_t Twlock);
16 extern int rwl\_writelock (rwlock\_t *rwlock);
17 extern int rwl\_writetrylock (rwlock\_t *rwlock);
18 extern int rwl\_writeunlock (rwlock\_t *rwlock);
| rwlock.h part 2 interfaces
The file rwlock. c contains the implementation of read/write locks. The follow-
ing examples break down each of the functions used to implement the rwlock.h
interfaces.
Part 1 shows rwl\_init, which initializes a read/write lock. It initializes the
Pthreads synchronization objects, initializes the counters and flags, and finally
sets the valid sentinel to make the read/write lock recognizable to the other
interfaces. If we are unable to initialize the read condition variable, we destroy
the mutex that we'd already created. Similarly, if we are unable to initialize the
write condition variable, we destroy both the mutex and the read condition
variable.
| rwlock.c part 1 rwl\_init
1 #include \<pthread.h\>
2 #include "errors.h"
258 CHAPTER 7 "Real code"
3 #include "rwlock.h"
4
5 /*
6 * Initialize a read/write lock.
7 */
8 int rwl\_init (rwlock\_t *rwl)
9 {
10 int status;
11
12 rwl-\>r\_active = 0;
13 rwl-\>r\_wait = rwl-\>w\_wait = 0;
14 rwl-\>w\_active = 0;
15 status = pthread\_mutex\_init (&rwl-\>mutex, NULL);
16 if (status != 0)
17 return status;
18 status = pthread\_cond\_init (&rwl-\>read, NULL);
19 if (status != 0) {
20 /* if unable to create read CV, destroy mutex */
21 pthread\_mutex\_destroy (&rwl-\>mutex);
22 return status;
23 }
24 status = pthread\_cond\_init (&rwl-\>write, NULL);
25 if (status != 0) {
26 /* if unable to create write CV, destroy read CV and mutex */
27 pthread\_cond\_destroy (&rwl-\>read);
28 pthread\_mutex\_destroy (&rwl-\>mutex);
29 return status;
30 }
31 rwl-\>valid = RWLOCK\_VALID;
32 return 0;
33 }
| rwlock.c part 1 rwl\_init
Part 2 shows the rwl\_destroy function, which destroys a read/write lock.
8-9 We first try to verify that the read/write lock was properly initialized by check-
ing the valid member. This is not a complete protection against incorrect usage,
but it is cheap, and it will catch some of the most common errors. See the anno-
tation for barrier.c, part 2, for more about how the valid member is used.
10-30 Check whether the read/write lock is in use. We look for threads that are
using or waiting for either read or write access. Using two separate if statements
makes the test slightly more readable, though there's no other benefit.
36-39 As in barrier\_destroy, we destroy all Pthreads synchronization objects, and
store each status return. If any of the destruction calls fails, returning a nonzero
value, rwl\_destroy will return that status, and if they all succeed it will return 0
for success.
Extended synchronization 259
| rwlock.c part 2 rwl\_destroy
1 /*
2 * Destroy a read/write lock.
3 */
4 int rwl\_destroy (rwlock\_t *rwl)
5 {
6 int status, statusl, status2;
7
8 if (rwl-\>valid != RWLOCK\_VALID)
9 return EINVAL;
10 status = pthread\_mutex\_lock (&rwl-\>mutex);
11 if (status != 0)
12 return status;
13
14 /*
15 * Check whether any threads own the lock; report "BUSY" if
16 * so.
17 */
18 if (rwl-\>r\_active \> 0 || rwl-\>w\_active) {
19 pthread\_mutex\_unlock (&rwl-\>mutex);
20 return EBUSY;
21 }
22
23 /*
24 * Check whether any threads are known to be waiting; report
25 * EBUSY if so.
26 */
27 if (rwl-\>r\_wait != 0 || rwl-\>w\_wait != 0) {
28 pthread\_mutex\_unlock (&rwl-\>mutex);
29 return EBUSY;
30 }
31
32 rwl-\>valid = 0;
33 status = pthread\_mutex\_unlock (&rwl-\>mutex);
34 if (status != 0)
35 return status;
36 status = pthread\_mutex\_destroy (&rwl-\>mutex);
37 statusl = pthread\_cond\_destroy (&rwl-\>read);
38 status2 = pthread\_cond\_destroy (&rwl-\>write);
39 return (status == 0 ? status
40 : (statusl == 0 ? statusl : status2));
41 }
| rwlock.c part 2 rwl\_destroy
260 CHAPTER 7 "Real code"
Part 3 shows the code for rwl\_readcleanup and rwl\_writecleanup, two can-
cellation cleanup handlers used in locking the read/write lock for read and write
access, respectively. As you may infer from this, read/write locks, unlike barriers,
are cancellation points. When a wait is canceled, the waiter needs to decrease the
count of threads waiting for either a read or write lock, as appropriate, and unlock
the mutex.
| rwlock.c part 3 cleanuphandlers
1 /*
2 * Handle cleanup when the read lock condition variable
3 * wait is canceled.
4 *
5 * Simply record that the thread is no longer waiting,
6 * and unlock the mutex.
7 */
8 static void rwl\_readcleanup (void *arg)
9 {
10 rwlock\_t *rwl = (rwlock\_t *)arg;
11
12 rwl-\>r\_wait-;
13 pthread\_mutex\_unlock (&rwl-\>mutex);
14 }
15
16 /*
17 * Handle cleanup when the write lock condition variable
18 * wait is canceled.
19 *
20 * Simply record that the thread is no longer waiting,
21 * and unlock the mutex.
22 */
23 static void rwl\_writecleanup (void *arg)
24 {
25 rwlock\_t *rwl = (rwlock\_t *)arg;
26
27 rwl-\>w\_wait-;
28 pthread\_mutex\_unlock (&rwl-\>mutex);
29 }
| rwlock.c part 3 cleanuphandlers
10-26 Part 4 shows rwl\_readlock, which locks a read/write lock for read access. If a
writer is currently active (w\_active is nonzero), we wait for it to broadcast the
read condition variable. The r\_wait member counts the number of threads wait-
ing to read. This could be a simple boolean variable, except for one problem-
when a waiter is canceled, we need to know whether there are any remaining
waiters. Maintaining a count makes this easy, since the cleanup handler only
needs to decrease the count.
Extended synchronization 261
This is one of the places where the code must be changed to convert our read/
write lock from "reader preference" to "writer preference," should you choose to
do that. To implement writer preference, a reader must block while there are
waiting writers (w\_wait \> 0), not merely while there are active writers, as we do
here.
15,21 Notice the use of the cleanup handler around the condition wait. Also, notice
that we pass the argument 0 to pthread\_cleanup\_pop so that the cleanup code is
called only if the wait is canceled. We need to perform slightly different actions
when the wait is not canceled. If the wait is not canceled, we need to increase the
count of active readers before unlocking the mutex.
| rwlock.c part 4 rwl\_readlock
1 /*
2 * Lock a read/write lock for read access.
3 */
4 int rwl\_readlock (rwlock\_t *rwl)
5 {
6 int status;
7
8 if (rwl-\>valid != RWLOCK\_VALID)
9 return EINVAL;
10 status = pthread\_mutex\_lock (&rwl-\>mutex);
11 if (status != 0)
12 return status;
13 if (rwl-\>w\_active) {
14 rwl-\>r\_wait++;
15 pthread\_cleanup\_push (rwl\_readcleanup, (void*)rwl);
16 while (rwl-\>w\_active) {
17 status = pthread\_cond\_wait (&rwl-\>read, &rwl-\>mutex);
18 if (status != 0)
19 break;
20 }
21 pthread\_cleanup\_pop @);
22 rwl-\>r\_wait--;
23 }
24 if (status == 0)
25 rwl-\>r\_active++;
26 pthread\_mutex\_unlock (&rwl-\>mutex);
27 return status;
28 \>
| rwlock.c part 4 rwl\_readlock
Part 5 shows rwl\_readtrylock. This function is nearly identical to rwl\_read-
lock, except that, instead of waiting for access if a writer is active, it returns
ebusy. It doesn't need a cleanup handler, and has no need to increase the count
of waiting readers.
262 CHAPTER 7 "Real code"
This function must also be modified to implement "writer preference" read/
write locks, by returning EBUSY when a writer is waiting, not just when a writer is
active.
| rwlock.c part 5 rwl\_readtrylock
1 /*
2 * Attempt to lock a read/write lock for read access (don't
3 * block if unavailable).
4 */
5 int rwl\_readtrylock (rwlock\_t *rwl)
6 {
7 int status, status2;
8
9 if (rwl-\>valid != RWLOCK\_VALID)
10 return EINVAL;
11 status = pthread\_mutex\_lock (&rwl-\>mutex);
12 if (status != 0)
13 return status;
14 if (rwl-\>w\_active)
15 status = EBUSY;
16 else
17 rwl-\>r\_active++;
18 status2 = pthread\_mutex\_unlock (&rwl-\>mutex);
19 return (status2 != 0 ? status2 : status);
20 }
| rwlock.c part 5 rwl\_readtrylock
13 Part 6 shows rwlreadunlock. This function essentially reverses the effect of
rwl\_readlock or rwl\_tryreadlock, by decreasing the count of active readers
(r\_active).
14-15 If there are no more active readers, and at least one thread is waiting for write
access, signal the write condition variable to unblock one. Note that there is a
race here, and whether you should be concerned about it depends on your notion
of what should happen. If another thread that is interested in read access calls
rwl\_readlock or rwl\_tryreadlock before the awakened writer can run, the reader
may "win," despite the fact that we just selected a writer.
Because our version of read/write locks has "reader preference," this is what
we usually want to happen-the writer will determine that it has failed and will
resume waiting. (It received a spurious wakeup.) If the implementation changes to
prefer writers, the spurious wakeup will not occur, because the potential reader
would have to block. The waiter we just unblocked cannot decrease w\_wait until it
actually claims the lock.
Extended synchronization 263
| rwlock.c part 6 rwl\_readunlock
1 /*
2 * Unlock a read/write lock from read access.
3 */
4 int rwl\_readunlock (rwlock\_t *rwl)
5 {
6 int status, status2;
8 if (rwl-\>valid != RWLOCK\_VALID)
9 return EINVAL;
10 status = pthread\_mutex\_lock (&rwl-\>mutex);
11 if (status != 0)
12 return status;
13 rwl-\>r\_active-;
14 if (rwl-\>r\_active == 0 && rwl-\>w\_wait \> 0)
15 status = pthread\_cond\_signal (&rwl-\>write);
16 status2 = pthread\_mutex\_unlock (&rwl-\>mutex);
17 return (status2 == 0 ? status : status2);
18 }
| rwlock.c part 6 rwl\_readunlock
13 Part 7 shows rwl\_writelock. This function is much like rwl\_readlock, except
for the predicate condition on the condition variable wait. In part 1, I explained
that, to convert from "preferred read" to "preferred write," a potential reader would
have to wait until there were no active or waiting writers, whereas currently it
waits only for active writers. The predicate in rwlwritelock is the converse of
that condition. Because we support "preferred read," in theory, we must wait here
if there are any active or waiting readers. In fact, it is a bit simpler, because if there
are any active readers, there cannot be any waiting readers-the whole point of a
read/write lock is that multiple threads can have read access at the same time.
On the other hand, we do have to wait if there are any active writers, because we
allow only one writer at a time.
25 Unlike ractive, which is a counter, wactive is treated as a boolean. Or is it
a counter? There's really no semantic difference, since the value of 1 can be con-
sidered a boolean true or a count of 1-there can be only one active writer at any
time.
m rwlock.c part 7 rwl\_writelock
1 /*
2 * Lock a read/write lock for write access.
3 */
4 int rwl\_writelock (rwlock\_t *rwl)
264 CHAPTER 7 "Real code"
5 {
6 int status;
7
8 if (rwl-\>valid != RWLOCK\_VALID)
9 return EINVAL;
10 status = pthread\_mutex\_lock (&rwl-\>mutex);
11 if (status != 0)
12 return status;
13 if (rwl-\>w\_active || rwl-\>r\_active \> 0) {
14 rwl-\>w\_wait++;
15 pthread\_cleanup\_push (rwl\_writecleanup, (void*)rwl);
16 while (rwl-\>w\_active || rwl-\>r\_active \> 0) {
17 status = pthread\_cond\_wait (&rwl-\>write, &rwl-\>mutex);
18 if (status != 0)
19 break;
20 }
21 pthread\_cleanup\_pop @);
22 rwl-\>w\_wait-;
23 }
24 if (status == 0)
25 rwl-\>w\_active = 1;
26 pthread\_mutex\_unlock (&rwl-\>mutex);
27 return status;
28 }
| rwlock.c part 7 rwl\_writelock
Part 8 shows rwl\_writetrylock. This function is much like rwl\_writelock,
except that it returns ebusy if the read/write lock is currently in use (either by a
reader or by a writer) rather than waiting for it to become free.
| rwlock.c part 8 rwl\_writetrylock
1 /*
2 * Attempt to lock a read/write lock for write access. Don't
3 * block if unavailable.
4 */
5 int rwl\_writetrylock (rwlock\_t *rwl)
6 {
7 int status, status2;
8
9 if (rwl-\>valid != RWLOCK\_VALID)
10 return EINVAL;
11 status = pthread\_mutex\_lock (&rwl-\>mutex);
12 if (status != 0)
13 return status;
14 if (rwl-\>w\_active || rwl-\>r\_active \> 0)
15 status = EBUSY;
Extended synchronization 265
16 else
17 rwl-\>w\_active = 1;
18 status2 = pthread\_mutex\_unlock (&rwl-\>mutex);
19 return (status != 0 ? status : status2);
20 }
B rwlock.c part 8 rwl\_writetrylock
Finally, part 9 shows rwl\_writeunlock. This function is called by a thread
with a write lock, to release the lock.
13-19 When a writer releases the read/write lock, it is always free; if there are any
threads waiting for access, we must wake one. Because we implement "preferred
read" access, we first look for threads that are waiting for read access. If there are
any, we broadcast the read condition variable to wake them all.
20-26 If there were no waiting readers, but there are one or more waiting writers,
wake one of them by signaling the write condition variable.
To implement a "preferred write" lock, you would reverse the two tests, waking
a waiting writer, if any, before looking for waiting readers.
| rwlock.c part 9 rwl\_writeunlock
1 /*
2 * Unlock a read/write lock from write access.
3 */
4 int rwl\_writeunlock (rwlock\_t *rwl)
5 {
6 int status;
7
8 if (rwl-\>valid != RWLOCK\_VALID)
9 return EINVAL;
10 status = pthread\_mutex\_lock (&rwl-\>mutex);
11 if (status != 0)
12 return status;
13 rwl-\>w\_active = 0;
14 if (rwl-\>r\_wait \> 0) {
15 status = pthread\_cond\_broadcast (&rwl-\>read);
16 if (status != 0) {
17 pthread\_mutex\_unlock (&rwl-\>mutex);
18 return status;
19 }
20 } else if (rwl-\>w\_wait \> 0) {
21 status = pthread\_cond\_signal (&rwl-\>write);
22 if (status != 0) {
23 pthread\_mutex\_unlock (&rwl-\>mutex);
24 return status;
25 }
26 }
266 CHAPTER 7 "Real code"
27 status = pthread\_mutex\_unlock (&rwl-\>mutex);
28 return status;
29 }
| rwlock.c part 9 writelock
Now that we have all the pieces, rwlock\_main.c shows a program that uses
read/write locks.
n-17 Each thread is described by a structure of type thread\_t. The thread\_num
member is the thread's index within the array of thread\_t structures. The
thread\_id member is the pthread\_t (thread identifier) returned by pthread\_
create when the thread was created. The updates and reads members are
counts of the number of read lock and write lock operations performed by the
thread. The interval member is generated randomly as each thread is created,
to determine how many iterations the thread will read before it performs a write.
22-26 The threads cycle through an array of data\_t elements. Each element has a
read/write lock, a data element, and a count of how many times some thread has
updated the element.
48-58 The program creates a set of threads running the thread\_routine function.
Each thread loops iterations times, practicing use of the read/write lock. It
cycles through the array of data elements in sequence, resetting the index (ele-
ment) to 0 when it reaches the end. At intervals specified by each thread's
interval member, the thread will modify the current data element instead of
reading it. The thread locks the read/write lock for write access, stores its
thread\_num as the new data value, and increases the updates counter.
59-73 On all other iterations, thread\_routine reads the current data element, lock-
ing the read/write lock for read access. It compares the data value against its
thread\_num to determine whether it was the most recent thread to update that
data element, and, if so, it increments a counter.
95-103 On Solaris systems, increase the thread concurrency level to generate more
interesting activity. Without timesllcing of user threads, each thread would tend
to execute sequentially otherwise.
| rwlock\_main.c
1 #include "rwlock.h"
2 #include "errors.h"
3
4 #define THREADS 5
5 #define DATASIZE 15
6 #define ITERATIONS 10000
7
8 /*
9 * Keep statistics for each thread.
10 */
11 typedef struct thread\_tag {
12 int thread num;
Extended synchronization 267
13 pthread\_t thread\_id;
14 int updates;
15 int reads;
16 int interval;
17 } thread\_t;
18
19 /*
20 * Read/write lock and shared data.
21 */
22 typedef struct data\_tag {
23 rwlock\_t lock;
24 int data;
25 int updates;
26 } data\_t;
27
28 thread\_t threads[THREADS];
29 data\_t data[DATASIZE];
30
31 /*
32 * Thread start routine that uses read/write locks.
33 */
34 void *thread\_routine (void *arg)
35 {
36 thread\_t *self = (thread\_t*)arg;
37 int repeats = 0;
38 int iteration;
39 int element =0;
40 int status;
41
42 for (iteration = 0; iteration \< ITERATIONS; iteration++) {
43 /*
44 * Each "self-\>interval" iterations, perform an
45 * update operation (write lock instead of read
46 * lock).
47 */
48 if ((iteration % self-\>interval) == 0) {
49 status = rwl\_writelock (&data[element].lock);
50 if (status != 0)
51 err\_abort (status, "Write lock");
52 data[element].data = self-\>thread\_num;
53 data[element].updates++;
54 self-\>updates++;
55 status = rwl\_writeunlock (&data[element].lock);
56 if (status != 0)
57 err\_abort (status, "Write unlock");
58 } else {
59 /*
60 * Look at the current data element to see whether
61 * the current thread last updated it. Count the
268 CHAPTER 7 "Real code"
62 * times, to report later.
63 */
64 status = rwl\_readlock (&data[element].lock);
65 if (status != 0)
66 err\_abort (status, "Read lock");
67 self-\>reads++;
68 if (data[element].data == self-\>thread\_num)
69 repeats++;
70 status = rwl\_readunlock (&data[element].lock);
71 if (status != 0)
72 err\_abort (status, "Read unlock");
73 }
74 element++;
75 if (element \>= DATASIZE)
76 element = 0;
77 }
78
79 if (repeats \> 0)
80 printf (
81 "Thread %d found unchanged elements %d times\n",
82 self-\>thread\_num, repeats);
83 return NULL;
84 }
85
86 int main (int argc, char *argv[])
87 {
88 int count;
89 int data\_count;
90 int status;
91 unsigned int seed = 1;
92 int thread\_updates = 0;
93 int data\_updates =0;
94
95 #ifdef sun
96 /*
97 * On Solaris 2.5, threads are not timesliced. To ensure
98 * that our threads can run concurrently, we need to
99 * increase the concurrency level to THREADS.
100 */
101 DPRINTF (("Setting concurrency level to %d\n", THREADS));
102 thr\_setconcurrency (THREADS);
103 #endif
104
105 /*
106 * Initialize the shared data.
107 */
108 for (data\_count = 0; data\_count \< DATASIZE; data\_count++) {
109 data[data\_count].data = 0;
110 data[data\_count].updates = 0;
Extended synchronization 269
111 status = rwl\_init (&data[data\_count].lock);
112 if (status != 0)
113 err\_abort (status, "Init rw lock");
114 }
115
116 /*
117 * Create THREADS threads to access shared data.
118 */
119 for (count = 0; count \< THREADS; count++) {
120 threads[count].thread\_num = count;
121 threads[count].updates = 0;
122 threads[count].reads = 0;
123 threads[count].interval = rand\_r (Sseed) % 71;
124 status = pthread\_create (&threads[count].thread\_id,
125 NULL, thread\_routine, (void*)&threads[count]);
126 if (status != 0)
127 err\_abort (status, "Create thread");
128 }
129
130 /*
131 * Wait for all threads to complete, and collect
132 * statistics.
133 */
134 for (count = 0; count \< THREADS; count++) {
135 status = pthread\_join (threads[count].thread\_id, NULL);
136 if (status != 0)
137 err\_abort (status, "Join thread");
138 thread\_updates += threads[count].updates;
139 printf ("%02d: interval %d, updates %d, reads %d\n",
140 count, threads[count].interval,
141 threads[count].updates, threads[count].reads);
142 }
143
144 /*
145 * Collect statistics for the data.
146 */
147 for (data\_count = 0; data\_count \< DATASIZE; data\_count++) {
148 data\_updates += data[data\_count].updates;
149 printf ("data %02d: value %d, %d updates\n",
150 data\_count, data[data\_count].data,
151 data[data\_count].updates);
152 rwl\_destroy (&data[data\_count].lock);
153 }
154
155 printf ("%d thread updates, %d data updates\n",
156 thread\_updates, data\_updates);
157 return 0;
158 }
| rwlock main.c
270 CHAPTER 7 "Real code"
## 7.2 Work queue manager
I've already briefly outlined the various models of thread cooperation. These
include pipelines, work crews, client/servers, and so forth. In this section, I
present the development of a "work queue," a set of threads that accepts work
requests from a common queue, processing them (potentially) in parallel.
The work queue manager could also be considered a work crew manager,
depending on your reference point. If you think of it as a way to feed work to a set
of threads, then "work crew" might be more appropriate. I prefer to think of it as a
queue that magically does work for you in the background, since the presence of
the work crew is almost completely invisible to the caller.
When you create the work queue, you can specify the maximum level of paral-
lelism that you need. The work queue manager interprets that as the maximum
number of "engine" threads that it may create to process your requests. Threads
will be started and stopped as required by the amount of work. A thread that
finds nothing to do will wait a short time and then terminate. The optimal "short
time" depends on how expensive it is to create a new thread on your system, the
cost in system resources to keep a thread going that's not doing anything, and
how likely it is that you'll need the thread again soon. I've chosen two seconds,
which is probably much too long.
The header file workq. h and the C source file workq. c demonstrate an imple-
mentation of a work queue manager. Part 1 shows the two structure types used
by the work queue package. The workq\_t type is the external representation of a
work queue, and the workqelet is an internal representation of work items
that have been queued.
6-9 The workq\_ele\_t structure is used to maintain a linked list of work items. It
has a link element (called next) and a data value, which is stored when the work
item is queued and passed to the caller's "engine function" with no interpretation.
14-16 Of course, there's a mutex to serialize access to the workqt, and a condition
variable (cv) on which the engine threads wait for work to be queued.
17 The attr member is a thread attributes object, used when creating new
engine threads. The attributes object could instead have been a static variable
within workq.c, but I chose to add a little memory overhead to each work queue,
rather than add the minor complexity of one-time initialization of a static data
item.
18 The first member points to the first item on the work queue. As an optimiza-
tion to make it easier to queue new items at the end of the queue, the last
member points to the last item on the queue.
19-24 These members record assorted information about the work queue. The valid
member is a magic number that's set when the work queue is initialized, as we've
seen before in barriers and read/write locks. (In this case, the magic number is
the month and year of my daughter's birthday.) The quit member is a flag that
allows the "work queue manager" to tell engine threads to terminate as soon as
the queue is empty. The parallelism member records how many threads the
creator chose to allow the work queue to utilize, counter records the number of
Work queue manager
27'1
threads created, and idle records the current number of threads that are waiting
for work. The engine member is the user's "engine function," supplied when the
work queue was created. As you can see, the engine function takes an "untyped"
(void *) argument, and has no return value.
workq.h
part 1 workq\_t
1
2
3
4
5
6
7
8
9
10
11
12
13
14
15
16
17
18
19
20
21
22
23
24
25
26
27
|include \<pthread.h\>
/*
* Structure to keep track of work queue requests.
*/
typedef struct workq\_ele\_tag {
struct workq\_ele\_tag *next;
void *data;
} workq\_ele\_t;
/*
* Structure describing a work queue.
*/
typedef struct workq\_tag {
pthread\_mutex\_t
pthread\_cond\_t
pthread\_attr\_t
workq\_ele\_t
int
int
int
int
int
void
} workq\_t;
tdefine WORKQ VALID
last;
mutex;
cv;
attr;
*first,
valid;
quit;
parallelism;
counter;
idle;
/* control access to queue */
/* wait for work */
/* create detached threads */
/* work queue */
/* valid */
/* workq should quit */
/* maximum threads */
/* current threads */
/* number of idle threads */
(*engine)(void *arg); /* user engine */
0xdecl992
workq.h
part 1 workq\_t
Part 2 shows the interfaces we'll create for our work queue. We need to create
and destroy work queue managers, so we'll define workq\_init and workq\_destroy.
Both take a pointer to a workq\_t structure. In addition, the initializer needs the
maximum number of threads the manager is allowed to create to service the queue,
and the engine function. Finally, the program needs to be able to queue work items
for processing-we'll call the interface for this workqadd. It takes a pointer to the
workq\_t and the argument that should be passed to the engine function.
272 CHAPTER 7 "Real code'
| workq.h part 2 interfaces
1 /*
2 * Define work queue functions.
3 */
4 extern int workq\_init (
5 workq\_t *wq,
6 int threads, /* maximum threads */
7 void (*engine)(void *)); /* engine routine */
8 extern int workq\_destroy (workq t *wq);
9 extern int workq\_add (workq\_t *wq, void *data);
| workq.h part 2 interfaces
The file workq.c contains the implementation of our work queue. The follow-
ing examples break down each of the functions used to implement the workq. h
interfaces.
Part 1 shows the workq\_init function, which initializes a work queue. We
create the Pthreads synchronization objects that we need, and fill in the remain-
ing members.
14-22 Initialize the thread attributes object attr so that the engine threads we
create will run detached. That means we do not need to keep track of their thread
identifier values, or worry about joining with them.
34-40 We're not ready to quit yet (we've hardly started!), so clear the quit flag. The
parallelism member records the maximum number of threads we are allowed to
create, which is the workq\_init parameter threads. The counter member will
record the current number of active engine threads, initially 0, and idle will record
the number of active threads waiting for more work. And of course, finally, we set
the valid member.
| workq.c part 1 workq\_init
1 #include \<pthread.h\>
2 #include \<stdlib.h\>
3 #include \<time.h\>
4 #include "errors.h"
5 #include "workq.h"
6
7 /*
8 * Initialize a work queue.
9 */
10 int workq\_init (workq\_t *wq, int threads, void (*engine)(void *arg))
11 {
12 int status;
13
14 status = pthread\_attr\_init (&wq-\>attr);
15 if (status != 0)
16 return status;
17 status = pthread attr setdetachstate (
Work queue manager 273
18 &wq-\>attr, PTHREAD\_CREATE\_DETACHED);
19 if (status != 0) {
20 pthread\_attr\_destroy (&wq-\>attr);
21 return status;
22 }
23 status = pthread\_mutex\_init (&wq-\>mutex, NULL);
24 if (status != 0) {
25 pthread\_attr\_destroy (&wq-\>attr);
26 return status;
27 }
2 8 status = pthread\_cond\_init (&wq-\>cv, NULL);
29 if (status != 0) {
30 pthread\_mutex\_destroy (&wq-\>mutex);
31 pthread\_attr\_destroy (&wq-\>attr);
32 return status;
33 }
34 wq-\>quit =0; /* not time to quit */
35 wq-\>first = wq-\>last = NULL; /* no queue entries */
36 wq-\>parallelism = threads; /* max servers */
37 wq-\>counter =0; /* no server threads yet */
38 wq-\>idle = 0; /* no idle servers */
39 wq-\>engine = engine;
40 wq-\>valid = WORKQ\_VALID;
41 return 0;
42 }
| workq.c part 1 workq\_init
Part 2 shows the workq\_destroy function. The procedure for shutting down a
work queue is a little different than the others we've seen. Remember that the
Pthreads mutex and condition variable destroy function fail, returning ebusy,
when you try to destroy an object that is in use. We used the same model for
barriers and read/write locks. But we cannot do the same for work queues-the
calling program cannot know whether the work queue is in use, because the caller
only queues requests that are processed asynchronously.
The work queue manager will accept a request to shut down at any time, but it
will wait for all existing engine threads to complete their work and terminate.
Only when the last work queue element has been processed and the last engine
thread has exited will workq\_destroy return successfully.
2 4 If the work queue has no threads, either it was never used or all threads have
timed out and shut down since it was last used. That makes things easy, and we
can skip all the shutdown complication.
25-33 If there are engine threads, they are asked to shut down by setting the quit
flag in the workq\_t structure and broadcasting the condition variable to awaken
any waiting (idle) engine threads. Each engine thread will eventually run and see
this flag. When they see it and find no more work, they'll shut themselves down.
44-50 The last thread to shut down will wake up the thread that's waiting in workq\_
destroy, and the shutdown will complete. Instead of creating a condition variable
274 CHAPTER 7 "Real code"
that's used only to wake up workq\_destroy, the last thread will signal the same
condition variable used to inform idle engine threads of new work. At this point,
all waiters have already been awakened by a broadcast, and they won't wait again
because the quit flag is set. Shutdown occurs only once during the life of the
work queue manager, so there's little point to creating a separate condition variable
for this purpose.
| workq.c part 2 workq\_destroy
1 /*
2 * Destroy a work queue.
3 */
4 int workq\_destroy (workq\_t *wq)
5 {
6 int status, statusl, status2;
7
8 if (wq-\>valid != WORKQ\_VALID)
9 return EINVAL;
10 status = pthread\_mutex\_lock (&wq-\>mutex);
11 if (status != 0)
12 return status;
13 wq-\>valid =0; /* prevent any other operations */
14
15 /*
16 * Check whether any threads are active, and run them down:
17 *
18 * 1. set the quit flag
19 * 2. broadcast to wake any servers that may be asleep
20 * 4. wait for all threads to quit (counter goes to 0)
21 * Because we don't use join, we don't need to worry
22 * about tracking thread IDs.
23 */
24 if (wq-\>counter \> 0) {
25 wq-\>quit = 1;
26 /* if any threads are idling, wake them. */
27 if (wq-\>idle \> 0) {
28 status = pthread\_cond\_broadcast (&wq-\>cv);
29 if (status != 0) {
30 pthread\_mutex\_unlock (&wq-\>mutex);
31 return status;
32 }
33 }
34
35 /*
36 * Just to prove that every rule has an exception, I'm
37 * using the "cv" condition for two separate predicates
38 * here. That's OK, since the case used here applies
39 * only once during the life of a work queue - during
40 * rundown. The overhead is minimal and it's not worth
41 * creating a separate condition variable that would
Work queue manager 275
42 * wait and be signaled exactly once!
43 */
44 while (wq-\>counter \> 0) {
45 status = pthread\_cond\_wait (&wq-\>cv, &wq-\>mutex);
46 if (status != 0) {
47 pthread\_mutex\_unlock (&wq-\>mutex);
48 return status;
49 }
50 }
51 }
52 status = pthread\_mutex\_unlock (&wq-\>mutex);
53 if (status != 0)
54 return status;
55 status = pthread\_mutex\_destroy (&wq-\>mutex);
56 statusl = pthread\_cond\_destroy (&wq-\>cv);
57 status2 = pthread\_attr\_destroy (&wq-\>attr);
58 return (status ? status : (statusl ? statusl : status2));
59 }
| workq.c part 2 workq\_destroy
Part 3 shows workq\_add, which accepts work for the queue manager system.
16-35 It allocates a new work queue element and initializes It from the parameters. It
queues the element, updating the first and last pointers as necessary.
40-45 If there are idle engine threads, which were created but ran out of work, signal
the condition variable to wake one.
46-59 If there are no idle engine threads, and the value of parallelism allows for
more, create a new engine thread. If there are no idle threads and it can't create a
new engine thread, workq\_add returns, leaving the new element for the next
thread that finishes its current assignment.
| workq.c part 3 workq\_add
1 /*
2 * Add an item to a work queue.
3 */
4 int workq\_add (workq\_t *wq, void *element)
5 {
6 workq\_ele\_t *item;
7 pthread\_t id;
8 int status;
9
10 if (wq-\>valid != WORKQ\_VALID)
11 return EINVAL;
12
13 /*
14 * Create and initialize a request structure.
15 */
16 item = (workq\_ele\_t *)malloc (sizeof (workq\_ele\_t));
276 CHAPTER 7 "Real code"
17 if (item == NULL)
18 return ENOMEM;
19 item-\>data = element;
20 item-\>next = NULL;
21 status = pthread\_mutex\_lock (&wq-\>mutex);
22 if (status != 0) {
23 free (item);
24 return status;
25 }
26
27 /*
28 * Add the request to the end of the queue, updating the
29 * first and last pointers.
30 */
31 if (wq-\>first == NULL)
32 wq-\>first = item;
33 else
34 wq-\>last-\>next = item;
35 wq-\>last = item;
36
37 /*
38 * if any threads are idling, wake one.
39 */
40 if (wq-\>idle \> 0) {
41 status = pthread\_cond\_signal (&wq-\>cv);
42 if (status != 0) {
43 pthread\_mutex\_unlock (&wq-\>mutex);
44 return status;
45 }
46 } else if (wq-\>counter \< wq-\>parallelism) {
47 /*
48 * If there were no idling threads, and we're allowed to
49 * create a new thread, do so.
50 */
51 DPRINTF (("Creating new worker\n"));
52 status = pthread\_create (
53 &id, &wq-\>attr, workq\_server, (void*)wq);
54 if (status != 0) {
55 pthread\_mutex\_unlock (&wq-\>mutex);
56 return status;
57 }
58 wq-\>counter++;
59 }
60 pthread\_mutex\_unlock (&wq-\>mutex);
61 return 0;
62 }
| workq.c part 3 workq\_add
Work queue manager 277
That takes care of all the external interfaces, but we will need one more func-
tion, the start function for the engine threads. The function, shown in part 4, is
called workq\_server. Although we could start a thread running the caller's
engine with the appropriate argument for each request, this is more efficient. The
workq\_server function will dequeue the next request and pass it to the engine
function, then look for new work. It will wait if necessary and shut down only
when a certain period of time passes without any new work appearing, or when
told to shut down by workq\_destroy.
Notice that the server begins by locking the work queue mutex, and the
"matching" unlock does not occur until the engine thread is ready to terminate.
Despite this, the thread spends most of its life with the mutex unlocked, either
waiting for work in the condition variable wait or within the caller's engine
function.
29-62 When a thread completes the condition wait loop, either there is work to be
done or the work queue is shutting down (wq-\>quit is nonzero).
67-80 First, we check for work and process the work queue element if there is one.
There could still be work queued when workq\_destroy is called, and it must all
be processed before any engine thread terminates.
The user's engine function is called with the mutex unlocked, so that the
user's engine can run a long time, or block, without affecting the execution of
other engine threads. That does not necessarily mean that engine functions can
run in parallel-the caller-supplied engine function is responsible for ensuring
whatever synchronization is needed to allow the desired level of concurrency or
parallelism. Ideal engine functions would require little or no synchronization and
would run in parallel.
86-104 When there is no more work and the queue is being shut down, the thread ter-
minates, awakening workq\_destroy if this was the last engine thread to shut
down.
no-114 Finally we check whether the engine thread timed out looking for work, which
would mean the engine has waited long enough. If there's still no work to be
found, the engine thread exits.
| workq.c part 4 workq\_server
1 /*
2 * Thread start routine to serve the work queue.
3 */
4 static void *workq\_server (void *arg)
5 {
6 struct timespec timeout;
7 workq\_t *wq = (workq\_t *)arg;
8 workq\_ele\_t *we;
9 int status, timedout;
10
11 /*
12 * We don't need to validate the workq\_t here... we don't
278 CHAPTER 7 "Real code"
13 * create server threads until requests are queued (the
14 * queue has been initialized by then!) and we wait for all
15 * server threads to terminate before destroying a work
16 * queue.
17 */
18 DPRINTF (("A worker is starting\n"));
19 status = pthread\_mutex\_lock (&wq-\>mutex);
20 if (status != 0)
21 return NULL;
22
23 while A) {
24 timedout = 0;
25 DPRINTF (("Worker waiting for work\n"));
26 clock\_gettime (CLOCK\_REALTIME, Stimeout);
27 timeout.tv\_sec += 2;
28
29 while (wq-\>first == NULL && !wq-\>quit) {
30 /*
31 * Server threads time out after spending 2 seconds
32 * waiting for new work, and exit.
33 */
34 status = pthread\_cond\_timedwait (
35 &wq-\>cv, &wq-\>mutex, Stimeout);
36 if (status == ETIMEDOUT) {
37 DPRINTF (("Worker wait timed out\n"));
38 timedout = 1;
39 break;
40 } else if (status != 0) {
41 /*
42 * This shouldn't happen, so the work queue
43 * package should fail. Because the work queue
44 * API is asynchronous, that would add
45 * complication. Because the chances of failure
46 * are slim, I choose to avoid that
47 * complication. The server thread will return,
48 * and allow another server thread to pick up
49 * the work later. Note that if this were the
50 * only server thread, the queue wouldn't be
51 * serviced until a new work item is
52 * queued. That could be fixed by creating a new
53 * server here.
54 */
55 DPRINTF ((
56 "Worker wait failed, %d (%s)\n",
57 status, strerror (status)));
58 wq-\>counter-;
59 pthread\_mutex\_unlock (&wq-\>mutex);
60 return NULL;
61 }
62 }
Work queue manager 279
63 DPRINTF (("Work queue: %#lx, quit: %d\n",
64 wq-\>first, wq-\>quit));
65 we = wq-\>first;
66
67 if (we != NULL) {
68 wq-\>first = we-\>next;
69 if (wq-\>last == we)
70 wq-\>last = NULL;
71 status = pthread\_mutex\_unlock (&wq-\>mutex);
72 if (status != 0)
73 return NULL;
74 DPRINTF (("Worker calling engine\n"));
75 wq-\>engine (we-\>data);
76 free (we);
77 status = pthread\_mutex\_lock (&wq-\>mutex);
78 if (status != 0)
79 return NULL;
80 }
81
82 /*
83 * If there are no more work requests, and the servers
84 * have been asked to quit, then shut down.
85 */
86 if (wq-\>first == NULL && wq-\>quit) {
87 DPRINTF (("Worker shutting down\n"));
88 wq-\>counter-;
89
90 /*
91 * NOTE: Just to prove that every rule has an
92 * exception, I'm using the "cv" condition for two
93 * separate predicates here. That's OK, since the
94 * case used here applies only once during the life
95 * of a work queue -- during rundown. The overhead
96 * is minimal and it's not worth creating a separate
97 * condition variable that would wait and be
98 * signaled exactly once!
99 */
100 if (wq-\>counter == 0)
101 pthread\_cond\_broadcast (&wq-\>cv);
102 pthread\_mutex\_unlock (&wq-\>mutex);
103 return NULL;
104 }
105
106 /*
107 * If there's no more work, and we wait for as long as
108 * we're allowed, then terminate this server thread.
109 */
110 if (wq-\>first == NULL && timedout) {
111 DPRINTF (("engine terminating due to timeout.\n"));
280
CHAPTER 7 "Real code"
112
113
114
115
116
117
118
119
120
wq-\>counter-;
break;
pthread\_mutex\_unlock (&wq-\>inutex);
DPRINTF (("Worker exiting\n"));
return NULL;
15-19
29-37
43-68
73-98
workq.c
part 4 workq\_server
Finally, workq\_main. c is a sample program that uses our work queue man-
ager. Two threads queue work elements to the work queue in parallel. The engine
function is designed to gather some statistics about engine usage. To accomplish
this, it uses thread-specific data. When the sample run completes, main collects
all of the thread-specific data and reports some statistics.
Each engine thread has an engine\_t structure associated with the thread-
specific data key engine\_key. The engine function gets the calling thread's value
of this key, and if the current value is NULL, creates a new engine\_t structure
and assigns it to the key. The calls member of engine\_t structure records the
number of calls to the engine function within each thread.
The thread-specific data key's destructor function, destructor, adds the ter-
minating thread's engine\_t to a list (engine\_list\_head), where main can find it
later to generate the final report.
The engine function's work is relatively boring. The argument is a pointer to a
power\_t structure, containing the members value and power. It uses a trivial
loop to multiply value by itself power times. The result is discarded in this exam-
ple, and the power\_t structure is freed.
A thread is started, by main, running the thread\_routine function. In addi-
tion, main calls thread\_routine. The thread\_routine function loops for some
number of iterations, determined by the macro iterations, creating and queu-
ing work queue elements. The value and power members of the power\_t
structure are determined semirandomly using rand\_r. The function sleeps for a
random period of time, from zero to four seconds, to occasionally allow engine
threads to time out and terminate. Typically when you run this program you
would expect to see summary messages reporting some small number of engine
threads, each of which processed some number of calls-which total 50 calls B5
each from the two threads).
workq\_main.c
1 #include \<pthread.h\>
2 #include \<stdlib.h\>
3 #include \<stdio.h\>
4 #include \<time.h\>
5 #include "workq.h"
Work queue manager 281
6 #include "errors.h"
7
8 #define ITERATIONS 25
9
10 typedef struct power\_tag {
11 int value;
12 int power;
13 } power\_t;
14
15 typedef struct engine\_tag {
16 struct engine\_tag *link;
17 pthread\_t thread\_id;
18 int calls;
19 } engine\_t;
20
21 pthread\_key\_t engine\_key; /* Keep track of active engines */
22 pthread\_mutex\_t engine\_list\_mutex = PTHREAD\_MUTEX\_INITIALIZER;
23 engine\_t *engine\_list\_head = NULL;
24 workq\_t workg;
25
26 /*
27 * Thread-specific data destructor routine for engine\_key.
28 */
29 void destructor (void *value\_ptr)
30 {
31 engine\_t *engine = (engine\_t*)value\_ptr;
32
33 pthread\_mutex\_lock (&engine\_list\_mutex);
34 engine-\>link = engine\_list\_head;
35 engine\_list\_head = engine;
36 pthread\_rautex\_unlock (&engine\_list\_mutex);
37 }
38
39 /*
40 * This is the routine called by the work queue servers to
41 * perform operations in parallel.
42 */
43 void engine\_routine (void *arg)
44 {
45 engine\_t *engine;
46 power\_t *power = (power\_t*)arg;
47 int result, count;
48 int status;
49
50 engine = pthread\_getspecific (engine\_key);
51 if (engine == NULL) {
52 engine = (engine\_t*)malloc (sizeof (engine\_t));
53 status = pthread\_setspecific (
54 engine key, (void*)engine);
282 CHAPTER 7 "Real code"
55 if (status != 0)
56 err\_abort (status, "Set tsd");
57 engine-\>thread\_id = pthread\_self ();
58 engine-\>calls = 1;
59 } else
60 engine-\>calls++;
61 result = 1;
62 printf (
63 "Engine: computing %d'%d\n",
64 power-\>value, power-\>power);
65 for (count = 1; count \<= power-\>power; count++)
66 result *= power-\>value;
67 free (arg);
68 }
69
70 /*
71 * Thread start routine that issues work queue requests.
72 */
73 void *thread\_routine (void *arg)
74 {
75 power\_t *element;
76 int count;
77 unsigned int seed = (unsigned int)time (NULL);
78 int status;
79
80 /*
81 * Loop, making requests.
82 */
83 for (count = 0; count \< ITERATIONS; count++) {
84 element = (power\_t*)malloc (sizeof (power\_t));
85 if (element == NULL)
86 errno\_abort ("Allocate element");
87 element-\>value = rand\_r (Sseed) % 20;
88 element-\>power = rand\_r (Sseed) % 7;
89 DPRINTF ((
90 "Request: %d~%d\n",
91 element-\>value, element-\>power));
92 status = workq\_add (Sworkq, (void*)element);
93 if (status != 0)
94 err\_abort (status, "Add to work queue");
95 sleep (rand\_r (Sseed) % 5);
96 }
97 return NULL;
98 }
99
100 int main (int argc, char *argv[])
101 {
102 pthread\_t thread\_id;
103 engine\_t *engine;
But what about existing libraries? 283
104 int count = 0, calls = 0;
105 int status;
106
107 status = pthread\_key\_create (&engine\_key, destructor);
108 if (status != 0)
109 err\_abort (status, "Create key");
110 status = workq\_init (sworkq, 4, engine\_routine);
111 if (status != 0)
112 err\_abort (status, "Init work queue");
113 status = pthread\_create (&thread\_id, NULL, thread\_routine, NULL);
114 if (status != 0)
115 err\_abort (status, "Create thread");
116 (void)thread\_routine (NULL);
117 status = pthread\_join (thread\_id, NULL);
118 if (status != 0)
119 err\_abort (status, "Join thread");
120 status = workq\_destroy (Sworkq);
121 if (status != 0)
122 err\_abort (status, "Destroy work queue");
123
124 /*
125 * By now, all of the engine\_t structures have been placed
126 * on the list (by the engine thread destructors), so we
127 * can count and summarize them.
128 */
129 engine = engine\_list\_head;
130 while (engine != NULL) {
131 count++;
132 calls += engine-\>calls;
133 printf ("engine %d: %d calls\n", count, engine-\>calls);
134 engine = engine-\>link;
135 }
136 printf ("%d engine threads processed %d calls\n",
137 count, calls);
138 return 0;
139 }
| workq\_main.c
## 7.3 But what about existing libraries?
"The great art of riding, as I was saying is-
to keep your balance properly. Like this, you know-"
He let go the bridle, and stretched out both his arms to
show Alice what he meant, and this time he fell flat on
his back, right under the horse's feet.
-Lewis Carroll, Through the Looking-Glass
284 CHAPTER 7 "Real code"
When you create a new library, all it takes is careful design to ensure that the
library will be thread-safe. As you decide what state is needed for the function,
you can determine which state needs to be shared between threads, which state
should be managed by the caller through external context handles, which state can
be kept in local variables within a function, and so forth. You can define the inter-
faces to the functions to support that state in the most efficient manner. But when
you're modifying an existing library to work with threads, you usually don't have
that luxury. And when you are using someone else's library, you may need simply
to "make do."
### 7.3.1 Modifying libraries to be thread-safe
Many functions rely on static storage across a sequence of calls, for example,
strtok or getpwd. Others depend on returning a pointer to static storage, for
example, asctime. This section points out some techniques that can help when
you need to make "legacy" libraries thread-safe, using some well-known exam-
ples in the ANSI C run-time library.
The simplest technique is to assign a mutex to each subsystem. At any call
into the subsystem you lock the mutex; at any exit from the subsystem you
unlock the mutex. Because this single mutex covers the entire subsystem, we
often refer to such a mechanism as a "big mutex" (see Section 3.2.4). The mutex
prevents more than one thread from executing within the subsystem at a time.
Note that this fixes only synchronization races, not sequence races (Section 8.1.2
describes the distinction between the two). The best candidates for this approach
are functions that do little except maintain some internal database. That includes
functions such as malloc and free that manage an internal resource pool but
grant limited (or no) external visibility into that pool.
One problem with using the "big mutex" approach is that you have to be care-
ful about your definition of "subsystem." You need to include all functions that
share data or that call each other. If malloc and free have one mutex while real-
loc uses another, then you've got a race as soon as one thread calls realloc
while another thread is in malloc or free.
And what if realloc is implemented to call malloc, copy data, and then call
free on the old pointer? The realloc function would lock the heap mutex and
call malloc. The malloc function would immediately try to lock the heap mutex
itself, resulting in a deadlock. There are several ways to solve this. One is to care-
fully separate each of the external interfaces into an internal "engine" function
that does the actual work and an external entry point that locks the subsystem
mutex and calls the engine. Other entry points within the subsystem that need
the same engine function would call it directly rather than using the normal entry
point. That's often the most efficient solution, but it is also harder to do. Another
possibility is to construct a "recursive" mutex that allows the subsystem to relock
But what about existing libraries? 285
its own mutex without deadlock.* Now malloc and free are allowed to relock the
mutex held by realloc, but another thread trying to call any of them will be
blocked until realloc completely unlocks the recursive mutex.
Most functions with persistent state require more substantial changes than
just a "big mutex," especially to avoid altering the interface. The asctime func-
tion, for example, returns a pointer to the character string representation of a
binary time. Traditionally, the string is formatted into a static buffer declared
within the asctime function, and the function returns a pointer to that buffer.
Locking a mutex within asctime isn't enough to protect the data. In fact, it is
not even particularly useful. After asctime returns, the mutex has been
unlocked. The caller needs to read the buffer, and there is nothing to prevent
another thread from calling asctime (and "corrupting" the first thread's result)
before the first thread has finished reading or copying it. To solve this problem
using a mutex, the caller would need to lock a mutex before calling asctime, and
then unlock it only after it had finished with the data or copied the returned
buffer somewhere "safe."
The problem can instead be fixed by recoding asctime to allocate a heap
buffer using malloc, formatting the time string into that buffer, and returning its
address. The function can use a thread-specific data key to keep track of the
heap address so that it can be reused on the next call within that thread. When
the thread terminates, a destructor function can free the storage.
It would be more efficient to avoid using malloc and thread-specific data, but
that requires changing the interface to asctime. Pthreads adds a new thread-safe
alternative to asctime, called asctime\_r, which requires the caller to pass the
address and length of a buffer. The asctime\_r function formats the time string
into the caller's buffer. This allows the caller to manage the buffer in any way
that's convenient. It can be on the thread's stack, in heap, or can even be shared
between threads. Although in a way this is "giving up" on the existing function
and defining a new function, it is often the best way (and sometimes the only
practical way) to make a function thread-safe.
### 7.3.2 Living with legacy libraries
Sometimes you have to work with code you didn't write, and can't change. A
lot of code is now being made thread-safe, and most operating systems that sup-
port threads can be expected to supply thread-safe implementations of the
* It is easy to construct a "recursive" mutex using a mutex, a condition variable, the pthread\_
t value of the current owner (if any), and a count of the owner's "recursion depth." The depth is
0 when the recursive mutex is not locked, and greater than 0 when it is locked. The mutex pro-
tects access to the depth and owner members, and the condition variable is used to wait for the
depth to become 0, should a thread wish to lock the recursive mutex while another thread has it
locked.
286 CHAPTER 7 "Real code"
common bundled library packages. The "inner circle" of thread-safe libraries will
gradually increase to become the rule rather than the exception as more applica-
tion and library developers demand thread-safety.
But inevitably you'll find that a library you need hasn't been made thread-
safe, for example, an older version of the X Windows windowing system, or a
database engine, or a simulation package. And you won't have source code. Of
course you'll immediately complain to the supplier of the library and convince
them to make the next version fully thread-safe. But what can you do until the
new version arrives?
If you really need the library, the answer is "use it anyway." There are a num-
ber of techniques you can use, from simple to complex. The appropriate level of
complexity required depends entirely on the library's interface and how (as well
as how much) you use the library in your code.
I Make the unsafe library into a server thread.
In some cases, you may find it convenient to restrict use of the library to one
thread, making that thread a "server" for the capabilities provided by the unsafe
library. This technique is commonly applied, for example, when using versions
of the XI1 protocol client library that are not thread-safe. The main thread or
some other thread created for the purpose processes queued XI1 requests on
behalf of other threads. Only the server thread makes calls into the XI1 library,
so it does not matter whether XI1 is thread-safe.
I Write your own "big mutex" wrappers around the interfaces.
If the function you need has a "thread-safe interface" but not a "thread-safe
implementation," then you may be able to encapsulate each call inside a wrapper
function (or a macro) that locks a mutex, calls the function, and then unlocks the
mutex. This is just an external version of the "big mutex" approach. By "thread-
safe interface" I mean that the function relies on the static state, but that any
data returned to the caller isn't subject to alteration by later calls. For example,
malloc fits that category. The allocation of memory involves static data that
needs to be protected, but once a block has been allocated and returned to a
caller, that address (and the memory to which it points) will not be affected by
later calls to malloc. The external "big mutex" is not a good solution for libraries
that may block for substantial periods of time-like XI1 or any other network
protocol. While the result may be safe, it will be very inefficient unless you rarely
use the library, because other threads may be locked out for long periods of time
while remote operations are taking place.
I Extend the implementation with external state.
A big mutex won't fix a function like asctime that writes data into a static
buffer and returns the address: The returned data must be protected until the
But what about existing libraries? 287
caller is finished using it, and the data is used outside the wrapper. For a func-
tion like strtok the data is in use until the entire sequence of tokens has been
parsed. In general, functions that have persistent static data are more difficult to
encapsulate.
A function like asctime can be encapsulated by creating a wrapper function
that locks a mutex, calls the function, copies the return value into a thread-safe
buffer, unlocks the mutex, and then returns. The thread-safe buffer can be
dynamically allocated by the wrapper function using malloc, for instance. You
can require the caller to free the buffer when done, which changes the interface,
or you can make the wrapper keep track of a per-thread buffer using thread-
specific data.
Alternatively, you could invent a new interface that requires the caller to sup-
ply a buffer. The caller can use a stack buffer, or a buffer in heap, or, if properly
synchronized (by the caller), it can share the buffer between threads. Remember
that if the wrapper uses thread-specific data to keep track of a per-thread heap
buffer, the wrapper can be made compatible with the original interface. The other
variants require interface changes: The caller must supply different inputs or it
must be aware of the need to free the returned buffer.
A function that keeps persistent state across a sequence of calls is more diffi-
cult to encapsulate neatly. The static data must be protected throughout. The
easiest way to do this is simply to change the caller to lock a mutex before the
first call and keep it locked until after the final call of a sequence. But remember
that no other thread can use the function until the mutex is unlocked. If the
caller does a substantial amount of processing between calls, a major processing
bottleneck can occur. Of course, this may also be difficult or impossible to inte-
grate into a simple wrapper-the wrapper would have to be able to recognize the
first and last of any series of calls.
A better, but harder, way is to find some way to encapsulate the function (or a
set of related functions) into a new thread-safe interface. There is no general
model for this transformation, and in many cases it may be impossible. But often
you just need to be creative, and possibly apply some constraints. While the
library function may not be easy to encapsulate, you may be able to encapsulate
"special cases" that you use. While strtok, for example, allows you to alter the
token delimiters at each call, most code does not take advantage of this flexibility.
Without the complication of varying delimiters, you could define a new token
parsing model on top of strtok where all tokens in a string are found by a thread-
safe setup function and stored where they can be retrieved one by one without
calling strtok again. Thus, while the setup function would lock a common mutex
and serialize access across all threads, the information retrieval function could
run without any serialization.
# 8 Hints to avoid debugging
"Other maps are such shapes, with their islands and capes!
But we've got our brave Captain to thank"
(So the crew would protest) "that he's bought us the best-
A perfect and absolute blank!"
-Lewis Carroll, The Hunting of the Snark
Writing a complicated threaded program is a lot harder than writing a simple
synchronous program, but once you learn the rules it is not much harder than
writing a complicated synchronous program. Writing a threaded program to per-
form a complicated asynchronous function will usually be easier than writing the
same program using more traditional asynchronous programming techniques.
The complications begin when you need to debug or analyze your threaded
program. That's not so much because using threads is hard, but rather because
the tools for debugging and analyzing threaded code are less well developed and
understood than the programming interfaces. You may feel as if you are navigat-
ing from a blank map. That doesn't mean you can't utilize the power of threaded
programming right now, but it does mean that you need to be careful, and maybe
a little more creative, in avoiding the rocks and shoals of the uncharted waters.
Although this chapter mentions some thread debugging and analysis tools
and suggests what you can accomplish with them, my goal isn't to tell you about
tools you can use to solve problems. Instead, I will describe some of the common
problems that you may encounter and impart something resembling "sage
advice" on avoiding those problems before you have to debug them-or, perhaps
more realistically, how to recognize which problems you may be encountering.
I Check your assumptions at the door.
Threaded programming is probably new to you. Asynchronous programming
may be new to you. If so, you'll need to be careful about your assumptions. You've
crossed a bridge, and behavior that's acceptable-or even required-in Synchro-
nous Land can be dangerous across the river in Asynchronous Land. You can
learn the new rules without a lot of trouble, and with practice you'll probably
even feel comfortable with them. But you have to start by being constantly aware
that something's changed.
289
290 CHAPTER 8 Hints to avoid debugging
## 8.1 Avoiding incorrect code
"For instance, now," she went on, sticking a large piece of plaster on her fin-
ger as she spoke, "there's the King's Messenger. He's in prison now,
being punished: and the trial doesn't even begin till next Wednesday:
and of course the crime comes last of all."
"Suppose he never commits the crime?" said Alice.
"That would be all the better, wouldn't it?" the Queen said, as she bound the
plaster round her finger with a bit of ribbon.
-Lewis Carroll, Through the Looking-Glass
Pthreads doesn't provide much assistance in debugging your threaded code.
That is not surprising, since POSIX does not recognize the concept of debugging
at all, even in explaining why the nearly universal sigtrap signal is not included
in the standard. There is no standard way to interact with your program or
observe its behavior as it runs, although every threaded system will provide some
form of debugging tool. Even in the unlikely event that the developers of the sys-
tem had no concern for you, the poor programmer, they needed to debug their
own code.
A vendor that provides threads with an operating system will provide at least a
basic thread "observation window" in a debugging utility. You should expect at
minimum the ability to display a list of the running threads and their current
state, the state of mutexes and condition variables, and the stack trace of all
threads. You should also be able to set breakpoints in specified threads and spec-
ify a "current thread" to examine registers, variables, and stack traces.
Because implementations of Pthreads are likely to maintain a lot of state in
user mode, within the process, debugging using traditional UNIX mechanisms
such as ptrace or the proc file system can be difficult. A common solution is to
provide a special library that is called by the debugger, which knows how to
search through the address space of the process being debugged to find the state
of threads and synchronization objects. Solaris, for example, provides the
libthread\_db. so shared library, and Digital UNIX provides libpthreaddebug. so.
A thread package placed on top of an operating system by a third party will
not be able to provide much integration with a debugger. For example, the porta-
ble "DCE threads" library provides a built-in debug command parser that you can
invoke from the debugger using the print or call command to report the state of
threads and synchronization objects within the process. This limited debugging
support is at best inconvenient-you can't analyze thread state within a core file
after a program has failed, and it cannot understand (or report) the symbolic
names of program variables.
* For historical reasons, the function is called cma\_debug. Should you find yourself stuck
with DCE threads code, try calling it, and enter the help command for a list of additional
commands.
Avoiding incorrect code 291
The following sections describe some of the most common classes of threaded
programming errors, with the intention of helping you to avoid these problems
while designing, as well as possibly making it easier to recognize them while
debugging.
### 8.1.1 Avoid relying on "thread inertia"
Always, always, remember that threads are asynchronous. That's especially
important to keep in mind when you develop code on uniprocessor systems
where threads may be "slightly synchronous." Nothing happens simultaneously
on a uniprocessor, where ready threads are serially timesliced at relatively pre-
dictable intervals. When you create a new thread on a uniprocessor or unblock a
thread waiting for a mutex or condition variable, it cannot run immediately
unless it has a higher priority than the creator or waker.
The same phenomenon may occur even on a multiprocessor, if you have
reached the "concurrency limit" of the process, for example, when you have more
ready threads than there are processors. The creator, or the thread waking
another thread, given equal priority, will continue running until it blocks or until
the next timeslice (which may be many nanoseconds away).
This means that the thread that currently has a processor has an advantage.
It tends to remain in motion, exhibiting behavior vaguely akin to physical inertia.
As a result, you may get away with errors that will cause your code to break in
mysterious ways when the newly created or awakened thread is able to run
immediately-when there are free processors. The following program, inertia.c,
demonstrates how this phenomenon can disrupt your program.
27-41 The question is one of whether the thread function printer\_thread will see
the value of stringPtr that was set before the call to pthread\_create, or the
value set after the call to pthread\_create. The desired value is "After value." This
is a very common class of programming error. Of course, in most cases the prob-
lem is less obvious than in this simple example. Often, the variable is uninitialized,
not set to some benign value, and the result may be data corruption or a segmen-
tation fault.
39 Now, notice the delay loop. Even on a multiprocessor, this program won't
break all the time. The program will usually be able to change stringPtr before
the new thread can begin executing-it takes time for a newly created thread to
get into your code, after all, and the "window of opportunity" in this particular
program is only a few instructions. The loop allows me to demonstrate the prob-
lem by delaying the main thread long enough to give the printer thread time to
start. If you make this loop long enough, you will see the problem even on a uni-
processor, if main is eventually timesliced.
| inertia.c
1 #include \<pthread.h\>
2 tinclude "errors.h"
292 CHAPTER 8 Hints to avoid debugging
3
4 void *printer\_thread (void *arg)
5 {
6 char *string = *(char**)arg;
7
8 printf ("%s\n", string);
9 return NULL;
10 }
11
12 int main (int argc, char *argv[])
13 {
14 pthread\_t printer\_id;
15 char *string\_ptr;
16 int i, status;
17
18 #ifdef sun
19 /*
20 * On Solaris 2.5, threads are not timesliced. To ensure
21 * that our two threads can run concurrently, we need to
22 * increase the concurrency level to 2.
23 */
24 DPRINTF (("Setting concurrency level to 2\n"));
25 thr\_setconcurrency B);
26 #endif
27 string\_ptr = "Before value";
28 status = pthread\_create (
29 &printer\_id, NULL, printer\_thread, (void*)&string\_ptr);
30 if (status != 0)
31 err\_abort (status, "Create thread");
32
33 /*
34 * Give the thread a chance to get started if it's going to run
35 * in parallel, but not enough that the current thread is likely
36 * to be timesliced. (This is a tricky balance, and the loop may
37 * need to be adjusted on your system before you can see the bug.)
38 */
39 for (i = 0; i \< 10000000; i++);
40
41 string\_ptr = "After value";
42 status = pthread\_join (printer\_id, NULL);
43 if (status != 0)
44 err\_abort (status, "Join thread");
45 return 0;
46 }
| inertia.c
The way to fix inertia.c is to set the "After value," the one you want the
threads to see, before creating the thread. That's not so hard, is it? There may
Avoiding incorrect code 293
still be a "Before value," whether it is uninitialized storage or a value that was
previously used for some other purpose, but the thread you create can't see it. By
the memory visibility rules given in Section 3.4, the new thread sees all memory
writes that occurred prior to the call into pthread\_create. Always design your
code so that threads aren't started until after all the resources they need have
been created and initialized exactly the way you want the thread to see them.
I Never assume that a thread you create will wait for you.
You can cause yourself as many problems by assuming a thread will run
"soon" as by assuming it won't run "too soon." Creating a thread that relies on
"temporary storage" in the creator thread is almost always a bad idea. I have seen
code that creates a series of threads, passing a pointer to the same local struc-
ture to each, changing the structure member values each time. The problem is
that you can't assume threads will start in any specific order. All of those threads
may start after your last creation call, in which case they all get the last value of
the data. Or the threads might start a little bit out of order, so that the first and
second thread get the same data, but the others get what you intended them to get.
Thread inertia is a special case of thread races. Although thread races are cov-
ered much more extensively in Section 8.1.2, thread inertia is a subtle effect, and
many people do not recognize it as a race. So test your code thoroughly on a mul-
tiprocessor, if at all possible. Do this as early as possible during development,
and continuously throughout development. Do this despite the fact that, espe-
cially without a perfect threaded debugger, testing on a multiprocessor will be
more difficult than debugging on a uniprocessor. And, of course, you should care-
fully read the following section.
### 8.1.2 Never bet your mortgage on a thread race
A race occurs when two or more threads try to get someplace or do something
at the same time. Only one can win. Which thread wins is determined by a lot of
factors, not all of which are under your control. The outcome may be affected by
how many processors are on the system, how many other processes are running,
how much network overhead the system is handling, and other things like that.
That's a nondeterministic race. It probably won't come out the same if you run
the same program twice in a row. You don't want to bet on races like that.*
I When you write threaded code, assume that at any arbitrary point,
within any statement of your program, each thread may go to sleep for
an unbounded period of time.
* My daughter had this figured out by the time she was three-when she wanted to race, she
told me ahead of time whether my job was to win or lose. There's really no point to leaving these
important things to chance!
294 CHAPTER 8 Hints to avoid debugging
Processors may execute your threads at differing rates, depending on proces-
sor load, interrupts, and so forth. Timeslicing on a processor may interrupt a
thread at any point for an unspecified duration. During the time that a thread
isn't running, any other thread may run and do anything that synchronization
protocols in your code don't specifically prevent it from doing, which means that
between any two instructions a thread may find an entirely different picture of
memory, with an entirely different set of threads active. The way to protect a
thread's view of the world from surprises is to rely only on explicit synchroniza-
tion between threads.
Most synchronization problems will probably show up pretty quickly if you're
debugging on a multiprocessor. Threads with insufficient synchronization will
compete for the honor of reaching memory last. It is a minor irony of thread races
that the "loser" generally wins because the memory system will keep the last
value written to an address. Sometimes, you won't notice a race at all. But some-
times you'll get a mystifying wrong result, and sometimes you'll get a segmentation
fault.
Races are usually difficult to diagnose. The problem often won't occur at all on
a uniprocessor system because races require concurrent execution. The level of
concurrency on a uniprocessor, even with timeslicing, is fairly low, and often an
unsynchronized sequence of writes will complete before another thread gets a
chance to read the inconsistent data. Even on a multiprocessor, races may be dif-
ficult to reproduce, and they often refuse to reveal themselves to a debugger.
Races depend on the relative timing of thread execution-something a debugger
is likely to change.
Some races have more to do with memory visibility than with synchronization
of multiple writes. Remember the basic rules of memory visibility (see Section 3.4):
A thread can always see changes to memory that were performed by a thread pre-
viously running on the same processor. On a uniprocessor all threads run on the
same processor, which makes it difficult to detect memory visibility problems dur-
ing debugging. On a multiprocessor, you may see visibility races only when the
threads are scheduled on different processors while executing specific vulnerable
sections of code.
No ordering exists between threads
unless you cause ordering.
Bill Gallmeister's corollary:
"Threads will run in the most evil order possible."
You don't want to find yourself debugging thread races. You may never see the
same outcome twice. The symptoms will change when you try to debug the code-
possibly by masquerading as an entirely different kind of problem, not just as the
same problem in a different place. Even worse, the problem may never occur at
all until a customer runs the code, and then it may fail every time, but only in the
customer's immense, monolithic application, and only after it has been running
Avoiding incorrect code 295
for days. It will be running on a secured system with no network access, they will
be unable to show you the proprietary code, and will be unable to reproduce the
problem with a simple test program.
I "Scheduling" is not the same as "synchronization."
It may appear at first that setting a thread to the sched\_fifo scheduling policy
and maximum priority would allow you to avoid using expensive synchronization
mechanisms by guaranteeing that no other thread can run until the thread blocks
itself or lowers its priority. There are several problems with this, but the main
problem is that it won't work on a multiprocessor. The SCHED\_fifo policy prevents
preemption by another thread, but on a multiprocessor other threads can run
without any form of preemption.
Scheduling exists to tell the system how important a specific job (thread) is to
your application so it can schedule the job you need the most. Synchronization
exists to tell the system that no other thread can be allowed into the critical sec-
tion until the calling thread is done.
In real life, a deterministic race, where the winner is guaranteed from the
beginning, isn't very exciting (except to a three year old). But a deterministic race
represents a substantially safer bet, and that's the kind of race you want to
design into your programs. A deterministic race, as you can guess, isn't much of
a race at all. It is more like waiting in line-nice, organized, and predictable.
Excitement is overrated, especially when it comes to debugging complicated
threaded applications.
The simplest form of race is when more than one thread tries to write shared
state without proper synchronization, for example, when two threads increment a
shared counter. The two threads may fetch the same value from memory, incre-
ment it independently, and store the same result into memory; the counter has
been incremented by one rather than by two, and both threads have the same
result value.
A slightly more subtle race occurs when one thread is writing some set of
shared data while another thread reads that data. If the reads occur in a different
order, or if the reader catches up to the writer, then the reader may get inconsis-
tent results. For example, one thread increments a shared array index and then
writes data into the array element at that index. Another thread fetches the
shared index before the writer has filled in the entire element and reads that ele-
ment. The reader finds inconsistent data because the element hasn't been
completely set up yet. It may take an unexpected code path because of something
it sees there or it may follow a bad pointer.
Always design and code assuming that threads are more asynchronous than
you can imagine. Anyone who's written a lot of code knows that computers have
little creatures that enjoy annoying you. Remember that when you code with
threads there are lots of them loose at the same time. Take no chances, make no
assumptions. Make sure any shared state is set up and visible before creating the
thread that will use it; or create it using static mutexes or pthread\_once. Use a
296 CHAPTER 8 Hints to avoid debugging
mutex to ensure that threads can't read inconsistent data. If you must share
stack data between threads, be sure all threads that use the data have termi-
nated before returning from the function that allocated the storage.
"Sequence races" may occur when you assume some ordering of events, but
that ordering isn't coded into the application. Sequence races can occur even
when you carefully apply synchronization control to ensure data consistency.
You can only avoid this kind of race by ensuring that ordering isn't important, or
by adding code that forces everything to happen in the order it needs to happen.
For example, imagine that three threads share a counter variable. Each will
store a private copy of the current value and increment the shared counter. If the
three threads are performing the same function, and none of them cares which
value of the counter they get, then it is enough to lock a mutex around the fetch
and increment operation. The mutex guarantees that each thread gets a distinct
value, and no values are skipped. There's no race because none of the threads
cares who wins.
But if it matters which value each thread receives, that simple code will not do
the job. For example, you might imagine that threads are guaranteed to start in
the order in which they are created, so that the first thread gets the value 1, the
second gets the value 2, and so forth. Once in a while (probably while you're
debugging), the threads will get the value you expect, and everything will work,
and at other times, the threads will happen to run in a different order.
There are several ways to solve this. For example, you could assign each of the
threads the proper value to begin with, by incrementing the counter in the thread
that creates them and passing the appropriate value to each thread in a data
structure. The best solution, though, is to avoid the problem by designing the
code so that startup order doesn't matter. The more symmetrical your threads
are, and the fewer assumptions they make about their environment, the less
chance that this kind of race will happen.
Races aren't always way down there at the level of memory address references,
though. They can be anywhere. The traditional ANSI C library, for example,
allows a number of sequence races when you use certain functions in an applica-
tion with multiple threads. The readdir function, for example, relies on static
storage within the function to maintain context across a series of identical calls to
readdir. If one thread calls readdir while another thread is in the middle of a
sequence of its own calls to readdir, the static storage will be overwritten with a
new context.
I "Sequence races" can occur even when all your code uses mutexes to
protect shared data!
This race occurs even if readdir is "thread aware" and locks a mutex to pro-
tect the static storage. It is not a synchronization race, it is a sequence race.
Thread A might call readdir to scan directory /usr/bin, for example, which locks
the mutex, returns the first entry, and then unlocks the mutex. Thread B might
then call readdir to scan directory /usr/include, which also locks the mutex,
Avoiding incorrect code 297
returns the first entry, and then unlocks the mutex. Now thread A calls readdir
again expecting the second entry in /usr/bin; but instead it gets the second
entry in /usr/include. No interface has behaved improperly, but the end result
is wrong. The interface to readdir simply is not appropriate for use by threads.
That's why Pthreads specifies a set of new reentrant functions, including
readdir\_r, which has an additional argument that is used to maintain context
across calls. The additional argument solves the sequence race by avoiding any
need for shared data. The call to readdir\_r in thread A returns the first entry
from /usr/bin in thread A's buffer, and the call to readdir\_r in thread B returns
the first entry from /usr/include in thread B's buffer . . . and the second call in
thread A returns the second entry from /usr/bin in thread A's buffer. Refer to
pipe. c, in Section 4.1, for a program that uses readdir\_r.
Sequence races can also be found at higher levels of coding. File descriptors in
a process, for example, are shared across all threads. If two threads attempt to
getc from the same file, each character in the file can go to only one thread. Even
though getc itself is thread-safe, the sequence of characters seen by each thread
is not deterministic-it depends on the ordering of each thread's independent
calls to getc. They may alternate, each getting every second character through-
out the file. Or one may get 2 or 100 characters in a row and then the other might
get 1 character before being preempted for some reason.
There are a number of ways you can resolve the getc race. You can open the
file under two separate file descriptors and assign one to each thread. In that
way, each thread sees every character, in order. That solves the race by removing
the dependency on ordering. Or you can lock the file across the entire sequence of
gets operations in each thread, which solves the race by enforcing the desired
order. The program putchar.c, back in Section 6.4.2, shows a similar situation.
Usually a program that doesn't care about ordering will run more efficiently
than a program that enforces some particular ordering, first, because enforcing
the ordering will always introduce computational overhead that's not directly
related to getting the job done. Remember Amdahl's law. "Unordered" programs
are more efficient because the greatest power of threaded programming is that
things can happen concurrently, and synchronization prevents concurrency.
Running an application on a multiprocessor system doesn't help much if most
processors spend their time waiting for one to finish something.
### 8.1.3 Cooperate to avoid deadlocks
Like races, deadlocks are the result of synchronization problems in a pro-
gram. While races are resource conflicts caused by insufficient synchronization,
deadlocks are usually conflicts in the use of synchronization. A deadlock can
happen when any two threads share resources. Essentially a deadlock occurs
when thread A has resource 1 and can't continue until it has resource 2, while
thread B has resource 2 and can't continue until it has resource 1.
298 CHAPTER 8 Hints to avoid debugging
The most common type of deadlock in a Pthreads program is mutex deadlock,
where both resources are mutexes. There is one really important advantage of a
deadlock over a race: It is much easier to debug the problem. In a race, the
threads do something incorrectly and move on. The problem shows up sometime
later, usually as a side effect. But in a deadlock the threads are still there waiting,
and always will be-if they could go anywhere, it wouldn't be a deadlock. So when
you attach to the process with the debugger or look at a crash dump, you can see
what resources are involved. With a little detective work you can often determine
why it happened.
The most likely cause is a resource ordering inconsistency. The study of dead-
locks goes way back to the early days of operating system design. Anyone who's
taken computer science courses has probably run into the classic dining philoso-
phers problem. Some philosophers sit at a round table with plates of spaghetti;
each alternately eats and discusses philosophy. Although no utensils are required
to discuss philosophy, each philosopher requires two forks to eat. The table is set
with a single fork between each pair. The philosophers need to synchronize their
eating and discussion to prevent deadlock. The most obvious form of deadlock is
when all philosophers simultaneously pick up one fork each and refuse to put it
down.
There's always a way to make sure that your philosophers can all eat, eventu-
ally. For example, a philosopher can take the fork to her right, and then look to
her left. If the fork is available, she can take it and eat. If not, she should return
the fork she's holding to the table and chat awhile. (That is the mutex backoff
strategy discussed in Section 3.2.5.1.) Since the philosophers are all in a good
mood and none has recently published papers severely critical of adjoining col-
leagues, those who get to eat will in reasonably short order return both of their
forks to the table so that their colleagues on each side can proceed.
A more reliable (and more sanitary) solution is to skip the spaghetti and serve
a dish that can be eaten with one fork. Mutex deadlocks can't happen if each
thread has only one mutex locked at a time. It is a good idea to avoid calling func-
tions with a mutex locked. First, if that function (or something it calls) locks
another mutex, you could end up with a deadlock. Second, it is a good idea to
lock mutexes for as short a time as possible (remember, locking a mutex prevents
another thread from "eating"-that is, executing-concurrently). Calling printf,
though, isn't likely to cause a deadlock in your code, because you don't lock any
ANSI C library mutexes, and the ANSI C library doesn't lock any of your mutexes.
If the call is into your own code, or if you call a library that may call back into
your code, be careful.
If you need to lock more than one mutex at a time, avoid deadlocks by using a
strict hierarchy or a backoff algorithm. The main disadvantage of mutex backoff
is that the backoff loop can run a long time if there are lots of other threads lock-
ing the mutexes, even if they do so without any possibility of a deadlock. The
backoff algorithm assumes that other threads may lock the first mutex after hav-
ing locked one or more of the other mutexes. If all threads always lock mutexes in
the order they're locked by the backoff loop, then you've got a fixed locking hierar-
chy and you don't need the backoff algorithm.
Avoiding incorrect code 299
When a program has hung because of a deadlock, you require two important
capabilities of your threaded debugger. First, it allows you to run your program in
a mode where mutex ownership is recorded, and may be displayed using debug-
ger commands. Finding a thread that is blocked on some mutex while it owns
other mutexes is a good indication that you may have a deadlock. Second, you
would like to be able to examine the call stack of threads that own mutexes to
determine why the mutexes have remained locked.
The call stack may not always be sufficient, though. One common cause of a
deadlock is that some thread has returned from a function without unlocking a
mutex. In this case, you may need a more sophisticated tool to trace the synchro-
nization behavior of the program. Such a tool would allow you to examine the
data and determine, for example, that function bad\_lock locked a mutex and
failed to unlock that mutex.
### 8.1.4 Beware of priority inversion
"Priority inversion" is a problem unique to applications (or libraries) that rely
on realtime priority scheduling. Priority inversion involves at least three threads
of differing priority. The differing priorities are important-priority inversion is a
conflict between synchronization and scheduling requirements. Priority inversion
allows a low-priority thread to indefinitely prevent a higher-priority thread from
running. The result usually is not a deadlock (though it can be), but it is always
a severe problem. See Section 5.5.4 for more about priority inversion.
Most commonly, a priority inversion results from three threads of differing pri-
ority sharing resources. One example of a priority inversion is when a low-priority
thread locks a mutex, and is preempted by a high-priority thread, which then
blocks on the mutex currently locked by the low-priority thread. Normally, the
low-priority thread would resume, allowing it to unlock the mutex, which would
unblock the high-priority thread to continue. However, if a medium-priority
thread was awakened (possibly by some action of the high-priority thread), it
might prevent the lower-priority thread from running. The medium-priority
thread (or other threads it awakens) may indefinitely prevent the low-priority
thread from releasing the mutex, so a high-priority thread is blocked by the
action of a lower-priority thread.
If the medium-priority thread blocks, the low-priority thread will be allowed to
resume and release the mutex, at which point operation resumes. Because of
this, many priority inversion deadlocks resolve themselves after a short time. If
all priority inversion problems in a program reliably resolve themselves within a
short time, the priority inversion may become a performance issue rather than a
correctness issue. In either case, priority inversion can be a severe problem.
Here are a few ideas to avoid priority inversion:
? Avoid realtime scheduling entirely. That clearly is not practical in many
realtime applications, however.
300 CHAPTER 8 Hints to avoid debugging
Design your threads so that threads of differing priority do not need to use
the same mutexes. This may be impractical, too; many ANSI C functions,
for example, use mutexes.
Use priority ceiling mutexes (Section 5.5.5.1) or priority inheritance (Sec-
tion 5.5.5.2). These are optional features of Pthreads and will not be
available everywhere. Also, you cannot set the mutex priority protocol for
mutexes you do not create, including those used by ANSI C functions.
Avoid calling functions that may lock mutexes you didn't create in any
thread with elevated priority.
### 8.1.5 Never share condition variables between predicates
Your code will usually be cleaner and more efficient if you avoid using a single
condition variable to manage more than one predicate condition. You should not,
for example, define a single "queue" condition variable that is used to awaken
threads waiting for the queue to become empty and also threads waiting for an
element to be added to the queue.
But this isn't just a performance issue (or it would be in another section). If
you use pthread\_cond\_signal to wake threads waiting on these shared condition
variables, the program may hang with threads waiting on the condition variable
and nobody left to wake them up.
Why? Because you can only signal a condition variable when you know that a
single thread needs to be awakened, and that any thread waiting on the condition
variable may be chosen. When multiple predicates share a condition variable, you
can never be sure that the awakened thread was waiting for the predicate you set.
If it was not, then it will see a spurious wakeup and wait again. Your signal has
been lost, because no thread waiting for your predicate had a chance to see that
it had changed.
It is not enough for a thread to resignal the condition variable when it gets a
spurious wakeup, either. Threads may not wake up in the order they waited, espe-
cially when you use priority scheduling. "Resignaling" might result in an infinite
loop with a few high-priority threads (all with the wrong predicate) alternately wak-
ing each other up.
The best solution, when you really want to share a condition variable between
predicates, is always to use pthread\_cond\_broadcast. But when you broadcast,
all waiting threads wake up to reevaluate their predicates. You always know that
one set or the other cannot proceed-so why make them all wake up to find out?
If 1 thread is waiting for write access, for example, and 100 are waiting for read
access, all 101 threads must wake up when the broadcast means that it is now
OK to write, but only the one writer can proceed-the other 100 threads must
wait again. The result of this imprecision is a lot of wasted context switches, and
there are more useful ways to keep your computer busy.
Avoiding incorrect code 301
### 8.1.6 Sharing stacks and related memory corrupters
There's nothing wrong with sharing stack memory between threads. That is, it
is legal and sometimes reasonable for a thread to allocate some variable on its
own stack and communicate that address to one or more other threads. A cor-
rectly written program can share stack addresses with no risk at all; however
(this may come as a surprise), not every program is written correctly, even when
you want it to be correct. Sharing stack addresses can make small programming
errors catastrophic, and these errors can be very difficult to isolate.
I Returning from the function that allocates shared stack memory, when
other threads may still use that data, will result in undesirable behavior.
If you share stack memory, you must ensure that it is never possible for the
thread that owns the stack to "pop" that shared memory from the stack until all
other threads have forever ceased to make use of the shared data. Should the
owning thread return from a stack frame containing the data, for example, the
owning thread may call another function and thereby reallocate the space occu-
pied by the shared variable. One or both of the following possible outcomes will
eventually be observed:
1. Data written by another thread will be overwritten with saved register val-
ues, a return PC, or whatever. The shared data has been corrupted.
2. Saved register values, return PC, or whatever will be overwritten by
another thread modifying the shared data. The owning thread's call frame
has been corrupted.
Having carefully ensured that there is no possible way for the owning thread
to pop the stack data while other threads are using the shared data, are you safe?
Maybe not. We're stretching the point a little, but remember, we're talking about a
programming error-maybe a silly thing like failing to initialize a pointer variable
declared with auto storage class, for example. A pointer to the shared data must
be stored somewhere to be useful-other threads have no other way to find the
proper stack address. At some point, the pointer is likely to appear in various
locations on the stack of every thread that uses the data. None of these pointers
will necessarily be erased when the thread ceases to make use of the stack.
Writes through uninitialized pointers are a common programming error,
regardless of threads, so to some extent this is nothing new or different. However,
in the presence of threads and shared stack data, each thread has an opportu-
nity to corrupt data used by some other thread asynchronously. The symptoms of
that corruption may not appear until some time later, which can pose a particu-
larly difficult debugging task.
If, in your program, sharing stack data seems convenient, then by all means
take advantage of the capability. But if something unexpected happens during
debugging, start by examining the code that shares stack data particularly care-
fully. If you routinely use an analysis tool that reports use of uninitialized
variables (such as Third Degree on Digital UNIX), you may not need to worry
about this class of problem-or many others.
302 CHAPTER 8 Hints to avoid debugging
## 8.2 Avoiding performance problems
"Well, in our country," said Alice, still panting a little, "you'd generally
get to somewhere else-if you ran very fast for a long time as we've
been doing."
"A slow sort of country!" said the Queen. "Now, here, you see, it takes all the
running you can do, to keep in the same place. If you want to get some-
where else, you must run at least twice as fast as that!"
-Lewis Carroll, Through the Looking-Glass
Sometimes, once a program works, it is "done." At least, until you want to
make it do something else. In many cases, though, "working" isn't good enough.
The program needs to meet performance goals. Sometimes the performance goals
are clear: "must perform so many transactions in this period of time." Other
times, the goals are looser: "must be very fast."
This section gives pointers on determining how fast you're going, what's slow-
ing you up, and how to tell (maybe) when you're going as fast as you can go.
There are some very good tools to help you, and there will be a lot more as the
industry adjusts to supporting eager and outspoken thread programmers. But
there are no portable standards for threaded analysis tools. If your vendor sup-
ports threads, you'll probably find at least a thread-safe version of prof, which is
a nearly universal UNIX tool. Each system will probably require different switches
and environments to use it safely for threads, and the output will differ.
Performance tuning requires more than just answering the traditional ques-
tion, "How much time does the application spend in each function?" You have to
analyze contention on mutexes, for example. Mutexes with high contention may
need to be split into several mutexes controlling more specialized data (finer-
grain concurrency), which can improve performance by increasing concurrency.
If finer grain mutexes have low contention, combining them may improve perfor-
mance by reducing locking overhead.
### 8.2.1 Beware of concurrent serialization
The ideal parallel code is a set of tasks that is completely compute-bound.
They never synchronize, they never block-they just "think." If you start with a
program that calls three compute-bound functions in series, and change it to
create three threads each running one of those functions, the program will run
(nearly) three times faster. At least, it should do so if you're running on a multi-
processor with at least three CPUs that are, at that moment, allocated for your
use.
The ideal concurrent code is a set of tasks that is completely I/O-bound. They
never synchronize, and do little computation-they just issue I/O requests and
wait for them. If you start with a program that writes chunks of data to three
Avoiding performance problems 303
separate files (ideally, on three separate disks, with separate controllers), and
change it to create three threads, each writing one of those chunks of data, all
three I/O operations can progress simultaneously.
But what if you've gone to all that trouble to write a set of compute-bound par-
allel or I/O-bound concurrent threads and it turns out that you've just converted
a straight-line serialized program into a multithreaded serialized program? The
result will be a slower program that accomplishes the same result with substan-
tially more overhead. Most likely, that is not what you intended. How could that
have happened?
Let's say that your compute-bound operations call malloc and free in their
work. Those functions modify the static process state, so they need to perform
some type of synchronization. Most likely, they lock a mutex. If your threads run
in a loop calling malloc and free, such that a substantial amount of their total
time may be spent within those functions, you may find that there's very little
real parallelism. The threads will spend a lot of time blocked on the mutex while
one thread or another allocates or frees memory.
Similarly, the concurrent I/O threads may be using serialized resources. If the
threads perform "concurrent" I/O using the same stdio file stream, for example,
they will be locking mutexes to update the stream's shared buffer. Even if the
threads are using separate files, if they are on the same disk there will be locking
within the file system to synchronize the file cache and so forth. Even when using
separate disks, true concurrency may be subject to limitations in the I/O bus or
disk controller subsystems.
The point of all this is that writing a program that uses threads doesn't magi-
cally grant parallelism or even concurrency to your application. When you're
analyzing performance, be aware that your program can be affected by factors
that aren't within your control. You may not even be able to see what's happening
in the file system, but what you can't see can hurt you.
### 8.2.2 Use the right number of mutexes
The first step in making a library thread-safe may be to create a "big mutex"
that protects all entries into the library. If only one thread can execute within the
library at a time, then most functions will be thread-safe. At least, no static data
will be corrupted. If the library has no persistent state that needs to remain con-
sistent across a series of calls, the big mutex may seem to be enough. Many
libraries are left in this state. The standard XI1 client library (Xlib) provides lim-
ited support for this big mutex approach to thread-safety, and has for years.
But thread-safety isn't enough anymore-now you want the library to perform
well with threads. In most cases, that will require redesigning the library so that
multiple threads can use it at the same time. The big mutex serializes all opera-
tions in the library, so you are getting no concurrency or parallelization within
the library. If use of that library is the primary function of your threads, the pro-
gram would run faster with a single thread and no synchronization. That big
304 CHAPTER 8 Hints to avoid debugging
mutex in Xlib, remember, keeps all other threads from using any Xlib function
until the first thread has received its response from the server, and that might
take quite a while.
Map out your library functions, and determine what operations can reason-
ably run in parallel. A common strategy is to create a separate mutex for each
data structure, and use those mutexes to serialize access to the shared data,
rather than using the "big mutex" to serialize access to the library.
With a profiler that supports threads, you can determine that you have too
much mutex activity, by looking for hot spots within calls to pthread\_mutex\_
lock, pthread\_mutex\_unlock, and pthread\_mutex\_trylock. However, this data
will not be conclusive, and it may be very difficult to determine whether the high
activity is due to too much mutex contention or too much locking without conten-
tion. You need more specific information on mutex contention and that requires
special tools. Some thread development systems provide detailed visual tracing
information that shows synchronization costs. Others provide "metering" infor-
mation on individual mutexes to tell how many times the mutex was locked, and
how often threads found the mutex already locked.
#### 8.2.2.1 Too many mutexes will not help
Beware, too, of exchanging a "big" mutex for lots of "tiny" mutexes. You may
make matters worse. Remember, it takes time to lock a mutex, and more time to
unlock that mutex. Even if you increase parallelism by designing a locking hier-
archy that has very little contention, your threads may spend so much time
locking and unlocking all those mutexes that they get less real work done.
Locking a mutex also affects the memory subsystem. In addition to the time
you spend locking and unlocking, you may decrease the efficiency of the memory
system by excessive locking. Locking a mutex, for example, might invalidate a
block of cache on all processors. It might stall all bus activity within some range
of physical addresses.
So find out where you really need mutexes. For example, in the previous sec-
tion I suggested creating a separate mutex for each data structure. Yet, if two
data structures are usually used together, or if one thread will hardly ever need to
use one data structure while another thread is using the second data structure,
the extra mutex may decrease your overall performance.
### 8.2.3 Never fight over cache lines
No modern computer reads data directly from main memory. Memory that is
fast enough to keep up with the computer is too expensive for that to be practi-
cal. Instead, data is fetched by the memory management unit into a fast local
cache array. When the computer writes data, that, too, goes into the local cache
array. The modified data may also be written to main memory immediately or
may be "flushed" to memory only when needed.
Avoiding performance problems 305
So if one processor in a multiprocessor system needs to read a value that
another processor has in its cache, there must be some "cache coherency" mech-
anism to ensure that it can find the correct data. More importantly, when one
processor writes data to some location, all other processors that have older copies
of that location in cache need to copy the new data, or record that the old data is
invalid.
Computer systems commonly cache data in relatively large blocks of 64 or 128
bytes. That can improve efficiency by optimizing the references to slow main
memory. It also means that, when the same 64- or 128-byte block is cached by
multiple processors, and one processor writes to any part of that block, all pro-
cessors caching the block must throw away the entire block.
This has serious implications for high-performance parallel computation. If
two threads access different data within the same cache block, no thread will be
able to take advantage of the (fast) cached copy on the processor it is using. Each
read will require a new cache fill from main memory, slowing down the program.
Cache behavior may vary widely even on different computer systems using the
same microprocessor chip. It is not possible to write code that is guaranteed to be
optimal on all possible systems. You can substantially improve your chances,
however, by being very careful to align and separate any performance-critical data
used by multiple threads.
You can optimize your code for a particular computer system by determining
the cache characteristics of that system, and designing your code so that no two
threads will ever need to write to the same cache block within performance-
critical parallel loops. About the best you can hope to do without optimizing for a
particular system would be to ensure that each thread has a private, page-
aligned, segment of data. It is highly unlikely that any system would use a cache
block as large as a page, because a page includes far too much varied data to pro-
vide any performance advantage in the memory management unit.
# 9 POSIX threads mini-reference
This chapter is a compact reference to the POSIX. lc standard.
# 9.1 POSIX 1003.1 c-1995 options
Pthreads is intended to address a wide variety of audiences. High-performance
computational programs can use it to support parallel decomposition of loops.
Realtime programs can use it to support concurrent realtime I/O. Database and
network servers can use it to easily support concurrent clients. Business or soft-
ware development programs can use it to take advantage of parallel and concurrent
operations on time-sharing systems.
The Pthreads standard allows you to determine which optional capabilities are
provided by the system, by defining a set of feature-test macros, which are shown
in Table 9.1. Any implementation of Pthreads must inform you whether each
option is supported, by three means:
? By making a formal statement of support in the POSIX Conformance Doc-
ument. You can use this information to help design your application to
work on specific systems.
? By defining compile-time symbolic constants in the \<unistd. h\> header file.
You can test for these symbolic constants using #ifdef or #ifndef prepro-
cessor conditionals to support a variety of Pthreads systems.
? By returning a positive nonzero value when the sysconf function is called
with the associated sysconf symbol. (This is not usually useful for the
"feature-test" macros that specify whether options are present-if they are
not, the associated interfaces usually are not supplied, and your code will
not link, and may not even compile.)
You might, for example, choose to avoid relying on priority scheduling because
after reading the conformance documents you discovered that three out of the
four systems you wish to support do not provide the feature. Or you might prefer
to use priority inheritance for your mutexes on systems that provide the feature,
but write the code so that it will not try to access the mutex protocol attribute on
systems that do not provide that option.
307
308
CHAPTER 9 POSIX threads mini-reference
Symbolic constant,
sysconf symbol name
POSIX THREADS
\_SC\_THREADS
POSIX THREAD ATTR STACKSIZE
\_SC\_THREAD\_ATTR STACKSIZE
POSIX THREAD ATTR STACKADDR
\_SC\_THREAD\_ATTR\_STACKADDR
POSIX THREAD PRIORITY SCHEDULING
\_SC\_THREAD\_PRIORITY\_SCHEDULING
POSIX THREAD PRIO INHERIT
\_SC\_THREAD\_PRIO INHERIT
POSIX THREAD PRIO PROTECT
\_SC\_THREAD\_PRIO\_PROTECT
\_POSIX THREAD PROCESS SHARED
\_SC\_THREAD\_PROCESS\_SHARED
POSIX THREAD SAFE FUNCTIONS
\_SC\_THREAD\_SAFE\_FUNCTIONS
Description
You can use threads (if your system
doesn't define this, you're out of luck).
You can control the size of a thread's
stack.
You can allocate and control a
thread's stack.
You can use realtime scheduling.
You can create priority inheritance
mutexes.
You can create priority ceiling mutexes.
You can create mutexes and condition
variables that can be shared with an-
other process.
You can use the special "\_r" library
functions that provide thread-safe
behavior.
TABLE 9.1 POSIX 1003.lc-1995 options
## 9.2 POSIX 1003.lc-1995 limits
The Pthreads standard allows you to determine the run-time limits of the sys-
tem that may affect your application, for example, how many threads you can
create, by defining a set of macros, which are shown in Table 9.2. Any implemen-
tation of Pthreads must inform you of its limits, by three means:
? By making a formal statement in the POSIX Conformance Document. You
can use this information to help design your application to work on specific
systems.
? By defining compile-time symbolic constants in the \<limits. h\> header file.
The symbolic constant may be omitted from \<limits.h\> when the limit is
at least as large as the required minimum, but cannot be determined at
compile time, for example, if it depends on available memory space. You
can test for these symbolic constants using #ifdef or #ifndef preproces-
sor conditionals.
? By returning a positive nonzero value when the sysconf function is called
with the associated sysconf symbol.
You might, for example, design your application to rely on no more than 64
threads, if the conformance documents showed that three out of the four systems
POSIX 1003.1c-1995 interfaces
309
Run-time invariant values,
sysconf symbol name
Description
PTHREAD DESTRUCTOR ITERATIONS
\_SC\_THREAD\_DESTRUCTOR\_ITERATIONS
PTHREAD KEYS MAX
\_SC\_THREAD\_KEYS\_MAX
PTHREAD STACK MIN
SC THREAD STACK MIN
PTHREAD THREADS MAX
\_SC\_THREAD\_THREADS\_MAX
Maximum number of attempts to
destroy a thread's thread-specific data
on termination (must be at least 4).
Maximum number of thread-specific
data keys available per process (must
be at least 128).
Minimum supported stack size for a
thread.
Maximum number of threads support-
ed per process (must be at least 64).
TABLE 9.2 POSIX 1003.1 c-1995 limits
you wish to support do not support additional threads. Or you might prefer to
write conditional code that relies on the value of the pthread\_threads\_max sym-
bolic constant (if defined) or call sysconf to determine the limit at run time.
## 9.3 POSIX 1003.1 c-1995 interfaces
The interfaces are sorted by functional categories: threads, mutexes, and so
forth. Within each category, the interfaces are listed in alphabetical order.
Figure 9.1 describes the format of the entries.
First, the header entry A) shows the name of the interface. If the interface is
an optional feature of Pthreads, then the name of the feature-test macro for that
? r
?
pthread\_mutexattr\_getpshared [\_posixjthread\_process\_shared]
int pthread\_mutexattr\_getpshared (
const pthread\_mutexattr\_t *attr,
int *pshared);
Determine whether mutexes created with attr can be shared by multiple processes.
psbared
PTHREAD\_PROCESS\_SHARED May be shared if in shared
memory.
pthread process PRIVATE Cannot be shared.
?
?
'?
References: 3.2,5.2.1
Headers: \<pthread.h\>
Errors: [EINVAL] attr invalid.
Hint: pshared mutexes must be allocated in shared memory.
FIGURE 9.1 Mini-reference format
310 CHAPTER 9 POSIX threads mini-reference
option is shown at the end of the line, in brackets. The interface pthread\_
mutexattr\_getpshared, for example, is an option under the \_POSIX\_THREAD\_
process\_shared feature.
The prototype entry B) shows the full C language prototype for the interface,
describing how to call the function, with all argument types.
The description entry C) gives a brief synopsis of the interface. In this case,
the purpose of the interface is to specify whether mutexes created using the
attributes object can be shared between multiple processes.
Functions with arguments that have symbolic values, like pshared in this
example, will include a table D) that describes each possible value. The default
value of the argument (the state of a new thread, or the default value of an
attribute in a new attributes object, in this case pthread\_process\_private) is
indicated by showing the name in bold.
The references entry E) gives cross-references to the primary sections of this
book that discuss the interface, or other closely related interfaces.
The headers entry F) shows the header files needed to compile code using the
function. If more than one header is shown, you need all of them.
The errors entry G) describes each of the possible error numbers returned by
the interface; Because Pthreads distinguishes between mandatory error detection
("if occurs" in POSIX terms) and optional error detection ("if detected" in POSIX
terms), the errors that an interface must report (if they occur) are shown in bold
(see Section 9.3.1 for details on Pthreads errors).
The hint entry (8) gives a single, and inevitably oversimplified, philosophical
comment regarding the interface. Some hints point out common errors in using
the interface; others describe something about the designers' intended use of the
interface, or some fundamental restriction of the interface. In pthread\_mutexattr\_
getpshared, for example, the hint points out that a mutex created to be "process
shared" must be allocated in shared memory that's accessible by all participating
processes.
### 9.3.1 Error detection and reporting
The POSIX standard distinguishes carefully between two categories of error:
1. Mandatory ("if occurs") errors involve circumstances beyond the control of
the programmer. These errors must always be detected and reported by the
system using a particular error code. If you cannot create a new thread
because your process lacks sufficient virtual memory, then the implemen-
tation must always tell you. You can't possibly be expected to check
whether there's enough memory before creating the thread-for one thing,
you have no way to know how much memory would be required.
2. Optional ("if detected") errors are problems that are usually your mistake.
You might try to lock a mutex that hadn't been initialized, for example, or
try to unlock a mutex that's locked by another thread. Some systems may
POSIX 1003. lc-1995 interfaces 311
not detect these errors, but they're still errors in your code, and you ought
to be able to avoid them without help from the system.
While it would be "nice" for the system to detect optional errors and return the
appropriate error number, sometimes it takes a lot of time to check or is difficult
to check reliably. It may be expensive, for example, for the system to determine
the identity of the current thread. Systems may therefore not remember which
thread locked a mutex, and would be unable to detect that the unlock was erro-
neous. It may not make sense to slow down the basic synchronization operations
for correct programs just to make it a little easier to debug incorrect programs.
Systems may provide debugging modes where some or all of the optional
errors are detected. Digital UNIX, for example, provides "error check" mutexes
and a "metered" execution mode, where the ownership of mutexes is always
tracked and optional errors in locking and unlocking mutexes are reported. The
UNIX98 specification includes "error check" mutexes (Section 10.1.2), so they will
soon be available on most UNIX systems.
### 9.3.2 Use of void* type
ANSI C requires that you be allowed to convert any pointer type to void* and
back, with the result being identical to the original value. However, ANSI C does
not require that all pointer types have the same binary representation. Thus, a
long* that you convert to void* in order to pass into a thread's start routine
must always be used as a long*, not as, for example, a char*. In addition, the
result of converting between pointer and integer types is "implementation
defined." Most systems supporting UNIX will allow you to cast an integer value to
void* and back, and to mix pointer types-but be aware that the code may not
work on all systems.
Some other standards, notably the POSIX. lb realtime standard, have solved
the same problem (the need for an argument or structure member that can take
any type value) in different ways. The sigevent structure in POSIX. lb, for exam-
ple, includes a member that contains a value to be passed into a signal-catching
function, called sigev\_value. Instead of defining sigev\_value as a void*, how-
ever, and relying on the programmer to provide proper type casting, the sigev\_
value member is a union sigval, containing overlayed int and void* members.
This mechanism avoids the problem of converting between integer and pointer
types, eliminating one of the conflicts with ANSI C guarantees.
### 9.3.3 Threads
Threads provide concurrency, the ability to have more than one "stream of
execution" within a process at the same time. Each thread has its own hardware
registers and stack. All threads in a process share the full virtual address space,
plus all file descriptors, signal actions, and other process resources.
312
CHAPTER 9 POSIX threads mini-reference
I pthread\_attr\_destroy
int pthread\_attr\_destroy (
pthread\_attr\_t *attr);
Destroy a thread attributes object. The object can no longer be used.
References: 2, 5.2.3
Headers: \<pthread.h\>
Errors: [EINVAL] attr is invalid.
Hint: Does not affect threads created using attr.
I pthread\_attr\_getdetachstate
int pthread\_attr\_getdetachstate (
const pthread\_attr\_t *attr,
int *detachstate);
Determine whether threads created with attr will run detached
detachstate
PTHREAD CREATE JOINABLE
\_ \_ Thread ID is valid, must be
joined.
pthread\_create\_detached Thread ID is invalid, cannot be
joined, canceled, or modified.
References: 2, 5.2.3
Headers: \<pthread.h\>
Errors: [EINVAL] attr is invalid.
Hint: You can't join or cancel detached threads.
pthread\_attr\_getstackaddr [\_posix\_thread\_attr\_stackaddr]
int pthread\_attr\_getstackaddr (
const pthread\_attr\_t *attr,
void **stackaddr);
Determine the address of the stack on which threads created with attr will run.
References: 2, 5.2.3
Headers: \<pthread.h\>
Errors: [EINVAL] attr is invalid.
[ENOSYS] stacksize not supported.
Hint: Create only one thread for each stack address!
POSIX 1003.1 c-1995 interfaces
313
pthread\_attr\_getstacksize [\_posix\_thread\_attr\_stacksize]
int pthread\_attr\_getstacksize (
const pthread\_attr\_t *attr,
size\_t *stacksize);
Determine the size of the stack on which threads created with attr will run.
References: 2, 5.2.3
Headers: \<pthread.h\>
Errors: [EINVAL] attr invalid.
[ENOSYS] stacksize not supported.
Hint: Use on newly created attributes object to find the default stack size.
pthread\_attr\_init
int pthread\_attr\_init (
pthread\_attr\_t *attr);
Initialize a thread attributes object with default attributes.
References: 2, 5.2.3
Headers: \<pthread.h\>
Errors: [ENOMEM] insufficient memory for attr.
Hint: Use to define thread types.
I pthread\_attr\_setdetachstate
int pthread\_attr\_setdetachstate (
pthread\_attr\_t *attr,
int detachstate);
Specify whether threads created with attr will run detached.
detachstate
Thread ID is valid, must be
joined.
Thread ID is invalid, cannot be
joinerl. c.-inri'leri. 'T
PTHREAD\_CREATE\_JOINABLE
PTHREAD CREATE DETACHED
References: 2, 5.2.3
Headers: \<pthread.h\>
Errors: [EINVAL] attr invalid.
I EINVAL] detachstate invalid.
Hint: You can't join or cancel detached threads.
314 CHAPTER 9 POSDC threads mini-reference
pthread\_attr\_setstackaddr [\_posix\_thread\_attr\_stackaddr]
int pthread\_attr\_setstackaddr (
pthread\_attr\_t *attr,
void *stackaddr);
Threads created with attr will run on the stack starting at stackaddr. Must be at
least pthread\_STACK\_min bytes.
References: 2, 5.2.3
Headers: \<pthread.h\>
Errors: [EINVAL] attr invalid.
[ENOSYS] stackaddr not supported.
Hint: Create only one thread for each stack address, and be careful of
stack alignment.
pthread\_attr\_setstacksize [\_posix\_thread\_attr\_stacksize ]
int pthread\_attr\_setstacksize (
pthread\_attr\_t *attr,
size\_t stacksize);
Threads created with attr will run on a stack of at least stacksize bytes. Must be
at least pthread\_stack\_min bytes.
References: 2, 5.2.3
Headers: \<pthread.h\>
Errors: [EINVAL] attr or stacksize invalid.
[EINVAL] stacksize too small or too big.
[ENOSXS] stacksize not supported.
Hint: Find the default first (pthread\_attr\_getstacksize), then increase
by multiplying. Use only if a thread needs more than the default.
pthread\_create
int pthread create (
pthread t
const pthread attr t
void
void
*tid,
*attr.
*(*start)
*arq);
(void *),
Create a thread running the start function, essentially an asynchronous call to the
function start with argument value arg. The attr argument specifies optional
creation attributes, and the identification of the new thread is returned intid.
References: 2, 5.2.3
Headers: \<pthread.h\>
Errors: [EINVAL] attr invalid.
[EA6AIN] insufficient resources.
Hint: All resources needed by thread must already be initialized.
POSIX 1003.1 c-1995 interfaces 315
I pthread\_detach
int pthread\_detach (
pthread\_t thread);
Detach the thread. Use this to detach the main thread or to "change your mind"
after creating a joinable thread in which you are no longer interested.
References: 2, 5.2.3
Headers: \<pthread.h\>
Errors: [EINVAL] thread is not a joinable thread.
[ESRCH] no thread could be found for ID thread.
Hint: Detached threads cannot be joined or canceled; storage is freed
immediately on termination.
I pthread\_equal
int pthread\_equal (
pthread\_t tl,
pthread\_t t2) ;
Return value 0 if tl and t2 are equal, otherwise return nonzero.
References: 2, 5.2.3
Headers: \<pthread.h\>
Hint: Compare pthread\_self against stored thread identifier.
I pthread\_exit
int pthread\_exit (
void *value\_ptr);
Terminate the calling thread, returning the value value\_ptr to any joining thread.
References: 2, 5.2.3
Headers: \<pthread.h\>
Hint: value\_ptr is treated as a value, not the address of a value.
pthreadjoin
int pthread\_join (
pthread\_t thread,
void **value\_ptr);
Wait for thread to terminate, and return thread's exit value if value\_ptr is not
null. This also detaches thread on successful completion.
References: 2, 5.2.3
Headers: \<pthread.h\>
Errors: [einvalj thread is not a joinable thread.
[ESRCH] no thread could be found for ID thread.
[edeadlk] attempt to join with self.
Hint: Detached threads cannot be joined or canceled.
316 CHAPTER 9 POSIX threads mini-reference
I pthread\_self
pthread\_t pthread\_self (void);
Return the calling thread's ID.
References: 2, 5.2.3
Headers: \<pthread.h\>
Hint: Use to set thread's scheduling parameters.
sched\_yield
int sched\_yield (void);
Make the calling thread ready, after other ready threads of the same priority, and
select a new thread to run. This can allow cooperating threads of the same priority
to share processor resources more equitably, especially on a uniprocessor. This
function is from POSIX. lb (realtime extensions), and is declared in \<sched.h\>. It
reports errors by setting the return value to -1 and storing an error code in errno.
References: 2, 5.2.3
Headers: \<sched.h\>
Errors: [ENOSYS] sched\_yield not supported.
Hint: Use before locking mutex to reduce chances of a timeslice while mu-
tex is locked.
### 9.3.4 Mutexes
Mutexes provide synchronization, the ability to control how threads share
resources. You use mutexes to prevent multiple threads from modifying shared
data at the same time, and to ensure that a thread can read consistent values for
a set of resources (for example, memory) that may be modified by other threads.
I pthread\_mutexattr\_destroy
int pthread\_mutexattr\_destroy (
pthread\_mutexattr\_t *attr);
Destroy a mutex attributes object. The object can no longer be used.
References: 3.2, 5.2.1
Headers: \<pthread.h\>
Errors: [EINVAL] attr invalid.
Hint: Does not affect mutexes created using attr.
POSIX 1003.1C-1995 interfaces
317
pthread\_mutexattr\_getpshared [\_posix\_thread\_process\_shared]
int pthread\_mutexattr\_getpshared (
const pthread\_mutexattr\_t *attr,
int *pshared);
Determine whether mutexes created with attr can be shared by multiple processes.
pshared
PTHREAD PROCESS SHARED
PTHREAD PROCESS PRIVATE
May be shared if in shared
memory.
Cannot be shared.
References: 3.2,5.2.1
Headers: \<pthread.h\>
Errors: [EINVAL] attr invalid.
Hint: pshared mutexes must be allocated in shared memory.
pthread\_mutexattr\_init
int pthread\_mutexattr\_init (
pthread\_mutexattr\_t *attr);
Initialize a mutex attributes object with default attributes.
References: 3.2, 5.2.1
Headers: \<pthread.h\>
Errors: [enomem] insufficient memory for attr.
Hint: Use to define mutex types.
pthread\_mutexattr\_setpshared [\_posix\_thread\_process\_shared]
int pthread\_mutexattr\_setpshared (
pthread\_mutexattr\_t *attr,
int pshared);
Mutexes created with attr can be shared between processes if the pthread\_mutex\_t
variable is allocated in memory shared by the processes.
PTHREAD\_PROCESS\_SHARED
PTHREAD PROCESS PRIVATE
pshared
May be shared if in shared
memory.
Cannot be shared.
References: 3.2,5.2.1
Headers: \<pthread.h\>
Errors: [EINVAL] attr or detachstate invalid.
Hint: pshared mutexes must be allocated in shared memory.
318 CHAPTER 9 POSIX threads mini-reference
I pthread\_mutex\_destroy
int pthread\_mutex\_destroy (
pthread\_mutex\_t *mutex);
Destroy a mutex that you no longer need.
References: 3.2,5.2.1
Headers: \<pthread.h\>
Errors: [EBUSY] mutex is in use.
[EINVAL] mutex is invalid.
Hint: Safest after unlocking mutex, when no other threads will lock.
I pthread\_mutex\_init
int pthread\_mutex\_init (
pthread\_mutex\_t *mutex,
const pthread\_mutexattr\_t *attr);
Initialize a mutex. The attr argument specifies optional creation attributes.
References: 3.2, 5.2.1
Headers: \<pthread.h\>
Errors: [EAGAiN] insufficient resources (other than memory).
[ENOMEM] insufficient memory.
[EPERM] no privilege to perform operation.
[EBUSY] mutex is already initialized.
[EINVAL] attr is invalid.
Hint: Use static initialization instead, if possible.
pthread\_mutex\_lock
int pthread\_mutex\_lock (
pthread\_mutex\_t *mutex);
Lock a mutex. If the mutex is currently locked, the calling thread is blocked until
mutex is unlocked. On return, the thread owns the mutex until it calls pthread\_
mutex\_unlock.
References: 3.2, 5.2.1
Headers: \<pthread.h\>
Errors: [EINVAL] thread priority exceeds mutex priority ceiling.
[EINVAL] mutex is invalid.
[EDEADLK] calling thread already owns mutex.
Hint: Always unlock within the same thread.
POSIX 1003. lc-1995 interfaces 319
I pthread\_mutex\_trylock
int pthread\_mutex\_trylock (
pthread\_mutex\_t *mutex);
Lock a mutex. If the mutex is currently locked, returns immediately with EBUSY. Oth-
erwise, calling thread becomes owner until it unlocks.
References: 3.2, 5.2.1
Headers: \<pthread.h\>
Errors: [EINVAL] thread priority exceeds rnutex priority ceiling.
[EBUSY] mutex is already locked.
[EINVAL] mutex is invalid.
[EDEADLK] calling thread already owns mutex.
Hint: Always unlock within the same thread.
I pthread\_mutex\_unlock
int pthread\_mutex\_unlock (
pthread\_mutex\_t *mutex);
Unlock a mutex. The mutex becomes unowned. If any threads are waiting for the
mutex, one is awakened (scheduling policy sched\_fifo and sched\_rr policy wait-
ers are chosen in priority order, then any others are chosen in unspecified order).
References: 3.2, 5.2.1
Headers: \<pthread.h\>
Errors: [EINVAL] mutex is invalid.
[EPERM] calling thread does not own mutex.
Hint: Always unlock within the same thread.
### 9.3.5 Condition variables
Condition variables provide communication, the ability to wait for some shared
resource to reach some desired state, or to signal that it has reached some state
in which another thread may be interested. Each condition variable is closely
associated with a mutex that protects the state of the resource.
I pthread\_condaftr\_destroy
int pthread\_condattr\_destroy (
pthread\_condattr\_t *attr);
Destroy a condition variable attributes object. The object can no longer be used.
References: 3.3, 5.2.2
Headers: \<pthread.h\>
Errors: [EINVAL] attr invalid.
Hint: Does not affect condition variables created using attr.
320
CHAPTER 9 POSIX threads mini-reference
pthread\_condattr\_getpshared [\_posix\_thread\_process\_shared]
int pthread\_condattr\_getpshared (
const pthread\_condattr\_t *attr,
int *pshared);
Determine whether condition variables created with attr can be shared by multiple
processes.
PTHREAD\_
PTHREAD\_
PROCESS
PROCESS
SHARED
PRIVATE
pshared
May be shared if in shared
memory.
Cannot be shared.
References:
Headers:
Errors:
Hint:
3.3, 5.2.2
\<pthread.h\>
[EINVAL] attr invalid.
pshared condition variables must be allocated in shared memory
and used with pshared mutexes.
I pthread\_condattr\_init
int pthread\_condattr\_init (
pthread\_condattr\_t
*attr);
Initialize a condition variable attributes object with default attributes.
References: 3.3, 5.2.2
Headers: \<pthread.h\>
Errors: [ENOMEM] insufficient memory for attr.
Hint: Use to define condition variable types.
pthread\_condattr\_setpshared [\_posix\_thread\_process\_shared]
int pthread\_condattr\_setpshared (
pthread\_condattr\_t *attr,
int pshared);
Condition variables created with attr can be shared between processes if the
pthread\_cond\_t variable is allocated in memory shared by the processes.
PTHREAD
PTHREAD\_
PROCESS
PROCESS
SHARED
PRIVATE
pshared
May be shared if in shared
memory.
Cannot be shared.
POSIX 1003.1c-1995 interfaces 321
References: 3.3, 5.2.2
Headers: \<pthread.h\>
Errors: [EINVAL] attr or detachstate invalid.
Hint: pshared condition variables must be allocated in shared memory
and used with pshared mutexes.
pthread\_cond\_destroy
int pthread\_cond\_destroy (
pthread\_cond\_t *cond);
Destroy condition variable cond that you no longer need.
References: 3.3, 5.2.2
Headers: \<pthread.h\>
Errors: [EBUSY] cond is in use.
[EINVAL] cond is invalid.
Hint: Safest after wakeup from cond, when no other threads will wait.
pthread\_cond\_init
int pthread\_cond\_init (
pthread\_cond\_t *cond,
const pthread\_condattr\_t *attr);
Initialize a condition variable cond. The attr argument specifies optional creation
attributes.
References: 3.3, 5.2.2
Headers: \<pthread.h\>
Errors: [EAGAIN] insufficient resources (other than memory).
[ ENOMEM] insufficient memory.
[ ebusy ] cond is already initialized.
[EINVAL] attr is invalid.
Hint: Use static initialization instead, if possible.
I pthread\_cond\_broadcast
int pthread\_cond\_broadcast (
pthread\_cond\_t *cond);
Broadcast condition variable cond, waking all current waiters.
References: 3.3, 5.2.2
Headers: \<pthread.h\>
Errors: [EINVAL] cond is invalid.
Hint: Use when more than one waiter may respond to predicate change
or if any waiting thread may not be able to respond.
322 CHAPTER 9 POSIX threads mini-reference
I pthread\_cond\_signal
int pthread\_cond\_signal (
pthread\_cond\_t *cond);
Signal condition variable cond, waking one waiting thread. If SCHED\_fifo or sched\_rr
policy threads are waiting, the highest-priority waiter is awakened. Otherwise, an
unspecified waiter is awakened.
References: 3.3, 5.2.2
Headers: \<pthread.h\>
Errors: [EINVAL] cond is invalid.
Hint: Use when any waiter can respond, and only one need respond. (All
waiters are equal.)
I pthread\_cond\_timedwait
int pthread\_cond\_timedwait (
pthread\_cond\_t *cond,
pthread\_mutex\_t *mutex,
const struct timespec *abstime);
Wait on condition variable cond, until awakened by a signal or broadcast, or until
the absolute time abstime is reached.
References: 3.3, 5.2.2
Headers: \<pthread.h\>
Errors: [ETIMEDOUT] time specified by abstime has passed.
[EINVAL] cond, mutex, or abstime is invalid.
[einval] different mutexes for concurrent waits.
[EINVAL] mutex is not owned by calling thread.
Hint: Mutex is always unlocked (before wait) and relocked (after wait)
inside pthread\_cond\_timedwait, even if the wait fails, times out, or
is canceled.
I pthread\_cond\_vrait
int pthread\_cond\_wait (
pthread\_cond\_t *cond,
pthread\_mutex\_t *mutex);
Wait on condition variable cond, until awakened by a signal or broadcast.
References: 3.3, 5.2.2
Headers: \<pthread.h\>
Errors: [EINVAL] cond or mutex is invalid.
[EINVAL] different mutexes for concurrent waits.
[EINVAL] mutex is not owned by calling thread.
Hint: Mutex is always unlocked (before wait) and relocked (after wait) in-
side pthread\_cond\_wait, even if the wait fails or is canceled.
POSIX 1003.1 c-1995 interfaces 323
### 9.3.6 Cancellation
Cancellation provides a way to request that a thread terminate "gracefully"
when you no longer need it to complete its normal execution. Each thread can
control how and whether cancellation affects it, and can repair the shared state
as it terminates due to cancellation.
I pthread\_cancel
int pthread\_cancel (
pthread\_t thread);
Requests that thread be canceled.
References: 5.3
Headers: \<pthread.h\>
Errors: [ESRCH] no thread found corresponding to thread.
Hint: Cancellation is asynchronous. Use pthread\_join to wait for termi-
nation of thread if necessary.
I pthread\_cleanup\_pop
void pthread\_cleanup\_pop (int execute);
Pop the most recently pushed cleanup handler. Invoke the cleanup handler if exe-
cute is nonzero.
References: 5.3
Headers: \<pthread.h\>
Hint: Specify execute as nonzero to avoid duplication of common cleanup
code.
I pthread\_cleanup\_push
void pthread\_cleanup\_push (
void ("routine)(void *),
void *arg);
Push a new cleanup handler onto the thread's stack of cleanup handlers. Invoke
the cleanup handler if execute is nonzero. Each cleanup handler pushed onto the
stack is popped and invoked with the argument arg when the thread exits by call-
ing pthread\_exit, when the thread acts on a cancellation request, or when the
thread calls pthread\_cleanup\_pop with a nonzero execute argument.
References: 5.3
Headers: \<pthread.h\>
Hint: pthread\_cleanup\_push and pthread\_cleanup\_pop must be paired
in the same lexical scope.
324
CHAPTER 9 POSIX threads mini-reference
I pthread\_setcancelstate
int pthread\_setcancelstate (
int state,
int *oldstate);
Atomically set the calling thread's cancelability state to state and return the pre-
vious cancelability state at the location referenced by oldstate.
PTHREAD
PTHREAD\_
CANCEL
\_CANCEL\_
ENABLE
DISABLE
state, oldstate
Cancellation is enabled.
Cancellation is disabled.
References: 5.3
Headers: \<pthread.h\>
Errors: [EINVAL] state is invalid.
Hint: Use to disable cancellation around "atomic" code that includes can-
cellation points.
I pthread\_setcanceltype
int pthread\_setcanceltype (
int type,
int *oldtype);
Atomically set the calling thread's cancelability type to type and return the previ-
ous cancelability type at the location referenced by oldtype.
PTHREAD CANCEL DEFERRED
type, oldtype
Only deferred caiic.ellal.ion is
allowed.
pthread\_cancel\_asynchronous Asynchronous cancellation is
allowed.
References: 5.3
Headers: \<pthread.h\>
Errors: [einval] type is invalid.
Hint: Use with caution-most code is not safe for use with asynchronous
cancelability type.
POSIX 1003.1 c-1995 interfaces 325
I pthreadjestcancel
void pthread\_testcancel (void);
Creates a deferred cancellation point in the calling thread. The call has no effect if
the current cancelability state is pthread\_cancel\_DISABLE.
References: 5.3
Headers: \<pthread.h\>
Hint: Cancellation is asynchronous. Use pthread\_join to wait for termi-
nation of thread if necessary.
### 9.3.7 Thread-specific data
Thread-specific data provides a way to declare variables that have a common
"name" in all threads, but a unique value in each thread. You should consider
using thread-specific data in a threaded program in many cases where a non-
threaded program would use "static" data. When the static data maintains con-
text across a series of calls to some function, for example, the context should
generally be thread-specific. (If not, the static data must be protected by a
mutex.)
I pthread\_getspecific
void *pthread\_getspecific (
pthread\_key\_t key);
Return the current value of key in the calling thread. If no value has been set for
key in the thread, null is returned.
References: 5.4, 7.2, 7.3.1
Headers: \<pthread.h\>
Errors: The effect of calling pthread\_getspecif ic with an invalid key is un-
defined. No errors are detected.
Hint: Calling pthread\_getspecif ic in a destructor function will return
null. Use destructor's argument instead.
pthread\_key\_create
int pthread\_key\_create (
pthread\_key\_t *key,
void (*destructor)(void *));
Create a thread-specific data key visible to all threads. All existing and new threads
have value null for key until set using pthread\_setspecif ic. When any thread
with a non-NULL value for key terminates, destructor is called with key's current
value for that thread.
326 CHAPTER 9 POSLK threads mini-reference
References: 5.4,7.2,7.3.1
Headers: \<pthread.h\>
Errors: [EAGAIN] insufficient resources or PTHREAD\_keys\_MAX exceeded.
[enomem] insufficient memory to create the key.
Hint: Each key (pthread\_key\_t variable) must be created only once; use
a mutex or pthread\_once.
I pthread\_key\_delete
int pthread\_key\_delete (
pthread\_key\_t key);
Delete a thread-specific data key. This does not change the value of the thread-
specific data key for any thread and does not run the key's destructor in any thread,
so it should be used with great caution.
References: 5.4
Headers: \<pthread.h\>
Errors: [Einval] key is invalid.
Hint: Use only when you know all threads have null value.
pthread.setspecific
int pthread\_setspecific (
pthread\_key\_t key,
const void *value);
Associate a thread-specific value within the calling thread for the specified key.
References: 5.4, 7.2, 7.3.1
Headers: \<pthread.h\>
Errors: [enomem] insufficient memory.
[EINVAL] key is invalid.
Hint: If you set a value of null, the key's destructor will not be called at
thread termination.
### 9.3.8 Realtime scheduling
Realtime scheduling provides a predictable response time to important events
within the process. Note that "predictable" does not always mean "fast," and in
many cases realtime scheduling may impose overhead that results in slower exe-
cution. Realtime scheduling is also subject to synchronization problems such as
priority inversion (Sections 5.5.4 and 8.1.4), although Pthreads provides optional
facilities to address some of these problems.
POSLK 1003. lc-1995 interfaces 327
pthread\_attr\_getinheritsched [\_posix\_thread\_priority\_scheduling]
int pthread\_attr\_getinheritsched (
const pthread\_attr\_t *attr,
int *inheritsched);
Determine whether threads created with attr will run using the scheduling policy
and parameters of the creator or those specified in the attributes object. The default
inheritsched is implementation-defined.
inheritsched
pthread\_inherit\_sched Use creator's scheduling
policy and parameters.
pthread\_explicit\_sched Use scheduling policy and
parameters in attributes
object.
References: 5.2.3, 5.5
Headers: \<pthread.h\>
Errors: [ENOSYS] priority scheduling is not supported.
[EINVAL] attr invalid.
pthread\_attr\_getschedparam [\_posix\_thread\_priority\_scheduling]
int pthread\_attr\_getschedparam (
const pthread\_attr\_t *attr,
struct sched\_param *param);
Determine the scheduling parameters used by threads created with attr. The default
param is implementation defined.
References: 5.2.3, 5.5
Headers: \<pthread.h\>
Errors: [ENOSYS] priority scheduling is not supported.
[EINVAL] attr invalid.
pthread\_attr\_getschedpolicy [\_posix\_thread\_priority\_scheduling]
int pthread\_attr\_getschedpolicy (
const pthread\_attr\_t *attr,
int *policy);
Determine the scheduling policy used by threads created with attr. The default
policy is implementation defined.
328
CHAPTER 9 POSIX threads mini-reference
SCHED FIFO
SCHED RR
SCHED OTHER
policy
Run thread until it blocks;
preempt lower-priority
threads when ready.
Like SCHED\_FIFO, but sub-
ject to periodic timeslicing.
Implementation defined (may
be SCHED\_FIFO, SCHED\_RR,
or something else).
References: 5.2.3, 5.5
Headers: \<pthread.h\>
Errors: [ENOSYS] priority scheduling is not supported.
[EINVAL] attr invalid.
pthread\_attr\_getscope [\_posix\_thread\_priority\_scheduling]
int pthread\_attr\_getscope (
const pthread\_attr\_t *attr,
int *contentionscope);
Determine the contention scope used by threads created with attr. The default is
implementation defined.
contentionscope
PTHREAD SCOPE PROCESS
Thread contends with other
threads in the process for pro-
cessor resources.
PTHREAD SCOPE SYSTEM
Thread contends with threads
in all processes for processor
resources.
References: 5.2.3, 5.5
Headers: \<pthread.h\>
Errors: [ENOSYS] priority scheduling is not supported.
[EINVAL] attr invalid.
Hint: Implementation must support one or both of these, but need not
support both.
POSIX 1003.1 c-1995 interfaces 329
pthread\_attr\_setinheritsched [\_posix\_thread\_priority\_scheduling]
int pthread\_attr\_setinheritsched (
pthread\_\_attr\_t *attr,
int inheritsched);
Specify whether threads created with attr will run using the scheduling policy and
parameters of the creator or those specified in the attributes object. When you
change the scheduling policy or parameters in a thread attributes object, you must
change the inheritsched attribute from pthread\_inherit\_sched to pthread\_
explicit\_SCHED. The default is implementation-defined.
inheritsched
pthread\_INHERIT\_SCHED Use creator's scheduling
policy and parameters.
pthread\_explicit\_SCHED Use scheduling policy and
parameters in attributes
object.
References: 5.2.3, 5.5
Headers: \<pthread.h\>
Errors: [ENOSYS] priority scheduling is not supported.
[EINVAL] attr or inheritsched invalid.
pthread\_attr\_setschedparam [\_posix\_thread\_priority\_scheduling]
int pthread\_attr\_setschedparam (
pthread\_attr\_t *attr,
const struct sched\_param *param);
Specify the scheduling parameters used by threads created with attr. The default
param is implementation defined.
References: 5.2.3, 5.5
Headers: \<pthread.h\>
Errors: [ENOSYS] priority scheduling is not supported.
[EINVAL] attr or param invalid.
[ ENOTSUP ] param set to supported value.
pthread\_attr\_setschedpolicy [\_posix\_thread\_priority\_scheduling]
int pthread\_attr\_setschedpolicy (
pthread\_attr\_t *attr,
int policy);
Specify the scheduling policy used by threads created with attr. The default
policy is implementation defined.
330
CHAPTER 9 POSIX threads mini-reference
SCHED FIFO
SCHED\_RR
SCHED OTHER
policy
Run thread until it blocks;
preempt lower-priority
threads when ready.
Like SCHED\_FIFO, but sub-
ject to periodic timeslicing.
Implementation denned (may
be SCHED\_FIFO, SCHED\_RR,
or something else).
References: 5.2.3, 5.5
Headers: \<pthread.h\>
Errors: [enosysj priority scheduling is not supported.
[EINVAL] attr or policy invalid.
[ENOTSUP] param set to supported value.
pthread\_attr\_setscope [\_posix\_thread\_priority\_scheduling]
int pthread\_attr\_setscope (
pthread\_attr\_t *attr,
int contentionscope);
Specify the contention scope used by threads created with attr. The default is
implementation denned.
contentionscope
PTHREAD SCOPE PROCESS
Thread contends with other
threads in the process for pro-
cessor resources.
PTHREAD SCOPE SYSTEM
Thread contends with threads
in all processes for processor
resources.
References: 5.2.3, 5.5
Headers: \<pthread.h\>
Errors: [ENOSYS] priority scheduling is not supported.
[EINVAL] attr or contentionscope invalid.
[ENOTSUP] contentionscope set to supported value.
Hint: Implementation must support one or both of these, but need not
support both.
POSIX 1003.1c~ 1995 interfaces
331
pthread\_getschedparam [\_posix\_thread\_priority\_scheduling]
int pthread\_getschedparam (
pthread\_t thread,
int *policy
struct sched\_param *param);
Determine the scheduling policy and parameters (param) currently used by thread.
policy
SCHED FIFO
SCHED\_RR
SCHED OTHER
Run thread until it blocks;
preempt lower-priority
threads when ready.
Like SCHED\_FIFO, but sub-
ject to periodic timeslicing.
Implementation denned (may
be SCHED\_FIFO, SCHED\_RR,
or something else).
References: 5.2.3, 5.5
Headers: \<pthread.h\>
Errors: [ENOSYS] priority scheduling is not supported.
[ESRCH] thread does not refer to an existing thread.
Hint: Try to avoid dynamically modifying thread scheduling policy and
parameters, if possible.
pthread\_mutex\_getprioceiling [\_posix\_thread\_prio\_protect]
int pthread\_rautex\_getprioceiling (
const pthread\_mutex\_t *mutex,
int *prioceiling);
Determine the priority ceiling at which threads will run while owning mutex.
References: 3.2,5.2.1,5.5.5
Headers: \<pthread.h\>
Errors: [ENOSYS] priority scheduling is not supported.
[EINVAL] mutex invalid.
Hint: Protect protocol is inappropriate unless the creator of the mutex
also creates and controls all threads that might lock the mutex.
332 CHAPTER 9 POSIX threads mini-reference
pthread\_mutex\_setprioceiling [\_posix\_thread\_prio\_protect]
int pthread\_mutex\_getprioceiling (
pthread\_mutex\_t *mutex,
int prioceiling,
int *old\_ceiling);
Specify the priority ceiling at which threads will run while owning mutex. Returns
previous priority ceiling for mutex.
References: 3.2,5.2.1,5.5.5
Headers: \<pthread.h\>
Errors: [ENOSYS] priority scheduling is not supported.
[EINVAL] mutex invalid, or prioceiling out of range.
[EPERM] no privilege to set prioceiling.
Hint: Protect protocol is inappropriate unless the creator of the mutex
also creates and controls all threads that might lock the mutex.
pthread\_mutexaftr\_getprioceiling [\_posix\_thread\_prio\_protect ]
int pthread\_mutexattr\_getprioceiling (
const pthread\_mutexattr\_t *attr,
int *prioceiling);
Determine the priority ceiling at which threads will run while owning a mutex cre-
ated with attr.
References: 3.2, 5.2.1, 5.5.5
Headers: \<pthread.h\>
Errors: [ENOSYS] priority scheduling is not supported.
[EINVAL] attr invalid.
Hint: Protect protocol is inappropriate unless the creator of the mutex
also creates and controls all threads that might lock the mutex.
pthread\_mutexattr\_getprotocol... [\_posix\_thread\_prio\_inherit\_posix\_thread\_prio\_protect]
int pthread\_mutexattr\_getprotocol (
const pthread\_mutexattr\_t *attr,
int *protocol);
Determine whether mutexes created with attr have priority ceiling protocol (pro-
tect), priority inheritance protocol {inherit), or no priority protocol (none).
POSIX 1003.1 c-1995 interfaces 333
protocol
PTHREAD\_PRIO\_HONE No priority inheritance
protocol.
pthread\_PRIO\_INHERit While owning mutex, thread
inherits highest priority of any
thread waiting for the mutex.
pthread\_prio\_protect While owning mutex, thread
inherits mutex priority ceiling.
References: 3.2,5.2.1,5.5.5
Headers: \<pthread.h\>
Errors: [ENOSYS] priority scheduling is not supported.
[EINVAL] attr invalid.
Hint: Inherit protocol is expensive, and protect protocol is inappropriate
unless the creator of the mutex also creates and controls all threads
that might lock the mutex.
pthread\_mutexattr\_setprioceiling [\_posix\_thread\_prio\_protect]
int pthread\_mutexattr\_setprioceiling (
pthread\_mutexattr\_t *attr,
int prioceiling);
Specify the priority ceiling at which threads will run while owning a mutex created
with attr. The value of prioceiling must be a valid priority parameter for the
SCHED\_FIFO policy.
References: 3.2,5.2.1,5.5.5
Headers: \<pthread.h\>
Errors: [ENOSYS] priority scheduling is not supported.
[EINVAL] attr or prioceiling invalid.
[EPERM] no permission to set prioceiling.
Hint: Protect protocol is inappropriate unless the creator of the mutex
also creates and controls all threads that might lock the mutex.
pthread\_mutexattr\_setprotocol.... (\_posix\_thread\_prio\_inherit\_posix\_thread\_prio\_protect]
int pthread\_mutexattr\_setprotocol (
pthread\_mutexattr\_t *attr,
int protocol);
Specify whether mutexes created with attr have priority ceiling protocol {protect),
priority inheritance protocol (inherit), or no priority protocol (none).
334
CHAPTER 9 POSIX threads mini-reference
PTHREAD\_PRIO\_NONE
PTHREAD\_PRIO\_IKHERIT
PTHREAD PRIO PROTECT
protocol
No priority inheritance
protocol.
While owning mutcx, thread
inherits highest priority or any
thread waiting for the mutex.
While owning mutex, thread
inherits mutex priority ceiling.
References: 3.2, 5.2.1, 5.5.5
Headers: \<pthread.h\>
Errors: [ENOSYS] priority scheduling is not supported.
[EINVAL] attr or protocol invalid.
[ENOTSUP] protocol value is not supported.
Hint: Inherit protocol is expensive, and protect protocol is inappropriate
unless the creator of the mutex also creates and controls all threads
that might lock the mutex.
pthread\_setschedparam [\_posix\_thread\_priority\_scheduling]
int pthread\_setschedparam (
pthread\_t thread,
int policy
const struct sched\_param *param);
Specify the scheduling policy and parameters (param) to be used by thread.
SCHED\_FIFO
SCHED\_RR
SCHED\_OTHER
policy
Run thread until it blocks;
preempt lower-priority
threads when ready.
Like SCHED\_FIFO, but sub-
ject to periodic timeslicing.
Implementation defined (may
be SCHED\_FIFO, SCHED\_RR,
or something else).
References:
Headers:
Errors:
Hint:
5.5
\<pthread.h\>
[ENOSYS] priority scheduling is not supported.
[ESRCH] thread does not refer to an existing thread.
[EINVAL] policy or param is invalid.
[ENOTSUP] policy or param is unsupported value.
[EPERM] no permission to set policy or param.
Try to avoid dynamically modifying thread scheduling policy and
parameters, if possible.
POSIX 1003.1C-1995 interfaces
335
sched\_get\_priority\_max [\_posix\_priority\_scheduling]
int sched\_get\_priority\_max (
int policy);
Return the maximum integer priority allowed for the specified scheduling policy.
policy
SCHED\_FIFO
SCHED\_RR
SCHED OTHER
Run thread until it blocks;
preempt lower-priority
threads when ready.
Like sched\_fifo, but subject
to periodic timeslicing.
Implementation defined (may
be SCHED\_FIFO, SCHED\_RR,
or something else).
References: 5.5.2
Headers: \<sched.h\>
Errors: [ENOSYS] priority scheduling is not supported.
[EINVAL] policy is invalid.
Hint: Priority min and max are integer values-you can compute relative
values, for example, half and quarter points in range.
sched\_get\_priority\_min [posixpriorityscheduling]
int sched\_get\_priority\_min (
int policy);
Return the minimum integer priority allowed for the specified scheduling policy.
policy
SCHED FIFO
SCHED RR
Run thread until it blocks;
preempt lower-priority
threads when ready.
SCHED OTHER
Like SCHED\_FIFO, but sub-
ject to periodic timeslicing.
Implementation denned (may
be SCHED\_FIFO, SCHED\_RR,
or something else).
References: 5.5.2
Headers: \<sched.h\>
Errors: [ENOSYS] priority scheduling is not supported.
[EINVAL] policy is invalid.
Hint: Priority min and max are integer values-you can compute relative
values, for example, half and quarter points in range.
336 CHAPTER 9 POSIX threads mini-reference
### 9.3.9 Fork handlers
Pthreads provides some new functions to help the new threaded environment
to coexist with the traditional process-based UNIX environment. Creation of a
child process by copying the full address space, for example, causes problems for
threaded applications because the fork call is asynchronous with respect to
other threads in the process.
I pthread\_atfork
int pthread\_atfork (
void (*prepare)(void),
void (*parent)(void),
void (*child)(void));
Define "fork handlers" that are run when the process creates a child process. Allows
protection of synchronization objects and shared data in the child process (which
is otherwise difficult to control).
References: 6.1.1
Headers: \<unistd.h\>
Errors: [ENOMEM] insufficient space to record the handlers.
Hint: All resources needed by child must be protected.
### 9.3.10 Stdio
Pthreads provides some new functions, and new versions of old functions, to
access ANSI C stdio features safely from a threaded process. For safety reasons,
the old forms of single-character access to stdio buffers have been altered to lock
the file stream, which can decrease performance. You can change old code to
instead lock the file stream manually and, within that locked region, use new
character access operations that do not lock the file stream.
| flockfile
void flockfile (
FILE *file);
Increase the lock count for a stdio file stream to gain exclusive access to the file
stream. If the file stream is currently locked by another thread, the calling thread
is blocked until the lock count for the file stream becomes zero. If the calling thread
already owns the file stream lock, the lock count is incremented-an identical num-
ber of calls to funlockfile is required to release the file stream lock.
* Digital UNIX and Solaris both (incorrectly) place the definition in \<pthread. h\>. The UNIX 98
brand will require that they be fixed.
POSIX 1003.1 c-1995 interfaces 337
Although most stdio functions, such as printf and f gets, are thread-safe, you
may sometimes find that it is important that a sequence of printf calls, for exam-
ple, from one thread cannot be separated by calls made from another thread. Also,
a few stdio functions are not thread-safe and can only be used while the file stream
is locked by the caller.
References: 6.4.1
Headers: \<stdio.h\>
Hint: Use to protect a sequence of stdio operations.
j ftrylockfile
int ftrylockfile (
FILE *file);
If the file stream is currently locked by another thread, return a nonzero value. Oth-
erwise, increase the lock count for the file stream, and return the value zero.
References: 6.4.1
Headers: \<stdio.h\>
Hint: Use to protect a sequence of stdio operations.
I funlockfile
void funlockfile (
FILE *file);
Decrease the lock count for a stdio file stream that was previously locked by a cor-
responding call to funlockfile. If the lock count becomes 0, release the lock so
that another thread can lock it.
References: 6.4.1
Headers: \<stdio.h\>
Hint: Use to protect a sequence of stdio operations.
I getc\_unlocked
int getc\_unlocked (
FILE *file);
Return a single character from the stdio stream file, without locking the file stream.
This operation must only be used while the file stream has been locked by calling
f lockf ile, or when you know that no other thread may access the file stream con-
currently. Returns EOF for read errors or end-of-file condition.
References: 6.4.2
Headers: \<stdio.h\>
Hint: Replace old calls to getc to retain fastest access.
338 CHAPTER 9 POSIX threads mini-reference
getchar\_unlocked
int getc\_unlocked (void);
Return a single character from the stdio stream stdin without locking the file
stream. This operation must only be used while the file stream has been locked by
calling f lockf ile, or when you know that no other thread may access the file
stream concurrently. Returns EOF for read errors or end-of-flle condition.
References: 6.4.2
Headers: \<stdio.h\>
Hint: Replace old calls to getchar to retain fastest access.
putc\_unlocked
int putc\_unlocked (
int c,
FILE *file);
Write a single character c (interpreted as an unsigned char) to the stdio stream file
without locking the file stream. This operation must only be used while the file
stream has been locked by calling f lockf ile, or when you know that no other
thread may access the file stream concurrently. Returns the character or the value
EOF if an error occurred.
References: 6.4.2
Headers: \<stdio.h\>
Hint: Replace old calls to putc to retain fastest access.
I putchar\_unlocked
int putchar\_unlocked (
int c);
Write a single character c (interpreted as an unsigned char) to the stdio stream
stdout without locking the file stream. This operation must only be used while the
file stream has been locked by calling f lockf ile, or when you know that no other
thread may access the file stream concurrently. Returns the character or the value
EOF if an error occurred.
References: 6.4.2
Headers: \<stdio.h\>
Hint: Replace old calls to putchar to retain fastest access.
### 9.3.11 Thread-safe functions
Thread-safe functions provide improved access to traditional features of
ANSI C and POSIX that cannot be effectively made thread-safe without interface
changes. These routines are designated by the "\_r" suffix added to the traditional
function name they replace, for example, getlogin\_r for getlogin.
POSIX 1003.1 c-1995 interfaces 339
I getlogirw
int getlogin\_r (
char *name,
size\_t namesize);
Write the user name associated with the current process into the buffer pointed to
by name. The buffer is namesize bytes long, and should have space for the name
and a terminating null character. The maximum size of the login name is LOGIN\_
NAME\_MAX.
References: 6.5.1
Headers: \<unistd.h\>
I readdir\_r
int readdir\_r (
DIR *dirp,
struct dirent *entry,
struct dirent **result);
Return a pointer (result) to the directory entry at the current position in the direc-
tory stream to which dirp refers. Whereas readdir retains the current position us-
ing a static variable, readdir\_r uses the entry parameter, supplied by the caller.
References: 6.5.2
Headers: \<sys/types.h\>, \<dirent.h\>
Errors: [EBADF] dirp is not an open directory stream.
r
strtokj
char *strtok\_r (
char *s,
const char *sep,
char **lasts);
Return a pointer to the next token in the string s. Whereas strtok retains the current
position within a string using a static variable, strtok\_r uses the lasts parameter,
supplied by the caller.
References: 6.5.3
Headers: \<string.h\>
asctime\_r
char *asctime\_r (
const struct tm*tm,
char *buf);
Convert the "broken-down" time in the structure pointed to by tm into a string,
which is stored in the buffer pointed to by buf. The buffer pointed to by buf must
340 CHAPTER 9 POS1X threads mini-reference
contain at least 26 bytes. The function returns a pointer to the buffer on success,
or null on failure.
References: 6.5.4
Headers: \<time.h\>
ctime\_r
char *ctime\_r (
const time\_t *clock,
char *buf);
Convert the calendar time pointed to by clock into a string representing the local
time, which is stored in the buffer pointed to by buf. The buffer pointed to by buf
must contain at least 26 bytes. The function returns a pointer to the buffer on suc-
cess, or NULL on failure.
References: 6.5.4
Headers: \< t ime.h\>
I gmtime\_r
struct tm *gmtime\_r (
const time\_t *clock,
struct tm *result);
Convert the calendar time pointed to by clock into a "broken-down time" expressed
as Coordinated Universal Time (UTC), which is stored in the structure pointed to
by result. The function returns a pointer to the structure on success, or null on
failure.
References: 6.5.4
Headers: \<time.h\>
I localtime\_r
struct tm *localtime\_r (
const time\_t *clock,
struct tm *result);
Convert the calendar time pointed to by clock into a "broken-down time" expressed
as local time, which is stored in the structure pointed to by result. The function
returns a pointer to the structure on success, or null on failure.
References: 6.5.4
Headers: \<time.h\>
POSIX 1003.1 c-1995 interfaces 341
rand\_r
int rand\_r (
unsigned int *seed);
Return the next value in a sequence of pseudorandom integers in the range of 0 to
rand\_max. Whereas rand uses a static variable to maintain the context between a
series of calls, rand\_r uses the value pointed to by seed, which is supplied by the
caller.
References: 6.5.5
Headers: \<stdlib.h\>
I getgrgicU
int getgrgid\_r (
gid\_t
struct group *group,
char *buffer,
size\_t bufsize,
struct group **result);
Locate an entry from the group database with a group id matching the gid argu-
ment. The group entry is stored in the memory pointed to by buffer, which con-
tains bufsize bytes, and a pointer to the entry is stored at the address pointed to
by result. The maximum buffer size required can be determined by calling
sysconf with the \_SC\_GETGR\_R\_SIZE\_MAX parameter.
References: 6.5.6
Headers: \<sys/types.h\>, \<grp.h\>
Errors: [ erange ] the specified buffer is too small.
getgrnam\_r
int getgrnam\_r (
const char *name,
struct group *group,
char *buffer,
size\_t bufsize,
struct group **result);
Locate an entry from the group database with a group name matching the name
argument. The group entry is stored in the memory pointed to by buffer, which
contains bufsize bytes, and a pointer to the entry is stored at the address pointed
to by result. The maximum buffer size required can be determined by calling
sysconf with the \_sc\_getgr\_r\_size\_max parameter.
References: 6.5.6
Headers: \<sys/types.h\>, \<grp.h\>
Errors: [ erange ] the specified buffer is too small.
342 CHAPTER 9 POSIX threads mini-reference
I getpwuid\_r
int getpwuid\_r (
uid\_t uid,
struct passwd *pwd,
char *buffer,
size\_t bufsize,
struct passwd **result);
Locate an entry from the user database with a user id matching the uid argument.
The user entry is stored in the memory pointed to by buffer, which contains
bufsize bytes, and a pointer to the entry is stored at the address pointed to by
result. The maximum buffer size required can be determined by calling sysconf
with the \_sc\_getpw\_r\_size\_max parameter.
References: 6.5.6
Headers: \<sys/types.h\>, \<pwd.h\>
Errors: [ERANGE ] the specified buffer is too small.
I getpwnam\_r
int getpwnam\_r (
const char *name,
struct passwd *pwd,
char *buffer,
size\_t bufsize,
struct passwd **result);
Locate an entry from the user database with a user name matching the name argu-
ment. The user entry is stored in the memory pointed to by buff er, which contains
bufsize bytes, and a pointer to the entry is stored at the address pointed to by
result. The maximum buffer size required can be determined by calling sysconf
with the \_SC\_GETPW\_r\_size\_MAX parameter.
References: 6.5.6
Headers: \<sys/types.h\>, \<pwd.h\>
Errors: [ERANGE ] the specified buffer is too small.
### 9.3.12 Signals
Pthreads provides functions that extend the POSIX signal model to support
multithreaded processes. All threads in a process share the same signal actions.
Each thread has its own pending and blocked signal masks. The process also
has a pending signal mask so that asynchronous signals can pend against the
process when all threads have the signal blocked. In a multithreaded process,
the behavior of sigprocmask is undefined.
POSIX 1003.1c-1995 interfaces
343
| pthreacLkill
int pthread\_kill (
pthread t
int
thread,
sig);
Request that the signal sig be delivered to thread. If sig is 0, no signal is sent, but
error checking Is performed. If the action of the signal is to terminate, stop, or con-
tinue, then the entire process is affected.
References: 6.6.3
Headers: \<signal.h\>
Errors: [ESRCH] no thread corresponding to thread.
[EINVAL] sig is an invalid signal number.
Hint: To terminate a thread, use cancellation.
I pthread\_sigmask
int pthread\_sigmask (
int how,
const sigset\_t *set,
sigset\_t *oset);
Control the masking of signals within the calling thread.
how
SIG BLOCK
SIG UNBLOCK
SIG SETMASK
Resulting sot is Ihe union ol the
cum'iil si-i and ihc iirftiuiiciii set.
Resulting set is ihc intersection of
llii* rurreni sel ;ind tin: argument
set.
Resulting set is the set pointed in
by the argument set.
References: 6.6.2
Headers: \<signal.h\>
Errors: [einval] how is not one of the defined values.
Hint: You cannot prevent delivery of asynchronous signals to the process
unless the signal is blocked in all threads.
I sigtimedwait
int sigtimedwait (
const sigset\_t *set,
siginfo\_t *info,
const struct timespec *timeout);
If a signal in set is pending, atomically clear it from the set of pending signals and
return the signal number in the si\_signo member of info. The cause of the signal
344 CHAPTER 9 POSIX threads mini-reference
shall be stored in the si\_code member. If any value is queued to the selected signal,
return the first queued value in the si\_value member. If no signal in set is pend-
ing, suspend the calling thread until one or more become pending. If the time in-
terval specified by timeout passes, sigtimedwait will return with the error eagain.
This function returns the signal number-on error, it returns -1 and sets err no to
the appropriate error code.
References: 6.6.4
Headers: \<signal.h\>
Errors: [EINVAL] set contains an invalid signal number.
[eagain] the timeout interval passed.
[enosyS] realtime signals are not supported.
Hint: Use only for asynchronous signal delivery. All signals in set must
be masked in the calling thread, and should usually be masked in
all threads.
sigwait
int sigwait (
const sigset\_t *set,
int *sig);
If a signal in set is pending, atomically clear it from the set of pending signals and
return the signal number in the location referenced by sig. If no signal in set is
pending, suspend the calling thread until one or more become pending.
References: 6.6.4
Headers: \<signal.h\>
Errors: [EINVAL] set contains an invalid signal number.
Hint: Use only for asynchronous signal delivery. All signals in set must
be masked in the calling thread, and should usually be masked in
all threads.
I sigwaitinfo
int sigwaitinfo (
const sigset\_t *set,
siginfo\_t *info);
If a signal in set is pending, atomically clear it from the set of pending signals and
return the signal number in the si\_signo member of info. The cause of the signal
shall be stored in the si\_code member. If any value is queued to the selected signal,
return the first queued value in the si\_value member. If no signal in set is pend-
ing, suspend the calling thread until one or more become pending. This function re-
turns the signal number-on error, it returns -1 and sets err no to the appropriate
error code.
POSIX 1003.lc-1995 interfaces 345
References: 6.6.4
Headers: \<signal.h\>
Errors: [einval] set contains an invalid signal number.
[ENOSYS] realtime signals are not supported.
Hint: Use only for asynchronous signal delivery. All signals in set must
be masked in the calling thread, and should usually be masked in
all threads.
### 9.3.13 Semaphores
Semaphores come from POSIX. lb (POSIX 1003.1b-1993) rather than from
Pthreads. They follow the older UNIX convention for reporting errors. That is, on
failure they return a value of -1 and store the appropriate error number into
errno. All of the semaphore functions require the header file \<semaphore.h\>.
I sem\_destroy [posixsemaphores]
int sem\_destroy (
sem\_t *sem);
Destroy an unnamed semaphore.
References: 6.6.6
Headers: \<semaphore.h\>
Errors: [einval] value exceeds SEM\_value\_max.
[ENOSYS] semaphores are not supported.
[ ebusy ] threads (or processes) are currently blocked on sem.
semjnit [posixsemaphores]
int sem\_init (
sem\_t *sem,
int pshared,
unsigned int value);
Initialize an unnamed semaphore. The initial value of the semaphore counter is
value. If the pshared argument has a nonzero value, the semaphore can be shared
between processes. With a zero value, it can be shared only between threads in the
same process.
References: 6.6.6
Headers: \< s emaphore.h\>
Errors: [EINVAL] sem is not a valid semaphore.
[Enospc] a required resource has been exhausted.
[ENOSYS] semaphores are not supported.
[EPERM] the process lacks appropriate privilege.
Hint: Use a value of 1 for a lock, a value of 0 for waiting.
346 CHAPTER 9 POSIX threads mini-reference
semjrywait [posixsemaphoresj
int sem\_trywait (
sem\_t *sem);
Try to wait on a semaphore (or "try to lock" the semaphore). If the semaphore value
is greater than zero, decrease the value by one. If the semaphore value is 0, then
return immediately with the error eagain.
References: 6.6.6
Headers: \<semaphore.h\>
Errors: [eagain] the semaphore was already locked.
[EINVAL] sem is not a valid semaphore.
[EINTR] the function was interrupted by a signal.
[enosys] semaphores are not supported.
[EDEADLK] a deadlock condition was detected.
Hint: When the semaphore's initial value was 1, this is a lock operation;
when the initial value was 0, this is a wait operation.
Sem\_pOSt [\_POSIX\_SEMAPHORES]
int sem\_post (
sem\_t *sem);
Post a wakeup to a semaphore. If there are waiting threads (or processes), one is
awakened. Otherwise the semaphore value is incremented by one.
References: 6.6.6
Headers: \<semaphore.h\>
Errors: [EINVAL] sem is not a valid semaphore.
[EMOSYS] semaphores are not supported.
Hint: May be used from within a signal-handling function.
sem\_wait [\_posix\_semaphores]
int sem\_wait (
sem\_t *sem);
Wait on a semaphore (or lock the semaphore). If the semaphore value is greater
than zero, decrease the value by one. If the semaphore value is 0, then the calling
thread (or process) is blocked until it can successfully decrease the value or until
interrupted by a signal.
References: 6.6.6
Headers: \<semaphore.h\>
Errors: [EINVALJ sem is not a valid semaphore.
[EINTR] the function was interrupted by a signal.
[ENOSYS] semaphores are not supported.
[ edeadlk ] a deadlock condition was detected.
Hint: When the semaphore's initial value was 1, this is a lock operation;
when the initial value was 0, this is a wait operation.
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
