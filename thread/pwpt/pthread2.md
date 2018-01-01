
# 2 Threads

Lewis Carroll, Through the Looking-Glass:
> "If seven maids with seven mops  
> Swept it for half a year,  
> Do you suppose," the Walrus said,  
> "That they could get it clear?"  
> "I doubt it," said the Carpenter,  
> And shed a bitter tear.

Threads are (and perhaps this will come as no surprise) the essential basis of the style of programming that I am advocating. Although this chapter focuses on threads, you will never learn everything you need to know about threads by simply skipping to this chapter and reading it. Threads are a critical part of the landscape, but you can't do much with only threads. Nevertheless, one must start somewhere, and here we are.

Section 2.1 describes the programming aspects of creating and managing threads in your program, that is, how to create threads, how they are represented in your program, and the most basic things you can do to them once you've created them.

Section 2.2 describes the life cycle of a thread, from creation through "recycling," taking you through all the scheduling states threads can assume along the way.

## 2.1 Creating and using threads
Lewis Carroll, Through the Looking-Glass:
> "A loaf of bread," the Walrus said,  
> "Is what we chiefly need:  
> Pepper and vinegar besides  
> Are very good indeed-  
> Now, if you're ready, Oysters dear,  
> We can begin to feed."

```c
pthread_t thread;
int pthread_equal (pthread_t t1, pthread_t t2);
int pthread_create (pthread_t *thread,
    const pthread_attr_t *attr,
    void *(*start)(void *), void *arg);
pthread_t pthread_self (void);
int sched_yield (void);
int pthread_exit (void *value_ptr);
int pthread_detach (pthread_t thread);
int pthread_join (pthread_t thread, void **value_ptr);
```

The introduction covered some of the basics of what a thread is, and what it means to the computer hardware. This section begins where the introduction left off. It explains how a thread is represented in your program, what it means to your program, and some of the operations you can perform on threads. If you haven't read the introduction, this would be a good time to skip back to it. (I'll wait for you here.)

Within your program a thread is represented by a thread identifier, of the opaque type `pthread_t`. To create a thread, you must declare a variable of type `pthread_t` somewhere in your program. If the identifier is needed only within a function, or if the function won't return until the thread is done, you could declare the identifier with auto storage class. Most of the time, though, the identifier will be stored in a shared (static or extern) variable, or in a structure allocated from the heap.

A Pthreads thread begins by calling some function that you provide. This "thread function" should expect a single argument of type void \*, and should return a value of the same type. You create a thread by passing the thread function's address, and the argument value with which you want the function to be called, to `pthread_create`.

When you create a thread, `pthread_create` returns an identifier, in the `pthread_t` value referred to by the thread argument, by which your code refers to the new thread. A thread can also get its own identifier using the `pthread_self` function. There is no way to find a thread's identifier unless either the creator or the thread itself stores the identifier somewhere. You need to have a thread's identifier to do anything to the thread. If you'll need to know when a thread completes, for example, you must keep the identifier somewhere.

Pthreads provides the `pthread_equal` function to compare two thread identifiers. You can only test for equality. It doesn't make any sense to ask whether one thread identifier is "greater than" or "less than" another, because there is no ordering between threads. The `pthread_equal` function returns a nonzero value if the thread identifiers refer to the same thread, and the value 0 if they do not refer to the same thread.

> The initial thread (main) is special.

When a C program runs, it begins in a special function named main. In a threaded program, this special stream of execution is called the "initial thread" or sometimes the "main thread." You can do anything within the initial thread that you can do within any other thread. It can determine its own thread identifier by calling `pthread_self`, for example, or terminate itself by calling `pthread_exit`. If the initial thread stores its thread identifier somewhere accessible to another thread, that thread can wait for the initial thread to terminate, or detach the initial thread.

The initial thread is special because Pthreads retains traditional UNIX process behavior when the function main returns; that is, the process terminates without allowing other threads to complete. In general, you do not want to do this in a threaded program, but sometimes it can be convenient. In many of the programs in this book, for example, threads are created that have no effect on anything outside the process. It doesn't really matter what those threads are doing, then, if the process goes away. When the process exits, those threads, all their states, and anything they might accomplish, simply "evaporate"-there's no reason to clean up.

> Detaching a thread that is still running doesn't affect the thread in any way-it just informs the system that the thread's resources can be reclaimed when the thread eventually terminates.

