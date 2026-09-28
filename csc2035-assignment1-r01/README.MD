# POSIX Shared-Memory Priority Job Queue

A concurrent multi-process task distribution system implemented in C using POSIX shared memory and semaphores. The subsystem resolves the bounded-buffer producer-consumer problem across isolated address spaces, enforcing strict mutual exclusion, priority-based scheduling, and persistent audit trail verification.

---

## Architecture Overview

The system orchestrates multi-process workflows where multiple independent producer processes generate prioritized workloads into a shared ring buffer/priority queue, consumed asynchronously by pool worker processes:

```text
+-------------------+      +-------------------+
| Producer Proc 1   |      | Producer Proc N   |
+---------+---------+      +---------+---------+
          |                          |
          +----------+    +----------+
                     |    |
                     v    v
       [ POSIX Shared Memory Segment ]
     +-----------------------------------+
     |        pri_jobqueue Buffer        |
     |   - Fixed-capacity array storage  |
     |   - Highest-priority-first logic  |
     +-----------------------------------+
     |   POSIX Semaphores (Mutex/Sync)   |
     +-----------------------------------+
                     |    |
          +----------+    +----------+
          |                          |
          v                          v
+---------+---------+      +---------+---------+
| Consumer Proc 1   |      | Consumer Proc M   |
+-------------------+      +-------------------+