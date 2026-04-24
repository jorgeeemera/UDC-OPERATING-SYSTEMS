# Operating Systems (Sistemas Operativos) - UDC Engineering
This repository contains a custom **Unix-like Shell** developed in **C** from scratch during the Operating Systems course at the University of A Coruña (FIC).

The project evolved through three main stages, implementing core OS concepts:

## 📂 Project Evolution
- **/P0 - Shell Foundations**: Basic command loop, string parsing, and command history. Implementation of fundamental system calls like `getpid`, `chdir`, and file descriptor management (`open`, `close`, `dup`).
- **/P1 - File Systems**: Extended functionality for file and directory management. Implementation of recursive directory traversal, file metadata inspection using `stat` calls, and permission management.
- **/P2 - Memory Management**: Advanced memory control including allocation/deallocation via `malloc`, file mapping with `mmap`, and **Shared Memory** segments using `shmget/shmat`. Includes tools for memory dumping and stack/heap analysis.

## 🛠 Tech Stack & Requirements
- **Language**: C (Standard C11/C17)
- **Environment**: POSIX-compliant systems (macOS / Linux)
- **Compilation**: Strict `gcc -Wall` compliance with no warnings.
- **Memory Safety**: Verified with **Valgrind** to ensure zero memory leaks.

## 🧠 Core System Calls Implemented
- **Process Info**: `getpid`, `getppid`, `uname`.
- **Filesystem**: `open`, `read`, `write`, `lseek`, `stat`, `opendir`, `readdir`.
- **Memory**: `malloc`, `free`, `mmap`, `munmap`, `shmget`, `shmat`, `shmdt`.
