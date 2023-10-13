/*******************************************************************************
 * Name        : waterjugpuzzle.cpp
 * Author      : Justin Chen
 * Date        : 10/11/2023
 * Description : Solves the waterjug puzzle
 * Pledge      : I pledge my honor that I have abided by the Stevens Honor System.
 ******************************************************************************/
#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>

using namespace std;

/**
 * The State struct that will store all our items
 * Taken from PA4 file
*/
struct State {
    int a, b, c;
    string directions;
    State *parent;
    
    State(int _a, int _b, int _c, string _directions) : 
        a{_a}, b{_b}, c{_c}, directions{_directions}, parent{nullptr} { }
    
    // String representation of state in tuple form.
    string to_string() {
        ostringstream oss;
        oss << "(" << a << ", " << b << ", " << c << ")";
        return directions + oss.str();              //Concat according to the beginning
    }
};


/**
 * Prints the step it took to get from original state to the current state
 * @param target_state The pointer of the state to print
*/
void print_state(State* target_state)
{
    //Because our State structure only records parent and we have the last child, we need a data structure to store every parent 
    //until the last child until the first parent (which has a null pointer)

    //That's why we create a vector to store that.
    State* current_state = target_state;
    vector<State*> parents;         //Stores pointers of the parents. Items in the back are more "senior" parents

    while (current_state)       //while current state exists
    {
        parents.push_back(current_state);
        current_state = (*current_state).parent;
    }

    //Now, print all the states (and their parents)
    //Parents are in reverse order. Hence, we print in reverse order
    //🚨🚨 Do not do for (i >= 0; i--) on size_t : that is an unsigned int and we will get integer underflow (I hate debugging)
    //To get around that, I just used i + 1 > 0 to force that 
    for (size_t i = parents.size() - 1; i + 1 > 0; i--)
    {
        cout << (*parents.at(i)).to_string() << endl;
    }
}

/**
 * Prints the text for no solution
*/
void print_no_solution()
{
    cout << "No solution." << endl;
}

/**
 * Creates a new state and return its pointer.
 * @param newA The new jug A
 * @param newB The new jug B
 * @param newC The new jug C
 * @param change_int_target The change in the target to print
 * @param parent The parent of the current state
 * @param originJug A char representing jug it originated from
 * @param targetJug A char representing jug that;s getting filled
*/
State* create_new_state(int newA, int newB, int newC, int change_in_target, State* parent, char originJug, char targetJug)
{   
    string gallon;
    if (change_in_target == 1)
    {
        gallon = "gallon";
    }
    else
    {
        gallon = "gallons";
    }

    // to_string method obtained from C++ documentation: https://cplusplus.com/reference/string/to_string/
    State* new_state = new State(newA, newB, newC, "Pour " + to_string(change_in_target) + " " + gallon + " from " + originJug + " to " + targetJug + ". ");
    (*new_state).parent = parent; //Parent is a pointer, so use & to get current address to fill it in with
    
    return new_state;
}

