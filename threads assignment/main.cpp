#include <iostream>
#include <fstream>
#include <vector>
#include <thread>
#include <mutex>
#include <string>

using namespace std;

struct ThreadResult
{
    int threadId;
    int startNode;
    int endNode;
    int localEdgeCount;
    int localMaxDegree;
    int localMaxNode;
    int localIsolatedCount;
};

vector<vector<int> > graphData;

// Shared data
int sharedTotalEdges = 0;
int sharedMaxDegree = -1;
int sharedMaxNode = -1;
int sharedIsolatedCount = 0;

// Mutexes
mutex statsMutex;
mutex printMutex;

bool loadGraph(const string& fileName)
{
    ifstream fin(fileName.c_str());
    if (!fin)
    {
        return false;
    }

    int numberOfNodes;
    int numberOfEdges;
    fin >> numberOfNodes >> numberOfEdges;

    if (!fin || numberOfNodes <= 0)
    {
        return false;
    }

    graphData.clear();
    graphData.resize(numberOfNodes);

    int from;
    int to;

    for (int i = 0; i < numberOfEdges; i++)
    {
        fin >> from >> to;

        if (fin && from >= 0 && from < numberOfNodes && to >= 0 && to < numberOfNodes)
        {
            graphData[from].push_back(to);
        }
    }

    fin.close();
    return true;
}

void processGraphSection(int threadId, int startNode, int endNode)
{
    ThreadResult result;
    result.threadId = threadId;
    result.startNode = startNode;
    result.endNode = endNode;
    result.localEdgeCount = 0;
    result.localMaxDegree = -1;
    result.localMaxNode = -1;
    result.localIsolatedCount = 0;

    for (int i = startNode; i <= endNode; i++)
    {
        int degree = graphData[i].size();
        result.localEdgeCount += degree;

        if (degree == 0)
        {
            result.localIsolatedCount++;
        }

        if (degree > result.localMaxDegree)
        {
            result.localMaxDegree = degree;
            result.localMaxNode = i;
        }
    }

    // Critical section: updating shared statistics
    statsMutex.lock();

    sharedTotalEdges += result.localEdgeCount;
    sharedIsolatedCount += result.localIsolatedCount;

    if (result.localMaxDegree > sharedMaxDegree)
    {
        sharedMaxDegree = result.localMaxDegree;
        sharedMaxNode = result.localMaxNode;
    }

    statsMutex.unlock();

    // Critical section: printing to console so output does not mix together
    printMutex.lock();

    cout << "Thread " << result.threadId
         << " processed nodes " << result.startNode
         << " to " << result.endNode << endl;

    cout << "  Local edge count: " << result.localEdgeCount << endl;
    cout << "  Local max degree: " << result.localMaxDegree
         << " at node " << result.localMaxNode << endl;
    cout << "  Local isolated nodes: " << result.localIsolatedCount << endl;
    cout << endl;

    printMutex.unlock();
}

int main()
{
    string fileName;
    cout << "Enter graph file name: ";
    cin >> fileName;

    if (!loadGraph(fileName))
    {
        cout << "Error: could not open or read graph file." << endl;
        return 1;
    }

    int numberOfThreads;
    cout << "Enter number of threads: ";
    cin >> numberOfThreads;

    if (numberOfThreads <= 0)
    {
        cout << "Error: number of threads must be positive." << endl;
        return 1;
    }

    int totalNodes = graphData.size();

    if (numberOfThreads > totalNodes)
    {
        numberOfThreads = totalNodes;
    }

    vector<thread> workers;

    int baseChunk = totalNodes / numberOfThreads;
    int extra = totalNodes % numberOfThreads;

    int currentStart = 0;

    for (int i = 0; i < numberOfThreads; i++)
    {
        int nodesForThisThread = baseChunk;
        if (i < extra)
        {
            nodesForThisThread++;
        }

        int currentEnd = currentStart + nodesForThisThread - 1;

        workers.push_back(thread(processGraphSection, i + 1, currentStart, currentEnd));

        currentStart = currentEnd + 1;
    }

    for (int i = 0; i < (int)workers.size(); i++)
    {
        workers[i].join();
    }

    cout << "========== FINAL SHARED RESULTS ==========" << endl;
    cout << "Total nodes: " << totalNodes << endl;
    cout << "Total edges: " << sharedTotalEdges << endl;
    cout << "Node with highest out-degree: " << sharedMaxNode << endl;
    cout << "Highest out-degree value: " << sharedMaxDegree << endl;
    cout << "Total isolated nodes: " << sharedIsolatedCount << endl;

    return 0;
}