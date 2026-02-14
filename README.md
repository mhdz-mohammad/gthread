# gthread
a library in development, the goal is to provide sufficient features to observe performance of classical OS methods on a broad set of systems
# GThread: Cooperative User-Space Threading Library

**GThread** is a lightweight user-space threading runtime library for C/C++.  
It provides cooperative threads (green threads), scheduling, and synchronization primitives with introspection support for research, experimentation, and educational purposes.

---

## Features

- **User-Level Threads (Green Threads)**  
  - Context switching implemented manually (register + stack preservation)
  - Thread Control Block (TCB) abstraction
  - Scheduler-managed execution

- **Scheduling Policies**  
  - Ready and blocked queues
  - Support for Shortest Job First (SJF)
  - Thread pools and task submission

- **Synchronization Primitives**  
  - Counting semaphores
  - Blocking and wake-up mechanisms
  - Thread wait queues tied to resources

- **Runtime Introspection**  
  - Central registry tracking all threads
  - Iterate over thread states for debugging and analysis

- **Reference-Counted Thread Lifetime Management**  
  - Automatic memory management for threads referenced by multiple runtime structures

- **Experimental Deadlock Detection**  
  - Infrastructure for building resource allocation graphs
  - Intended for advanced runtime analysis

---

## Installation

1. Clone the repository:

```bash
git clone https://github.com/yourusername/gthread.git
cd gthread
