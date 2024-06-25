//Authored by Anand from 11-25 June 2024

//Thank you for reviewing

//Main functionalities
//Draws hex board based on user input for size.
//Checks for and allows valid moves
//Determines end of game by using Union Find algorithm with path compression

//Apart from the scope of this assignment, this file also includes the Week 4 assignment aspects for making use of Monte Carlo simulations to predict best move.

//This is a Human - versus - Computer implementation.Human plays north-south (top-bottom), Computer plays east-west (left-right)
//By changing the variable 'automated' to true, this can be made into a computer versus computer game too.

//This version uses Monte Carlo simulation to assess best moves. On my Mac, I am able to run 30,000 iterations per move evaluation.


#include <iostream>
#include <algorithm>
#include <vector>
#include <random>
#include <stdlib.h> //Included to clear screen using system command
#include <utility> // Required for std::make_pair
#include <list> //Using list since it allows insert and deletion at any position in constant time. Find is O(n) but deletion is constant time.


using namespace ::std;

//Utilities class encapsulates all common functions used in this program.
//All functions inside this Utilities class are declared as static so that they can be called without creating an object.
class Utilities{
private:
    
public:
    //constructor
    Utilities(){
        //cout<<endl<<"Class Utilities created.";
    }
    
    //destructor
    ~Utilities(){
        //cout <<endl<< "Destructor called"<< endl;
    }
    
    // Utility functions frequently used this program
    template <typename T>
    static void print_vector(vector<vector <T> > &v, bool tab){
        cout << endl;
        for(int i=0;i<v.size();i++){
            for(int j=0;j<v[i].size();j++){
                cout << v[i][j] << " ";
            }
            cout <<endl;
            if(tab){for (int k=0;k<=i;k++){cout << "  ";}}
        }
        cout <<endl;
    }
    
    template <typename T>
    static void print_vector_pair(vector<pair <T,T> > &v){
        cout<<endl;
        for(auto r:v){
            cout<< "(" <<r.first <<","<<r.second<<") ";
        }
        cout<<endl;
    }
    
    static void add_pair(short i, short j, vector<pair <short,short> > &vector_of_pairs){
        pair <short,short> n;
        n = make_pair(i,j);
        vector_of_pairs.push_back(n);
    }
    
    template <typename T>
    static short coord_to_node(T row, T column, short board_size){
        short node = row*board_size + column;
        return node;
    }
    
    
    static pair<short,short> node_to_coord(short node, short board_size){
        short x = node / board_size;
        short y = node % board_size;
        return make_pair(x, y);
    }
    
    template <typename T>
    static short find_index(const vector<T>& vec, short value) {
        auto it = std::find(vec.begin(), vec.end(), value);
        if (it != vec.end()) {
            return std::distance(vec.begin(), it);
        } else {
            return -1; // Return -1 if value not found
        }
    }
    
    template <typename T>
    static void delete_from_list(list<T>& mylist, const T& value){
        auto it = std::find(mylist.begin(),mylist.end(), value);
        if (it!=mylist.end()){
            mylist.erase(it);
            //cout <<endl<<"Deleted "<<value<<" from list. Length of list is now "<< mylist.size()<< endl;
        }
    }
    
    template <typename T>
    static void shuffle_list(list<T>& mylist){
        // Copy the list to a vector
        std::vector<short> vec(mylist.begin(), mylist.end());

        // Obtain a random number generator
        std::random_device rd;
        std::mt19937 g(rd());

        // Shuffle the vector
        std::shuffle(vec.begin(), vec.end(), g);

        // Copy the shuffled vector back to the list
        mylist.assign(vec.begin(), vec.end());

        // Print the shuffled list
        /*for (short n : mylist) {
            std::cout << n << " ";
        }
        std::cout << std::endl;*/
    }
    
};

class HexBoard{
private:
    const short board_size;

public:
    
    HexBoard(short board_size): board_size(board_size) {
        //cout<<endl<<"HexBoard of size of size "<< board_size <<" created." <<endl;
    }
    ~HexBoard(){
        //cout<<endl<<"HexBoard destructor called."<<endl;
    };
    
