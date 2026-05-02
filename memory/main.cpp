#include <iostream>
#include <cstdlib>
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>

using namespace std;

double getTimeSeconds()
{
    timeval tv;
    gettimeofday(&tv, nullptr);
    return tv.tv_sec + tv.tv_usec / 1000000.0;
}

void getFaults(long &minorFaults, long &majorFaults)
{
    rusage usage;
    getrusage(RUSAGE_SELF, &usage);

    minorFaults = usage.ru_minflt;
    majorFaults = usage.ru_majflt;
}

void runWorkload(char *memory, size_t size)
{
    volatile unsigned long long sum = 0;

    for (size_t i = 0; i < size; i += 64)
    {
        memory[i] = static_cast<char>(memory[i] + 1);
        sum += memory[i];
    }

    if (sum == 123456789)
    {
        cout << "This will probably never print: " << sum << endl;
    }
}

void pretouchMemory(char *memory, size_t size, size_t pageSize)
{
    for (size_t i = 0; i < size; i += pageSize)
    {
        memory[i] = 0;
    }
}

void testNormalAllocation(size_t size)
{
    long minorBefore;
    long majorBefore;
    long minorAfter;
    long majorAfter;

    char *memory = new char[size];

    getFaults(minorBefore, majorBefore);

    double start = getTimeSeconds();
    runWorkload(memory, size);
    double end = getTimeSeconds();

    getFaults(minorAfter, majorAfter);

    cout << "=== Normal Allocation ===" << endl;
    cout << "Workload time: " << end - start << " seconds" << endl;
    cout << "Minor page faults during workload: "
         << minorAfter - minorBefore << endl;
    cout << "Major page faults during workload: "
         << majorAfter - majorBefore << endl << endl;

    delete[] memory;
}

void testOptimizedAllocation(size_t size, size_t pageSize)
{
    long minorBefore;
    long majorBefore;
    long minorAfter;
    long majorAfter;

    char *memory = new char[size];

    pretouchMemory(memory, size, pageSize);

    getFaults(minorBefore, majorBefore);

    double start = getTimeSeconds();
    runWorkload(memory, size);
    double end = getTimeSeconds();

    getFaults(minorAfter, majorAfter);

    cout << "=== Optimized With Page Pre Touching ===" << endl;
    cout << "Workload time: " << end - start << " seconds" << endl;
    cout << "Minor page faults during workload: "
         << minorAfter - minorBefore << endl;
    cout << "Major page faults during workload: "
         << majorAfter - majorBefore << endl << endl;

    delete[] memory;
}

int main()
{
    size_t pageSize = getpagesize();

    size_t megabytes = 512;
    size_t size = megabytes * 1024 * 1024;

    cout << "Low Level Memory Optimization Demo" << endl;
    cout << "----------------------------------" << endl;
    cout << "Memory size: " << megabytes << " MB" << endl;
    cout << "System page size: " << pageSize << " bytes" << endl << endl;

    testNormalAllocation(size);
    testOptimizedAllocation(size, pageSize);

    cout << "The optimized version touches each memory page before the timed workload." << endl;
    cout << "This reduces page faults during the actual workload section." << endl;

    return 0;
}