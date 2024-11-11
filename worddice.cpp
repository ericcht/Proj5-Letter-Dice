#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <queue>
#include <set>

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

void buildGraph(const vector<string> &dice, const string &word, unordered_map<int, set<int>> &graph)
{
    int source = 0;
    int sink = word.size() + dice.size();

    for (int i = 0; i < dice.size(); ++i)
    {
        graph[source].insert(i + 1); // each dice is a node starting from node 1
    }

    for (int i = 0; i < word.size(); ++i)
    {
        char c = word[i]; // letter in the word
        for (int j = 0; j < dice.size(); ++j)
        {
            if (dice[j].find(c) != string::npos)
            {
                graph[j + 1].insert(word.size() + 1 + i);
            }
        }
    }
    for (int i = 0; i < word.size(); i++)
    {
        graph[dice.size() + 1 + i].insert(sink);
    }
}

void printGraph(const unordered_map<int, set<int>> &graph, const vector<string> &dice, const vector<string> &word)
{
    int sink = dice.size() + word.size();
    int source = 0; // Source node is 0

    // Print edges from source
    cout << "Node " << source << ": SOURCE Edges to ";
    for (int i = 0; i < dice.size(); ++i)
    {
        cout << (i + 1) << (i < dice.size() - 1 ? " " : "\n");
    }

    // Print edges from each die to the letters
    for (int i = 0; i < dice.size(); ++i)
    {
        cout << "Node " << (i + 1) << ": " << dice[i] << " Edges to ";
        for (int neighbor : graph.at(i + 1))
        {
            cout << neighbor << " ";
        }
        cout << endl;
    }

    // Print edges from each letter to sink
    for (int i = 0; i < word.size(); ++i)
    {
        cout << "Node " << (dice.size() + 1 + i) << ": " << word[i] << " Edges to " << sink << endl;
    }

    // Print sink node
    cout << "Node " << sink << ": SINK Edges to" << endl;
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

    for (int i = 0; i < words.size(); ++i)
    {
        string word = words[i];
        buildGraph(dice, word, graph);
        // Print the graph structure for debugging
        cout << "Graph for word: " << word << endl;
        printGraph(graph, dice, words);
        cout << endl;
    }
}