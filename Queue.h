/**
 * @file Queue.h
 * @author Michel Vouillarmet
 * @brief Queue module interface declaration.
 * @version 1.1
 * @date 2022-12-06
 * @see https://github.com/monstermichl/Queue
 */

#ifndef QUEUE_H
#define QUEUE_H

#include "QueueCfg.h"

// ---------------------- defines ---------------------------------------------------------

// ---------------------- types -----------------------------------------------------------
struct Queue_QueueStruct;

#if QUEUE_LOCK_CALLBACK
typedef void (*Queue_CallbackLock)(struct Queue_QueueStruct *queuePtr);
#endif
#if QUEUE_UNLOCK_CALLBACK
typedef void (*Queue_CallbackUnlock)(struct Queue_QueueStruct *queuePtr);
#endif

typedef struct Queue_QueueEntryStruct
{
    struct Queue_QueueEntryStruct* next;
#if !QUEUE_ONE_SIZE_TO_RULE_THEM_ALL
    size_t elementSize;
#endif
    void* data;
}
Queue_QueueEntryStruct;

typedef struct Queue_QueueStruct
{
    int count;
#if QUEUE_ONE_SIZE_TO_RULE_THEM_ALL
    size_t elementSize;
#endif
#if QUEUE_THREAD_SAFE
    bool inUse;
# if QUEUE_LOCK_CALLBACK
    Queue_CallbackLock lockCb; /**< If needed, must be set directly in the struct after the queue has been initialized. */
# endif
# if QUEUE_UNLOCK_CALLBACK
    Queue_CallbackLock unlockCb; /**< If needed, must be set directly in the struct after the queue has been initialized. */
# endif
#endif
    Queue_QueueEntryStruct *head;
    Queue_QueueEntryStruct *tail;
}
Queue_QueueStruct;

// ---------------------- constants -------------------------------------------------------

// ---------------------- variables -------------------------------------------------------

// ---------------------- function declarations -------------------------------------------

#if QUEUE_ONE_SIZE_TO_RULE_THEM_ALL
/**
 * @brief Initializes a queue.
 * 
 * @param queuePtr Pointer to a queue structure.
 * @param elementSize Amount of bytes per element.
 * 
 * @return true 
 * @return false 
 */
extern bool Queue_Init(Queue_QueueStruct *queuePtr, size_t elementSize);
#else
/**
 * @brief Initializes a queue.
 * 
 * @param queuePtr Pointer to a queue structure.
 * 
 * @return true 
 * @return false 
 */
extern bool Queue_Init(Queue_QueueStruct *queuePtr);
#endif

/**
 * @brief Gets the amount of elements in the queue.
 * 
 * @param queuePtr Pointer to a queue structure.
 * 
 * @return Queue length.
 */
extern size_t Queue_Length(Queue_QueueStruct *queuePtr);

/**
 * @brief Checks if a queue is empty.
 * 
 * @param queuePtr Pointer to a queue structure.
 * 
 * @return true 
 * @return false 
 */
extern bool Queue_IsEmpty(Queue_QueueStruct *queuePtr);

#if !QUEUE_ONE_SIZE_TO_RULE_THEM_ALL
/**
 * @brief Gets the element size of the next element in the queue.
 * 
 * @param queuePtr Pointer to a queue structure.
 * 
 * @return Size of the next element in the queue. 0 if the queue is empty.
 */
extern size_t Queue_NextElementSize(Queue_QueueStruct *queuePtr);
#endif

#if QUEUE_ONE_SIZE_TO_RULE_THEM_ALL
/**
 * @brief Inserts a new element into a queue. CAUTION: Queue_Enqueue only copies the
 *        provided data structure. If the data structure contains pointers to external
 *        data or dynamically allocated data, the data is not copied. No deep copy is
 *        performed.
 * 
 * @param queuePtr Pointer to a queue structure.
 * @param dataPtr  Pointer to the data to insert.
 * 
 * @return true 
 * @return false 
 */
extern bool Queue_Enqueue(Queue_QueueStruct* queuePtr, const void* dataPtr);
#else
/**
 * @brief Inserts a new element into a queue. CAUTION: Queue_Enqueue only copies the
 *        provided data structure. If the data structure contains pointers to external
 *        data or dynamically allocated data, the data is not copied. No deep copy is
 *        performed.
 * 
 * @param queuePtr    Pointer to a queue structure.
 * @param dataPtr     Pointer to the data to insert.
 * @param elementSize Size of the provided data in bytes.
 * 
 * @return true 
 * @return false 
 */
extern bool Queue_Enqueue(Queue_QueueStruct* queuePtr, const void* dataPtr, const size_t elementSize);
#endif

/**
 * @brief Gets the next element from a queue.
 * 
 * @param queuePtr Pointer to a queue structure.
 * @param dataPtr  Pointer to the allocated memory to which the data from the queue
 *                 shall be copied.
 * 
 * @return true 
 * @return false 
 */
extern bool Queue_Dequeue(Queue_QueueStruct *queuePtr, void* dataPtr);

/**
 * @brief Purges the next element.
 * 
 * @param queuePtr Pointer to a queue structure.
 * 
 * @return true 
 * @return false 
 */
extern bool Queue_Purge(Queue_QueueStruct *queuePtr);

// ---------------------- function implementations ----------------------------------------

#endif