    //Functions related to Hex Board
    //Initialize the various variables needed to manage the game
    void initialize_game(const short& board_size,
                         vector<vector <short> >& player,
                         vector<vector <short> >& neighbors,
                         
                         vector<short>& p1_history,
                         vector<short>& p2_history,
                         
                         //Keep track of parents of nodes
                         vector<short>& p1_parents,
                         vector<short>& p2_parents,
                         
                         vector<short>& p1_node_ranks,
                         vector<short>& p2_node_ranks,
                         short& north_node, short& south_node, short& east_node, short& west_node,
                         list<short>& unoccupied_nodes){
        
        //Create 4 virtual nodes and join them with the edge nodes on the 4 edges of the board
        short n = board_size;
        north_node = n*n;
        south_node =n*n+1;
        east_node = n*n;
        west_node =n*n+1;
        
        for(short i=0;i<board_size*board_size;i++){
            p1_parents.push_back(i);
            p2_parents.push_back(i);
            unoccupied_nodes.push_back(i);
        }
        
        //cout <<endl<<"List size = "<<unoccupied_nodes.size()<<endl;
        
        p1_parents.push_back(north_node);
        p1_parents.push_back(south_node);
        p2_parents.push_back(east_node);
        p2_parents.push_back(west_node);
        
        //cout<<endl<< "Parents size " <<p1_parents.size()<<" , " <<p2_parents.size() <<endl;
        
        //Initialize node_ranks - In the beginning each node will have a rank 0 meaning it connects to itself. As more nodes becomes its child, rank will be incremented.
        for(short i=0;i<p1_parents.size();i++){ //We add the 2 virtual nodes also
            p1_node_ranks.push_back(0);
            p2_node_ranks.push_back(0);
        }
        
        for(short i=0;i<board_size;i++){
            union_set(north_node, i, p1_parents, p1_node_ranks);
            union_set(east_node, i*board_size, p2_parents, p2_node_ranks);
        }
        
        for(short i=board_size*(board_size-1);i<board_size*board_size;i++){
            union_set(south_node, i, p1_parents, p1_node_ranks);
        }
        
        for(short i=board_size-1;i<board_size*board_size;i=i+board_size){
            union_set(west_node, i, p2_parents, p2_node_ranks);
        }
        
        player.resize(board_size);
        neighbors.resize(board_size);
        for(int i=0;i<board_size;i++){
            player[i].resize(board_size);
            neighbors[i].resize(board_size);
        }
        for(int i=0;i<board_size;i++){
            for(int j=0;j<board_size;j++){
                player[i][j]=0;
                neighbors[i][j]=0;
            }
        }
        cout<<"Let's play Hex !"<<endl;
    }
    
