# Queue
Simple C implementation of a generic queue.

## Usage
Include Queue.c, Queue.h and QueueCfg.h into your project. To use the module's functions, only Queue.h must be loaded within the source.

Each queue is stored as a separate struct which must be allocated in RAM before it's passed to the *Queue_Init* function. *Queue_Init* does **NOT** reserve space on the heap for the queue itself. However, each call of *Queue_Enqueue* allocates and each call to *Queue_Dequeue* frees memory on the heap.

For further documentation please refer to the generated Doxygen documentation.

```c
#include <string.h>
#include "Queue.h"

int main(void)
{
    Queue_QueueStruct queue;
    char* newEntry = "New Entry";
    const int entryBufferLength = 20;
    char entryBuffer[entryBufferLength];

    Queue_Init(&queue); /* Init queue. */
    Queue_Enqueue(&queue, newEntry, strlen(newEntry)); /* Enqueue new element. */

    if (!Queue_IsEmpty(&queue))
    {
        if (Queue_NextElementSize(&queue) <= entryBufferLength) /* Check next element size. */
        {
            Queue_Dequeue(&queue, entryBuffer); /* Dequeue next element into entryBuffer. */
        }
        else
        {
            Queue_Purge(&queue); /* Purge next element. */
        }
    }
    return 0;
}
```
