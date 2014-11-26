/**
 * threadpool.h
 *
 * This file declares the functionality associated with
 * your implementation of a threadpool.
 */

// maximum number of threads allowed in a pool
#define MAXT_IN_POOL 200

// You must hide the internal details of the threadpool
// structure from callers, thus declare threadpool of type "void".
// In threadpool.c, you will use type conversion to coerce
// variables of type "threadpool" back and forth to a
// richer, internal type.  (See threadpool.c for details.)

typedef void *threadpool;

// "dispatch_fn" declares a typed function pointer.  A
// variable of type "dispatch_fn" points to a function
// with the following signature:
// 
//     void dispatch_function(void *arg);

typedef void (*dispatch_fn)(void *);

/**
 * create_threadpool creates a fixed-sized thread
 * pool.  If the function succeeds, it returns a (non-NULL)
 * "threadpool", else it returns NULL.
 */
threadpool create_threadpool(int num_threads_in_pool);

/**
 * dispatch sends a thread off to do some work.  If
 * all threads in the pool are busy, dispatch will
 * block until a thread becomes free and is dispatched.
 * 
 * Once a thread is dispatched, this function returns
 * immediately.
 * 
 * The dispatched thread calls into the function
 * "dispatch_to_here" with argument "arg".
 */
void dispatch(threadpool from_me, dispatch_fn dispatch_to_here,
	      void *arg);

/**
 * Works the same as dispatch, but enables the user to define cleanup handlers in 
 * cases of immediate cancel.  The cleanup handler function (cleaner_func) is 
 * executed automatically with cleaner_arg as the argument after the 
 * dispatch_to_here function is done; in the case that destroy_threadpool_immediately
 * is called, cleaner_func is also called before the thread exits.
 */
void dispatch_with_cleanup(threadpool from_me, dispatch_fn dispatch_to_here,
			   void * arg, dispatch_fn cleaner_func, void* cleaner_arg);

/**
 * destroy_threadpool kills the threadpool, causing
 * all threads in it to commit suicide, and then
 * frees all the memory associated with the threadpool.
 */
void destroy_threadpool(threadpool destroyme);

/**
 * destroy_threadpool_immediately cancels all threads in the threadpool immediately.
 * It is potentially dangerous to use with libraries that are not specifically 
 * asynchronous cancel thread safe.  Also, without cleanup handlers, any dynamic 
 * memory or system resource in use could potentially be left without a reference
 * to it. 
 */
void destroy_threadpool_immediately(threadpool destroymenow);