    float evaluate_position(const list<short>& unoccupied_nodes,
                            const short& evaluated_position, //next position to evaluate
                            const float& trials, short& board_size, const short& active_player,
                            const vector<vector <short> >& player, //current game position to evaluate
                            vector<vector <short> >& neighbors, //For rendering neighbors of each move
                            //Keep track of history of moves of the players as a sequence
                            const vector<short>& p1_history,
                            const vector<short>& p2_history,
                            //Keep track of parents of nodes
                            const vector<short>& p1_parents,
                            const vector<short>& p2_parents,
                            //Keep track of ranks of nodes
                            const vector<short>& p1_node_ranks,
                            const vector<short>& p2_node_ranks,
                            //Virtual nodes for each edge of the board
                            const short& north_node, const short& south_node,const short& east_node,const short& west_node){
        float win_probability = 0;
        short count_1_wins=0;short count_2_wins=0;
        //cout<<endl<<"Running evaluation for player "<<active_player<<endl;
        //cout <<endl <<"Evaluating position " <<evaluated_position <<". Please wait..."<<endl;
        
        for(short i=0;i<trials;i++){
            //cout <<endl<<"Trial " <<i <<endl;
            list<short> l_unoccupied_nodes = unoccupied_nodes;
            vector<vector<short> > l_neighbors = neighbors;
            vector<vector<short> > l_player = player;
            vector<short> l_p1_history = p1_history;
            vector<short> l_p2_history = p2_history;
            //Keep track of parents of nodes
            vector<short> l_p1_parents = p1_parents;
            vector<short> l_p2_parents = p2_parents;
            //Keep track of ranks of nodes
            vector<short> l_p1_node_ranks = p1_node_ranks;
            vector<short> l_p2_node_ranks = p2_node_ranks;
            short l_active_player = active_player;
            
            //Initial setting for starting evaluation trial
            pair<short,short> coordinate = Utilities::node_to_coord(evaluated_position,board_size);
            l_player[coordinate.first][coordinate.second]=active_player;
            log_history(evaluated_position, coordinate.first+1,coordinate.second, l_active_player,l_p1_history,l_p2_history,l_player); //Very important step
            Utilities::delete_from_list(l_unoccupied_nodes,evaluated_position);
            /*
            for(auto x:l_unoccupied_nodes){
                cout << x<< " ";
            }*/
        
            bool game_decided = false;
            
            Utilities::delete_from_list(l_unoccupied_nodes,evaluated_position);
            Utilities::shuffle_list(l_unoccupied_nodes);
            //End of intial settings for starting evaluation trial
            
            while(!game_decided && l_unoccupied_nodes.size()>0){
                short row, column;
                bool automated = true;
                short node = l_unoccupied_nodes.front();
                row = Utilities::node_to_coord(node, board_size).first+1;
                column = Utilities::node_to_coord(node, board_size).second+1;
                log_history(node, row, column, l_active_player,l_p1_history,l_p2_history,l_player);
                
                //cout <<"Active Player " <<l_active_player <<endl;
                //cout <<"Node = "<<node <<endl;
                //cout << "Row - column = "<<row << ", "<<column<<endl;
                //Find neighbors
                vector<pair <short,short> > vec_pair_neighbors;
                vec_pair_neighbors = find_neighbors(row-1,column-1, board_size, l_neighbors);
                //cout <<"Neighbour count = " <<vec_pair_neighbors.size()<<endl;
                for (auto& n:vec_pair_neighbors){
                    if(l_active_player==1 and !game_decided){
                        auto f = Utilities::find_index(l_p1_history,Utilities::coord_to_node(n.first,n.second,board_size));
                        if(f!=-1){
                            short nearby_node = l_p1_history[f];
                            //cout<<endl<<"H: Made a connection "<<node <<"&" <<nearby_node<<endl;
                            short x = find_parent(node,l_p1_parents);
                            //cout<<endl;
                            short y = find_parent(nearby_node,l_p1_parents);
                            //cout<<endl;
                            
                            //cout << endl<< "Parents are :" << x << " " << y<< endl;
                            
                            union_set(x, y, l_p1_parents, l_p1_node_ranks);
                            
                            //cout<<endl<<"Parent of N Node = "<< find_parent(north_node,l_p1_parents) <<endl;
                            //cout<<endl<<"Parent of S Node = "<< find_parent(south_node,l_p1_parents) <<endl;
                            
                            if (find_parent(north_node,l_p1_parents)==find_parent(south_node,l_p1_parents)){
                                //cout<<endl<<"Player 1 wins"<<endl;
                                count_1_wins++;
                                //draw_board(board_size,l_player, l_neighbors);
                                
                                game_decided = true;
                            }
                        }
                    }else if(l_active_player==2 and !game_decided){
                        auto f = Utilities::find_index(l_p2_history,Utilities::coord_to_node(n.first,n.second,board_size));
                        if(f!=-1){
                            short nearby_node = l_p2_history[f];
                            //cout<<endl<<"C: Made a connection "<<node <<"&" <<nearby_node<<endl;
                            short x = find_parent(node,l_p2_parents);
                            //cout<<endl;
                            short y = find_parent(nearby_node,l_p2_parents);
                            //cout<<endl;
                            
                            //cout << endl<< "Parents are :" << x << " " << y<< endl;
                            
                            union_set(x, y, l_p2_parents, l_p2_node_ranks);
                            
                            //cout<<endl<<"Parent of E Node = "<< find_parent(east_node,l_p2_parents) <<endl;
                            //cout<<endl<<"Parent of W Node = "<< find_parent(west_node,l_p2_parents) <<endl;
                            
                            if (find_parent(east_node,l_p2_parents)==find_parent(west_node,l_p2_parents)){
                                //cout<<endl<<"Player 2 wins"<<endl;
                                count_2_wins++;
                                //draw_board(board_size,l_player, l_neighbors);
                                
                                game_decided = true;
                            }
                        }
                    }
                }
                //cout<<endl;
                //draw_board(board_size,l_player, l_neighbors);
                //Reset and change player
                reset_neighbors(l_neighbors, board_size);
                if(l_active_player==1){l_active_player=2;}else{l_active_player=1;}
                Utilities::delete_from_list(l_unoccupied_nodes,node);
            }
        }
        if(active_player==1){win_probability = count_1_wins/trials;}else{win_probability = count_2_wins/trials;}
        return win_probability;
    }
    
