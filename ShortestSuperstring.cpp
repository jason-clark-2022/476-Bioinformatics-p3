// ShortestSuperstring
// Jason Clark 800617442

#include <iostream>
#include <cstring>
#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <limits>
#include <chrono>
#include <thread>
#include <queue>


using namespace std;

struct node;
void printPath(vector<string> file, vector<node> path);
vector<string> readFile(string inFile);
vector<node> findPath(vector<vector<int>> matrix, vector<int> ignore);
bool checkIgnore(vector<int> ignore, int i1, int i2);
int calculateOverlap(string s1, string s2);
pair<vector<vector<int>>, vector<int>> calculateOverlapMatrix(vector<string> data);
void printMatrix(vector<vector<int>> m);

struct node
{
    int node;
    int pointer;
    int weight;
};

int main()
{
    //cout << __cplusplus << endl;

    string f1 = "fragex1.txt";
    string f2 = "fragtext162.txt";
    string f3 = "test1.txt";
    string f4 = "test2.txt";
    
    string filename;
    cout << "Enter the name of the file you would like to run: \n>";
    cin  >> filename;

    vector<string> file = readFile(filename);

    if(file.size() != 0)
    {
        pair<vector<vector<int>>, vector<int>> vals = calculateOverlapMatrix(file); 
        vector<node> path = findPath(vals.first, vals.second);
        printPath(file, path);
    }

    return 0;
}

void printPath(vector<string> file, vector<node> path)
{
    string spaces = "";
    string superstring = "";
    
    cout << "Fragment Alignments: \n";
    cout << file[path[0].node] << endl;
    superstring += file[path[0].node];
    
    for(int i = 0; i < path.size(); i++)
    {
        int spaceCount = file[path[i].node].size() - path[i].weight;
        for(int j = 0; j < spaceCount; j++)
        {
            spaces += " ";
        }
        cout << spaces << file[path[i].pointer] << endl;

        string s = file[path[i].pointer];
        int front = path[i].weight;
        int back = s.size();
        string token = s.substr(front, back);
        superstring += token;
    }
    cout <<"\nSuperstring:\n" <<  superstring << endl;
}

vector<string> readFile(string inFile)
{
    vector<string> lines;
    string newLine;
    ifstream file;
    file.open(inFile);
    
    if(file.is_open())
    {
        while(getline(file, newLine))
        {
            newLine.erase(newLine.size()-1); // use for linux
            //newLine.erase(newLine.size()); // use for windows
            lines.push_back(newLine);
        }
        file.close();
    }
    else
    {
        cerr << "Error, file " << inFile << " not found\n"; 
    }
    return lines;
}

vector<node> findPath(vector<vector<int>> matrix, vector<int> ignore)
{
    vector<node> nodeList;
    bool isIgnored;

    for(int i=0; i < matrix.size(); i++)
    {
        for(int j=0; j < matrix[i].size(); j++)
        {
            isIgnored = false;
            isIgnored = checkIgnore(ignore, i, j);
            if(i == j)
                isIgnored = true;
            if(!isIgnored)
            {
                node n;
                n.node = i;
                n.pointer = j;
                n.weight = matrix[i][j];
                nodeList.push_back(n);
            }            
        }
    }

    for(int i = 0; i < nodeList.size(); i++)
    {
        for(int j = 0; j < nodeList.size(); j++)
        {
            if(nodeList[i].weight > nodeList[j].weight)
            {
                node temp = nodeList[i];
                nodeList[i] = nodeList[j];
                nodeList[j] = temp;
            }
        }
    }

    vector<bool> edgeIn(matrix.size(), false);
    vector<bool> edgeOut(matrix.size(), false);
    for(int i = 0; i < ignore.size(); i++)
    {
        int index = ignore[i];
        edgeIn[index] = true;
        edgeOut[index] = true;
    }

    vector<node> pathOfNodes;
    int sizeOfPath = matrix.size() - ignore.size() - 1;
    for(node n: nodeList)
    {
        if(pathOfNodes.size() < sizeOfPath)
        if( !(edgeIn[n.node]) && !(edgeOut[n.pointer]) )
        {
            pathOfNodes.push_back(n);
            edgeIn[n.node] = true;
            edgeOut[n.pointer] = true;
        } 
    }

    cout << endl;

    auto it1 = find(edgeOut.begin(), edgeOut.end(), false);
    int pathHelper = it1 - edgeOut.begin();

    vector<node> temp;
    for(int i = 0; i < pathOfNodes.size() + 1; i++)
    {

        for(int j = 0; j < pathOfNodes.size(); j++)
        {
            
            if(pathOfNodes[j].node == pathHelper)
            {
                temp.push_back(pathOfNodes[j]);
                pathHelper = pathOfNodes[j].pointer;
                j = pathOfNodes.size();
            }
        }
    }
    pathOfNodes = temp;
 
    return pathOfNodes;
    
}

bool checkIgnore(vector<int> ignore, int i1, int i2)
{
    bool isIgnored = false;
    for(int i = 0; i< ignore.size(); i++)
    {
        if(ignore[i] == i1 || ignore[i] == i2)
        {
            isIgnored = true;
        }
    }
    return isIgnored;
}

int calculateOverlap(string s1, string s2)
{
    int overlap = 0;
    string tokens1;
    string tokens2;
    int fronts1, backs1;
    int fronts2, backs2;

    for(int i = 0; i < s1.size(); i++)
    {
        backs1 = s1.size(); 
        fronts1 = backs1-i-1;
        if(fronts1<= 0)
            fronts1 = 0;
        tokens1 = s1.substr(fronts1, backs1);
        
        fronts2 = 0;
        backs2 = i+1;
        if(i >= s2.size())
            backs2 = s2.size();
        
        tokens2 = s2.substr(fronts2, backs2);

        if(tokens1 == tokens2)
        {
            overlap = i + 1;
        }
    }

    if(s1.find(s2) != string::npos)
    {
        overlap = -1;
    }

    return overlap;
}

pair<vector<vector<int>>, vector<int>> calculateOverlapMatrix(vector<string> data)
{
    int overlap;
    pair<vector<vector<int>>, vector<int>> retvals;
    vector<int> ignore;
    vector<vector<int>> matrix(data.size(), vector<int> (data.size(), 0));
        
    for(int i = 0; i < matrix.size(); i++)
    {
        for(int j = 0; j < matrix[i].size(); j++)
        {
            if(i == j)
            {
                matrix[i][j] = 0;
            }
            else
            {
                
                overlap = calculateOverlap(data[i], data[j]); 
                
                matrix[i][j] = overlap; 
                if(overlap == -1)
                {
                    if(!(find(ignore.begin(), ignore.end(), j) != ignore.end()))
                        ignore.push_back(j);
                }
            }
        }
    }
    cout << endl;
    cout << "Overlap Matrix:\n";
    printMatrix(matrix);

    retvals.first = matrix;
    retvals.second = ignore;
    return retvals;
}

void printMatrix(vector<vector<int>> m)
{
    for(int i = 0; i < m.size(); i++)
    {
        for(int j = 0; j < m[i].size(); j++)
        {
            cout << m[i][j] << " ";
        }
        cout << endl;
    }
} 