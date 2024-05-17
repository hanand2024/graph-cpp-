
//Author - Anand
//May 2024

//Hello there. How can you know that this is original code? Check the part where I have added the check for complete graph. Most of the other submissions here won't have that :)) !
//No programmer is perfect. Happy to hear your feedback and make fixes.

//Though the assignment requires reading from a file, this program develops a random graph based on user inputs - this has been done to make it easy to test the program.
//The program writes out the graph thus generated

//It computes Djikstra minimum distances from a user chosen source node
//It also computes the Minimum Spanning Tree for color of edge chosen by user using Prim's algorithm

#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <limits>
#include <algorithm> // Required for std::min_element
#include <utility> // Required for std::make_pair

using namespace std;

class Graph{
private:
    int V; //number of vertices
    const int INF;
    
public:
    const int V_COUNT;
    vector<vector <int> > vector_adj;
    vector<vector <int> > vector_dist;
    vector<vector <int> > vector_color; //Assign different colors to the edges
    
    //keeping track of edges found
    vector<vector<int> > selected_edges;
    
    //finding all edges in the graph for Kruskal
    vector<vector <int> > vector_edges;
    
    //constructor
    // Define infinity as a very large value in the constructor
    Graph(int vertices): V(vertices), V_COUNT(vertices), INF(numeric_limits<int>::max()){
        vector_adj.resize(V); //Resize outer vector to have V rows
        vector_dist.resize(V); //Resize outer vector to have V rows
        vector_color.resize(V); //Resize outer vector to have V rows
        // Resize each inner vector (row) to have V columns
        for (int i = 0; i < V; ++i) {
            vector_adj[i].resize(V);
            vector_dist[i].resize(V);
            vector_color[i].resize(V);
        }
        cout<< "Constructor called and resized as per given input " << V <<endl;
    };
    
    //Function to add an edge to the graph
    void addEdge(int u,int v, int distance, int edge_exists, int color){
        cout<< "u,v = " <<u<<v <<endl;
        cout<< "v,u = " <<v<<u <<endl;
        vector_adj[u][v] = vector_adj[v][u] = edge_exists;
        vector_dist[u][v] = vector_dist[v][u] = distance;
        vector_color[u][v] = vector_color[v][u] = color;
    }
    
    //Function to generate random graph
    //Note that the graph has to be complete which means no node should be left isolated.
    void generateRandomGraph(float density, int distanceRange, int numColors){
        srand(time(NULL));
        
        for(int i=0; i<V;++i){
            cout << "i =" << i <<endl;
            for(int j=i+1; j<V; ++j){
                cout << "J = " <<j <<endl;
                //cout <<rand() << " " << RAND_MAX << " " << (float) (rand()/RAND_MAX) << endl; //Note: Always prefer the C++ style of casting over C style casting using (type)
                //In the below If statement, only the rand() part is typecasted. If rand()/RAND_MAX is casted, it will always return 0 since it is an integer division
                int distance = 0;
                int color = 0;
                if((static_cast<double>(rand())/RAND_MAX < density)){
                    if(i!=j){
                        distance = 1 + rand() % distanceRange; // Random distance between 1 and distanceRange
                        color = 1 + rand() % numColors; // Random distance between 1 and number of Colors
                    }
                    addEdge(i,j,distance,1, color);
                    cout<< "Added Edge (" << i << ", "<< j << ") with distance = " << distance << endl;
                }else{
                    addEdge(i,j,distance,0,0);
                    cout<< "No Edge added between (" << i << ", "<< j << ") " << endl;
                }
            }
            
            //Check that the node has at least one connection to avoid incomplete graphs. Else redo the loop
            vector<int> isolated_node(V_COUNT,0);
            if (vector_adj[i]==isolated_node){
                cout<<"Isolated node formed at i = "<< i <<endl;
                i = i-1;
                cout<< "Reset i to "<< i <<endl;
            }
        }
    }
    
    //Function to print the Graph for debugging purposes
    template <typename T>
    void printGraph(const vector<vector <T> > array){
        for (int i=0;i<V;i++){
            cout << "Row "<< i << ": " ;
            for(int j=0; j<V;++j){
                cout << array[i][j];
            }
            cout << endl;
        }
        cout << endl;
    }
    
