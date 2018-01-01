# 6 POSIX adjusts to threads
Lewis Carroll, Alice's Adventures in Wonderland:
> "Who are you ?" said the Caterpillar.  
> This was not an encouraging opening for a conversation.  
> Alice replied, rather shyly, "I-/ hardly know, Sir,  
> lust at present-at least I know who I was when / got up this morning, but  
> I think I must have been changed several times since then."  

Pthreads changes the meaning of a number of traditional POSIX process functions. Most of the changes are obvious, and you'd probably expect them even if the standard hadn't added specific wording. When a thread blocks for I/O, for example, only the calling thread blocks, while other threads in the process can continue to run.

But there's another class of POSIX functions that doesn't extend into the threaded world quite so unambiguously. For example, when you fork a threaded process, what happens to the threads? What does exec do in a threaded process? What happens when one of the threads in a threaded process calls exit?

## 6.1 fork
> Avoid using fork in a threaded program (if you can)  
> unless you intend to exec a new program immediately.  

When a threaded process calls fork to create a child process, Pthreads specifies that only the thread calling fork exists in the child. Although only the calling thread exists on return from fork in the child process, all other Pthreads states remain as they were at the time of the call to fork. In the child process, the thread has the same thread state as in the parent. It owns the same mutexes, has the same value for all thread-specific data keys, and so forth. All mutexes and condition variables exist, although any threads that were waiting on a synchronization object at the time of the fork are no longer waiting. (They don't exist in the child process, so how could they be waiting?)

Pthreads does not "terminate" the other threads in a forked process, as if they exited with `pthread_exit` or even as if they were canceled. They simply cease to exist. That is, the threads do not run thread-specific data destructors or cleanup handlers. This is not a problem if the child process is about to call exec to run a new program, but if you use fork to clone a threaded program, beware that you may lose access to memory, especially heap memory stored only as thread-specific data values.

> The state of mutexes is not affected by a fork. If it was locked in the parent it is locked in the child!

If a mutex was locked at the time of the call to fork, then it is still locked in the child. Because a locked mutex is owned by the thread that locked it, the mutex can be unlocked in the child only if the thread that locked the mutex was the one that called fork. This is important to remember-if another thread has a mutex locked when you call fork, you will lose access to that mutex and any data controlled by that mutex.

Despite the complications, you can fork a child that continues running and even continues to use Pthreads. You must use fork handlers carefully to protect your mutexes and the shared data that the mutexes are protecting. Fork handlers are described in Section 6.1.1.

Because thread-specific data destructors and cleanup handlers are not called, you may need to worry about memory leaks. One possible solution would be to cancel threads created by your subsystem in the prepare fork handler, and wait for them to terminate before allowing the fork to continue (by returning), and then create new threads in the parent handler that is called after fork completes. This could easily become messy, and I am not recommending it as a solution. Instead, take another look at the warning back at the beginning of this section: Avoid using fork in threaded code except where the child process will immediately exec a new program.

POSIX specifies a small set of functions that may be called safely from within signal-catching functions ("async-signal safe" functions), and fork is one of them. However, none of the POSIX threads functions is async-signal safe (and there are good reasons for this, because being async-signal safe generally makes a function substantially more expensive). With the introduction of fork handlers, however, a call to fork is also a call to some set of fork handlers.

The purpose of a fork handler is to allow threaded code to protect synchronization state and data invariants across a fork, and in most cases that requires locking mutexes. But you cannot lock mutexes from a signal-catching function. So while it is legal to call fork from within a signal-catching function, doing so may (beyond the control or knowledge of the caller) require performing other operations that cannot be performed within a signal-catching function.

This is an inconsistency in the POSIX standard that will need to be fixed. Nobody yet knows what the eventual solution will be. My advice is to avoid using fork in a signal-catching function.

### 6.1.1 Fork handlers
```c
int pthread_atfork (void (*prepare)(void),
    void (*parent)(void), void (*child)(void));
```
Pthreads added the `pthread_atfork` "fork handler" mechanism to allow your code to protect data invariants across fork. This is somewhat analogous to atexit, which allows a program to perform cleanup when a process terminates. With `pthread_atfork` you supply three separate handler addresses. The prepare fork handler is called before the fork takes place in the parent process. The parent fork handler is called after the fork in the parent process, and the child fork handler is called after the fork in the child process.

> If you write a subsystem that uses mutexes and does not establish fork handlers, then that subsystem will not function correctly in a child process after a fork.

Normally a prepare fork handler locks all mutexes used by the associated code (for a library or an application) in the correct order to prevent deadlocks. The thread calling fork will block in the prepare fork handler until it has locked all the mutexes. That ensures that no other threads can have the mutexes locked or be modifying data that the child might need. The parent fork handler need only unlock all of those mutexes, allowing the parent process and all threads to continue normally.

The child fork handler may often be the same as the parent fork handler; but sometimes you'll need to reset the program or library state. For example, if you use "daemon" threads to perform functions in the background you'll need to either record the fact that those threads no longer exist or create new threads to perform the same function in the child. You may need to reset counters, free heap memory, and so forth.

> Your fork handlers are only as good as everyone else's fork handlers.

The system will run all prepare fork handlers declared in the process when any thread calls fork. If you code your prepare and child fork handlers correctly then, in principle, you will be able to continue operating in the child process. But what if someone else didn't supply fork handlers or didn't do it right? The ANSI C library on a threaded system, for example, must use a set of mutexes to synchronize internal data, such as stdio file streams.

If you use an ANSI C library that doesn't supply fork handlers to prepare those mutexes properly for a fork, for example, then, sometimes, you may find that your child process hangs when it calls printf, because another thread in the parent process had the mutex locked when your thread called fork. There's often nothing you can do about this type of problem except to file a problem report against the system. These mutexes are usually private to the library, and aren't visible to your code-you can't lock them in your prepare handler or before calling fork.

The program atfork.c shows the use of fork handlers. When run with no argument, or with a nonzero argument, the program will install fork handlers. When run with a zero argument, such as atf ork 0, it will not.

With fork handlers installed, the result will be two output lines reporting the result of the fork call and, in parentheses, the pid of the current process. Without fork handlers, the child process will be created while the initial thread owns the mutex. Because the initial thread does not exist in the child, the mutex cannot be unlocked, and the child process will hang-only the parent process will print its message.

Function `fork_prepare` is the prepare handler. This will be called by fork, in the parent process, before creating the child process. Any state changed by this function, in particular, mutexes that are locked, will be copied into the child process. The `fork_prepare` function locks the program's mutex.

Function `fork_parent` is the parent handler. This will be called by fork, in the parent process, after creating the child process. In general, a parent handler should undo whatever was done in the prepare handler, so that the parent process can continue normally. The `fork_parent` function unlocks the mutex that was locked by `fork_prepare`.

Function `fork_child` is the child handler. This will be called by fork, in the child process. In most cases, the child handler will need to do whatever was done in the `fork_parent` handler to "unlock" the state so that the child can continue. It may also need to perform additional cleanup, for example, `fork_child` sets the `self_pid` variable to the child process's pid as well as unlocking the process mutex.

After creating a child process, which will continue executing the `thread_routine` code, the `thread_routine` function locks the mutex. When run with fork handlers, the fork call will be blocked (when the prepare handler locks the mutex) until the mutex is available. Without fork handlers, the thread will fork before main unlocks the mutex, and the thread will hang in the child at this point. 99-106 The main program declares fork handlers unless the program is run with an argument of 0.

The main program locks the mutex before creating the thread that will fork. It then sleeps for several seconds, to ensure that the thread will be able to call fork while the mutex is locked, and then unlocks the mutex. The thread running `thread_routine` will always succeed in the parent process, because it will simply block until main releases the lock.

However, without the fork handlers, the child process will be created while the mutex is locked. The thread (main) that locked the mutex does not exist in the child, and cannot unlock the mutex in the child process. Mutexes can be unlocked in the child only if they were locked by the thread that called fork-and fork handlers provide the best way to ensure that.

```c
/*  atfork.c  */
#include <sys/types.h>
#include <pthread.h>
#include <sys/wait.h>
#include "errors.h"

pid_t self_pid; /* pid of current process */
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

/*
 * This routine will be called prior to executing the fork,
 * within the parent process.
 */
void fork_prepare (void)
{
    int status;

    /*
     * Lock the mutex in the parent before creating the child,
     * to ensure that no other thread can lock it (or change any
     * associated shared state) until after the fork completes.
     */
    status = pthread_mutex_lock (&mutex);
    if (status != 0)
        err_abort (status, "Lock in prepare handler");
}

/*
 * This routine will be called after executing the fork, within
 * the parent process
 */
void fork_parent (void)
{
    int status;

    /*
     * Unlock the mutex in the parent after the child has been
     * created.
     */
    status = pthread_mutex_unlock (&mutex);
    if (status != 0)
        err_abort (status, "Unlock in parent handler");
}

/*
 * This routine will be called after executing the fork, within
 * the child process.
 */
void fork_child (void)
{
    int status;

    /*
     * Update the file scope "self_pid" within the child process, and
     * unlock the mutex.
     */
    self_pid = getpid ();
    status = pthread_mutex_unlock (&mutex);
    if (status != 0)
        err_abort (status, "Unlock in child handler");
}

/*
 * Thread start routine, which will fork a new child process.
 */
void *thread_routine (void *arg)
{
    pid_t child_pid;
    int status;

    child_pid = fork ();
    if (child_pid == (pid_t)-1)
        errno_abort ("Fork");

    /*
     * Lock the mutex - without the atfork handlers, the mutex will
     * remain locked in the child process and this lock attempt will
     * hang (or fail with EDEADLK) in the child.
     */
    status = pthread_mutex_lock (&mutex);
    if (status != 0)
        err_abort (status, "Lock in child");
    status = pthread_mutex_unlock (&mutex);
    if (status != 0)
        err_abort (status, "Unlock in child");
    printf ("After fork: %d (%d)\n", child_pid, self_pid);
    if (child_pid != 0) {
        if ((pid_t)-1 == waitpid (child_pid, (int*)0, 0))
            errno_abort ("Wait for child");
    }
    return NULL;
}

int main (int argc, char *argv[])
{
    pthread_t fork_thread;
    int atfork_flag = 1;
    int status;

    if (argc > 1)
        atfork_flag = atoi (argv[1]);
    if (atfork_flag) {
        status = pthread_atfork (
            fork_prepare, fork_parent, fork_child);
        if (status != 0)
            err_abort (status, "Register fork handlers");
    }
    self_pid = getpid ( );
    status = pthread_mutex_lock (&mutex);
    if (status != 0)
        err_abort (status, "Lock mutex");
    /*
     * Create a thread while the mutex is locked. It will fork a
     * process, which (without atfork handlers) will run with the
     * mutex locked.
     */
    status = pthread_create (
        &fork_thread, NULL, thread_routine, NULL);
    if (status != 0)
        err_abort (status, "Create thread");
    sleep (5);
    status = pthread_mutex_unlock (&mutex);
    if (status != 0)
        err_abort (status, "Unlock mutex");
    status = pthread_join (fork_thread, NULL);
    if (status != 0)
        err_abort (status, "Join thread");
    return 0;
}
```

Now, imagine you are writing a library that manages network server connections, and you create a thread for each network connection that listens for service requests. In your prepare fork handler you lock all of the library's mutexes to make sure the child's state is consistent and recoverable. In your parent fork handler you unlock those mutexes and return. When designing the child fork handler, you need to decide exactly what a fork means to your library. If you want to retain all network connections in the child, then you would create a new listener thread for each connection and record their identifiers in the appropriate data structures before releasing the mutexes. If you want the child to begin with no open connections, then you would locate the existing parent connection data structures and free them, closing the associated files that were propagated by fork.

## 6.2 exec
The exec function isn't affected much by the presence of threads. The function of exec is to wipe out the current program context and replace it with a new program. A call to exec immediately terminates all threads in the process except the thread calling exec. They do not execute cleanup handlers or thread-specific data destructors-the threads simply cease to exist.

All synchronization objects also vanish, except for pshared mutexes (mutexes created using the `pthread_process_shared` attribute value) and pshared condition variables, which remain usable as long as the shared memory is mapped by some process. You should, however, unlock any pshared mutexes that the current process may have locked-the system will not unlock them for you.

## 6.3 Process exit
In a nonthreaded program, an explicit call to the exit function has the same effect as returning from the program's main function. The process exits. Pthreads adds the `pthread_exit` function, which can be used to cause a single thread to exit while the process continues. In a threaded program, therefore, you call exit when you want the process to exit, or `pthread_exit` when you want only the calling thread to exit.

In a threaded program, main is effectively the "thread start function" for the process's initial thread. Although returning from the start function of any other thread terminates that thread just as if it had called `pthread_exit`, returning from main terminates the process. All memory (and threads) associated with the process evaporate. Threads do not run cleanup handlers or thread-specific data destructors. Calling exit has the same effect.

When you don't want to make use of the initial thread or make it wait for other threads to complete, you can exit from main by calling `pthread_exit` rather than by returning or calling exit. Calling `pthread_exit` from main will terminate the initial thread without affecting the other threads in the process, allowing them to continue and complete normally.

The exit function provides a simple way to shut down the entire process. For example, if a thread determines that data has been severely corrupted by some error, it may be dangerous to allow the program to continue to operate on the data. When the program is somehow broken, it might be dangerous to attempt to shut down the application threads cleanly. In that case, you might call exit to stop all processing immediately.

## 6.4 Stdio
Pthreads specifies that the ANSI C standard I/O [stdio) functions are thread-safe. Because the stdio package requires static storage for output buffers and file state, stdio implementations will use synchronization, such as mutexes or semaphores.

### 6.4.1 flockfile and funlockfile
```c
void flockfile (FILE *file);
int ftrylockfile (FILE *file);
void funlockfile (FILE *file);
```
In some cases, it is important that a sequence of stdio operations occur in uninterrupted sequence; for example, a prompt followed by a read from the terminal, or two writes that need to appear together in the output file even if another thread attempts to write data between the two stdio calls. Therefore, Pthreads adds a mechanism to lock a file and specifies how file locking interacts with internal stdio locking. To write a prompt string to stdin and read a response from stdout without allowing another thread to read from stdin or write to stdout between the two, you would need to lock both stdin and stdout around the two calls as shown in the following program, flock.c.

This is the important part: Two separate calls to flockfile are made, one for each of the two file streams. To avoid possible deadlock problems within stdio, Pthreads recommends always locking input streams before output streams, when you must lock both. That's good advice, and I've taken it by locking stdin before stdout.

The two calls to funlockfile must, of course, be made in the opposite order. Despite the specialized call, you are effectively locking mutexes within the stdio library, and you should respect a consistent lock hierarchy.

```c
/*  flock.c  */
#include <pthread.h>
#include "errors.h"

/*
 * This routine writes a prompt to stdout (passed as the thread's
 * "arg"), and reads a response. All other I/O to stdin and stdout
 * is prevented by the file locks until both prompt and fgets are
 * complete.
 */
void *prompt_routine (void *arg)
{
    char *prompt = (char*)arg;
    char *string;
    int len;

    string = (char*)malloc (128);
    if (string == NULL)
        errno_abort ("Alloc string");
    flockfile (stdin);
    flockfile (stdout);
    printf (prompt);
    if (fgets (string, 128, stdin) == NULL)
        string[0] = '\0';
    else {
        len = strlen (string);
        if (len > 0 && string[len-1] == '\n')
            string[len-1] = '\0';
    }
    funlockfile (stdout);
    funlockfile (stdin);
    return (void*)string;
}

int main (int argc, char *argv[])
{
    pthread_t thread1, thread2, thread3;
    char *string;
    int status;

#ifdef sun
    /*
     * On Solaris 2.5, threads are not timesliced. To ensure
     * that our threads can run concurrently, we need to
     * increase the concurrency level.
     */
    DPRINTF (("Setting concurrency level to 4\n"));
    thr_setconcurrency (4);
#endif
    status = pthread_create (
        &thread1, NULL, prompt_routine, "Thread 1> ");
    if (status != 0)
        err_abort (status, "Create thread");
    status = pthread_create (
        &thread2, NULL, prompt_routine, "Thread 2> ");
    if (status != 0)
        err_abort (status, "Create thread");
    status = pthread_create (
        &thread3, NULL, prompt_routine, "Thread 3> ");
    if (status != 0)
        err_abort (status, "Create thread");
    status = pthread_join (thread1, (void**)&string);
    if (status != 0)
        err_abort (status, "Join thread");
    printf ("Thread 1: \"%s\"\n", string);
    free (string);
    status = pthread join (thread2, (void**)&string);
    if (status != 0)
        err_abort (status, "Join thread");
    printf ("Thread 1: \"%s\"\n", string);
    free (string);
    status = pthread_join (thread3, (void**)&string);
    if (status != 0)
        err_abort (status, "Join thread");
    printf ("Thread 1: \"%s\"\n", string);
    free (string);
    return 0;
}
```

You can also use the flockfile and funlockfile functions to ensure that a series of writes is not interrupted by a file access from some other thread. The f trylockf ile function works like `pthread_mutex_trylock` in that it attempts to lock the file and, if the file is already locked, returns an error status instead of blocking.

### 6.4.2 `getchar_unlocked` and `putchar_unlocked`
```c
int** getc_unlocked (FILE *stream);
int getchar_unlocked (void);
int putc_unlocked (int c, FILE *stream);
int putchar_unlocked (int c);
```

ANSI C provides functions to get and put single characters efficiently into stdio buffers. The functions getchar and putchar operate on stdin and stdout, respectively, and getc and putc can be used on any stdio file stream. These are traditionally implemented as macros for maximum performance, directly reading or writing the file stream's data buffer. Pthreads, however, requires these functions to lock the stdio stream data, to prevent code from accidentally corrupting the stdio buffers.

The overhead of locking and unlocking mutexes will probably vastly exceed the time spent performing the character copy, so these functions are no longer high performance. Pthreads could have denned new functions that provided the locked variety rather than redefining the existing functions; however, the result would be that existing code would be unsafe for use in threads. The working group decided that it was preferable to make existing code slower, rather than to make it incorrect.

Pthreads adds new functions that replace the old high-performance macros with essentially the same implementation as the traditional macros. The functions `getc_unlocked`, `putc_unlocked`, `getchar_unlocked`, and `putchar_unlocked` do not perform any locking, so you must use flockfile and funlockfile around any sequence of these operations. If you want to read or write a single character, you should usually use the locked variety rather than locking the file stream, calling the new unlocked get or put function, and then unlocking the file stream.

If you want to perform a sequence of fast character accesses, where you would have previously used getchar and putchar, you can now use `getchar_unlocked` and `putchar_unlocked`. The following program, putchar.c, shows the difference between using putchar and using a sequence of `putchar_unlocked` calls within a file lock.

When the program is run with a nonzero argument or no argument at all, it creates threads running the `lock_routine` function. This function locks the stdout file stream, and then writes its argument (a string) to stdout one character at a time using `putchar_unlocked`.

When the program is run with a zero argument, it creates threads running the `unlock_routine` function. This function writes its argument to stdout one character at a time using putchar. Although putchar is internally synchronized to ensure that the stdio buffer is not corrupted, the individual characters may appear in any order.

```c
/*  putchar.c  */
#include <pthread.h>
#include "errors.h"

/*
 * This function writes a string (the function's arg) to stdout,
 * by locking the file stream and using putchar_unlocked to write
 * each character individually.
 */
void *lock_routine (void *arg)
{
    char *pointer;

    flockfile (stdout);
    for (pointer = arg; *pointer != '\0'; pointer++) {
        putchar_unlocked (*pointer);
        sleep (1);
    }
    funlockfile (stdout);
    return NULL;
}

/*
 * This function writes a string (the function's arg) to stdout,
 * by using putchar to write each character individually.
 * Although the internal locking of putchar prevents file stream
 * corruption, the writes of various threads may be interleaved.
 */
void *unlock routine (void *arg)
{
    char *pointer;

    for (pointer = arg; *pointer != '\0'; pointer++) {
        putchar (*pointer);
        sleep (1);
    }
    return NULL;
}

int main (int argc, char *argv[])
{
    pthread_t thread1, thread2, thread3;
    int flock_flag = 1;
    void *(*thread_func)(void *);
    int status;

    if (argc > 1)
        flock_flag = atoi (argv[1]);
    if (flock_flag)
        thread_func = lock_routine;
    else
        thread_func = unlock_routine;
    status = pthread_create (
        &thread1, NULL, thread_func, "this is thread l\n");
    if (status != 0)
        err_abort (status, "Create thread");
    status = pthread_create (
        &thread2, NULL, thread_func, "this is thread 2\n");
    if (status != 0)
        err_abort (status, "Create thread");
    status = pthread_create (
        &thread3, NULL, thread_func, "this is thread 3\n");
    if (status != 0)
        err_abort (status, "Create thread");
    pthread_exit (NULL);
}
```
## 6.5 Thread-safe functions
Although ANSI C and POSIX 1003.1-1990 were not developed with threads in mind, most of the functions they define can be made thread-safe without changing the external interface. For example, although malloc and free must be changed to support threads, code calling these functions need not be aware of the changes. When you call malloc, it locks a mutex (or perhaps several mutexes) to perform the operation, or may use other equivalent synchronization mechanisms. But your code just calls malloc as it always has, and it does the same thing as always.

In two main classes of functions, this is not true:
- Functions that traditionally return pointers to internal static buffers, for example, asctime. An internal mutex wouldn't help, since the caller will read the formatted time string some time after the function returns and, therefore, after the mutex has been unlocked.
- Functions that require static context between a series of calls, for example, strtok, which stores the current position within the token string in a local static variable. Again, using a mutex within strtok wouldn't help, because other threads would be able to overwrite the current location between two calls.

In these cases, Pthreads has defined variants of the existing functions that are thread-safe, which are designated by the suffix "\_r" at the end of the function name. These variants move context outside the library, under the caller's control. When each thread uses a private buffer or context, the functions are thread-safe. You can also share context between threads if you want-but the caller must provide synchronization between the threads. If you want two threads to search a directory in parallel, you must synchronize their use of the shared struct dirent passed to `readdir_r`.

A few existing functions, such as ctermid, are already thread-safe as long as certain restrictions are placed on parameters. These restrictions are noted in the following sections.

### 6.5.1 User and terminal identification
```c
int getlogin_r (char *name, size_t namesize);
char *ctermid (char *s);
int ttyname_r (int fildes,
    char *name, size t namesize);
```
These functions return data to a caller-specified buffer. For `getlogin_r`, namesize must be at least `LOGIN_NAME_MAX` characters. For `ttyname_r`, namesize must be at least `tty_name_max` characters. Either function returns a value of 0 on success, or an error number on failure. In addition to errors that might be returned by getlogin or ttyname, `getlogin_r` and `ttyname_r` may return ERANGE to indicate that the name buffer is too small.

Pthreads requires that when ctermid (which has not changed) is used in a threaded environment, the s return argument must be specified as a pointer to a character buffer having at least `L_ctermid` bytes. It was felt that this restriction was sufficient, without defining a new variant to also specify the size of the buffer.

Program getlogin.c shows how to call these functions. Notice that these functions do not depend on threads, or `<pthread.h>`, in any way, and may even be provided on systems that don't support threads.

```c
/*  getlogin.c  */
#include <limits.h>
#include "errors.h"

/*
 * If either TTY_NAME_MAX or LOGIN_NAME_MAX are undefined
 * (this means they are "indeterminate" values), assume a
 * reasonable size (for simplicity) rather than using sysconf
 * and dynamically allocating the buffers.
 */
#ifndef TTY_NAME_MAX
# define TTY_NAME_MAX       128
#endif
#ifndef LOGIN_NAME_MAX
# define LOGIN_NAME_MAX     32
#endif

int main (int argc, char *argv[])
{
    char login_str[LOGIN_NAME_MAX];
    char stdin_str[TTY_NAME_MAX];
    char cterm_str[L_ctermid], *cterm_str_ptr;
    int status;

    status = getlogin_r (login_str, sizeof (login_str));
    if (status != 0)
        err_abort (status, "Get login");
    cterm_str_ptr = ctermid (cterm_str);
    if (cterm_str_ptr == NULL)
        errno_abort ("Get cterm");
    status = ttyname_r (0, stdin_str, sizeof (stdin_str));
    if (status != 0)
        err_abort (status, "Get stdin");
    printf ("User: %s, cterm: %s, fd 0: %s\n",
        login_str, cterm_str, stdin_str);
    return 0;
}
```
### 6.5.2 Directory searching
```c
int readdir_r (DIR *dirp, struct dirent *entry,
    struct dirent **result);
```
This function performs essentially the same action as readdir. That is, it returns the next directory entry in the directory stream specified by dirp. The difference is that instead of returning a pointer to that entry, it copies the entry into the buffer specified by entry. On success, it returns 0 and sets the pointer specified by result to the buffer entry. On reaching the end of the directory stream, it returns 0 and sets result to null. On failure, it returns an error number such as EBADF.

Refer to program pipe. c, in Section 4.1, for a demonstration of using `readdir_r` to allow your threads to search multiple directories concurrently.

### 6.5.3 String token
```c
char *strtok_r (
    char *s, const char *sep, char **lasts);
```
This function returns the next token in the string s. Unlike strtok, the context (the current pointer within the original string) is maintained in lasts, which is specified by the caller, rather than in a static pointer internal to the function. In the first call of a series, the argument s gives a pointer to the string. In subsequent calls to return successive tokens of that string, s must be specified as NULL. The value lasts is set by `strtok_r` to maintain the function's position within the string, and on each subsequent call you must return that same value of lasts. The `strtok_r` function returns a pointer to the next token, or NULL when there are no more tokens to be found in the original string.

### 6.5.4 Time representation
```c
char *asctime_r (const struct tm *tm, char *buf);
char *ctime_r (const time_t *clock, char *buf);
struct tm *gmtime_r (
    const time_t *clock, struct tm *result);
struct tm *localtime_r (
    const time t *clock, struct tm *result);
```
The output buffers (buf and result) are supplied by the caller, instead of returning a pointer to static storage internal to the functions. Otherwise, they are identical to the traditional variants. The `asctime_r` and `ctime_r` routines, which return ASCII character representations of a system time, both require that their buf argument point to a character string of at least 26 bytes.

### 6.5.5 Random number generation
```c
int rand_r (unsigned int *seed);
```
The seed is maintained in caller-supplied storage (seed) rather than using static storage internal to the function. The main problem with this interface is that it is not usually practical to have a single seed shared by all application and library code within a program. As a result, the application and each library will generally have a separate "stream" of random numbers. Thus, a program converted to use `rand_r` instead of rand is likely to generate different results, even if no threads are created. (Creating threads would probably change the order of calls to rand, and therefore change results anyway.)

### 6.5.6 Group and user database
Group database:
```c
int getgrgid_r (
    gid_t gid, struct group *grp, char *buffer,
    size_t bufsize, struct group **result);
int getgrnam_r (
    const char *name, struct group *grp,
    char *buffer, size_t bufsize,
    struct group **result);
```
User database:
```c
int getpwuid_r (
    uid_t uid, struct passwd *pwd, char *buffer,
    size_t bufsize, struct passwd **result);
int getpwnam_r (
    const char *name, struct passwd *pwd,
    char *buffer, size_t bufsize,
    struct passwd **result);
```
These functions store a copy of the group or user record (grp or pwd, respectively) for the specified group or user (gid, uid, or name) in a buffer designated by the arguments buffer and bufsize. The function return value is in each case either 0 for success, or an error number (such as erange when the buffer is too small) to designate an error. If the requested record is not present in the group or passwd database, the functions may return success but store the value null into the result pointer. If the record is found and the buffer is large enough, result becomes a pointer to the struct group or struct passwd record within buffer.

The maximum required size for buffer can be determined by calling sysconf with the argument `_SC_GETGR_R_SIZE_MAX` (for group data) or with the argument `_SC_GETPW_R_SIZE_MAX` (for user data).

## 6.6 Signals
Lewis Carroll, Through the Looking-Glass:
> Beware the Jabberwock, my son!  
> The jaws that bite, the claws that catch!  
> Beware the Jubjub bird, and shun  
> The frumious Bandersnatch!  

The history of the Pthreads signal-handling model is the most convoluted and confusing part of the standard. There were several different viewpoints, and it was difficult to devise a compromise that would satisfy everyone in the working group (much less the larger and more diverse balloting group). This isn't surprising, since signals are complicated anyway, and have a widely divergent history in the industry.

There were two primary conflicting goals:
- First, "signals should be completely compatible with traditional UNIX." That means signal handlers and masks should remain associated with the process. That makes them virtually useless with multiple threads, which is as it should be since signals have complicating semantics that make it difficult for signals and threads to coexist peacefully. Tasks should be accomplished synchronously using threads rather than asynchronously using signals.
- Second, "signals should be completely compatible with traditional UNIX." This time, "compatible" means signal handlers and masks should be completely thread-private. Most existing UNIX code would then function essentially the same running within a thread as it had within a process. Code migration would be simplified.

The problem is that the definitions of "compatible" were incompatible. Although many people involved in the negotiation may not agree with the final result, nearly everyone would agree that those who devised the compromise did an extraordinarily good job, and that they were quite courageous to attempt the feat.

> When writing threaded code, treat signals as Jabberwocks-curious and potentially dangerous creatures to be approached with caution, if at all.

It is always best to avoid using signals in conjunction with threads. At the same time, it is often not possible or practical to keep them separate. When signals and threads meet, beware. If at all possible, use only `pthread_sigmask` to mask signals in the main thread, and `sigwait` to handle signals synchronously within a single thread dedicated to that purpose. If you must use sigaction (or equivalent) to handle synchronous signals (such as sigsegv) within threads, be especially cautious. Do as little work as possible within the signal-catching function.

### 6.6.1 Signal actions
All signal actions are process-wide. A program must coordinate any use of sigaction between threads. This is nonmodular, but also relatively simple, and signals have never been modular. A function that dynamically modifies signal actions, for example, to catch or ignore sigfpe while it performs floating-point operations, or sigpipe while it performs network I/O, will be tricky to code on a threaded system.

While modifying the process signal action for a signal number is itself thread-safe, there is no protection against some other thread setting a new signal action immediately afterward. Even if the code tries to be "good" by saving the original signal action and restoring it, it may be foiled by another thread, as shown in Figure 6.1.

Signals that are not "tied" to a specific hardware execution context are delivered to one arbitrary thread within the process. That means a sigchld raised by a child process termination, for example, may not be delivered to the thread that created the child. Similarly, a call to kill results in a signal that may be delivered to any thread.

Thread 1          | Thread 2          | Comments
 ---              | ---               | ---
`sigaction(SIGFPE)` |                   | Thread1's signal action active.
                  | `sigaction(SIGFPE)` | Thread 2's signal action active.
Generate `SIGFPE`   |                   | Thread 1 signal is handled by the thread 2 signal action (but still in the context of thread 1).
Restore action    |                   | Thread 1 restores original signal action.
                  | restore action    | Thread 2 restores thread 1 's signal action-origina action is lost.

<center>**FIGURE 6.1** *Nonmodularity of signal actions*</center>

The synchronous "hardware context" signals, including `SIGFPE`, `SIGSEGV`, and `SIGTRAP`, are delivered to the thread that caused the hardware condition, never to another thread.

> You cannot kill a thread by sending it a `SIGKILL` or stop a thread by sending it a `SIGSTOR`

Any signal that affected a process still affects the process when multiple threads are active, which means that sending a `SIGKILL` to a process or to any specific thread in the process (using `pthread_kill`, which we'll get to in Section 6.6.3) will terminate the process. Sending a `SIGSTOP` will cause all threads to stop until a `SIGCONT` is received. This ensures that existing process control functions continue to work-otherwise most threads in a process could continue running when you stopped a command by sending a sigstop. This also applies to the default action of the other signals, for example, `SIGSEGV`, if not handled, will terminate the process and generate a core file-it will not terminate only the thread that generated the `SIGSEGV`.

What does this mean to a programmer? It has always been common wisdom that library code should not change signal actions-that this is exclusively the province of the main program. This philosophy becomes even more wise when you are programming with threads. Signal actions must always be under the control of a single component, at least, and to assign that responsibility to the main program makes the most sense in nearly all situations.

### 6.6.2 Signal masks
```c
int pthread_sigmask (int how,
const sigset_t *set, sigset_t *oset);
```
Each thread has its own private signal mask, which is modified by calling `pthread_sigmask`. Pthreads does not specify what sigprocmask does within a threaded process-it may do nothing. Portable threaded code does not call sigprocmask. A thread can block or unblock signals without affecting the ability of other threads to handle the signal. This is particularly important for synchronous signals. It would be awkward if thread A were unable to process a sigfpe because thread B was currently processing its own sigfpe or, even worse, because thread C had blocked sigfpe. When a thread is created, it inherits the signal mask of the thread that created it-if you want a signal to be masked everywhere, mask it first thing in main.

### 6.6.3 pthread_kill
```c
int pthread_kill (pthread_t thread, int sig);
```
Within a process, one thread can send a signal to a specific thread (including itself) by calling `pthread_kill`. When calling `pthread_kill`, you specify not only the signal number to be delivered, but also the `pthread_t` identifier for the thread to which you want the signal sent. You cannot use `pthread_kill` to send a signal to a thread in another process, however, because a thread identifier (`pthread_t`) is meaningful only within the process that created it.

The signal sent by `pthread_kill` is handled like any other signal. If the "target" thread has the signal masked, it will be marked pending against that thread. If the thread is waiting for the signal in `sigwait` (covered in Section 6.6.4), the thread will receive the signal. If the thread does not have the signal masked, and is not blocked in `sigwait`, the current signal action will be taken.

Remember that, aside from signal-catching functions, signal actions affect the process. Sending the `SIGKILL` signal to a specific thread using `pthread_kill` will kill the process, not just the specified thread. Use `pthread_cancel` to get rid of a particular thread (see Section 5.3). Sending sigstop to a thread will stop all threads in the process until a `SIGCONT` is sent by some other process.

The raise function specified by ANSI C has traditionally been mapped to a kill for the current process. That is, raise (`SIGABRT`) is usually the same as kill (`getpid ( )`, `SIGABRT`).

With multiple threads, code calling raise is most likely to intend that the signal be sent to the calling thread, rather than to some arbitrary thread within the process. Pthreads specifies that raise (`SIGABRT`) is the same as `pthread_kill` (`pthread_self ()`, `SIGABRT`).

The following program, susp.c, uses `pthread_kill` to implement a portable "suspend and resume" (or, equivalently, "suspend and continue") capability much like that provided by the Solaris "UI threads" interfaces `thr_suspend` and `thr_continue`(see **Hint** below). You call the `thd_suspend` function with the `pthread_t` of a thread, and when the function returns, the specified thread has been suspended from execution. The thread cannot execute until a later call to `thd_continue` is made with the same `pthread_t`.

> **Hint:**  
> The algorithm (and most of the code) for susp.c was developed by a coworker of mine, Brian Silver. The code shown here is a simplified version for demonstration purposes.  

A request to suspend a thread that is already suspended has no effect. Calling `thd_continue` a single time for a suspended thread will cause it to resume execution, even if it had been suspended by multiple calls to `thd_suspend`. Calling `thd_continue` for a thread that is not currently suspended has no effect.

Suspend and resume are commonly used to solve some problems, for example, multithread garbage collectors, and may even work sometimes if the programmer is very careful. This emulation of suspend and resume may therefore be valuable to the few programmers who really need these functions. Beware, however, that should you suspend a thread while it holds some resource (such as a mutex), application deadlock can easily result.

The symbol ITERATIONS defines how many times the "target" threads will loop. If this value is set too small, some or all of the threads will terminate before the main thread has been able to suspend and continue them as it desires. If that happens, the program will fail with an error message-increase the value of iterations until the problem goes away.

The variable sentinel is used to synchronize between a signal-catching function and another thread. "Oh?" you may ask, incredulously. This mechanism is not perfect-the suspending thread (the one calling `thd_suspend`) waits in a loop, yielding the processor until this sentinel changes state. The volatile storage attribute ensures that the signal-catching function will write the value to memory.(see **Hint** below) Remember, you cannot use a mutex within a signal-catching function.

> **Hint:**  
> A semaphore, as described later in Section 6.6.6, would provide cleaner, and somewhat safer, synchronization. The `thd_suspend` would call `sem_wait` on a semaphore with an initial value of 0, and the signal-catching function would call `sem_post` to wake it.  

The `suspend_signal_handler` function will be established as the signal-catching function for the "suspend" signal, `SIGUSR1.` It initializes a signal mask to block all signals except `SIGUSR2`, which is the "resume" signal, and then waits for that signal by calling sigsuspend. Just before suspending itself, it sets the sentinel variable to inform the suspending thread that it is no longer executing user code-for most practical purposes, it is already suspended.

The purpose for this synchronization between the signal-catching function and `thd_suspend` is that, to be most useful, the thread calling `thd_suspend` must be able to know when the target thread has been successfully suspended. Simply calling `pthread_kill` is not enough, because the system might not deliver the signal for a substantial period of time; we need to know when the signal has been received.

The `resume_signal_handler` function will be established as the signal-catching function for the "resume" signal, sigusri. The function isn't important, since the signal is sent only to interrupt the call to sigsuspend in `suspend_signal_handler`.

```c
/*  susp.c part 1 signal-catching functions  */
#include <pthread.h>
#include <signal.h>
#include "errors.h"

#define THREAD_COUNT    20
#define ITERATIONS      40000

unsigned long thread_count = THREAD_COUNT;
unsigned long iterations = ITERATIONS;
pthread_mutex_t the_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
volatile int sentinel = 0;
pthread_once_t once = PTHREAD_ONCE_INIT;
pthread_t *array = NULL, null_pthread = {0};
int bottom = 0;
int inited = 0;

/*
 * Handle SIGUSR1 in the target thread, to suspend it until
 * receiving SIGUSR2 (resume).
 */
void
suspend_signal_handler (int sig)
{
    sigset_t signal_set;

    /*
     * Block all signals except SIGUSR2 while suspended.
     */
    sigfillset (&signal_set);
    sigdelset (&signal_set, SIGUSR2);
    sentinel = 1;
    sigsuspend (&signal_set);

    /*
     * Once I'm here, I've been resumed, and the resume signal
     * handler has been run to completion.
     */
    return;
}

/*
 * Handle SIGUSR2 in the target thread, to resume it. Note that
 * the signal handler does nothing. It exists only because we need
 * to cause sigsuspend() to return.
 */
void
resume_signal_handler (int sig)
{
    return;
}
```
The `suspend_init_routine` function dynamically initializes the suspend/ resume package when the first call to `thd_suspend` is made. It is actually called indirectly by `pthread_once`.

It allocates an initial array of thread identifiers, which is used to record the identifiers of all threads that have been suspended. This array is used to ensure that multiple calls to `thd_suspend` have no additional effect on the target thread, and that calling `thd_continue` for a thread that is not suspended has no effect.

It sets up signal actions for the `SIGUSRI` and `SIGUSR2` signals, which will be used, respectively, to suspend and resume threads.

```c
/*  susp.c part 2 initialization  */
/*
 * Dynamically initialize the "suspend package" when first used
 * (called by pthread_once).
 */
void
suspend_init_routine (void)
{
    int status;
    struct sigaction sigusr1, sigusr2;

    /*
     * Allocate the suspended threads array. This array is used
     * to guarentee idempotency
     */
    bottom = 10;
    array = (pthread_t*) calloc (bottom, sizeof (pthread_t));

    /*
     * Install the signal handlers for suspend/resume.
     */
    sigusr1.sa_flags = 0;
    sigusr1.sa_handler = suspend_signal_handler;

    sigemptyset (&sigusr1.sa_mask);
    sigusr2.sa_flags = 0;
    sigusr2.sa_handler = resume_signal_handler;
    sigusr2.sa_mask = sigusr1.sa_mask;

    status = sigaction (SIGUSRI, &sigusr1, NULL);
    if (status == -1)
        errno_abort ("Installing suspend handler");

    status = sigaction (SIGUSR2, &sigusr2, NULL);
    if (status == -1)
        errno abort ("Installing resume handler");

    inited = 1;
    return;
}
```
The `thd_suspend` function suspends a thread, and returns when that thread has ceased to execute user code. It first ensures that the suspend/resume package is initialized by calling `pthread_once`. Under protection of a mutex, it searches for the target thread's identifier in the array of suspended thread identifiers. If the thread is already suspended, `thd_suspend` returns successfully.

Determine whether there is an empty entry in the array of suspended threads and, if not, realloc the array with an extra entry.

The sentinel variable is initialized to 0, to detect when the target thread suspension occurs. The thread is sent a `SIGUSR1` signal by calling `pthread_kill`, and `thd_suspend` loops, calling `sched_yield` to avoid monopolizing a processor, until the target thread responds by setting sentinel. Finally, the suspended thread's identifier is stored in the array.

```c
/*  susp.c part 3 thd_suspend  */
/*
 * Suspend a thread by sending it a signal (SIGUSR1), which will
 * block the thread until another signal (SIGUSR2) arrives.
 *
 * Multiple calls to thd_suspend for a single thread have no
 * additional effect on the thread a single thd_continue
 * call will cause it to resume execution.
 */
int
thd_suspend (pthread_t target_thread)
{
    int status;
    int i = 0;

    /*
     * The first call to thd_suspend will initialize the
     * package.
     */
    status = pthread_once (&once, suspend_init_routine);
    if (status != 0)
        return status;

    /*
     * Serialize access to suspend, makes life easier.
     */
    status = pthread_mutex_lock (&mut);
    if (status != 0)
        return status;

    /*
     * Threads that are suspended are added to the target_array;
     * a request to suspend a thread already listed in the array
     * is ignored. Sending a second SIGUSR1 would cause the
     * thread to resuspend itself as soon as it is resumed.
     */
    while (i < bottom)
        if (array[i++] == target_thread) {
            status = pthread_mutex_unlock (&mut);
            return status;
        }

    /*
     * Ok, we really need to suspend this thread. So, let's find
     * the location in the array that we'll use. If we run off
     * the end, realloc the array for more space.
     */
    i = 0;
    while (array[i] != 0)
        i++;

    if (i == bottom) {
        array = (pthread_t*) realloc (
            array, (++bottom * sizeof (pthread_t)));
        if (array == NULL) {
            pthread_mutex_unlock (&mut);
            return errno;
        }

        array[bottom] = null_pthread;       /* Clear new entry */
    }

    /*
     * Clear the sentinel and signal the thread to suspend.
     */
    sentinel = 0;
    status = pthread_kill (target_thread, SIGUSR1);
    if (status != 0) {
        pthread_mutex_unlock (&mut);
        return status;
    }

    /*
     * Wait for the sentinel to change.
     */
    while (sentinel == 0)
        sched_yield ();

    array[i] = target_thread;

    status = pthread_mutex_unlock (&mut);
    return status;
}
```
The `thd_continue` function first checks whether the suspend/resume package has been initialized (inited is not 0). If it has not been initialized, then no threads are suspended, and `thd_continue` returns with success.

If the specified thread identifier is not found in the array of suspended threads, then it is not suspended-again, return with success.

Send the resume signal, `SIGUSR2.` There's no need to wait-the thread will resume whenever it can, and the thread calling `thd_continue` doesn't need to know.

```c
/*  susp.c part 4 thd_continue  */
/*
 * Resume a suspended thread by sending it SIGUSR2 to break
 * it out of the sigsuspend() in which it's waiting. If the
 * target thread isn't suspended, return with success.
 */
int
thd_continue (pthread_t target_thread)
{
    int status;
    int i = 0;

    /*
     * Serialize access to suspend, makes life easier.
     */
    status = pthread_mutex_lock (&mut);
    if (status != 0)
        return status;

    /*
     * If we haven't been initialized, then the thread must be
     * "resumed"; it couldn't have been suspended!
     */
    if (!inited) {
        status = pthread_mutex_unlock (&mut);
        return status;
    }

    /*
     * Make sure the thread is in the suspend array. If not, it
     * hasn't been suspended (or it has already been resumed) and
     * we can just carry on.
     */
    while (array[i] != target_thread && i < bottom)
        i++;

    if (i >= bottom) {
        pthread_mutex_unlock (&mut);
        return 0;
    }

    /*
     * Signal the thread to continue, and remove the thread from
     * the suspended array.
     */
    status = pthread_kill (target_thread, SIGUSR2);
    if (status != 0) {
        pthread_mutex_unlock (&mut);
        return status;
    }

    array[i] = 0;                   /* Clear array element */
    status = pthread_mutex_unlock (&mut);
    return status;
}
```
The `thread_routine` function is the thread start routine for each of the "target" threads created by the program. It simply loops for a substantial period of time, periodically printing a status message. On each iteration, it yields to other threads to ensure that the processor time is apportioned "fairly" across all the threads.

Notice that instead of calling printf, the function formats a message with sprintf and then displays it on stdout (file descriptor 1) by calling write. This illustrates one of the problems with using suspend and resume (`thd_suspend` and `thd_continue`) for synchronization. Suspend and resume are scheduling functions, not synchronization functions, and using scheduling and synchronization controls together can have severe consequences.

> Incautious use of suspend and resume can deadlock your application.

In this case, if a thread were suspended while modifying a stdio stream, all other threads that tried to modify that stdio stream might block, waiting for a mutex that is locked by the suspended thread. The write function, on the other hand, is usually a call to the kernel-the kernel is atomic with respect to signals, and therefore can't be suspended. Use of write, therefore, cannot cause a deadlock.

In general, you cannot suspend a thread that may possibly hold any resource, if that resource may be required by some other thread before the suspended thread is resumed. In particular, the result is a deadlock if the thread that would resume the suspended thread first needs to acquire the resource. This prohibition includes, especially, mutexes used by libraries you call-such as the mutexes used by malloc and free, or the mutexes used by stdio.

Threads are created with an attributes object set to create threads detached, rather than joinable. The result is that threads will cease to exist as soon as they terminate, rather than remaining until main calls `pthread_join`. The `pthread_kill` function does not necessarily fail if you attempt to send a signal to a terminated thread (the standard is silent on this point), and you may be merely setting a pending signal in a thread that will never be able to act on it. If this were to occur, the `thd_suspend` routine would hang waiting for the thread to respond. Although `pthread_kill` may not fail when sending to a terminated thread, it will fail when sending to a thread that doesn't exist-so this attribute converts a possible hang, when the program is run with iterations set too low, into an abort with an error message.

The main thread sleeps for two seconds after creating the threads to allow them to reach a "steady state." It then loops through the first half of the threads, suspending each of them. It waits an additional two seconds and then resumes each of the threads it had suspended. It waits another two seconds, suspends each of the remaining threads (the second half), and then after another two seconds resumes them.

By watching the status messages printed by the individual threads, you can see the pattern of output change as the threads are suspended and resumed.

```c
/*  susp.c part 5 sampleprogram  */
static void *
thread_routine (void *arg)
{
    int number = (int)arg;
    int status;
    int i;
    char buffer[128] ;

    for (i = 1; i <= iterations; i++) {
        /*
         * Every time each thread does 5000 interations, print
         * a progress report.
         */
        if (i % 2000 == 0) {
            sprintf (
                buffer, "Thread %02d: %d\n",
                number, i);
            write (1, buffer, strlen (buffer));
        }

        sched_yield ();
    }

    return (void *)0;
}

int
main (int argc, char *argv[])
{
    pthread_t threads[THREAD_COUNT];
    pthread_attr_t detach;
    int status;
    void *result;
    int i;

    status = pthread_attr_init (&detach);
    if (status != 0)
        err_abort (status, "Init attributes object");
    status = pthread_attr_setdetachstate (
        &detach, PTHREAD_CREATE_DETACHED);
    if (status != 0)
        err_abort (status, "Set create-detached");

    for (i = 0; i< THREAD_COUNT; i++) {
        status = pthread_create (
            &threads[i], &detach, thread_routine, (void *)i);
        if (status != 0)
            err_abort (status, "Create thread");
    }

    sleep (2);

    for (i = 0; i < THREAD_COUNT/2; i++) {
        printf ("Suspending thread %d.\n", i);
        status = thd_suspend (threads[i]);
        if (status != 0)
            err_abort (status, "Suspend thread");
    }

    printf ("Sleeping ...\n");
    sleep (2);

    for (i = 0; i < THREAD_COUNT/2; i++) {
        printf ("Continuing thread %d.\n", i);
        status = thd_continue (threads[i]);
        if (status != 0)
            err_abort (status, "Suspend thread");
    }

    for (i = THREAD_C0UNT/2; i < THREAD_COUNT; i++) {
        printf ("Suspending thread %d.\n", i);
        status = thd_suspend (threads[i]);
        if (status != 0)
            err_abort (status, "Suspend thread");
    }

    printf ("Sleeping ...\n");
    sleep (2);

    for (i = THREAD_COUNT/2; i < THREAD_COUNT; i++) {
        printf ("Continuing thread %d.\n", i);
        status = thd_continue (threads[i]);
        if (status != 0)
            err_abort (status, "Continue thread");
    }

    pthread_exit (NULL);    /* Let threads finish */
}
```
### 6.6.4 `sigwait` and `sigwaitinfo`
```c
int sigwait (const sigset_t *set, int *sig);
#ifdef _POSIX_REALTIME_SIGNALS
int sigwaitinfo (
    const sigset t *set, siginfo t *info);
int sigtimedwait (
    const sigset t *set, siginfo_t *info,
    const struct timespec *timeout);
#endif
```
> Always use sigwait to work with asynchronous signals within threaded code.

Pthreads adds a function to allow threaded programs to deal with "asynchronous" signals synchronously. That is, instead of allowing a signal to interrupt a thread at some arbitrary point, a thread can choose to receive a signal synchronously. It does this by calling `sigwait`, or one of `sigwait`'s siblings.

> The signals for which you `sigwait` must be masked in the sigwaiting thread, and should usually be masked in all threads.

The `sigwait` function takes a signal set as its argument, and returns a signal number when any signal in that set occurs. You can create a thread that waits for some signal, for example, sigint, and causes some application activity when it occurs. The nonobvious rule is that the signals for which you wait must be masked before calling `sigwait`. In fact, you should ideally mask these signals in main, at the start of the program. Because signal masks are inherited by threads you create, all threads will (by default) have the signal masked. This ensures that the signal will never be delivered to any thread except the one that calls `sigwait`.

Signals are delivered only once. If two threads are blocked in `sigwait`, only one of them will receive a signal that's sent to the process. This means you can't, for example, have two independent subsystems using `sigwait` that catch `SIGINT`. It also means that the signal will not be caught by `sigwait` in one thread and also delivered to some signal-catching function in another thread. That's not so bad, since you couldn't do that in the old nonthreaded model either-only one signal action can be active at a time.

While `sigwait`, a Pthreads function, reports errors by returning an error number, its siblings, `sigwaitinfo` and sigtimedwait, were added to POSIX prior to Pthreads, and use the older errno mechanism. This is confusing and awkward, and that is unfortunate. The problem is that they deal with the additional information supplied by the POSIX realtime signals option (`<unistd.h>` defines the symbol `_POSIX_REALTIME_SIGNALS`), and the POSIX realtime amendment, POSIX.1b, was completed before the Pthreads amendment.

Both `sigwaitinfo` and `sigtimedwait` return the realtime signal information, `siginfo_t`, for signals received. In addition, `sigtimedwait` allows the caller to specify that `sigtimedwait` should return with the error `EAGAIN` in the event that none of the selected signals is received within the specified interval.

The `sigwait.c` program creates a "sigwait thread" that handles `SIGINT`.

The `signal_waiter` thread repeatedly calls `sigwait`, waiting for a `SIGINT` signal. It counts five occurrences of `SIGINT` (printing a message each time), and then signals a condition variable on which main is waiting. At that time, main will exit.

The main program begins by masking `SIGINT`. Because all threads inherit their initial signal mask from their creator, `SIGINT` will be masked in all threads. This prevents `SIGINT` from being delivered at any time except when the `signal_waiter` thread is blocked in `sigwait` and ready to receive the signal.

```c
/*  sigwait.c  */
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include "errors.h"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
int interrupted = 0;
sigset_t signal_set;

/*
 * Wait for the SIGINT signal. When it has occurred 5 times, set the
 * "interrupted" flag (the main thread's wait predicate) and signal a
 * condition variable. The main thread will exit.
 */
void *signal_waiter (void *arg)
{
    int sig_number;
    int signal_count = 0;
    int status;

    while (1) {
        sigwait (&signal_set, &sig_number);
        if (sig_number == SIGINT) {
            printf ("Got SIGINT (%d of 5)\n", signal_count+1);
            if (++signal_count >= 5) {
                status = pthread_mutex_lock (&mutex);
                if (status != 0)
                    err_abort (status, "Lock mutex");
                interrupted = 1;
                status = pthread_cond_signal (&cond);
                if (status != 0)
                    err_abort (status, "Signal condition");
                status = pthread_mutex_unlock (&mutex);
                if (status != 0)
                    err_abort (status, "Unlock mutex");
                break;
            }
        }
    }
    return NULL;
}

int main (int argc, char *argv[])
{
    pthread_t signal_thread_id;
    int status;

    /*
     * Start by masking the "interesting" signal, SIGINT in the
     * initial thread. Because all threads inherit the signal mask
     * from their creator, all threads in the process will have
     * SIGINT masked unless one explicitly unmasks it. The
     * semantics of sigwait requires that all threads (including
     * the thread calling sigwait) have the signal masked, for
     * reliable operation. Otherwise, a signal that arrives
     * while the sigwaiter is not blocked in sigwait might be
     * delivered to another thread.
     */
    sigemptyset (&signal_set);
    sigaddset (&signal_set, SIGINT);
    status = pthread_sigmask (SIG_BL0CK, &signal_set, NULL);
    if (status != 0)
        err_abort (status, "Set signal mask");

    /*
     * Create the sigwait thread.
     */
    status = pthread_create (&signal_thread_id, NULL,
        signal_waiter, NULL);
    if (status != 0)
        err_abort (status, "Create sigwaiter");

    /*
     * Wait for the sigwait thread to receive SIGINT and signal
     * the condition variable.
     */
    status = pthread_mutex_lock (&mutex);
    if (status != 0)
        err_abort (status, "Lock mutex");
    while (!interrupted) {
        status = pthread_cond_wait (&cond, &mutex);
        if (status != 0)
            err_abort (status, "Wait for interrupt");
    }
    status = pthread_mutex_unlock (&mutex);
    if (status != 0)
        err_abort (status, "Unlock mutex");
    printf ("Main terminating with SIGINT\n");
    return 0;
}
```
### 6.6.5 `SIGEV_THREAD`
Some of the functions in the POSIX.1b realtime standard, which provide for asynchronous notification, allow the programmer to give specific instructions about how that notification is to be accomplished. For example, when initiating an asynchronous device read or write using `aio_read` or `aio_write`, the programmer specifies a `struct aiocb`, which contains, among other members, a `struct sigevent`. Other functions that accept a `struct sigevent` include `timer_create` (which creates a per-process timer) and `sigqueue` (which queues a signal to a process).

The struct `sigevent` structure in POSIX.1b provides a "notification mechanism" that allows the programmer to specify whether a signal is to be generated, and, if so, what signal number should be used. Pthreads adds a new notification mechanism called `SIGEV_THREAD`. This new notification mechanism causes the signal notification function to be run as if it were the start routine of a thread.

Pthreads adds several members to the POSIX.1b struct sigevent structure. The new members are `sigev_notify_function`, a pointer to a thread start function; and `sigev_notify_attributes`, a pointer to a thread attributes object (`pthread_attr_t`) containing the desired thread creation attributes. If `sigev_notify_attributes` is NULL, the notify thread is created as if the detachstate attribute was set to `pthread_create_detached`. This avoids a memory leak-in general, the notify thread's identifier won't be available to any other thread. Furthermore, Pthreads says that the result of specifying an attributes object that has the detachstate attribute set to `pthread_create_joinable` is "undefined." (Most likely, the result will be a memory leak because the thread cannot be joined-if you are lucky, the system may override your choice and create it detached anyway.)

The `SIGEV_THREAD` notification function may not actually be run in a new thread-Pthreads carefully specifies that it behaves as if it were run in a new thread, just as I did a few paragraphs ago. The system may, for example, queue `SIGEV_THREAD` events and call the start routines, serially, in some internal "server thread." The difference is effectively indistinguishable to the application. A system that uses a server thread must be very careful about the attributes specified for the notification thread-for example, scheduling policy and priority, contention scope, and minimum stack size must all be taken into consideration.

The `SIGEV_THREAD` feature is not available to any of the "traditional" signal generation mechanisms, such as setitimer, or for `SIGCHLD`, `SIGINT`, and so forth. Those who are programming using the POSIX.1b "realtime signal" interfaces, including timers and asynchronous I/O, may find this new capability useful.

The following program, `sigev_thread.c`, shows how to use the `SIGEV_THREAD` notification mechanism for a POSIX.1b timer.

The function `timer_thread` is specified as the "notification function" (thread start routine) for the `SIGEV_THREAD` timer. The function will be called each time the timer expires. It counts expirations, and wakes the main thread after five. Notice that, unlike a signal-catching function, the `SIGEV_THREAD` notification function can make full use of Pthreads synchronization operations. This can be a substantial advantage in many situations.

Unfortunately, neither Solaris 2.5 nor Digital UNIX 4.0 correctly implemented `SIGEV_THREAD`. Thus, unlike all other examples in this book, this code will not compile on Solaris 2.5. This #ifdef block allows the code to compile, and to fail gracefully if the resulting program is run, with an error message. Although the program will compile on Digital UNIX 4.0, it will not run. The implementation of `SIGEV_THREAD` has been fixed in Digital UNIX 4.0D, which should be available by the time you read this, and it should also be fixed in Solaris 2.6.

These statements initialize the sigevent structure, which describes how the system should notify the application when an event occurs. In this case, we are telling it to call `timer_thread` when the timer expires, and to use default attributes.

```c
/*  sigev_thread.c  */
#include <pthread.h>
#include <sys/signal.h>
#include <sys/time.h>
#include "errors.h"

timer_t timer_id;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
int counter = 0;

/*
 * Thread start routine to notify the application when the
 * timer expires. This routine is run "as if" it were a new
 * thread, each time the timer expires.
 *
 * When the timer has expired 5 times, the main thread will
 * be awakened, and will terminate the program.
 */
void
timer_thread (void *arg)
{
    int status;

    status = pthread_mutex_lock (&mutex);
    if (status != 0)
        err_abort (status, "Lock mutex");
    if (++counter >= 5) {
        status = pthread_cond_signal (&cond);
        if (status != 0)
            err_abort (status, "Signal condition");
    }
    status = pthread_mutex_unlock (&mutex);
    if (status != 0)
        err_abort (status, "Unlock mutex");

    printf ("Timer %d\n", counter);
}

main( )
{
    int status;
    struct itimerspec ts;
    struct sigevent se;

#ifdef sun
    fprintf (
        stderr,
        "This program cannot compile on Solaris 2.5.\n"
        "To build and run on Solaris 2.6, remove the\n"
        "\"#ifdef sun\" block in main().\n");
#else
    /*
     * Set the sigevent structure to cause the signal to be
     * delivered by creating a new thread.
     */
    se.sigev_notify = SIGEV_THREAD;
    se.sigev_value.sival_ptr = &timer_id;
    se.sigev_notify_function = timer_thread;
    se.sigev_notify_attributes = NULL;

    /*
     * Specify a repeating timer that fires every 5 seconds.
     */
    ts.it_value.tv_sec = 5;
    ts.it_value.tv_nsec = 0;
    ts.it_interval.tv_sec =5;
    ts.it_interval.tv_nsec = 0;

    DPRINTF (("Creating timer\n"));
    status = timer_create(CLOCK_REALTIME, &se, &timer_id);
    if (status == -1)
        errno_abort ("Create timer");

    DPRINTF ((
        "Setting timer %d for 5-second expiration...\n", timer_id));
    status = timer_settime(timer_id, 0, &ts, 0);
    if (status == -1)
        errno_abort ("Set timer");

    status = pthread_mutex_lock (&mutex);
    if (status != 0)
        err_abort (status, "Lock mutex");
    while (counter < 5) {
        status = pthread_cond_wait (&cond, &mutex);
        if (status != 0)
            err_abort (status, "Wait on condition");
    }
    status = pthread_mutex_unlock (&mutex);
    if (status != 0)
        err_abort (status, "Unlock mutex");

#endif /* Sun */
    return 0;
}
```
### 6.6.6 Semaphores: synchronizing with a signal-catching function
```c
#ifdef _POSIX_SEMAPHORES
int sem_init (sem t *sem,
    int pshared, unsigned int value);
int sem_destroy (sem_t *sem);
int sem_wait (sem_t *sem);
int sem_trywake (sem_t *sem);
int sem_post (sem_t *sem);
int sem_getvalue (sem_t *sem, int *sval);
#endif
```
Although mutexes and condition variables provide an ideal solution to most synchronization needs, they cannot meet all needs. One example of this is a need to communicate between a POSIX signal-catching function and a thread waiting for some asynchronous event. In new code, it is best to use sigwait or sigwaitinfo rather than relying on a signal-catching function, and this neatly avoids this problem. However, the use of asynchronous POSIX signal-catching functions is well established and widespread, and most programmers working with threads and existing code will probably encounter this situation.

To awaken a thread from a POSIX signal-catching function, you need a mechanism that's reentrant with respect to POSIX signals (async-signal safe). POSIX provides relatively few of these functions, and none of the Pthreads functions is included. That's primarily because an async-signal safe mutex lock operation would be many times slower than one that isn't async-signal safe. Outside of the kernel, making a function async-signal safe usually requires that the function mask (block) signals while it runs-and that is expensive.

In case you're curious, here is the full list of POSIX 1003.1-1996 functions that are async-signal safe (some of these functions exist only when certain POSIX options are denned, such as `_posix_asynchronous_io` or `_posix_timers`):

|                |             |                   |
| --             | --          | --                |
| `access`         | `getoverrun`  | `sigismember`       |
| `aio_error`     | `getgroups`   | `sigpending`        |
| `aio_return`    | `getpgrp`     | `sigprocmask`       |
| `aio_suspend`   | `getpid`      | `sigqueue`          |
| `alarm`          | `getppid`     | `sigsuspend`        |
| `cfgetispeed`    | `getuid`      | `sleep`             |
| `cfgetospeed`    | `kill`        | `stat`              |
| `cfsetispeed`    | `link`        | `sysconf`           |
| `cfsetospeed`    | `lseek`       | `tcdrain`           |
| `chdir`          | `mkdir`       | `tcflow`            |
| `chmod`          | `mkfifo`      | `tcflush`           |
| `chown`          | `open`        | `tcgetattr`         |
| `clock_gettime` | `pathconf`    | `tcgetpgrp`         |
| `close`          | `pause`       | `tcsendbreak`       |
| `creat`          | `pipe`        | `tcsetattr`         |
| `dup2`           | `read`        | `tcsetpgrp`         |
| `dup`            | `rename`      | `time`              |
| `execle`         | `rmdir`       | `timer_getoverrun` |
| `execve`         | `sem_post`   | `timer_gettime`    |
| `exit`           | `setgid`      | `timer_settime`    |
| `fcntl`          | `setpgid`     | `times`             |
| `fdatasync`      | `setsid`      | `umask`             |
| `fork`           | `setuid`      | `uname`             |
| `fstat`          | `sigaction`   | `unlink`            |
| `fsync`          | `sigaddset`   | `utime`             |
| `getegid`        | `sigdelset`   | `wait`              |
| `geteuid`        | `sigemptyset` | `waitpid`           |
| `getgid`         | `sigfillset`  | `write`             |

POSIX.1b provides counting semaphores, and most systems that support Pthreads also support POSIX.1b semaphores. You may notice that the `sem_post` function, which wakes threads waiting on a semaphore, appears in the list of async-signal safe functions. If your system supports POSIX semaphores (`<unistd.h>` defines the `_posix_semaphores` option), then Pthreads adds the ability to use semaphores between threads within a process. That means you can post a semaphore, from within a POSIX signal-catching function, to wake a thread in the same process or in another process.

A semaphore is a different kind of synchronization object-it is a little like a mutex, a little like a condition variable. The differences can make semaphores a little harder to use for many common tasks, but they make semaphores substantially easier to use for certain specialized purposes. In particular, semaphores can be posted (unlocked or signaled) from a POSIX signal-catching function.

> Semaphores are a general synchronization mechanism.  
> We just have no reason to use them that way.  

I am emphasizing the use of semaphores to pass information from a signal-catching function, rather than for general use, for a couple of reasons. One reason is that semaphores are part of a different standard. As I said, most systems that support Pthreads will also support POSIX.1b, but there is no such requirement anywhere in the standard. So you may well find yourself without access to semaphores, and you shouldn't feel dependent on them. {Of course, you may also find yourself with semaphores and without threads-but in that case, you should be reading a different book.)

Another reason for keeping semaphores here with signals is that, although semaphores are a completely general synchronization mechanism, it can be more difficult to solve many problems using semaphores-mutexes and condition variables are simpler. If you've got Pthreads, you only need semaphores to handle this one specialized function-waking a waiting thread from a signal-catching function. Just remember that you can use them for other things when they're convenient and available.

POSIX semaphores contain a count, but no "owner," so although they can be used essentially as a lock, they can also be used to wait for events. The terminology used in the POSIX semaphore operations stresses the "wait" behavior rather than the "lock" behavior. Don't be confused by the names, though; there's no difference between "waiting" on a semaphore and "locking" the semaphore.

A thread waits on a semaphore (to lock a resource, or wait for an event) by calling `sem_wait`. If the semaphore counter is greater than zero, `sera_wait` decrements the counter and returns immediately. Otherwise, the thread blocks. A thread can post a semaphore (to unlock a resource, or awaken a waiter) by calling `sem_post`. If one or more threads are waiting on the semaphore, `sem_post` will wake one waiter (the highest priority, or earliest, waiter). If no threads are waiting, the semaphore counter is incremented.

The initial value of the semaphore counter is the distinction between a "lock" semaphore and a "wait" semaphore. By creating a semaphore with an initial count of 1, you allow one thread to complete a `sem_wait` operation without blocking-this "locks" the semaphore. By creating a semaphore with an initial count of 0, you force all threads that call `sem_wait` to block until some thread calls `sem_post`.

The differences in how semaphores work give the semaphore two important advantages over mutexes and condition variables that may be of use in threaded programs:
1. Unlike mutexes, semaphores have no concept of an "owner." This means that any thread may release threads blocked on a semaphore, much as if any thread could unlock a mutex that some thread had locked. (Although this is usually not a good programming model, there are times when it is handy.)
2. Unlike condition variables, semaphores can be independent of any external state. Condition variables depend on a shared predicate and a mutex for waiting-semaphores do not.

A semaphore is represented in your program by a variable of type `sem_t`. You should never make a copy of a `sem_t` variable-the result of using a copy of a `sem_t` variable in the `sem_wait`, `sem_trywait`, `sem_post`, and `sem_destroy` functions is undefined. For our purposes, a `sem_t` variable is initialized by calling the `sem_init` function. POSIX.1b provides other ways to create a "named" semaphore that can be shared between processes without sharing memory, but there is no need for this capability when using a semaphore within a single process.

Unlike Pthreads functions, the POSIX semaphore functions use errno to report errors. That is, success is designated by returning the value 0, and errors are designated by returning the value -1 and setting the variable errno to an error code.

If you have a section of code in which you want up to two threads to execute simultaneously while others wait, you can use a semaphore without any additional state. Initialize the semaphore to the value 2; then put a `sem_wait` at the beginning of the code and a `sem_post` at the end. Two threads can then wait on the semaphore without blocking, but a third thread will find the semaphore's counter at 0, and block. As each thread exits the region of code it posts the semaphore, releasing one waiter (if any) or restoring the counter.

The `sem_getvalue` function returns the current value of the semaphore counter if there are no threads waiting. If threads are waiting, `sem_getvalue` returns a negative number. The absolute value of that number tells how many threads are waiting on the semaphore. Keep in mind that the value it returns may already be incorrect-it can change at any time due to the action of some other thread.

The best use for `sem_getvalue` is as a way to wake multiple waiters, somewhat like a condition variable broadcast. Without `sem_getvalue`, you have no way of knowing how many threads might be blocked on a semaphore. To "broadcast" a semaphore, you could call `sem_getvalue` and `sem_post` in a loop until `sem_getvalue` reports that there are no more waiters.

But remember that other threads can call `sem_post` during this loop, and there is no synchronization between the various concurrent calls to `sem_post` and `sem_getvalue`. You can easily end up issuing one or more extra calls to `sem_post`, which will cause the next thread that calls `sem_wait` to find a value greater than 0, and return immediately without blocking.

The program below, `semaphore_signal.c`, uses a semaphore to awaken threads from within a POSIX signal-catching function. Notice that the `sem_init` call sets the initial value to 0 so that each thread calling `sem_wait` will block. The main program then requests an interval timer, with a POSIX signal-catching function that will wake one waiting thread by calling `sem_post`. Each occurrence of the POSIX timer signal will awaken one waiting thread. The program will exit when each thread has been awakened five times.

Notice the code to check for eintr return status from the `sem_wait` call. The POSIX timer signal in this program will always occur while one or more threads are blocked in `sem_wait`. When a signal occurs for a process (such as a timer signal), the system may deliver that signal within the context of any thread within the process. Likely "victims" include threads that the kernel knows to be waiting, for example, on a semaphore. So there is a fairly good chance that the `sem_wait` thread will be chosen, at least sometimes. If that occurs, the call to `sem_wait` will return with eintr. The thread must then retry the call. Treating an eintr return as "success" would make it appear that two threads had been awakened by each call to `sem_post`: the thread that was interrupted, and the thread that was awakened by the `sem_post` call.
```c
/*  semaphore_signal.c  */
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <time.h>
#include "errors.h"

sem_t semaphore;

/*
 * Signal-catching function.
 */
void signal_catcher (int sig)
{
    if (sem_post (&semaphore) == -1)
        errno_abort ("Post semaphore");
}

/*
 * Thread start routine which waits on the semaphore.
 */
void *sem_waiter (void *arg)
{
    int number = (int)arg;
    int counter;

    /*
     * Each thread waits 5 times.
     */
    for (counter = 1; counter <= 5; counter++) {
        while (sem wait (&semaphore) == -1) {
            if (errno != EINTR)
                errno_abort ("Wait on semaphore");
        }
        printf ("%d waking (%d)...\n", number, counter);
    }
    return NULL;
}

int main (int argc, char *argv[])
{
    int thread_count, status;
    struct sigevent sig_event;
    struct sigaction sig_action;
    sigset_t sig_mask;
    timer_t timer_id;
    struct itimerspec timer_val;
    pthread_t sem_waiters[5];

#if !defined(_POSIX_SEMAPHORES) || !defined(_POSIX_TIMERS)
# if !defined(_POSIX_SEMAPHORES)
    printf ("This system does not support POSIX semaphores\n");
# endif
# if !defined(_POSIX_TIMERS)
    printf ("This system does not support POSIX timers\n");
# endif
    return -1;
#else
    sem_init (&semaphore, 0, 0);

    /*
     * Create 5 threads to wait on a semaphore.
     */
    for (thread_count = 0; thread_count < 5; thread_count++) {
        status = pthread_create (
            &sem_waiters[thread_count], NULL,
            sem_waiter, (void*)thread_count);
        if (status != 0)
            err_abort (status, "Create thread");
    }

    /*
     * Set up a repeating timer using signal number SIGRTMIN,
     * set to occur every 2 seconds.
     */
    sig_event.sigev_value.sival_int = 0;
    sig_event.sigev_signo = SIGRTMIN;
    sig_event.sigev_notify = SIGEV_SIGNAL;
    if (timer_create (CLOCK_REALTIME, &sig_event, &timer_id) == -1)
        errno_abort ("Create timer");
    sigemptyset (&sig_mask);
    sigaddset (&sig_mask, SIGRTMIN);
    sig_action.sa_handler = signal_catcher;
    sig_action.sa_mask = sig_mask;
    sig_action.sa_flags = 0;
    if (sigaction (SIGRTMIN, &sig_action, NULL) == -1)
        errno_abort ("Set signal action");
    timer_val.it_interval.tv_sec = 2;
    timer_val.it_interval.tv_nsec = 0;
    timer_val.it_value.tv_sec = 2 ;
    timer_val.it_value.tv_nsec = 0;
    if (timer_settime (timer_id, 0, &timer_val, NULL) == -1)
        errno_abort ("Set timer");

    /*
     * Wait for all threads to complete.
     */
    for (thread_count = 0; thread_count < 5; thread_count++) {
        status = pthread_join (sem_waiters[thread_count], NULL);
        if (status != 0)
            err_abort (status, "Join thread");
    }
    return 0;
#endif
}
```

