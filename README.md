_This project has been created as part of the 42 curriculum by pgougne_

![42 banner](assets/42banner.jpg)

# Codexion

## --- Description ---

Computer programming (often simply called "coding") is the process of designing and
building executable software to accomplish specific computing tasks. It involves analysis,
generating algorithms, profiling algorithms’ accuracy and resource consumption, and im-
plementing these algorithms in a chosen programming language.

In the modern era, with teams spread across time zones and projects using time-limited or
scarce hardware, the challenge is not only to write correct code, but also to design fair and
efficient protocols for sharing resources. This simulation models such a scenario: coders
working in a shared space, requiring two rare USB dongles to compile their quantum
code. They must coordinate, avoid deadlocks, and prevent burnout — because in the 
world of collaborative coding, as in life, access and timing are everything

Project Overview: Concurrent Programming

This project explores the complexities of Multithreading and shared resource management in a synchronized environment.
The Challenge

    Threads as Coders: Multiple coders must perform tasks (compile, debug, refactor) simultaneously.

    Shared Resources: To compile, a coder must acquire two specific dongles shared with their immediate neighbors.

    Real-time Monitoring: A "Manager" thread tracks burnout levels to ensure no coder fails their deadline.

Key Implementation Details

    Race Condition Prevention: Every shared variable (dongle state, logs, global flags) is protected by a pthread_mutex_t. No data is accessed without a lock.

    CPU Efficiency: Instead of "busy-waiting" for resources, I used Condition Variables (pthread_cond_t). Threads sleep when dongles are busy and wake up instantly when they are freed.

    Deadlock Proof: Designed with a "Try-and-Backoff" strategy to ensure no two coders can block each other indefinitely.

    Precision: High-frequency monitoring and millisecond-accurate timing to handle strict burnout constraints.

## --- Instructions ---
To compile:
>make

To run after make:
>./codexion 10 800 100 100  100 10 50 "fifo"

recompile:
>make re

delete all .o files:
>make clean

delete all .o files and executable:
>make fclean

compile + run + clean:
>make run

compile + valgrind + run + clean:
>make leak

compile + run * 100 + clean:
>make tests


### Arguments:

This is the command line to run the project. There is 8 differents arguments.
>./codexion 10 800 100 100  100 10 50 "fifo"

| arguments | name                        | description                                                                                                                                                                                                                                                                                                                              |
|-----------|-----------------------------|------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| 1         | number_of_coders            | The number of coders and also the number of dongles.                                                                                                                                                                                                                                                                                     |
| 2         | time_to_burnout             | If a coder did not start compiling within time_to_burnout milliseconds since the beginning of their last compile or the beginning of the simulation, they burn out.                                                                                                                                                                      |
| 3         | time_to_compile             | The time it takes for a coder to compile. During that time, they must hold two dongles.                                                                                                                                                                                                                                                  |
| 4         | time_to_debug               | The time a coder will spend debugging                                                                                                                                                                                                                                                                                                    |
| 5         | time_to_refactor            | The time a coder will spend refactoring. After completing the refactoring phase, the coder will immediately attempt to acquire dongles and start compiling again.                                                                                                                                                                        |
| 6         | number_of_compiles_required | If all coders have compiled at least this many times, the simulation stops. Otherwise, it stops when a coder burns out.                                                                                                                                                                                                                  |
| 7         | dongle_cooldown             | After being released, a dongle is unavailable until its cooldown has passed.                                                                                                                                                                                                                                                             |
| 8         | scheduler                   | The arbitration policy used by dongles to decide who gets them when multiple coders request them. The value must be exactly one of: fifo or edf. fifo means First In, First Out: the dongle is granted to the coder whose request arrived first. edf means Earliest Deadline First with deadline = last_compile_start + time_to_burnout. |


## --- Resources ---
https://www.youtube.com/watch?v=uA8X5zNOGw8&list=PL9IEJIKnBJjFZxuqyJ9JqVYmuFZHr7CFM
https://www.irif.fr/~sangnier/enseignement/15-16/Concurrence/concurrence-tp1.pdf
https://www.codequoi.com/threads-mutex-et-programmation-concurrente-en-c/
https://stackoverflow.com/questions/42919437/error-in-c-code-error-expression-is-not-assignable
https://stackoverflow.com/questions/19419193/how-to-destroy-thread-in-c-linux
https://c-for-dummies.com/blog/?p=4236

Use of AI:
Made exercices for me to understand pthreads, mutex, cond, gettimeofday and the synthax.
Explained me traps in multi-threading such as race condition.
Debug to understand the output of the -fsanitize=thread flag.
Explained me some bugs that i could not find for a long time.

## --- Difficult concepts ---

## Blocking cases handled