    bool generate_move(short& row,short& column, short& board_size, bool& automated, short& active_player,vector<vector <short> > &player){
        //If automated = true: Fully automated play with random number generation based position
        //Mode 2: Use position evaluation using Monte-Carlo trials and recommend best move
        //If automated = false: Take human input
        bool valid_move = false;
        while(valid_move!=true){
            if(automated){// Random number generator
                random_device rd;  // Seed for the random number engine
                mt19937 gen(rd()); // Mersenne Twister engine
                
                // Define the range [0, boardSize)
                uniform_int_distribution<> dis(1, board_size);
                
                // Generate the random number
                row = dis(gen);
                column = dis(gen);
                
                //cout<< endl<<"Generated row - column " <<row<<" , " <<column <<endl;
            }else{
                cout <<endl<< "Player "<<active_player<< " : Which row? (1 to board size) [Enter -1 to end game]:";
                cin >> row;
                if (row!=-1){
                    cout << "Player "<<active_player<< " : Which column? (1 to board size):";
                    cin >> column;
                }
            }
            
            if(row==-1){valid_move=true;cout<<endl<<"Game ended"<< endl;return valid_move;} //Exit game
            
            if((row>=1 && row<=board_size)&&(column>0 && column<=board_size) && player[row-1][column-1]==0){
                valid_move=true;
                cout<< endl<<"Player 1 moving to (" <<row<<" , " <<column <<")" <<endl;
            }else if(row!=-1){
                //cout<<endl<<"Invalid move"<<endl;
            }

        }
        return valid_move;
    }
    
    //Render the Hex board
    void draw_board(short &board_size,vector<vector <short> > &player, vector<vector <short> > &neighbors){
        
        //print_vector(player, true);
        
        for (int i=0; i<board_size; i++){
            for (int j=0; j<board_size; j++){
                if(player[i][j]==1){
                    cout << " X ";
                }else if(player[i][j]==2){
                    cout<< " T ";
                }else{
                    cout<< " - ";
                }
            }
            cout << endl;
            for (int k=0;k<=i;k++){cout << "  ";}
        }
        cout <<endl;
        
        //Optional code for visualizing neighbours.
        for (int i=0; i<board_size; i++){
            for (int j=0; j<board_size; j++){
                if (neighbors[i][j]==1){
                    cout << " * ";
                }else{
                    cout << " - ";
                }
                
            }
            cout << endl;
            for (int k=0;k<=i;k++){cout << "  ";}
        }
    }
    