Although "thread evaporation" is sometimes useful, most of the time your process will outlive the individual threads you create. To be sure that resources used by terminated threads are available to the process, you should always detach each thread you create when you're finished with it. Threads that have terminated but are not detached may retain virtual memory, including their stacks, as well as other system resources. Detaching a thread tells the system that you no longer need that thread, and allows the system to reclaim the resources it has allocated to the thread.

If you create a thread that you will never need to control, you can use an attribute to create the thread so that it is already detached. (We'll get to attributes later, in Section 5.2.3.) If you do not want to wait for a thread that you created, and you know that you will no longer need to control that thread, you can detach it at any time by calling `pthread_detach`. A thread may detach itself, or any other thread that knows its `pthread_t` identifier may detach it at any time. If you need to know a thread's return value, or if you need to know when a thread has completed, call `pthread_join`. The `pthread_join` function will block the caller until the thread you specify has terminated, and then, optionally, store the terminated thread's return value. Calling `pthread_join` detaches the specified thread automatically.

As we've seen, threads within a process can execute different instructions, using different stacks, all at the same time. Although the threads execute independently of each other, they always share the same address space and file descriptors. The shared address space provides an important advantage of the threaded programming model by allowing threads to communicate efficiently.

Some programs may create threads that perform unrelated activities, but most often a set of threads works together toward a common goal. For example, one set of threads may form an assembly line in which each performs some specific task on a shared data stream and then passes the data on to the next thread. A set of threads may form a work crew and divide independent parts of a common task. Or one "manager" thread may take control and divide work among a "crew" of worker threads. You can combine these models in a variety of ways; for example, a work crew might perform some complicated step in a pipeline, such as transforming a slice of an array.

The following program, lifecycle.c, creates a thread. We'll refer to this simple example in the following sections about a thread's life cycle.

The thread function, `thread_routine`, returns a value to satisfy the standard thread function prototype. In this example the thread returns its argument, and the value is always null.

The program creates a thread by calling `pthread_create`, and then waits for it by calling `pthread_join`. You don't need to wait for a thread, but if you don't, you'll need to do something else to make sure the process runs until the thread completes. Returning from main will cause the process to terminate, along with all threads. You could, for example, code the main thread to terminate by calling `pthread_exit`, which would allow the process to continue until all threads have terminated.

When the join completes, the program checks the thread's return value, to be sure that the thread returned the value it was given. The program exits with 0 (success) if the value is null, or with 1 otherwise.

It is a good idea for all thread functions to return something, even if it is simply null. If you omit the return statement, `pthread_join` will still return some value-whatever happens to be in the place where the thread's start function would have stored a return value (probably a register).

```c
/** lifecycle.c */
    #include <pthread.h>
    #include "errors.h"

    /*
    * Thread start routine.
    */
    void *thread_routine (void *arg)
    {
        return arg;
    }

    main (int argc, char *argv[])
    {
        pthread_t thread_id;
        void *thread result;
        int status;

        status = pthread_create (
            &thread_id, NULL, thread_routine, NULL);
        if (status != 0)
            err_abort (status, "Create thread");

        status = pthread_join (thread_id, &thread_result);
        if (status != 0)
            err_abort (status, "Join thread");
        if (thread_result == NULL)
            return 0;
        else
            return 1;
    }
```

If the "joining" thread doesn't care about the return value, or if it knows that the "joinee" (the thread with which it is joining) didn't return a value, then it can pass NULL instead of sretval in the call to `pthread_join`. The joinee's return value will be ignored.

When the call to `pthread_join` returns, the joinee has been detached and you can't join with it again. In the rare cases where more than one thread might need to know when some particular thread has terminated, the threads should wait on a condition variable instead of calling `pthread_join`. The terminating thread would store its return value (or any other information) in some known location, and broadcast the condition variable to wake all threads that might be interested.

## 2.2 The life of a thread

Lewis Carroll, The Hunting of the Snark:
> Come, listen, my men, while I tell you again  
> The five unmistakable marks  
> By which you may know, wheresoever you go,  
> The warranted genuine Snarks.

At any instant, a thread is in one of the four basic states described in Table 2.1. In implementations, you may see additional "states" that distinguish between various reasons for entering the four basic states. Digital UNIX, for example, represents these finer distinctions as "substates," of which each state may have several. Whether they're called "substates" or additional states, "terminated" might be divided into "exited" and "cancelled"; "blocked" might be broken up into "blocked on condition variable," "blocked on mutex," "blocked in read," and so forth.

| State      | Meaning                                  |
| ---------- | ---------------------------------------- |
| Ready      | The thread is able to run, but is waiting for a processor. It may have just started, or just been unblocked, or preempted by another thread. |
| Running    | The thread is currently running; on a multiprocessor there may be more than one running thread in the process. |
| Blocked    | The thread is not able to run because it is waiting for something; for example, it may be waiting for a condition variable, or waiting to lock a mutex, or waiting for an I/O operation to complete. |
| Terminated | The thread has terminated by returning from its start function, calling pthread exit, or having been cancelled and completing all cleanup handlers. It was not detached, and has not yet been joined. Once it is detached or joined, it will be recycled. |
<center>**TABLE 2.1** *Thread states*</center>

These finer distinctions can be important in debugging and analyzing threaded programs. However, they do not add substantially to the basic understanding of thread scheduling, and we will not deal with them here.

Threads begin in the ready state. When the new thread runs it calls your specified thread start function. It may be preempted by other threads, or block itself to wait for external events any number of times. Eventually it completes and either returns from the thread start function or calls the `pthread_exit` function. In either case it terminates. If the thread has been detached, it is immediately recycled. (Doesn't that sound nicer than "destroyed"-and most systems reuse the resources to make new threads.) Otherwise the thread remains in the terminated state until joined or detached. Figure 2.1 shows the relationships between these thread states, and the events that cause threads to move from one state to another.

### 2.2.1 Creation
The "initial thread" of a process is created when the process is created. In a system that fully supports threaded programming, there's probably no way to execute any code without a thread. A thread is likely to be the only software context that includes the hardware state needed to execute code: registers, program counter, stack pointer, and so forth.

Additional threads are created by explicit calls. The primary way to create threads on a Pthreads system is to call `pthread_create`. Threads may also be created when the process receives a POSIX signal if the process signal notify mechanism is set to `sigev_thread`. Your system may provide additional nonstandard mechanisms to create a thread.

```mermaid
graph TB;
    I(Input)==>A((Thread A))
    I==>B((Thread B))
    I==>C((Thread C))
    A==>O(Output)
    B==>O
    C==>O
end
```

<center>**FIGURE 2.1** *Thread state transitions*</center>

When a new thread is created, its state is ready. Depending on scheduling constraints, it may remain in that state for a substantial period of time before executing. Section 5.5 contains more information on thread scheduling. Going back to lifecycle.c, the thread running `thread_routine` becomes ready during main's call to `pthread_create`, at line 18.

The most important thing to remember about thread creation is that there is no synchronization between the creating thread's return from `pthread_create` and the scheduling of the new thread. That is, the thread may start before the creating thread returns. The thread may even run to completion and terminate before `pthread_create` returns. Refer to Section 8.1.1 for more information and warnings about what to expect when you create a thread.

### 2.2.2 Startup

Once a thread has been created, it will eventually begin executing machine instructions. The initial sequence of instructions will lead to the execution of the thread start/unction that you specified to `pthread_create`. The thread start function is called with the argument value you specified when you created the thread. In lifecycle.c, for example, the thread begins executing user code at function `thread_routine`, with the formal parameter argument having a value of NULL. In the initial thread, the thread "start function" (main) is called from outside your program; for example, many UNIX systems link your program with a file called crtO.o, which initializes the process and then calls your main. This is a minor implementation distinction, but it is important to remember because there are a few ways in which the initial thread is different. For one thing, main is called with different arguments than a thread start function: the program's argument array (argc and argv) instead of a single void\* argument. For another thing, when a thread start function returns, the thread terminates but other threads continue to run. When the function main returns in the initial thread, the process will be terminated immediately. If you want to terminate the initial thread while allowing other threads in the process to continue running, call `pthread_exit` instead of returning from main.

Another important difference to remember is that on most systems, the initial thread runs on the default process stack, which can grow to a substantial size. 'Thread" stacks may be much more limited on some implementations, and the program will fail with a segmentation fault or bus error if a thread overflows its stack.

### 2.2.3 Running and blocking

Like us, threads usually can't stay awake their entire life. Most threads occasionally go to sleep. A thread can go to sleep because it needs a resource that is not available (it becomes "blocked") or because the system reassigned the processor on which it was running (it is "preempted"). A thread spends most of its active life in three states: ready, running, and blocked.

A thread is ready when it is first created, and whenever it is unblocked so that it is once again eligible to run. Ready threads are waiting for a processor. Also, when a running thread is preempted, for example, if it is timesliced (because it has run too long), the thread immediately becomes ready.

A thread becomes running when it was ready and a processor selects the thread for execution. Usually this means that some other thread has blocked, or has been preempted by a timeslice-the blocking (or preempted) thread saves its context and restores the context of the next ready thread to replace itself. On a multiprocessor, however, a previously unused processor may execute a readied thread without any other thread blocking.

A thread becomes blocked when it attempts to lock a mutex that is currently locked, when it waits on a condition variable, when it calls sigwait for a signal that is not currently pending, or when it attempts an I/O operation that cannot be immediately completed. A thread may also become blocked for other system operations, such as a page fault.

When a thread is unblocked after a wait for some event, it is made ready again. It may execute immediately, for example, if a processor is available. In lifecycle.c, the main thread blocks at line 23, in `pthread_join`, to wait for the thread it created to run. If the thread had not already run at this point, it would move from ready to running when main becomes blocked. As the thread runs to completion and returns, the main thread will be unblocked-returning to the ready state. When processor resources are available, either immediately or after the thread becomes terminated, main will again become running, and complete.

### 2.2.4 Termination

A thread usually terminates by returning from its start function (the one you pass to the `pthread_create` function). The thread shown in lifecycle.c terminates by returning the value null, for example. Threads that call `pthread_exit` or that are cancelled using `pthread_cancel` also terminate after calling each cleanup handler that the thread registered by calling `pthread_cleanup_push` and that hasn't yet been removed by calling `pthread_cleanup_pop`. Cleanup handlers are discussed in Section 5.3.3.

Threads may have private "thread-specific data" values (thread-specific data is discussed in Section 5.4). If the thread has any non-NULL thread-specific data values, the associated destructor functions for those keys (if any) are called.

If the thread was already detached it moves immediately to the next section, recycling. Otherwise, the thread becomes terminated. It will remain available for another thread to join with it using `pthread_join`. This is analogous to a UNIX process that's terminated but hasn't yet been "reaped" by a wait operation. Sometimes it is called a "zombie" because it still exists even though it is "dead." A zombie may retain most or all of the system resources that it used when running, so it is not a good idea to leave threads in this state for longer than necessary. Whenever you create a thread with which you won't need to join, you should use the detachstate attribute to create it "detached" (see Section 5.2.3).

At a minimum, a terminated thread retains the identification (`pthread_t value`) and the `void *` return value that was returned from the thread's start function or specified in a call to `pthread_exit`. The only external difference between a thread that terminated "normally" by returning or calling pthreadexit, and one that terminated through cancellation, is that a cancelled thread's return value is always `pthread_cancelled`. (This is why "cancelled" is not considered a distinct thread state.)

If any other thread is waiting to join with the terminating thread, that thread is awakened. It will return from its call to `pthread_join` with the appropriate return value. Once `pthread_join` has extracted the return value, the terminated thread is detached by `pthread_join`, and may be recycled before the call to `pthread_join` returns. This means that, among other things, the returned value should never be a stack address associated with the terminated thread's stack-the value at that address could be overwritten by the time the caller could use it. In lif ecycle. c, the main thread will return from the `pthread_join` call at line 23 with the value NULL.

> `pthread_join` is a convenience, not a rule.

Even when you need a return value from a thread that you create, it is often at least as simple to create the thread detached and devise your own customized return mechanism as it is to use `pthread_join`. For example, if you pass information to a worker thread in some form of structure that another thread can find later, you might have the worker thread simply place the result in that same structure and broadcast a condition variable when done. The Pthreads context for the thread, including the thread identifier, can then be recycled immediately when the thread is done, and you still have the part you really need, the return value, where you can find it easily at any time.

If `pthread_join` does exactly what you want, then by all means use it. But remember that it is nothing more than a convenience for the simplest and most limited model of communicating a thread's results. If it does not do exactly what you need, build your own return mechanism instead of warping your design to fit the limitations of `pthread_join`.

### 2.2.5 Recycling

If the thread was created with the detachstate attribute set to `pthread_create_detached` (see Section 5.2.3), or if the thread or some other thread has already called `pthread_detach` for the thread's identifier, then the thread is immediately recycled when it becomes terminated.

If the thread has not been detached when it terminates, it remains in the terminated state until the thread's `pthread_t` identifier is passed to `pthread_detach` or `pthread_join`. When either function returns, the thread cannot be accessed again. In lifecycle.c, for example, the thread that had run `thread_routine` will be recycled by the time the main thread returns from the `pthread_join` call at line 23.

Recycling releases any system or process resources that weren't released at termination. That includes the storage used for the thread's return value, the stack, memory used to store register state, and so forth. Some of these resources may have been released at termination; it is important to remember that none of it should be accessed from any other thread after termination. For example, if a thread passes a pointer to its stack storage to another thread through shared data, you should treat that information as obsolete from the time the thread that owns the stack terminates.

