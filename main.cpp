#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <thread>
#include <mutex>

// Mutex for thread-safe access to max_sum
std::mutex mtx;

// Function to calculate the maximum security value sum for a range of starting nodes
void calculateMaxForRange(const std::vector<int>& aSecValues, int aJumpSize, int aStart, int aEnd, int& aGlobalMax)
{
    int local_max = std::numeric_limits<int>::min();
    int n = aSecValues.size();

    for (int i = aStart; i < aEnd; ++i) {
        int current_sum = 0;
        for (int j = i; j < n; j += aJumpSize) {
            current_sum += aSecValues[j];
        }
        local_max = std::max(local_max, current_sum);
    }

    // Update global maximum in a thread-safe way
    std::lock_guard<std::mutex> lock(mtx);
    aGlobalMax = std::max(aGlobalMax, local_max);
}

// Function to calculate the maximum security value sum using multithreading
int gainMaxValue(const std::vector<int>& aSecValues, int aJumpSize) 
{
    int n = aSecValues.size();
    int global_max = std::numeric_limits<int>::min();

    // Number of threads to use
    int num_threads = std::min(n, 4); // Use up to 4 threads
    std::vector<std::thread> threads;

    // Divide work among threads
    int chunk_size = (n + num_threads - 1) / num_threads;
    for (int t = 0; t < num_threads; ++t) {
        int start = t * chunk_size;
        int end = std::min(start + chunk_size, n);
        threads.emplace_back(calculateMaxForRange, std::cref(aSecValues), aJumpSize, start, end, std::ref(global_max));
    }

    // Join all threads
    for (auto& thread : threads) {
        thread.join();
    }

    return global_max;
}

// Function to generate random security values
std::vector<int> generateRandomSecurityValues(int aAmount, int aMin = -1000, int aMax = 1000) 
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(aMin, aMax);

    std::vector<int> security_val(aAmount);
    for (int i = 0; i < aAmount; ++i) 
    {
        security_val[i] = dis(gen);
    }
    return security_val;
}

int main() {
    int n = 10; 
    auto securityVal = generateRandomSecurityValues(n, -100, 100);
    int k = 1;

    // Output generated security values
    std::cout << "Security values: ";
    for (int val : securityVal) {
        std::cout << val << " ";
    }
    std::cout << std::endl;

    // Calculate and output the result
    int result = gainMaxValue(securityVal, k);
    std::cout << "Maximum security value sum: " << result << std::endl;

    return 0;
}
