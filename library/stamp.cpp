#include <stamp.h>

inline void create_thread(pthread_t &thread, void* (*fptr)(void*))
{
        int error = pthread_create(&thread, NULL, fptr, NULL);
        if(error)
        {
                std::cerr << "Failed to create thread: " << std::strerror(error);
                std::exit(-1);
        }
}

void create_thread_no_arg(pthread_t &thread, std::function<void()> &&lambda)
{
        std::function<void*(void*)> func = [=](void*) -> void*
                {
                        lambda();
                        return nullptr;
                };
        create_thread(thread, (void*(*)(void*))&func);
}

void create_thread_int(pthread_t &thread, std::function<void(int)> &&lambda, int arg)
{
        
        std::function<void*(void*)> func = [=](void*) -> void*
                {
                        lambda(arg);
                        return nullptr;
                };
        create_thread(thread, (void*(*)(void*))&func);
}

void create_thread_int_int(pthread_t &thread, std::function<void(int, int)> &&lambda, int arg1, int arg2)
{
        std::function<void*(void*)> func = [=](void*) -> void*
                {
                        lambda(arg1, arg2);
                        return nullptr;
                };
        create_thread(thread, (void*(*)(void*))&func);
}

void join_thread(pthread_t &thread)
{
        int error = pthread_join(thread, NULL);
        if(error)
        {
                std::cerr << "Failed to join thread: " << std::strerror(error);
                std::exit(-1);
        }
}

// accepts two C++11 lambda functions and runs the them in parallel
void stamp::execute_tuple(std::function<void()> &&lambda1, std::function<void()> &&lambda2)
{
        auto start = std::chrono::high_resolution_clock::now();

        pthread_t thread1, thread2;
        create_thread_no_arg(thread1, std::move(lambda1));
        create_thread_no_arg(thread2, std::move(lambda2));

        join_thread(thread1);
        join_thread(thread2);
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        std::cout << "StaMp Statistics: Threads = 2, Parallel execution time = " << (float)duration.count()/1000 << " seconds" << std::endl;
}

// parallel_for accepts a C++11 lambda function and runs the loop body (lambda) in 
// parallel by using ‘numThreads’ number of Pthreads created inside StaMp
void stamp::parallel_for(int low, int high, int stride, std::function<void(int)> &&lambda, int numThreads)
{
        auto start = std::chrono::high_resolution_clock::now();
        
        int count = (high - low)/stride;
        pthread_t* threads = (pthread_t*)malloc(count * sizeof(pthread_t));

        int running_threads = 0;
        int next_thread_to_join = 0;
        int arg = low;
        for(int i = 0; i < count; i += 1)
        {
                create_thread_int(threads[i], std::move(lambda), arg);
                arg += stride;
                running_threads++;

                // Join older threads if running threads is greater than or equal to maximum allowed threads
                if(running_threads >= numThreads)
                {
                        join_thread(threads[next_thread_to_join++]);
                        running_threads--;
                }
        }
        
        for(int i = next_thread_to_join; i < count; i += 1)
                join_thread(threads[i]);
        
        free(threads);
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        std::cout << "StaMp Statistics: Threads = " << numThreads << ", Parallel execution time = " << (float)duration.count()/1000 << " seconds" << std::endl;
}

// Shorthand for using parallel_for when lowbound is zero and stride is one.
void stamp::parallel_for(int high, std::function<void(int)> &&lambda, int numThreads)
{
        stamp::parallel_for(0, high, 1, std::move(lambda), numThreads);
}

// This version of parallel_for is for parallelizing two-dimensional for-loops, i.e., an outter for-i loop and 
// an inner for-j loop. Loop properties, i.e. low, high, and stride are mentioned below for both outter 
// and inner for-loops. The suffixes “1” and “2” represents outter and inner loop properties respectively. 
void stamp::parallel_for(int low1, int high1, int stride1, int low2, int high2, int stride2, std::function<void(int, int)> &&lambda, int numThreads)
{
        auto start = std::chrono::high_resolution_clock::now();
        
        int outer_count = (high1 - low1)/stride1;
        int inner_count = (high2 - low2)/stride2;

        pthread_t* threads = (pthread_t*)malloc(outer_count * inner_count * sizeof(pthread_t));
        
        int running_threads = 0;
        int next_thread_to_join = 0;
        int outer_arg = low1;
        for(int i = 0; i < outer_count; i += 1)
        {
                int inner_arg = low2;
                for(int  j = 0; j < inner_count; j++)
                {
                        create_thread_int_int(threads[(i * inner_count) + j], std::move(lambda), outer_arg, inner_arg);
                        inner_arg += stride2;
                        running_threads++;
                
                        // Join older threads if running threads is greater than or equal to maximum allowed threads
                        if(running_threads >= numThreads)
                        {
                                join_thread(threads[next_thread_to_join++]);
                                running_threads--;
                        }
                }
                outer_arg += stride1;
        }
        
        for(int i = next_thread_to_join; i < outer_count * inner_count; i += 1)
                join_thread(threads[i]);
        
        free(threads);
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        std::cout << "StaMp Statistics: Threads = " << numThreads << ", Parallel execution time = " << (float)duration.count()/1000 << " seconds" << std::endl;
}

// Shorthand for using parallel_for if for both outer and inner for-loops, lowbound is zero and stride is one.
// In that case only highBounds for both these loop should be sufficient to provide. The suffixes “1” and “2”
// represents outter and inner loop properties respectively.
void stamp::parallel_for(int high1, int high2, std::function<void(int, int)> &&lambda, int numThreads)
{
        stamp::parallel_for(0, high1, 1, 0, high2, 1, std::move(lambda), numThreads);
}
