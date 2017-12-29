
# 9 POSIX threads mini-reference
This chapter is a compact reference to the POSIX.1c standard.

# 9.1 POSIX 1003.1c-1995 options
Pthreads is intended to address a wide variety of audiences. High-performance computational programs can use it to support parallel decomposition of loops. Realtime programs can use it to support concurrent realtime I/O. Database and network servers can use it to easily support concurrent clients. Business or soft- ware development programs can use it to take advantage of parallel and concurrent operations on time-sharing systems.

The Pthreads standard allows you to determine which optional capabilities are provided by the system, by defining a set of feature-test macros, which are shown in Table 9.1. Any implementation of Pthreads must inform you whether each option is supported, by three means:
- By making a formal statement of support in the POSIX Conformance Doc- ument. You can use this information to help design your application to work on specific systems.
- By defining compile-time symbolic constants in the \<unistd. h\> header file. You can test for these symbolic constants using #ifdef or #ifndef prepro- cessor conditionals to support a variety of Pthreads systems.
- By returning a positive nonzero value when the sysconf function is called with the associated sysconf symbol. (This is not usually useful for the "feature-test" macros that specify whether options are present-if they are not, the associated interfaces usually are not supplied, and your code will not link, and may not even compile.)

You might, for example, choose to avoid relying on priority scheduling because after reading the conformance documents you discovered that three out of the four systems you wish to support do not provide the feature. Or you might prefer to use priority inheritance for your mutexes on systems that provide the feature, but write the code so that it will not try to access the mutex protocol attribute on systems that do not provide that option.

Symbolic constant, sysconf symbol name | Description
 ---- | ----
\_POSIX\_THREADS \_SC\_THREADS | You can use threads (if your system doesn't define this, you're out of luck).
\_POSIX\_THREAD\_ATTR\_STACKSIZE \_SC\_THREAD\_ATTR\_STACKSIZE | You can control the size of a thread's stack.
\_POSIX\_THREAD\_ATTR\_STACKADDR \_SC\_THREAD\_ATTR\_STACKADDR | You can allocate and control a thread's stack.
\_POSIX\_THREAD\_PRIORITY\_SCHEDULING \_SC\_THREAD\_PRIORITY\_SCHEDULING | You can use realtime scheduling.
\_POSIX\_THREAD\_PRIO\_INHERIT \_SC\_THREAD\_PRIO\_INHERIT | You can create priority inheritance mutexes.
\_POSIX\_THREAD\_PRIO\_PROTECT \_SC\_THREAD\_PRIO\_PROTECT | You can create priority ceiling mutexes.
\_POSIX\_THREAD\_PROCESS\_SHARED \_SC\_THREAD\_PROCESS\_SHARED | You can create mutexes and condition variables that can be shared with another process.
\_POSIX\_THREAD\_SAFE\_FUNCTIONS \_SC\_THREAD\_SAFE\_FUNCTIONS | You can use the special "\_r" library functions that provide thread-safe behavior.

<center>**TABLE 9.1** *POSIX 1003.1c-1995 options*</center>
## 9.2 POSIX 1003.1c-1995 limits
The Pthreads standard allows you to determine the run-time limits of the sys- tem that may affect your application, for example, how many threads you can create, by defining a set of macros, which are shown in Table 9.2. Any implemen- tation of Pthreads must inform you of its limits, by three means:
- By making a formal statement in the POSIX Conformance Document. You can use this information to help design your application to work on specific systems.
- By defining compile-time symbolic constants in the \<limits. h\> header file. The symbolic constant may be omitted from \<limits.h\> when the limit is at least as large as the required minimum, but cannot be determined at compile time, for example, if it depends on available memory space. You can test for these symbolic constants using #ifdef or #ifndef preproces- sor conditionals.
- By returning a positive nonzero value when the sysconf function is called with the associated sysconf symbol.

You might, for example, design your application to rely on no more than 64
threads, if the conformance documents showed that three out of the four systems
Run-time invariant values, sysconf symbol name | Description
 --- | --- 
\_PTHREAD\_DESTRUCTOR\_ITERATIONS \_SC\_THREAD\_DESTRUCTOR\_ITERATIONS   | Maximum number of attempts to destroy a thread's thread-specific data on termination (must be at least 4).
\_PTHREAD\_KEYS\_MAX \_SC\_THREAD\_KEYS\_MAX                             | Maximum number of thread-specific data keys available per process (must be at least 128).
\_PTHREAD\_STACK\_MIN SC\_THREAD\_STACK\_MIN                             | Minimum supported stack size for a thread.
\_PTHREAD\_THREADS\_MAX \_SC\_THREAD\_THREADS\_MAX                       | Maximum number of threads support- ed per process (must be at least 64).

<center>**TABLE 9.2** *POSIX 1003.1c-1995 limits*</center>

