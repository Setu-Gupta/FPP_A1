#include <stamp.h>

void create_thread(pthread_t &thread, std::function<void()> &&lambda)
{
        auto _lambda = [&]() -> void* {
                lambda();
                return nullptr;
        };

        auto wrapper = [](void* data) -> void* {
                return (*static_cast<decltype(_lambda)*>(data))();
        };

        int error = pthread_create(&thread, NULL, wrapper, &_lambda);
        if(error)
        {
                std::cerr << "Failed to create thread: " << std::strerror(error);
                std::exit(-1);
        }
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
        pthread_t thread1, thread2;
        create_thread(thread1, std::move(lambda1));
        create_thread(thread2, std::move(lambda2));

        join_thread(thread1);
        join_thread(thread2);
}

// parallel_for accepts a C++11 lambda function and runs the loop body (lambda) in 
// parallel by using ‘numThreads’ number of Pthreads created inside StaMp
void stamp::parallel_for(int low, int high, int stride, std::function<void(int)> &&lambda, int numThreads)
{
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
}

// Shorthand for using parallel_for if for both outer and inner for-loops, lowbound is zero and stride is one.
// In that case only highBounds for both these loop should be sufficient to provide. The suffixes “1” and “2”
// represents outter and inner loop properties respectively.
void stamp::parallel_for(int high1, int high2, std::function<void(int, int)> &&lambda, int numThreads)
{
        stamp::parallel_for(0, high1, 1, 0, high2, 1, std::move(lambda), numThreads);
}