/**
 * Solves the waterjug puzzle
 * @param capA The capacity for jug A
 * @param capB The capacity for jug B
 * @param capC The capacity for jug C. This is completely filled at the start
 * @param goalA The goal for jug A
 * @param goalB The goal for jug B
 * @param goalC The goal for jug C
*/
void solve_waterjug(int capA, int capB, int capC, int goalA, int goalB, int goalC)
{
    //Now, let's create the initial state and goal states
    //We want to create this in the heap because we'll have pointer references to this
    //Remember initial state is completely filled at the start
    State* initial_state = new State(0, 0, capC, "Initial state. ");

    //--First, let's declare the 2D boolean array so we can track whether or not a state has occured---
    //Idea for code taken from Philipe Meunier's class notes
    
    //Initialize an array of boolean arrays
    //This array creation returns a pointer that contains more boolean pointers (to array indices)
    bool** tracker = new bool*[capC + 1];

    for (int i = 0; i <= capC; i++)
    {
        tracker[i] = new bool[capC + 1];

        //Now fill it all up with false
        for (int j = 0; j <= capC; j++)
        {
            tracker[i][j] = false;
        }
    }

    //This might be a bit dumb, but I'm storing all the pointers of the states in here so we can cleanup later
    //Not the most effiicent thing, but hey, that's kind of C's fault for not having a Mark and Sweep algorithm
    vector<State*> state_pointers;
    //Add 1st to here

    //--------BFS Start!--------------
    
    //Create a boolean variable to track if we found the solution
    bool found_solution = false;
    state_pointers.push_back(initial_state);

    //Step 1: Create a bfs_list (in this case, a vector) to track what node we're BFSing right now
    vector<State*> bfs_list;
    bfs_list.push_back(initial_state);      //Begin with the initial state

    while (bfs_list.size() != 0)        //While we still have stuff in our queue
    {
        //Pop the first item out of the "queue" and chucks it in current_state
        //In this case, we'll just need to extract 0 and remove it from the vector
        State* current_state = (bfs_list.at(0));
        bfs_list.erase(bfs_list.begin());       //Taken from PA3 code

        //If it matches our goals, we return it
        if ((*current_state).a == goalA && (*current_state).b == goalB && (*current_state).c == goalC)
        {
            print_state(current_state);
            found_solution = true;
            break;
        }

        //If the pattern has been matched, we ignore it
        if (tracker[(*current_state).a][(*current_state).b])
        {
            continue;
        }

        //cout << current_state.to_string() << endl;

        //Step 2: Append all the states of all 6 possible steps into the bfs_list
        
        //BUT! RED FLAG! BEFORE WE START: Create variables that represent how much empty stuff is in each jug
        int missing_jug_A = capA - (*current_state).a;
        int missing_jug_B = capB - (*current_state).b;
        int missing_jug_C = capC - (*current_state).c;

        //Ok now we can begin.

        //Remember for each dump you can either:
        //For the jug that's getting dumped: Either you empty the jug (0), or you fill the target jug (current state - target jug missing)
        //For the jug that's targeted: Either you fill the jug (cap A/B/C), or you end up dumping everything in origin jug into target jug
        //Which is (currentstate target + currentstate origin)

        //After that, mark it as flagged and add parent
        //For the descriptor, find the percent change in jug between target jug after dump and before dump

        // 1) C to A
        int new_origin = max((*current_state).c - missing_jug_A, 0);
        int new_target = min(capA, (*current_state).a + (*current_state).c);
        int change_in_target = new_target - (*current_state).a;

        //Call create_new_state to create the new state to use
        State* new_state1 = create_new_state(new_target, (*current_state).b, new_origin, change_in_target, current_state, 'C', 'A');
        state_pointers.push_back(new_state1);
        bfs_list.push_back(new_state1);
        
        // 2) B to A
        new_origin = max((*current_state).b - missing_jug_A, 0);
        new_target = min(capA, (*current_state).a + (*current_state).b);
        change_in_target = new_target - (*current_state).a;

        State* new_state2 = create_new_state(new_target, new_origin, (*current_state).c, change_in_target, current_state, 'B', 'A');
        state_pointers.push_back(new_state2);
        bfs_list.push_back(new_state2);

        // 3) C to B
        new_origin = max((*current_state).c - missing_jug_B, 0);
        new_target = min(capB, (*current_state).b + (*current_state).c);
        change_in_target = new_target - (*current_state).b;

        State* new_state3 = create_new_state((*current_state).a, new_target, new_origin, change_in_target, current_state, 'C', 'B');
        state_pointers.push_back(new_state3);
        bfs_list.push_back(new_state3);

        // 4) A to B
        new_origin = max((*current_state).a - missing_jug_B, 0);
        new_target = min(capB, (*current_state).b + (*current_state).a);
        change_in_target = new_target - (*current_state).b;

        State* new_state4 = create_new_state(new_origin, new_target, (*current_state).c, change_in_target, current_state, 'A', 'B');
        state_pointers.push_back(new_state4);
        bfs_list.push_back(new_state4);

        // 5) B to C
        new_origin = max((*current_state).b - missing_jug_C, 0);
        new_target = min(capC, (*current_state).c + (*current_state).b);
        change_in_target = new_target - (*current_state).c;

        State* new_state5 = create_new_state((*current_state).a, new_origin, new_target, change_in_target, current_state, 'B', 'C');
        state_pointers.push_back(new_state5);
        bfs_list.push_back(new_state5);

        // 6) A to C
        new_origin = max((*current_state).a - missing_jug_C, 0);
        new_target = min(capC, (*current_state).c + (*current_state).a);
        change_in_target = new_target - (*current_state).c;

        State* new_state6 = create_new_state(new_origin, (*current_state).b, new_target, change_in_target, current_state, 'A', 'C');
        state_pointers.push_back(new_state6);
        bfs_list.push_back(new_state6);

        //Mark that we processed the current state
        tracker[(*current_state).a][(*current_state).b] = true;
    }

    //If there's no solution when "queue" is empty, it's impossible
    if (!found_solution)
    {
        print_no_solution();
    }

    //--------Garbage Collection------------
    //Delete all the internal arrays
    for (int i = 0; i < capC + 1; i++)
    {
        delete[] tracker[i];
    }

    //Delete array overall
    delete[] tracker;

    //Delete all the States we created in the heap
    for (size_t j = 0; j < state_pointers.size(); j++)
    {
        delete state_pointers.at(j);
    }
}