you wish to support do not support additional threads. Or you might prefer to write conditional code that relies on the value of the pthread\_threads\_max sym- bolic constant (if defined) or call sysconf to determine the limit at run time.
## 9.3 POSIX 1003.1c-1995 interfaces
The interfaces are sorted by functional categories: threads, mutexes, and so forth. Within each category, the interfaces are listed in alphabetical order. Figure 9.1 describes the format of the entries.

<center>![FIGURE 9.1 Mini-reference format](./img/fig9.1.png)

First, the header entry A) shows the name of the interface. If the interface is an optional feature of Pthreads, then the name of the feature-test macro for that option is shown at the end of the line, in brackets. The interface pthread\_ mutexattr\_getpshared, for example, is an option under the \_POSIX\_THREAD\_ process\_shared feature.

The prototype entry B) shows the full C language prototype for the interface, describing how to call the function, with all argument types. The description entry C) gives a brief synopsis of the interface. In this case, the purpose of the interface is to specify whether mutexes created using the attributes object can be shared between multiple processes.

Functions with arguments that have symbolic values, like pshared in this example, will include a table D) that describes each possible value. The default value of the argument (the state of a new thread, or the default value of an attribute in a new attributes object, in this case pthread\_process\_private) is indicated by showing the name in bold.

The references entry E) gives cross-references to the primary sections of this book that discuss the interface, or other closely related interfaces.

The headers entry F) shows the header files needed to compile code using the function. If more than one header is shown, you need all of them.

The errors entry G) describes each of the possible error numbers returned by the interface; Because Pthreads distinguishes between mandatory error detection ("if occurs" in POSIX terms) and optional error detection ("if detected" in POSIX terms), the errors that an interface must report (if they occur) are shown in bold (see Section 9.3.1 for details on Pthreads errors).

The hint entry (8) gives a single, and inevitably oversimplified, philosophical comment regarding the interface. Some hints point out common errors in using the interface; others describe something about the designers' intended use of the interface, or some fundamental restriction of the interface. In pthread\_mutexattr\_ getpshared, for example, the hint points out that a mutex created to be "process shared" must be allocated in shared memory that's accessible by all participating processes.

### 9.3.1 Error detection and reporting
The POSIX standard distinguishes carefully between two categories of error:
1. Mandatory ("if occurs") errors involve circumstances beyond the control of the programmer. These errors must always be detected and reported by the system using a particular error code. If you cannot create a new thread because your process lacks sufficient virtual memory, then the implemen- tation must always tell you. You can't possibly be expected to check whether there's enough memory before creating the thread-for one thing, you have no way to know how much memory would be required.
2. Optional ("if detected") errors are problems that are usually your mistake. You might try to lock a mutex that hadn't been initialized, for example, or try to unlock a mutex that's locked by another thread. Some systems may not detect these errors, but they're still errors in your code, and you ought to be able to avoid them without help from the system.

While it would be "nice" for the system to detect optional errors and return the appropriate error number, sometimes it takes a lot of time to check or is difficult to check reliably. It may be expensive, for example, for the system to determine the identity of the current thread. Systems may therefore not remember which thread locked a mutex, and would be unable to detect that the unlock was erro- neous. It may not make sense to slow down the basic synchronization operations for correct programs just to make it a little easier to debug incorrect programs.

Systems may provide debugging modes where some or all of the optional errors are detected. Digital UNIX, for example, provides "error check" mutexes and a "metered" execution mode, where the ownership of mutexes is always tracked and optional errors in locking and unlocking mutexes are reported. The UNIX98 specification includes "error check" mutexes (Section 10.1.2), so they will soon be available on most UNIX systems.

### 9.3.2 Use of void\* type
ANSI C requires that you be allowed to convert any pointer type to void\* and back, with the result being identical to the original value. However, ANSI C does not require that all pointer types have the same binary representation. Thus, a long\* that you convert to void\* in order to pass into a thread's start routine must always be used as a long\*, not as, for example, a char\*. In addition, the result of converting between pointer and integer types is "implementation defined." Most systems supporting UNIX will allow you to cast an integer value to void\* and back, and to mix pointer types-but be aware that the code may not work on all systems.

Some other standards, notably the POSIX. lb realtime standard, have solved the same problem (the need for an argument or structure member that can take any type value) in different ways. The sigevent structure in POSIX. lb, for exam- ple, includes a member that contains a value to be passed into a signal-catching function, called sigev\_value. Instead of defining sigev\_value as a void\*, how- ever, and relying on the programmer to provide proper type casting, the sigev\_ value member is a union sigval, containing overlayed int and void\* members. This mechanism avoids the problem of converting between integer and pointer types, eliminating one of the conflicts with ANSI C guarantees.

### 9.3.3 Threads
Threads provide concurrency, the ability to have more than one "stream of execution" within a process at the same time. Each thread has its own hardware registers and stack. All threads in a process share the full virtual address space, plus all file descriptors, signal actions, and other process resources.