Developing a multi-threaded system requires addressing several classical concurrency issues. My implementation specifically handles the following:
### 1. Deadlock Prevention (Coffman's Conditions)

A deadlock occurs when threads are stuck waiting for each other indefinitely. To prevent this, I addressed the four Coffman conditions:

    Mutual Exclusion: Only one coder can hold a specific dongle at a time.

    Hold and Wait: To prevent a coder from holding one dongle while waiting forever for another, I implemented a "Try-and-Backoff" strategy. If a coder cannot acquire both required dongles, it releases any dongle it already picked up, allowing others to work while it waits.

    No Preemption: Dongles are never forcibly taken; they are only released by the coder once the compilation is finished.

    Circular Wait: By using a shared queue, I ensure that resources are requested in a consistent manner, breaking the potential cycle of dependency.

### 2. Starvation Prevention & Scheduling (FIFO/EDF)

Starvation happens when a thread never gets the resources it needs because others are "faster".

    FIFO (First-In-First-Out): Every dongle maintains a queue of coders. When a dongle is freed, the first coder in the queue has priority.

    EDF (Earliest Deadline First): In this mode, priority is given to the coder closest to its "burnout" limit. This dynamic reordering ensures that the most "at-risk" threads get the CPU time they need to survive.

### 3. Precise Burnout Detection & Cooldown

The Monitor (Manager) runs in a dedicated high-frequency loop to detect burnouts with millisecond precision.

    Cooldown handling: After a compilation, a mandatory dongle_cooldown is applied before the dongles can be reused. This is managed by a timestamp check within the try_to_grab_dongles logic, preventing rapid "resource hogging" by a single thread.

    Atomic State Check: The stop_sim flag is checked before and after every blocking call (usleep, pthread_cond_timedwait), ensuring that all threads stop within a few milliseconds of a burnout detection.

### 4. Log Serialization

Without synchronization, printf calls from 10 different threads would interleave (e.g., 10 1 comp20 2 iling...).

    I implemented a print_lock (mutex) that wraps every log_action call. This guarantees that each log line is written as an atomic unit, preserving the chronological integrity of the simulation output.



## Thread synchronization mechanisms
A bad implementation of multi-threading can be very dangerous. When multiple threads access a shared variable simultaneously, it leads to Race Conditions, where the final state depends on the unpredictable timing of thread execution.
e.g:

int i = 0;

thread_1 wants to do: 
>i += 5;
 
thread_2 wants to do:
>i += 1;

the goal is to have 6 after every thread add their numbers to i.
So somethimes, thread_1 access first to i, add 5 and then thread_2 access to i and add 1, and this case lead to the good result. i = 6

BUT

Threads are running at the same times, so some times it will run like the first example, but the next example can also happen:
i = 0; 
```c
thread_1: access to i = 0;  // When thread_1 acces to i, i = 0
thread_2: access to i = 0;  // When thread_2 acces to i, i still = 0 because thread_1 didn't modify i yet

thread_1: i += 5 || 0 += 5; 
thread_2: i += 1 || 0 += 1; // As the read 0 for i when they access to this variable, they add 1 to it and write over the thread_1

printf("%d", i); -> output: 1 // thread_2 overwritten the thread_1 result
```

Instead of having the result 6, we've got 1. This is *Race conditions*

### 1. Mutual Exclusion (pthread_mutex_t)

To prevent data corruption, I used mutexes as "locks". A thread must acquire the lock before accessing a shared resource and release it immediately after.

    state_lock: Protects the stop_sim flag and the shared data used by the Monitor (Manager) to check for burnouts. This ensures the Monitor doesn't read a value while a Coder is modifying it.

    print_lock: Coordinates access to the standard output. Without this, logs from different coders would overlap, creating unreadable garbage in the terminal.

    dongle->lock: Each dongle has its own mutex. This ensures that only one coder can "grab" a specific dongle at any given time.

### 2. Condition Variables (pthread_cond_t)

Wait-loops (busy waiting) consume 100% of CPU. To avoid this, I implemented Condition Variables.

When a Coder fails to grab the required dongles, it doesn't loop infinitely. Instead, it calls pthread_cond_timedwait. This puts the thread to sleep, releasing its CPU usage. When another coder finishes compiling and calls free_dongles, it uses pthread_cond_broadcast to wake up all waiting coders, signaling that resources are now available.
### 3. Thread-Safe Communication

Communication between Coders and the Monitor is achieved through a shared state structure protected by the state_lock.

    Safety Example: When the Monitor detects a burnout, it locks the mutex, sets stop_sim = 1, and unlocks.

    Prevention: Every Coder checks this flag at the start of their loop and after every synchronization point. Because this check is protected by the same mutex, we guarantee that no Coder continues working after the simulation has officially ended.
