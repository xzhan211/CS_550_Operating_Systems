# CS 550 Operating Systems Programming Project 2
Overview: in this project, you are going to build a special Multi-Level Feedback Queue (MLFQ) scheduler in xv6. This MLFQ scheduler has two queues:
* the first queue has the higher priority, and uses round robin scheduling for processes within the queue;
* the second queue has the lower priority, and uses priority-based scheduling for processes within the same queue.

A process in the first queue is demoted to the second queue when its accumulated running time in the first queue exceeds a configured value. A process in the second queue is promoted to the first queue when its accumulated wait time in the second queue exceeds a configured value.

## Building an MLFQ scheduler
The original scheduler of xv6 adopts a round-robin (RR) policy. In this part, you are going to implement a special MLFQ scheduler, which adopts the following scheduling policies.
### 1. The MLFQ Scheduling Policies
1. We only consider the single CPU case. The Makefile in the base code has been set up accordingly. You don’t need to do anything on this.

2. The MLFQ has two queues — the first queue (i.e., Queue 0) has the higher priority, and the second queue (i.e., Queue 1) has the lower priority.

3. Queue 0 (round-robin)
* When a job/process enters the system, it is always placed at the end of Queue 0.
* Queue 0 has higher priority than Queue 1. Specifically, at any given point in time, if there are ready/runnable processes in Queue 0, they have higher priority to be scheduled
than processes in Queue 1.
* If there are multiple processes in Queue 0, these process are scheduled in a round robin
manner.
* Each process in Queue 0 has a run time counter (named as running tick), which records
the process’s running time in timer ticks since joining Queue 0. Therefore, this counter should be set to 0 when the process is initially inserted to Queue 0, and every time when it is promoted from Queue 1.
* When the scheduler function is invoked (i.e., when a timer tick1 occurs), the process that is scheduled to run next is considered to consume a timer tick, and therefore its running tick value should be incremented by one.
* Demotion: Queue 0 is associated with a configurable value (named as
RUNNING THRESHOLD), which is the timer ticks quota of Queue 0. Once a process’s running tick exceeds RUNNING THRESHOLD, this process is moved to Queue 1.
* The first two system processes “init” and xv6’s “shell” (with the pid of 1 and 2 sepa- rately) should always stay in Queue 0.
4. Queue 1 (priority-based)
* If there are multiple processes in Queue 1, these process are scheduled in a priority-based manner. The priorities of the processes in Queue 1 are determined as follows.
* Each process in Queue 1 has a wait time counter (named as waiting tick), which records the process’s waiting time for the CPU in timer ticks since joining Queue 1. Therefore, this counter should be set to 0 every time when the process is demoted from Queue 0.
* When the scheduler function is invoked (i.e., when a timer tick occurs), all the processes in Queue 1 that are not to be scheduled to run next are considered to spend one timer tick in waiting for the CPU, and therefore the waiting tick values of all these processes should be incremented by one.
1Here a “timer tick” is a “clock tick” in xv6’s term. It essentially a timer interrupt. Hint: check trap() in trap.c to see how a timer interrupt leads up to the process scheduler function.
* The waiting tick values of the processes in Queue 1 serve as the priorities of these processes in Queue 1: if there are multiple processes in Queue 1 while none in Queue 0, the process in Queue 1 with the largest waiting tick value has the highest priority to be scheduled.
* Promotion: Queue 1 is associated with a configurable value (named as
WAITING THRESHOLD), which is promotion threshold. Once a process’s waiting tick exceeds WAITING THRESHOLD, this process is moved to Queue 0.
5. Pinning processes to Queue 0. We need to allow some processes to always stay in Queue 0. For this purpose, a system call is needed to pin processes to Queue 0 (see details below).
### 2. What To Do?
1. Implement an MLFQ scheduler that strictly follows the above scheduling policies presented in Section 2.1.
2. Switch between xv6’s default RR (round robin) scheduler and the MLFQ scheduler.
* Your MLFQ code should not overwrite xv6’s defautl RR scheduler. Instead, you should provide an option to switch between these two schedulers. You only need to choose the desired scheduler before xv6 runs.
* Place an integer variable (named as “sched policy”) at the top of “proc.c”: **int sched_policy = 1;**
Use this variable in your code to determine which scheduling policy is active: if the value of sched policy is 0, the default RR policy is adopted. If the value of sched policy is 1, the MLFQ scheduler is used.
Notice that, your default scheduler should be MLFQ (i.e., set the default value of sched policy to 1). We will test your code based on this assumption. Failure to comply will lead to point deduction.
3. Implement a system call that allows user programs to set the RUNNING THRESHOLD for Queue 0.
* Write the corresponding system call user space wrapper function, and declare it in “user.h”. The wrapper function’s prototype should be:
**int setrunningticks(int time_allotment);**
This user-level wrapper function takes one positive integer argument: the value of RUNNING THRESHOLD to be set. The return value indicates whether this functions succeeds (i.e., 0) or not (i.e., 1).
Notice that, the default time of RUNNING THRESHOLD should be 2.
4. Implement a system call that allows user programs to set the WAITING THRESHOLD for Queue 1.
* Write the corresponding system call user space wrapper function, and declare it in “user.h”. The wrapper function’s prototype should be:
**int setwaitingticks(int waiting_thres);**
This user-level wrapper function takes one positive integer argument: the value of WAITING THRESHOLD to be set. The return value indicates whether this functions succeeds (i.e., 0) or not (i.e., 1).
Notice that, the default time of WAITING THRESHOLD should be 4.
5. Implement a system call that allows user program to pin a process in Queue 0.
* Write the corresponding system call user space wrapper function, and declare it in “user.h”. The wrapper function’s prototype should be:
**int setpriority(int pid, int priority);**

