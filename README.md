# Multithreaded Producer-Consumer Solution in C

Engineered a C solution for the classic Bounded Buffer (Producer-Consumer) problem, a foundational challenge in operating systems that requires robust synchronization to prevent deadlocks and race conditions.

The application uses Pthreads to create multiple producer and consumer threads that operate on a shared, fixed-size buffer. The core synchronization logic was built using POSIX semaphores (`sem_init`, `sem_wait`, `sem_post`) and a mutex.

![Language](https://img.shields.io/badge/Language-C-A8B9CC.svg?logo=c&logoColor=white)
![Concurrency](https://img.shields.io/badge/API-Pthreads-00758F.svg)
![Sync](https://img.shields.io/badge/Synchronization-Semaphores_&_Mutex-red.svg)

---

## 🎯 The Bounded Buffer Problem

The Producer-Consumer problem involves a shared, fixed-size (bounded) buffer that is filled by **Producer** threads and emptied by **Consumer** threads.

This scenario introduces two major concurrency challenges:
1.  **Race Condition:** If two threads (e.g., a producer and a consumer) try to access the buffer's internal pointers at the *exact same time*, the data structure can become corrupted, leading to lost or duplicated data.
2.  **Busy-Waiting / Deadlock:**
    * If a producer finds the buffer is **full**, it must wait (sleep) until a consumer makes space.
    * If a consumer finds the buffer is **empty**, it must wait (sleep) until a producer adds an item.

A naive solution using only a mutex will lead to inefficient busy-waiting or deadlocks.

## 🛠️ The Synchronization Solution

This program implements an elegant, efficient solution using a combination of one mutex and two counting semaphores.

* **`pthread_mutex_t (mutex)`:** A single mutex (lock) is used to provide **mutual exclusion**. It protects the *physical* buffer (the array and its `in`/`out` indices) during the brief moment of insertion or removal. This solves the **race condition**.

* **`sem_t empty` (Counting Semaphore):** This semaphore is initialized to the buffer's `SIZE`. It tracks the number of *empty slots* available.
    * Producers must `sem_wait(&empty)` (decrement) before they can produce.
    * If `empty` is 0, the buffer is full, and the producer thread is automatically put to sleep by the OS.
    * Consumers signal `sem_post(&empty)` (increment) after they consume, waking up a sleeping producer.

* **`sem_t full` (Counting Semaphore):** This semaphore is initialized to `0`. It tracks the number of *items available* in the buffer.
    * Consumers must `sem_wait(&full)` (decrement) before they can consume.
    * If `full` is 0, the buffer is empty, and the consumer thread is automatically put to sleep.
    * Producers signal `sem_post(&full)` (increment) after they produce, waking up a sleeping consumer.



---

### Implementation Logic

The logic for the producer and consumer threads demonstrates this pattern:

**Producer Thread Logic:**
```c
while (true) {
    // Produce a new item...
    produce_item(&item);

    // Wait for an empty slot to become available
    sem_wait(&empty);

    // Lock the buffer to prevent race conditions
    pthread_mutex_lock(&mutex);
    
    // --- CRITICAL SECTION ---
    insert_item_into_buffer(item);
    // --- END CRITICAL ---

    // Unlock the buffer
    pthread_mutex_unlock(&mutex);
    
    // Signal that one more slot is now full
    sem_post(&full);
}
```

**Consumer Thread Logic:**
```c
while (true) {
    // Wait for a full slot to become available
    sem_wait(&full);

    // Lock the buffer to prevent race conditions
    pthread_mutex_lock(&mutex);

    // --- CRITICAL SECTION ---
    remove_item_from_buffer(&item);
    // --- END CRITICAL ---

    // Unlock the buffer
    pthread_mutex_unlock(&mutex);

    // Signal that one more slot is now empty
    sem_post(&empty);

    // Consume the item...
    consume_item(item);
}
```

This design is highly efficient as threads sleep automatically when no work is available and are woken up only when their counterpart (producer or consumer) has completed a task.

---

## 🚀 How to Compile & Run

### Prerequisites
* A C compiler (e.g., `gcc`)
* A POSIX-compliant system (Linux, macOS)

### Compile
You must link the Pthreads library (`-lpthread`).

```bash
# Compile the source file and create an executable
gcc producer_consumer_problem.c -o producer_consumer -lpthread
```

### Run
Execute the program from your terminal:

```bash
./producer_consumer
```
The program will start, initialize the buffer, and launch the producer and consumer threads, which will begin processing and printing their actions to the console.