    vector<pair <short,short> > find_neighbors(short row,short column,short board_size,vector<vector <short> > &neighbors){
        
        vector<pair <short,short> > n;
        
        //Top edge
        if (row==0){
            if(column==0){
                neighbors[row][column+1]=1; //(1,1) | (0,1) //(0,j+1)
                Utilities::add_pair(row, column+1,n);
                
            }else{
                neighbors[row][column-1]=1; //(0,9) //(0,j−1) //
                neighbors[row+1][column-1]=1; //(1,9) //(1,j−1)
                
                Utilities::add_pair(row, column-1,n);
                Utilities::add_pair(row+1, column-1,n);
            }
            
            neighbors[row+1][column]=1; //(1,10) | (1,0) //(1,j)
            Utilities::add_pair(row+1, column,n);
            
            if(column!=board_size-1 && column!=0){
                neighbors[row][column+1]=1;
                Utilities::add_pair(row, column+1,n);
            }
        }
        //Bottom edge
        if(row==board_size-1){
            if(column==0){
                neighbors[row][column+1]=1;
                neighbors[row-1][column+1]=1;
                
                Utilities::add_pair(row, column+1,n);
                Utilities::add_pair(row-1, column+1,n);
            }else{
                neighbors[row][column-1]=1;
                Utilities::add_pair(row, column-1,n);
            }
            
            neighbors[row-1][column]=1;
            Utilities::add_pair(row-1, column,n);
            
            if(column!=board_size-1 && column!=0){
                neighbors[row][column+1]=1;
                Utilities::add_pair(row, column+1,n);
                
                neighbors[row-1][column+1]=1;
                Utilities::add_pair(row-1, column+1,n);
            }
        }
        //Left edge
        if(column==0 && (row!=0&&row!=(board_size-1))){
            neighbors[row-1][column]=1;
            neighbors[row+1][column]=1;
            
            neighbors[row-1][column+1]=1;
            neighbors[row][column+1]=1;
            
            Utilities::add_pair(row-1, column,n);
            Utilities::add_pair(row+1, column,n);
            Utilities::add_pair(row-1, column+1,n);
            Utilities::add_pair(row, column+1,n);
        }
        //Right edge
        if(column==(board_size-1) && (row!=0&&row!=(board_size-1))){
            neighbors[row-1][column]=1;
            neighbors[row+1][column]=1;
            
            neighbors[row+1][column-1]=1;
            neighbors[row][column-1]=1;
            
            Utilities::add_pair(row-1, column,n);
            Utilities::add_pair(row+1, column,n);
            Utilities::add_pair(row+1, column-1,n);
            Utilities::add_pair(row, column-1,n);
        }
        
        if(row!=0 && row!=(board_size-1) && column!=0 && column!=(board_size-1)){
            neighbors[row-1][column] = 1;
            neighbors[row+1][column] = 1;
            neighbors[row][column+1] = 1;
            neighbors[row][column-1] = 1;
            neighbors[row-1][column+1] = 1;
            neighbors[row+1][column-1] = 1;
            
            Utilities::add_pair(row-1, column,n);
            Utilities::add_pair(row+1, column,n);
            Utilities::add_pair(row, column+1,n);
            Utilities::add_pair(row, column-1,n);
            Utilities::add_pair(row-1, column+1,n);
            Utilities::add_pair(row+1, column-1,n);
        }
        
        return n;
    };
    
    void reset_neighbors(vector<vector <short> > &neighbors, short board_size){
        for(int i=0;i<board_size;i++){
            for(int j=0;j<board_size;j++){
                neighbors[i][j]=0;
            }
        }
    }
    
    short find_parent(int node, vector<short>& parent){
        //cout<< "1. Parent of "<< node <<" is "<<parent[node]<<endl;
        if(parent[node]!=node){
            //cout<< "2. Parent of "<< node <<" is "<<parent[node]<<endl;
            parent[node]=find_parent(parent[node], parent);
        }
        //cout<< "3. Parent of "<< node <<" is "<<parent[node]<<endl;
        return parent[node];
    }
    
