/**
 * @file QueueCfg.h
 * @author Michel Vouillarmet
 * @brief Queue module configuration file.
 * @version 1.1
 * @date 2022-12-06
 * @see https://github.com/monstermichl/Queue
 */

#define QUEUE_ONE_SIZE_TO_RULE_THEM_ALL (0u) /* If this is set to anything but 0, the size for each element can only be set when the queue instance is initialized. */
#define QUEUE_THREAD_SAFE               (0u) /* If this is set to anything but 0, the queues use mutexes to be thread safe. */

/* WARNING: The following flags should only be used for debugging purpose as they can dramatically decrease performance! */
#if QUEUE_THREAD_SAFE
#define QUEUE_LOCK_CALLBACK             (0u) /* If this is set to anything but 0, a callback will be triggered for each queue lock. */
#define QUEUE_UNLOCK_CALLBACK           (0u) /* If this is set to anything but 0, a callback will be triggered for each queue unlock. */
#endif