```c
int pthread_attr_destroy (
    pthread_attr_t *attr);
```
Destroy a thread attributes object. The object can no longer be used.  

> References: 2, 5.2.3  
> Headers: \<pthread.h\>  
> Errors: [EINVAL] attr is invalid.  
> Hint: Does not affect threads created using attr.  

```c
int pthread_attr_getdetachstate (
    const pthread_attr_t *attr,
    int *detachstate);
```
Determine whether threads created with attr will run detached

 | detachstate
  --- | ---
PTHREAD\_CREATE\_JOINABLE | Thread ID is valid, must be joined.
PTHREAD\_CREATE\_DETACHED | Thread ID is invalid, cannot be joined, canceled, or modified.

> References: 2, 5.2.3
> Headers: \<pthread.h\>
> Errors: [EINVAL] attr is invalid.
> Hint: You can't join or cancel detached threads.

```c
int pthread_attr_getstackaddr (
    const pthread_attr_t *attr,
    void **stackaddr);
```
Determine the address of the stack on which threads created with attr will run.

> References: 2, 5.2.3
> Headers: \<pthread.h\>
> Errors: [EINVAL] attr is invalid.
>         [ENOSYS] stacksize not supported.
> Hint: Create only one thread for each stack address!
```c
int pthread_attr_getstacksize (
    const pthread_attr_t *attr,
    size_t *stacksize);
```
Determine the size of the stack on which threads created with attr will run.

> References: 2, 5.2.3
> Headers: \<pthread.h\>
> Errors: [EINVAL] attr invalid.
>         [ENOSYS] stacksize not supported.
> Hint: Use on newly created attributes object to find the default stack size.

```c
int pthread_attr_init (
    pthread_attr_t *attr);
```
Initialize a thread attributes object with default attributes.

> References: 2, 5.2.3
> Headers: \<pthread.h\>
> Errors: [ENOMEM] insufficient memory for attr.
> Hint: Use to define thread types.

```c
int pthread_attr_setdetachstate (
    pthread_attr_t *attr,
    int detachstate);
```
Specify whether threads created with attr will run detached.

 | detachstate
 --- | ---
PTHREAD\_CREATE\_JOINABLE | Thread ID is valid, must be joined.
PTHREAD\_CREATE\_DETACHED | Thread ID is invalid, cannot be joined, canceled, or modified.

> References: 2, 5.2.3
> Headers: \<pthread.h\>
> Errors: [EINVAL] attr invalid.
> [EINVAL] detachstate invalid.
> Hint: You can't join or cancel detached threads.

```c
int pthread_attr_setstackaddr (
    pthread_attr_t *attr,
    void *stackaddr);
```
Threads created with attr will run on the stack starting at stackaddr. Must be at least PTHREAD\_STACK\_MIN bytes.

> References: 2, 5.2.3
> Headers: \<pthread.h\>
> Errors: [EINVAL] attr invalid.
>         [ENOSYS] stackaddr not supported.
> Hint: Create only one thread for each stack address, and be careful of stack alignment.

```c
int pthread_attr_setstacksize (
    pthread_attr_t *attr,
    size_t stacksize);
```
Threads created with attr will run on a stack of at least stacksize bytes. Must be at least pthread\_stack\_min bytes.

> References: 2, 5.2.3
> Headers: \<pthread.h\>
> Errors: [EINVAL] attr or stacksize invalid.
>         [EINVAL] stacksize too small or too big.
>         [ENOSXS] stacksize not supported.
> Hint: Find the default first (pthread\_attr\_getstacksize), then increase by multiplying. Use only if a thread needs more than the default.

```c
int pthread create (
    pthread_t               *tid,
    const pthread_attr_t    *attr,
    void                    *(*start) (void *),
    void                    *arg);
```

Create a thread running the start function, essentially an asynchronous call to the function start with argument value arg. The attr argument specifies optional creation attributes, and the identification of the new thread is returned intid.

> References: 2, 5.2.3
> Headers: \<pthread.h\>
> Errors: [EINVAL] attr invalid.
>         [EA6AIN] insufficient resources.
> Hint: All resources needed by thread must already be initialized.

```c
int pthread_detach (
    pthread_t thread);
```
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
POSIX 1003.1c-1995 interfaces 319
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
POSIX 1003.1c-1995 interfaces 323
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
POSIX 1003.1c-1995 interfaces 325
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
POSLK 1003.1c-1995 interfaces 327
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
POSIX 1003.1c-1995 interfaces 329
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
POSIX 1003.1c-1995 interfaces
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
POSIX 1003.1c-1995 interfaces 333
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
POSIX 1003.1c-1995 interfaces
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
POSIX 1003.1c-1995 interfaces 337
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
POSIX 1003.1c-1995 interfaces 339
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
POSIX 1003.1c-1995 interfaces 341
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
POSIX 1003.1c-1995 interfaces 345
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