    void union_set(short x, short y, vector<short>& parent, vector<short>& rank){ //Join the two nodes with path compression
        //cout<<endl<< "Union operation - Nodes are "<< x <<" " <<y <<" "<<endl;
        //cout<<endl<<"Ranks of nodes are: " <<rank[x] << " , " <<rank[y] <<endl;
        
        if(rank[x]>=rank[y]){
            parent[y]=x;
            rank[x]++;
            //cout<<endl<< "Parent of "<< y <<" set to " <<x  <<" - Rank of " << x << " set to " << rank[x] <<endl;
        }else{
            parent[x]=y;
            rank[y]++;
            //cout<<endl<< "Parent of "<< x <<" set to " <<y <<" - Rank of " << y << " set to " << rank[y] <<endl;
        }
    }
    
    void log_history(const short& node, const short& row, const short& column, const short& active_player, vector<short>& p1_history,vector<short>& p2_history,vector<vector <short> >& player){
        //Log history
        if(active_player==1){p1_history.push_back(node);}else{p2_history.push_back(node);}
        player[row-1][column-1]=active_player;
        
        /*cout<<endl<<"Player 1 : ";
        for(auto v:p1_history){cout<<v<< "-";}
        cout<<endl<<"Player 2 : ";
        for(auto v:p2_history){cout<<v<< "-";}
        cout<<endl;*/
    }
    
};

