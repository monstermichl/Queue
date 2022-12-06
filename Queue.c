/**
 * @file Queue.c
 * @author Michel Vouillarmet
 * @brief Queue module implementation.
 * @version 1.1
 * @date 2022-12-06
 * @see https://github.com/monstermichl/Queue
 */

#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "Queue.h"

// ---------------------- defines ---------------------------------------------------------
#if QUEUE_THREAD_SAFE
#define QUEUE_CHANGE_LOCK(queuePtr, lock) queuePtr->inUse = lock
#define QUEUE_WAIT_FOR_MUTEX(queuePtr) while (queuePtr->inUse)
# if QUEUE_LOCK_CALLBACK
# define QUEUE_RESERVE_MUTEX(queuePtr) QUEUE_CHANGE_LOCK(queuePtr, true); Queue_CallCallback(queuePtr)
# else
# define QUEUE_RESERVE_MUTEX(queuePtr) QUEUE_CHANGE_LOCK(queuePtr, true)
# endif
# if QUEUE_UNLOCK_CALLBACK
# define QUEUE_FREE_MUTEX(queuePtr) QUEUE_CHANGE_LOCK(queuePtr, false); Queue_CallCallback(queuePtr)
# else
# define QUEUE_FREE_MUTEX(queuePtr) QUEUE_CHANGE_LOCK(queuePtr, false)
# endif
#endif

// ---------------------- types -----------------------------------------------------------

// ---------------------- constants -------------------------------------------------------

// ---------------------- variables -------------------------------------------------------

// ---------------------- function declarations -------------------------------------------
#if QUEUE_THREAD_SAFE && (QUEUE_LOCK_CALLBACK || QUEUE_UNLOCK_CALLBACK)
static void Queue_CallCallback(Queue_QueueStruct *queuePtr);
#endif
static bool Queue_DequeueInternal(Queue_QueueStruct *queuePtr, void* dataPtr);

// ---------------------- function implementations ----------------------------------------
#if QUEUE_THREAD_SAFE && (QUEUE_LOCK_CALLBACK || QUEUE_UNLOCK_CALLBACK)
static void Queue_CallCallback(Queue_QueueStruct *queuePtr)
{
    if (queuePtr != NULL)
    {
# if QUEUE_LOCK_CALLBACK
        if (queuePtr->inUse && (queuePtr->lockCb != NULL))
        {
            queuePtr->lockCb(queuePtr);
        }
# endif
# if QUEUE_UNLOCK_CALLBACK
        if (!queuePtr->inUse && (queuePtr->unlockCb != NULL))
        {
            queuePtr->unlockCb(queuePtr);
        }
# endif
    }
}
#endif

static bool Queue_DequeueInternal(Queue_QueueStruct *queuePtr, void* dataPtr)
{
    bool result = false;

    if (!Queue_IsEmpty(queuePtr))
    {
        Queue_QueueEntryStruct* tmpQueueNodeTmpPtr = queuePtr->head;

        if ((tmpQueueNodeTmpPtr == NULL) || (tmpQueueNodeTmpPtr->data == NULL))
        {
            /* No data available. */
        }
        else
        {
            /* If data pointer is provided, copy the data to the output, otherwise, just purge the queue entry. */
            if (dataPtr != NULL)
            {
                size_t elementSizeTemp;
                /* Use element size based on QueueCfg.h configuration. */
#if QUEUE_ONE_SIZE_TO_RULE_THEM_ALL
                elementSizeTemp = queuePtr->elementSize;
#else
                elementSizeTemp = tmpQueueNodeTmpPtr->elementSize;
#endif

                /* Copy data to output. */
                memcpy(dataPtr, tmpQueueNodeTmpPtr->data, elementSizeTemp);
            }
            queuePtr->head = tmpQueueNodeTmpPtr->next;

            /* If no head is set, no tail shall be set either. */
            if (queuePtr->head == NULL)
            {
                queuePtr->tail = NULL;
            }
            /* Free queue data memory. */
            free(tmpQueueNodeTmpPtr->data);

            /* Free queue entry memory. */
            free(tmpQueueNodeTmpPtr);

            queuePtr->count--;
        }
    }
    return result;
}

// ---------------------------------------------------------------------------------------