    //Function to implement Djiskstra's algorithm from a given source vertex
    vector<int> djikstra(const vector<vector<int> >& connections, const vector<vector<int> >& distances, int src){
        int n = connections.size(); //number of vertices
        // Initialize distances to all vertices as infinity and all visited to false
        vector<int> dist(n,INF);
        vector<bool> visited(n,0);
        
        //Set distance from source to itself as 0
        dist[src] =0;
        
        //Loop through all vertices/nodes
        for(int i=0;i<n;++i){
            int minDist = INF;
            int minIndex = -1;
            for(int j=0;j<n;j++){
                if(!visited[j] && dist[i]<minDist){ // Find the vertex with the minimum distance that is not yet visited
                    minDist = dist[j];
                    minIndex = j;
                }
            }
            
            // Update distances to all adjacent vertices of the visited vertex
            if (minIndex!=-1){
                visited[minIndex] = 1; // Mark the vertex as visited
                for(int v=0;v<n;++v){
                    if(connections[minIndex][v]==1){
                        dist[v]=min(dist[v],dist[minIndex]+distances[minIndex][v]);
                    }
                }
            }
        }
        return dist;
    }
    
    void printDistances(const vector<int>& shortestDistances, int src){
        // Print the shortest distances from the source vertex to all vertices
        cout << "**** Shortest distances from vertex using Djikstra algorithm *****" <<endl;
        cout << "Shortest distances from vertex " << src << " to all other vertices:\n";
        for (int i = 0; i < shortestDistances.size(); ++i) {
            cout << "Distance to vertex " << i << " is " << shortestDistances[i] << "\n";
        }
        cout << "********************************************************************" <<endl;
    }
    
    //Find shortest edge which is of specified color and which does not loop (i.e., should not connect to an already visited loop)
    pair<int, int> findMinValueInRow(const vector<int>& row, const vector<bool>& visited, const vector<int>& vector_color_row, int color) {
        // Check if the row is empty
        int minElement = numeric_limits<int>::max();
        int columnIndex = -1;
        if (row.empty()) {
            // Return a sentinel value indicating that the row is empty
            return make_pair(minElement, -1);
        }else {
            for(int i=0;i<row.size();++i){
                if (row[i]<minElement && row[i]!=0 && visited[i]==false && vector_color_row[i]==color){
                    minElement = row[i];
                    columnIndex = i;
                }
            }
        }
        // Return a pair containing the minimum value and its column index
        return make_pair(minElement, columnIndex);
    }
    
    //Implementation of Prim Algorithm by finding shortest edges at each vertex/node
    pair<vector<int>,int> shortestEdges(vector<vector <int> >& vector_dist, int start_node,vector<vector <int> >& vector_color, int color){
        int i=0;
        vector<bool> visited(vector_dist.size(),false);
        vector<bool> all_visited(vector_dist.size(),true);
        int node = start_node;
        int mst_length = 0;
        vector<int> mst;
        cout << "Starting MST at :"<< start_node <<endl;
        mst.push_back(start_node);
        
        //keeping track of edges found
        int edge_index = 0;
        
        while(visited!=all_visited){
            auto minPair = findMinValueInRow(vector_dist[node],visited, vector_color[node], color);
            cout << "Minimum length of edge at node: " << node <<"  = "<< minPair.first << " connecting to node " << minPair.second << std::endl;
            visited[node] = true;
            if (minPair.second!=-1){
                vector<int> edge_detail;
                edge_detail.push_back(node);
                edge_detail.push_back(minPair.second);
                edge_detail.push_back(minPair.first);
                edge_detail.push_back(color);
                selected_edges.push_back(edge_detail);
                
                cout << "Going to :"<< minPair.second <<endl;
                node = minPair.second;
                mst_length += minPair.first;
                mst.push_back(minPair.second);
                
                cout <<"MST Length is now = " <<mst_length <<endl;
            }else{
                node = distance(visited.begin(), find(visited.begin(), visited.end(), false)); //Go back and find the first among the unvisited nodes;
                if (node == V_COUNT){
                    cout << "MST Complete. No more nodes to visit."<<endl << endl;
                }else{
                    cout << "Dead end or Loop. Tracking back and going to :"<< node <<endl;
                    mst.push_back(node); //Add the track back node to MST
                }
            }
        }
        return make_pair(mst,mst_length);
    }
    
    
    static void sortMatrix(std::vector<std::vector<int> >& matrix) {
            auto comparator = [](const std::vector<int>& a, const std::vector<int>& b) {
                if (a[3] != b[3]) // Compare based on fourth column - which has colors
                    return a[3] < b[3];
                else // If the first column values are equal, compare based on the third column - distances
                    return a[2] < b[2];
            };

            std::sort(matrix.begin(), matrix.end(), comparator);
    }
    