This user-level wrapper function takes two integer arguments. The first argument is the process’s pid. The second is the priority value — 0 indicates that the process stays in Queue 0 all the time, a non-zero value indicates that the process should be scheduled according to the MLFQ policy described above. The return value indicates whether this functions succeeds (i.e., 0) or not (i.e., 1). Notice that, the default priority for processes should be NON-ZERO.
### 3. Scheduling tracing functionality
To help you implement and debug, a simple scheduling tracing functionality has been added to the base code. When this tracing functionality is enabled, the kernel prints the PID of the currently running process every time before the CPU is given back to the scheduler in the timer interrupt handler.

With this scheduling tracing functionality enabled, you can see the sequence of processes that the scheduler schedules. To enable it, you simply put “1” to the following variable in “proc.c”:
**int sched_trace_enabled = 1;**
### Hints
* Most of the code of xv6 scheduler is located in proc.c. The header file proc.h is also useful and you may need to modify accordingly. In addition, you may want to understand the default RR scheduler before implementing the MLFQ scheduler.
* Do not mess up xv6’s default RR scheduler. Thus, you will always have a working scheduler for the system.
* Build and test one function at a time. For example, in the beginning, you could only implement a round robin queue for Queue 0. Then, you could use round robin queues for both Queue 0 and Queue 1. Last, you replace the round robin queue with a queue that realizes the priority-based policy for Queue 1.
* In addition to the test driver program (in Section 2.3, you would probably need other tracing points to track the status of each process and the behaviors of the scheduler (using cprintf) for debugging your code — you will probably want to trace the processes in each queue, the current running process, the consumed ticks (before a process will be demoted to Queue 1), the waiting ticks (before a process will be promoted to Queue 0), etc.
* Please DISABLE all your debugging print functions and output information, but only leave the test driver program enabled (i.e., int sched trace enabled = 1) before submitting your code. (You will lose points if you keep your debugging information on, and/or not enable the test driver program).
### Test Cases
We have included 3 test cases (test1 ∼ test3) included the BASE code, which cover some basic test scenarios. They are not included in the compilation — you may need to compile them after you correctly implement all required system call functions.

Obviously, they do not cover all test scenarios. Thus, first make sure your code work properly under these test cases. Then come up with as many test cases as possible to test your code extensively. Our final test cases are not limited to the three cases.
* Test 1: This test case tests the round robin scheduling in Queue 0.
In the test program, we set the RUNNING THRESHOLD to a large number, so that processes in Queue 0 (round robin) will not get demoted to Queue 1 (priority based).
Three processes, A, B, and C are created (we use pipe to synchronize them to make sure they start running almost at the same time).
As these three processes reside in the round robin queue, they should run by turns in a round robin manner. Hence, the expected results should be like:
**......CBACBACBACBACBA......**
Of course, it could be:
**...ABCABC... or, ...ACBACB..., etc.**
Notice that, you need to enable the scheduling tracing functionality (see Section 2.3), and the output will be the pid of process A, B, and C.
* Test 2: This test case tests the priority-based scheduling in Queue 1.
In the test program, we set the WAITING THRESHOLD to a large number, so that once a process is demoted, it will stay in Queue 1.
— Three processes, A, B, and C, are created. At the beginning, C is pinned to stay in Queue 0 using the setpriority() system call. During the time when C is pinned in Queue 0, A and B are first demoted to Queue 1, and start accumulating their waiting tick values.
— After some time, C is unpinned from Queue 0 (using the setpriority() system call), and thus it is demoted to Queue 1 shortly.
— After C is demoted to Queue 1, all the three processes A, B, and C are in Queue 1 and there is no process in Queue 0. In this case, it is expected that only process A and B should get scheduled at the beginning. This is because, when C is demoted, A and B has a large waiting tick value, and C’s waiting tick value is 0. Therefore A and B have higher priority than C to get scheduled.
— But while C is waiting in Queue 1, its waiting tick value increased faster than A and B’s. So eventually C’s waiting tick will catch up with A and B’s. In this case, A, B, and C are scheduled as if it is round robin.
— With the above explanation, the expected results after C is demoted into Queue 0 should be:
**......ABABABABABAB......ABCABCABC......**
or something like: 
**......AABBAABBAABB......ABCCBAABCCBA......**
depending on how you deal wilt two processes with the same priority value. In other words, you are fine as long as the scheduling pattern of your implementation matches what’s ex- pected.
* Test 3: This test case tests demotion and promotion.
In the test program, we set both RUNNING THRESHOLD and WAITING THRESHOLD to a reasonable value (e.g., 10 and 20 separately).
Two processes, A and B, are created. We pin process B to the Queue 0. So, we except that:
(1) A and B should first execute in the round robin queue for a while: 
**......ABABABABABAB......**
(2) Then A gets demoted to the Queue 1. As B still stays in the Queue 0, B will be executing during this period:
**......BBBBBBBBBBBB......**
(3) During this period, A accumulates waiting ticks and gets promoted to Queue 0. So the output should look like:
**......ABABABABAB......**
(4) So on and so forth
Overall, the expected results should be: 
**......ABABABABABAB......BBBBBBBBBB......ABABABABAB......**


