/**
 * Does error checking on the inputs from main to make sure they are valid
 * We don't want to check for bad user input which exceeds capacity because we want to put them into ints before that happens
 * @precondition argv.length (in the java sense) == argc. This should already have been done before 
 * running error_check
 * @param argc Pass in the arc from main()
 * @param argv Pass in the argv from main()
 * @return An integer representing the exit code. If it's 1, we will return 1 in main() too to terminate
*/
int error_check_invalid(int argc, char* argv[])
{
    //Create an array in stack that matches argv[i] with the jug name it corresponds to
    //I took array declaration code from W3School: https://www.w3schools.com/cpp/cpp_arrays.asp
    //There's no new keyword, so we assume this not being dynamically declared
    char jug_name[7] = {'Z', 'A', 'B', 'C', 'A', 'B', 'C'};
    //                  ^^^ This is useless because we will never use index 1 (corresponds to file name)

    //Create iss to parse strings
    istringstream iss;
    int dummy_num;          //Create a dummy number to chuck things from iss into

    //Check everything in argv[]
    //Skip index 1 because that's the program name
    for (int i = 1; i < argc; i++)
    {
        //Check invalid capacity and invalid goal
        iss.str(argv[i]);

        //If it's - Not a number    -   Has 0 capacity          -     Capacity flat out does not exist
        if ((!(iss >> dummy_num)) || ((i < 4) && (dummy_num <= 0)) || (dummy_num < 0))    
        {
            //If it's 1, 2, 3, then it's invalid capacity. Otherwise, it's invalid goals
            if (i < 4)
            {
                cerr << "Error: Invalid capacity '" << argv[i] << "' for jug " << jug_name[i] << "." << endl;
            }
            else
            {
                cerr << "Error: Invalid goal '" << argv[i] << "' for jug " << jug_name[i] << "." << endl;
            }

            //Exit with error
            return 1;
        }

        //After every iteration, flush the istringstream so we can reuse it
        iss.clear();
    }

    return 0;
}

int main(int argc, char* argv[]) 
{
    //Check to make sure exactly 6 inputs + 1 input for the program name
    if (argc != 7)
    {
        cerr << "Usage: ./waterjugpuzzle <cap A> <cap B> <cap C> <goal A> <goal B> <goal C>" << endl;
        return 1;
    }
    
    //Check to make sure all the inputs are valid numbers
    //If error_check_invalid gives us exit code 1, we need to actually... well... exit code 1
    if (error_check_invalid(argc, argv) == 1)
    {
        return 1;
    }

    //Now that we know all the argv[] are valid numbers, we can chuck all of them into integer variables
    istringstream iss;
    
    int capA;
    iss.str(argv[1]);
    iss >> capA;        //We don't need to error check because we already made sure the numbers are valid + right # of inputs (so inputs exists)
    iss.clear();        //Clear the iss

    int capB;
    iss.str(argv[2]);
    iss >> capB;
    iss.clear();

    int capC;
    iss.str(argv[3]);
    iss >> capC;
    iss.clear();

    int goalA;
    iss.str(argv[4]);
    iss >> goalA;
    iss.clear();
    
    int goalB;
    iss.str(argv[5]);
    iss >> goalB;
    iss.clear();

    int goalC;
    iss.str(argv[6]);
    iss >> goalC;
    iss.clear();

    //Check for bad user input. 
    //Checks if goal exceeds capacity
    if (goalA > capA)
    {
        cerr << "Error: Goal cannot exceed capacity of jug A." << endl;
        return 1;
    }

    if (goalB > capB)
    {
        cerr << "Error: Goal cannot exceed capacity of jug B." << endl;
        return 1;
    }

    if (goalC > capC)
    {
        cerr << "Error: Goal cannot exceed capacity of jug C." << endl;
        return 1;
    }

    //The total capacity in the goals != capacity of jug C
    //This is because jug C will initially be full. And that water in jug C is preserved
    //This triggers when it's not the same
    if (capC != (goalA + goalB + goalC))
    {
        cerr << "Error: Total gallons in goal state must be equal to the capacity of jug C." << endl;
        return 1;
    }

    //Solve waterjug!
    solve_waterjug(capA, capB, capC, goalA, goalB, goalC);

    //State s(0, 0, 8, "Initial state.");
    return 0;
}