int main(){
    
    Utilities u;
    float trials = 30000;
    short count_1_wins=0;short count_2_wins=0;
    
    vector<vector <short> > player; //For tracking plays on the hex board by each player
    vector<vector <short> > neighbors; //For rendering neighbors of each move
    //Keep track of history of moves of the players as a sequence
    vector<short> p1_history;
    vector<short> p2_history;
    //Keep track of parents of nodes
    vector<short> p1_parents;
    vector<short> p2_parents;
    //Keep track of ranks of nodes
    vector<short> p1_node_ranks;
    vector<short> p2_node_ranks;
    //Virtual nodes for each edge of the board
    short north_node, south_node,east_node, west_node;
    //Other variables for game play
    short board_size=5, row, column,active_player=1;
    bool game_decided = false;
    bool automated=false; //To toggle automated play versus human play - helps in position evaluation
    list<short> unoccupied_nodes;
    
    system("clear"); //clear screen on MacOS
    
    //cout << "Enter board size :";
    //cin >> board_size;

    HexBoard h(board_size); //Creates and initializes hex board
    h.initialize_game(board_size,player,neighbors,p1_history,p2_history, p1_parents,p2_parents, p1_node_ranks,p2_node_ranks, north_node, south_node,east_node, west_node, unoccupied_nodes );
    //u.print_vector(player, true); //Initially this will be an empty board.

    h.draw_board(board_size,player, neighbors);
        
        while(!game_decided){
            bool valid_move;
            if(active_player == 1){
                valid_move = h.generate_move(row, column, board_size, automated, active_player, player);
            }
            if(row==-1){return 0;}//Row = -1 is for end of game
            
            if(active_player == 2){
                short best_move=0;
                float best_move_win_probability = 0;
                short count_of_positions_evaluated = 0;
                for(short evaluated_position:unoccupied_nodes){
                    float position_score = h.evaluate_position(unoccupied_nodes,evaluated_position,
                                                               trials,board_size, active_player,
                                                               player, //current game position to evaluate
                                                               neighbors, //For rendering neighbors of each move
                                                               //Keep track of history of moves of the players as a sequence
                                                               p1_history,
                                                               p2_history,
                                                               //Keep track of parents of nodes
                                                               p1_parents,
                                                               p2_parents,
                                                               //Keep track of ranks of nodes
                                                               p1_node_ranks,
                                                               p2_node_ranks,
                                                               //Virtual nodes for each edge of the board
                                                               north_node, south_node,east_node,west_node);
                    
                    //cout<< endl<< "Evaluated Position = " <<" "<<evaluated_position << " , Win probability = "<<position_score <<endl;
                    //cout<<endl <<"Best move till now is " <<best_move << ", Win probability = "<<position_score <<endl;
                    count_of_positions_evaluated++;
                    if(position_score>best_move_win_probability){
                        best_move = evaluated_position;
                        best_move_win_probability = position_score;
                        //cout<< endl<< "Positions evaluated = "<< count_of_positions_evaluated<< ", Best move updated to " <<best_move << ", Win probability = "<<best_move_win_probability <<endl;
                    }else{
                        //cout <<endl<< evaluated_position << " is not the best move."<<endl;
                        //cout<< endl<< "Positions evaluated = "<< count_of_positions_evaluated<< ", Best move till now is " <<best_move << ", Win probability = "<<best_move_win_probability <<endl;
                    }
                }
                pair<short,short> coordinate = Utilities::node_to_coord(best_move,board_size);
                row = coordinate.first+1;
                column = coordinate.second+1;
                valid_move = true;
                
                cout<<endl<<"Player " <<active_player <<" moving to Node "<<best_move <<" ("<< row<<","<<column <<")"<<endl;
                
            }
                        
            if(valid_move){
                short node = Utilities::coord_to_node(row-1,column-1,board_size);
                Utilities::delete_from_list(unoccupied_nodes,node);
                
                h.log_history(node, row, column, active_player,p1_history,p2_history,player);
                
                //Find neighbors
                vector<pair <short,short> > vec_pair_neighbors;
                vec_pair_neighbors = h.find_neighbors(row-1,column-1, board_size, neighbors);
                
                for (auto& n:vec_pair_neighbors){
                    if(active_player==1){
                        auto f = Utilities::find_index(p1_history,Utilities::coord_to_node(n.first,n.second,board_size));
                        if(f!=-1){
                            short nearby_node = p1_history[f];
                            //cout<<endl<<"H: Made a connection "<<node <<"&" <<nearby_node<<endl;
                            short x = h.find_parent(node,p1_parents);
                            //cout<<endl;
                            short y = h.find_parent(nearby_node,p1_parents);
                            //cout<<endl;
                            
                            //cout << endl<< "Parents are :" << x << " " << y<< endl;
                            
                            h.union_set(x, y, p1_parents, p1_node_ranks);
                            
                            //cout<<endl<<"Parent of N Node = "<< h.find_parent(north_node,p1_parents) <<endl;
                            //cout<<endl<<"Parent of S Node = "<< h.find_parent(south_node,p1_parents) <<endl;
                            
                            if (h.find_parent(north_node,p1_parents)==h.find_parent(south_node,p1_parents)){
                                cout<<endl<<"Player 1 wins"<<endl;
                                count_1_wins++;
                                h.draw_board(board_size,player, neighbors);
                                game_decided = true;
                            }
                        }
                    }else{
                        auto f = Utilities::find_index(p2_history,Utilities::coord_to_node(n.first,n.second,board_size));
                        if(f!=-1){
                            short nearby_node = p2_history[f];
                            //cout<<endl<<"C: Made a connection "<<node <<"&" <<nearby_node<<endl;
                            short x = h.find_parent(node,p2_parents);
                            //cout<<endl;
                            short y = h.find_parent(nearby_node,p2_parents);
                            //cout<<endl;
                            
                            //cout << endl<< "Parents are :" << x << " " << y<< endl;
                            
                            h.union_set(x, y, p2_parents, p2_node_ranks);
                            
                            //cout<<endl<<"Parent of E Node = "<< h.find_parent(east_node,p2_parents) <<endl;
                            //cout<<endl<<"Parent of W Node = "<< h.find_parent(west_node,p2_parents) <<endl;
                            
                            if (h.find_parent(east_node,p2_parents)==h.find_parent(west_node,p2_parents)){
                                cout<<endl<<"Player 2 wins"<<endl;
                                count_2_wins++;
                                h.draw_board(board_size,player, neighbors);
                                game_decided = true;
                            }
                        }
                    }
                }
                cout<<endl;
                h.draw_board(board_size,player, neighbors);
                cout<<endl;
                //Reset and change player
                h.reset_neighbors(neighbors, board_size);
                if(active_player==1){active_player=2;}else{active_player=1;}
            }
        }
        
    
    
    return 0;
}
