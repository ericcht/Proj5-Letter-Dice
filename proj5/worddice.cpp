// Alex Chen & Eric Chtilianov
/* The program reads a list of dice and words, then constructs a flow network to check if each word can be spelled using the dice. 
It uses the Edmonds-Karp algorithm to find the max flow and determines if the word can be fully spelled by matching letters to dice.*/ 
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <queue>
#include <climits>

using namespace std;

// bfs to find augmenting path in residual graph
bool bfs(const unordered_map<int, vector<int>> &graph, unordered_map<int, unordered_map<int, int>> &capacity,
         int source, int sink, unordered_map<int, int> &parent) {
    queue<int> q;
    q.push(source);
    parent.clear();
    parent[source] = -1;

    while(!q.empty()) {
        int u = q.front();
        q.pop();

        if(graph.find(u) == graph.end()) continue; // skip if node not in graph

        for(int v: graph.at(u)) {
            // check if node not visited and has available capacity
            if(parent.find(v) == parent.end() && capacity[u][v] > 0) {
                parent[v] = u;
                if(v == sink) return true; // path to sink found
                q.push(v);
            }
        }
    }
    return false; // no path found
}

// build initial graph and capacity matrix
void buildGraph(const vector<string> &dice, const string &word, unordered_map<int, vector<int>> &graph,
                unordered_map<int, unordered_map<int, int>> &capacity) {
    int source = 0;
    int sink = word.size() + dice.size() + 1;

    // connect source to dice nodes
    for(int i = 0; i < dice.size(); ++i) {
        graph[source].push_back(i + 1);
        graph[i + 1].push_back(source);
        capacity[source][i + 1] = 1;
        capacity[i + 1][source] = 0;
    }

    // connect dice nodes to letter nodes if match found
    for(int i = 0; i < word.size(); ++i) {
        char c = word[i];
        for(int j = 0; j < dice.size(); ++j) {
            if(dice[j].find(c) != string::npos) {
                graph[j + 1].push_back(dice.size() + 1 + i);
                graph[dice.size() + 1 + i].push_back(j + 1);
                capacity[j + 1][dice.size() + 1 + i] = 1;
                capacity[dice.size() + 1 + i][j + 1] = 0;
            }
        }
    }

    // connect letter nodes to sink
    for(int i = 0; i < word.size(); ++i) {
        int letterNode = dice.size() + 1 + i;
        graph[letterNode].push_back(sink);
        graph[sink].push_back(letterNode);
        capacity[letterNode][sink] = 1;
        capacity[sink][letterNode] = 0;
    }
}

// edmonds-karp algorithm to find max flow
int edmondsKarp(unordered_map<int, vector<int>> &graph, unordered_map<int, unordered_map<int, int>> &capacity,
                int source, int sink, vector<int> &path, int wordLength, int diceSize) {
    int maxFlow = 0;
    unordered_map<int, int> parent;
    path.clear();
    vector<int> diceOrder(wordLength, -1);

    // repeat bfs until no more augmenting paths
    while(bfs(graph, capacity, source, sink, parent)) {
        int pathFlow = INT_MAX;
        // find minimum capacity in the path
        for(int v = sink; v != source; v = parent[v]) {
            int u = parent[v];
            pathFlow = min(pathFlow, capacity[u][v]);
        }

        // update residual capacities
        for(int v = sink; v != source; v = parent[v]) {
            int u = parent[v];
            capacity[u][v] -= pathFlow;
            capacity[v][u] += pathFlow;
        }

        // record dice usage in order
        for(int v = sink; v != source; v = parent[v]) {
            int u = parent[v];
            if(u != source && v != sink) {
                int letterIndex = v - (diceSize + 1);
                if(letterIndex >= 0 && letterIndex < wordLength)
                    diceOrder[letterIndex] = u - 1;
            }
        }

        maxFlow += pathFlow; // increment max flow
    }

    // add dice indices to path if used
    for(int diceIndex: diceOrder)
        if(diceIndex != -1) path.push_back(diceIndex);

    return maxFlow;
}

// print result of spelling the word
void printResult(const string &word, const vector<int> &path) {
    if(path.size() == word.size()) {
        for(size_t i = 0; i < path.size(); ++i) {
            cout << path[i];
            if(i < path.size() - 1) cout << ",";
        }
        cout << ": " << word << endl;
    } else
        cout << "Cannot spell " << word << endl;
}

int main(int argc, char *argv[]) {
    if(argc != 3) {
        cerr << "usage: ./worddice dicefile wordfile" << endl;
        return 1;
    }

    string diceFile = argv[1];
    string wordsFile = argv[2];

    vector<string> dice;
    vector<string> words;
    unordered_map<int, vector<int>> graph;
    unordered_map<int, unordered_map<int, int>> capacity;

    ifstream dfin(diceFile);
    ifstream wfin(wordsFile);
    string line;

    if(!dfin.is_open() || !wfin.is_open()) {
        cerr << "Error opening input files." << endl;
        return 1;
    }

    while(getline(dfin, line)) dice.push_back(line);
    dfin.close();

    while(getline(wfin, line)) words.push_back(line);
    wfin.close();

    for(const string &word: words) {
        graph.clear();
        capacity.clear();
        buildGraph(dice, word, graph, capacity);

        int source = 0;
        int sink = word.size() + dice.size() + 1;

        vector<int> path;
        int maxFlow = edmondsKarp(graph, capacity, source, sink, path, word.size(), dice.size());
        if(maxFlow == word.size()) printResult(word, path);
        else cout << "Cannot spell " << word << endl;
    }

    return 0;
}