    void kruskal(vector<vector <int> >& vector_dist, int color){
        //First create the list of edges from the output of the random graph generator which is a 2-d matrix
        for(int i=0;i<vector_dist.size();++i){
            for(int j=i+1;j<vector_dist[i].size();++j){
                if(vector_dist[i][j]>0){
                    vector<int> edge;
                    edge.push_back(i);
                    edge.push_back(j);
                    edge.push_back(vector_dist[i][j]);
                    edge.push_back(vector_color[i][j]);
                    vector_edges.push_back(edge);
                }
            }
        }
        
        cout<<"Found " << vector_edges.size() << " distinct edges" <<endl;
        
        sortMatrix(vector_edges);
        
        //cout<< "Edge matrix shape is [" <<vector_edges.size()<< "," <<vector_edges[0].size()<<"]" <<endl;
        for(const auto& row:vector_edges){
            for(int val:row){
                cout<< val << "\t";
            }
            cout<< endl;
        }
        
        vector<int> mst_edges; //Minimum spanning tree edges
        int mst_length_kruskal = 0;
        
        mst_edges.push_back(vector_edges[0][0]);
        mst_edges.push_back(vector_edges[0][1]);
        mst_length_kruskal +=vector_edges[0][2];
        
        cout<<"Added edge to Kruskal ["<< vector_edges[0][0]<<" , "<< vector_edges[0][1] << "]"<< endl;
        cout<<"Kruskal MST Length = "<< mst_length_kruskal << endl;
        
        for(int i=1;i<vector_edges.size();i++){
            auto first_edge =find(mst_edges.begin(),mst_edges.end(),vector_edges[i][0]);
            auto second_edge =find(mst_edges.begin(),mst_edges.end(),vector_edges[i][1]);
            //If both edges have not been added, then it means the edge is unique and can be added to picked_edges
            if(first_edge!=mst_edges.end() && second_edge!=mst_edges.end()){
                cout<<"Edges "<<vector_edges[i][0]<< " and " << vector_edges[i][1] << " already have been added to MST. Closed loop." <<endl;
            }else{
                if(color == vector_edges[i][3]){
                    mst_length_kruskal +=vector_edges[i][2];
                    
                    mst_edges.push_back(vector_edges[i][0]);
                    mst_edges.push_back(vector_edges[i][1]);
                    cout<<"Added edge to Kruskal ["<< vector_edges[i][0]<<" , "<< vector_edges[i][1] << "]"<< endl;
                    cout<<"Kruskal MST Length = "<< mst_length_kruskal << endl;
                }
            }
        }
        cout << "MST with Kruskal's algorithm is = " ;
        int x=0;
        for (const auto val : mst_edges){
            cout << val << " ";
            if(x%2==1){cout<< ", ";}else{cout << "->";}
            x++;
        }
        cout<<endl<<"Kruskal MST Length = "<< mst_length_kruskal << endl;
    }
    
    //destructor
    ~Graph(){
        cout <<endl<< "Destructor called"<< endl;
    };
};

int main(){
    int numGraphs = 1; // Number of randomly generated graphs
    
    double edgeDensity = 0.3; // Edge density for random graphs
    int distanceRange = 9; // Distance range for random edges
    
    int src;
    int numVertices; // Number of vertices in each graph
    int numColors=1;
    int color;
    
    cout<< "How many vertices would you like to have (should be >1,ideal is >10) ? : ";
    cin >>numVertices;
    
    cout<< "Which node do you want as source? : ";
    cin >>src;
    
    cout<< "How many colors do you want for edges? : ";
    cin >>numColors;
    
    cout<< "What color edges do you want to find the MST for? [Enter any number between 0 and one less than number of colors] : ";
    cin >>color;
    
    Graph g(numVertices);
    
    for (int i=0;i<numGraphs;++i){
        g.generateRandomGraph(edgeDensity,distanceRange, numColors);
    }
    
    //Print the adjacency and distance matrix
    g.printGraph(g.vector_adj);
    g.printGraph(g.vector_dist);
    g.printGraph(g.vector_color);
    
    // Run Dijkstra's algorithm and get the shortest distances from the source
    vector<int> shortestDistances = g.djikstra(g.vector_adj, g.vector_dist, src);

    // Print the shortest distances from the source vertex to all vertices
    g.printDistances(shortestDistances,src);
    
    //Prim's algorithm for Minimum Spanning Tree
    pair<vector<int>, int> mst_output;
    mst_output = g.shortestEdges(g.vector_dist,0, g.vector_color,color);
    
    cout << "The nodes were visited in this sequence: ";
    for(int i=0;i<mst_output.first.size();++i){
        cout <<"->"<<mst_output.first[i]  ;
    }
    cout << endl;
    
    cout << "The Minimum Spanning Tree for your chosen color " << color << " is: " <<endl;
    for(int i=0;i<g.selected_edges.size();i++){
        for(int j=0;j<2;j++){
            cout<< g.selected_edges[i][j]<< "->";
        }
        cout<< "Length = " << g.selected_edges[i][2]<<endl;
    }
    cout << "Length of minimum spanning tree = " <<mst_output.second <<endl;
    
    //Kruskal's algorithm for Minimum Spanning Tree
    g.kruskal(g.vector_dist,color);
    
    return 0;
}