#if QUEUE_ONE_SIZE_TO_RULE_THEM_ALL
bool Queue_Init(Queue_QueueStruct *queuePtr, size_t elementSize)
#else
bool Queue_Init(Queue_QueueStruct *queuePtr)
#endif
{
    bool ok = (queuePtr != NULL);
    
    /* If the control field check evaluates true, the queue has already been initalized. */
    if (ok)
    {
        queuePtr->count = 0;
        queuePtr->head = NULL;
        queuePtr->tail = NULL;
#if QUEUE_ONE_SIZE_TO_RULE_THEM_ALL
        queuePtr->elementSize = elementSize;
#endif
#if QUEUE_THREAD_SAFE
        queuePtr->inUse = false;
# if QUEUE_LOCK_CALLBACK
        queuePtr->lockCb = NULL;
# endif
# if QUEUE_UNLOCK_CALLBACK
        queuePtr->unlockCb = NULL;
# endif
#endif
    }
    return ok;
}

// ---------------------------------------------------------------------------------------

size_t Queue_Length(Queue_QueueStruct *queuePtr)
{
    return (!Queue_IsEmpty(queuePtr)) ? queuePtr->count : 0;
}

// ---------------------------------------------------------------------------------------

bool Queue_IsEmpty(Queue_QueueStruct *queuePtr)
{
    return ((queuePtr        == NULL) ||
            (queuePtr->head  == NULL) ||
            (queuePtr->tail  == NULL) ||
            (queuePtr->count == 0));
}

// ---------------------------------------------------------------------------------------

#if !QUEUE_ONE_SIZE_TO_RULE_THEM_ALL
size_t Queue_NextElementSize(Queue_QueueStruct *queuePtr)
{
    size_t size = 0;

    if (!Queue_IsEmpty(queuePtr))
    {
        size = queuePtr->head->elementSize;
    }
    return size;
}
#endif

// ---------------------------------------------------------------------------------------

#if QUEUE_ONE_SIZE_TO_RULE_THEM_ALL
bool Queue_Enqueue(Queue_QueueStruct* queuePtr, const void* dataPtr)
#else
bool Queue_Enqueue(Queue_QueueStruct* queuePtr, const void* dataPtr, const size_t elementSize)
#endif
{
    bool result = false;

    if ((queuePtr != NULL) && (dataPtr != NULL))
    {
#if QUEUE_THREAD_SAFE
        QUEUE_WAIT_FOR_MUTEX(queuePtr);
        QUEUE_RESERVE_MUTEX(queuePtr);
#endif
        Queue_QueueEntryStruct* queueNodeTmpPtr = malloc(sizeof(Queue_QueueEntryStruct)); /* Reserve memory for a new queue entry. */

        if (queueNodeTmpPtr != NULL)
        {
            size_t elementSizeTemp;

            /* Use element size based on QueueCfg.h configuration */
#if QUEUE_ONE_SIZE_TO_RULE_THEM_ALL
            elementSizeTemp = queuePtr->elementSize;
#else
            elementSizeTemp = elementSize;
            queueNodeTmpPtr->elementSize = elementSize;
#endif
            queueNodeTmpPtr->data = malloc(elementSizeTemp); /* Reserve memory for the actual data. */

            /* Check if reservation of data memory was successful. */
            if (queueNodeTmpPtr->data != NULL)
            {
                memcpy(queueNodeTmpPtr->data, dataPtr, elementSizeTemp);
                queueNodeTmpPtr->next = NULL;

                if (Queue_IsEmpty(queuePtr))
                {
                    /* Insert the first element. */
                    queuePtr->head = queueNodeTmpPtr;
                }
                else
                {
                    queuePtr->tail->next = queueNodeTmpPtr;
                }
                queuePtr->tail = queueNodeTmpPtr;
                queuePtr->count++;

                result = true;
            }
            else
            {
                /* If not, cleanup queue entry. */
                free(queueNodeTmpPtr);
            }
        }
#if QUEUE_THREAD_SAFE
        QUEUE_FREE_MUTEX(queuePtr);
#endif
    }
    return result;
}

// ---------------------------------------------------------------------------------------

bool Queue_Dequeue(Queue_QueueStruct *queuePtr, void* dataPtr)
{
    bool result = false;

    if (dataPtr != NULL)
    {
#if QUEUE_THREAD_SAFE
        QUEUE_WAIT_FOR_MUTEX(queuePtr);
        QUEUE_RESERVE_MUTEX(queuePtr);
#endif
        result = Queue_DequeueInternal(queuePtr, dataPtr);
#if QUEUE_THREAD_SAFE
        QUEUE_FREE_MUTEX(queuePtr);
#endif
    }
    return result;
}

// ---------------------------------------------------------------------------------------

bool Queue_Purge(Queue_QueueStruct *queuePtr)
{
    return Queue_DequeueInternal(queuePtr, NULL);
}
