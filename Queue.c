/**
 * @file Queue.c
 * @author Michel Vouillarmet
 * @brief Queue module implementation.
 * @version 1.0
 * @date 2022-09-06
 * @see https://github.com/monstermichl/Queue
 */

#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "Queue.h"

// ---------------------- defines ---------------------------------------------------------

// ---------------------- types -----------------------------------------------------------

// ---------------------- constants -------------------------------------------------------

// ---------------------- variables -------------------------------------------------------

// ---------------------- function declarations -------------------------------------------
static bool Queue_DequeueInternal(Queue_QueueStruct *queuePtr, void* dataPtr);

// ---------------------- function implementations ----------------------------------------

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
#if ONE_SIZE_TO_RULE_THEM_ALL
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

#if ONE_SIZE_TO_RULE_THEM_ALL
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
#if ONE_SIZE_TO_RULE_THEM_ALL
        queuePtr->elementSize = elementSize;
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

#if !ONE_SIZE_TO_RULE_THEM_ALL
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

#if ONE_SIZE_TO_RULE_THEM_ALL
bool Queue_Enqueue(Queue_QueueStruct* queuePtr, const void* dataPtr)
#else
bool Queue_Enqueue(Queue_QueueStruct* queuePtr, const void* dataPtr, const size_t elementSize)
#endif
{
    bool result = false;

    if ((queuePtr != NULL) && (dataPtr != NULL))
    {
        Queue_QueueEntryStruct* queueNodeTmpPtr = malloc(sizeof(Queue_QueueEntryStruct)); /* Reserve memory for a new queue entry. */

        if (queueNodeTmpPtr != NULL)
        {
            size_t elementSizeTemp;

            /* Use element size based on QueueCfg.h configuration */
#if ONE_SIZE_TO_RULE_THEM_ALL
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
    }
    return result;
}

// ---------------------------------------------------------------------------------------

bool Queue_Dequeue(Queue_QueueStruct *queuePtr, void* dataPtr)
{
    bool result = false;

    if (dataPtr != NULL)
    {
        result = Queue_DequeueInternal(queuePtr, dataPtr);
    }
    return result;
}

// ---------------------------------------------------------------------------------------

bool Queue_Purge(Queue_QueueStruct *queuePtr)
{
    return Queue_DequeueInternal(queuePtr, NULL);
}
