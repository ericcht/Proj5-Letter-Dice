#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <queue>
#include <set>
#include <climits>
#include <algorithm> // For reverse()

using namespace std;

bool bfs(const unordered_map<int, set<int>> &graph, unordered_map<int, unordered_map<int, int>> &capacity,
         int source, int sink, unordered_map<int, int> &parent)
{
    queue<int> q;
    q.push(source);
    parent.clear();
    parent[source] = -1; // Mark the source node
    while (!q.empty())
    {
        int u = q.front();
        q.pop();

        if (graph.find(u) == graph.end())
            continue;

        for (int v : graph.at(u))
        {
            // If there is remaining capacity and v is not visited
            if (parent.find(v) == parent.end() && capacity[u][v] > 0)
            {
                parent[v] = u;
                if (v == sink)
                    return true; // Found path to sink
                q.push(v);
            }
        }
    }
    return false;
}

int edmondsKarp(unordered_map<int, set<int>> &graph, unordered_map<int, unordered_map<int, int>> &capacity,
                int source, int sink, vector<int> &path, int wordLength)
{
    int maxFlow = 0;
    unordered_map<int, int> parent;
    path.clear();
    vector<int> diceOrder(wordLength, -1); // To store the dice index used for each letter in order

    while (bfs(graph, capacity, source, sink, parent))
    {
        int pathFlow = INT_MAX;
        for (int v = sink; v != source; v = parent[v])
        {
            int u = parent[v];
            pathFlow = min(pathFlow, capacity[u][v]);
        }

        // Update residual capacities of the edges and reverse edges
        for (int v = sink; v != source; v = parent[v])
        {
            int u = parent[v];
            capacity[u][v] -= pathFlow;
            capacity[v][u] += pathFlow;
        }

        // Collect the dice indices used in the path in order of appearance
        for (int v = sink; v != source; v = parent[v])
        {
            int u = parent[v];
            if (u != source && v != sink)
            {
                int letterIndex = v - (graph.size() - wordLength); // Calculate letter index from v
                diceOrder[letterIndex] = u - 1;                    // Store the dice index (0-based)
            }
        }

        maxFlow += pathFlow;
    }

    // Store the final path in the order of dice used to spell the word
    for (int diceIndex : diceOrder)
    {
        if (diceIndex != -1)
        {
            path.push_back(diceIndex);
        }
    }

    return maxFlow;
}

void buildGraph(const vector<string> &dice, const string &word, unordered_map<int, set<int>> &graph,
                unordered_map<int, unordered_map<int, int>> &capacity)
{
    int source = 0;
    int sink = word.size() + dice.size() + 1;

    for (int i = 0; i < dice.size(); ++i)
    {
        graph[source].insert(i + 1);
        capacity[source][i + 1] = 1; // Capacity from source to dice node
    }

    for (int i = 0; i < word.size(); ++i)
    {
        char c = word[i];
        for (int j = 0; j < dice.size(); ++j)
        {
            if (dice[j].find(c) != string::npos)
            {
                graph[j + 1].insert(dice.size() + 1 + i);
                capacity[j + 1][dice.size() + 1 + i] = 1; // Capacity from dice to letter node
            }
        }
    }

    for (int i = 0; i < word.size(); ++i)
    {
        graph[dice.size() + 1 + i].insert(sink);
        capacity[dice.size() + 1 + i][sink] = 1; // Capacity from letter node to sink
    }
}

void printResult(const string &word, const vector<int> &path)
{
    if (path.size() == word.size())
    {
        for (size_t i = 0; i < path.size(); ++i)
        {
            cout << path[i];
            if (i < path.size() - 1)
                cout << ",";
        }
        cout << ": " << word << endl;
    }
    else
    {
        cout << "Cannot spell " << word << endl;
    }
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        cerr << "usage: ./worddice dicefile wordfile" << endl;
        return 1;
    }

    string diceFile = argv[1];
    string wordsFile = argv[2];

    vector<string> dice;
    vector<string> words;
    unordered_map<int, set<int>> graph;
    unordered_map<int, unordered_map<int, int>> capacity;

    ifstream dfin(diceFile);
    ifstream wfin(wordsFile);
    string line;

    // Read dice file
    while (getline(dfin, line))
    {
        dice.push_back(line);
    }
    dfin.close();

    // Read words file
    while (getline(wfin, line))
    {
        words.push_back(line);
    }
    wfin.close();

    for (const string &word : words)
    {
        graph.clear();
        capacity.clear();
        buildGraph(dice, word, graph, capacity);

        int source = 0;
        int sink = word.size() + dice.size() + 1;

        vector<int> path;
        int maxFlow = edmondsKarp(graph, capacity, source, sink, path, word.size());
        if (maxFlow == word.size())
        {
            printResult(word, path);
        }
        else
        {
            cout << "Cannot spell " << word << endl;
        }
    }

    return 0;
}
