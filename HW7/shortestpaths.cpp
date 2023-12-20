/*******************************************************************************
 * Name        : shortestpaths.cpp
 * Author      : Justin Chen & Aaren Patel
 * Version     : 1.0
 * Date        : 11/29/2023
 * Description : Implementation of Floyd's shortest path algorithm
 * Pledge      : I pledge my honor that I have abided by the Stevens Honor System.
 ******************************************************************************/
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <limits>
#include <cmath>
#include <iomanip>

using namespace std;

// We use long max because we must declare a long array due to specifications saying we must handle INT_MAX
// This is obtained from https://stackoverflow.com/questions/8690567/setting-an-int-to-infinity-in-c
long INF = numeric_limits<long>::max();

/**
 * Converts vertex number integer to letter. A = 1, B = 2....
 * This relies on ASCII calculations from CS382
 * @param letter The integer to convert to a Floyd algorithm letter
*/
char int_to_letter(int integer)
{
    return (char)(integer + 64);
}

/**
 * Converts letter to an vertex number integer. A = 1, B = 2....
 * This relies on ASCII calculations from CS382
 * @param letter The letter to convert to an vertex number integer
*/
int letter_to_int(char letter)
{
    return (int)letter - 64;
}

/**
 * This takes care of integer overflows. It prevents things from becoming infinity.
 * @param item Item to normalize
 * @return The return value of the normalized item
*/
long normalize(long item)
{
    return ((item - 1) % INF) + 1;
}

/**
 * Deallocates memory to prevent valgrind
*/
void deallocate_memory(int num_vertices, long** weight_matrix, long** traceback_matrix, long** path_matrix)
{
    for (int i = 0; i < num_vertices; i++)
    {
        delete[] weight_matrix[i];
        delete[] traceback_matrix[i];
        delete[] path_matrix[i];
    }
    delete[] weight_matrix;
    delete[] traceback_matrix;
    delete[] path_matrix;
}

/**
 * Computes Floyd's algorithm for calculating the shortest path
 * @param path_matrix The path matrix of the initial graph. Initially, this should be R0
 * @param num_vertices The number of vertices in the graph
 * @param backtrack_array An empty array of num_vertices x num_vertices that is used to backtrack node path
*/
void floyds_algorithm(long** path_matrix, int num_vertices, long** backtrack_array)
{
    // First step in Floyd - iterate up to R^k.
    // The beauty of the Warshall algorithm is that we don't have to worry about arr[i][k] or arr[k][j] being updated
    // On the current iteration due to how transitive closures work (not reflexive)
    for (int k = 0; k < num_vertices; k++)
    {
        // Now we iterate through the array
        for (int i = 0; i < num_vertices; i++)
        {
            for (int j = 0; j < num_vertices; j++)
            {
                // Now, we find the minimum between the current path length
                // and the "destination from getting chucked into the cannon and then kaboom'd to target location" 

                long current_val = path_matrix[i][j];
                long path_val;

                // WARNING: HANDLE INFINITY!!!
                // Prevent infinity addition from happening to begin with
                if (path_matrix[i][k] == INF || path_matrix[k][j] == INF)
                {
                    path_val = INF;
                }
                else if (path_matrix[i][k] + path_matrix[k][j] < 0) // Prevent integer overflow
                {
                    // Negatives will really never happen since we already checked to prevent negative indices
                    path_val = INF;
                }
                else
                {
                    path_val = path_matrix[i][k] + path_matrix[k][j];
                }
                
                path_matrix[i][j] = min(current_val, path_val);

                // We will only update if the new intermediate path is shorter than the current path we have
                bool will_update = path_val < current_val;

                // If we updated the array, then we update the backtrack array
                if (will_update)
                {
                    // Fix the backtrack array
                    // Fill it out with the current letter that represents R^k
                    // But since k is indexed, we should add 1 when doing the conversion
                    backtrack_array[i][j] = int_to_letter(k + 1);
                }
            }
        }
    }
}

/**
 * Displays the matrix on the screen as formatted by the table. This code is stolen directly from PA7.
 * @param matrix The matrix to print
 * @param label The title of the table to print
 * @param num_vertices The number of vertices in the matrix. This should be constant regardless of which matrix you use
 * @param use_letters Whether or not to display using letters. This is false by default.
*/
void display_table(long ** const matrix, const string & label, int num_vertices, const bool use_letters = false) {
    cout << label << endl;
    long max_val = 0;
    for (int i = 0; i < num_vertices; i++) {
        for (int j = 0; j < num_vertices; j++) {
            long cell = matrix[i][j];
            if (cell < INF && cell > max_val) {
                max_val = matrix[i][j];
            }
        }
    }

    // len() doesn't exist, so we just use log10
    // If we use letters, the length will just be 1
    int max_cell_width = use_letters ? 1 : int(log10(max(static_cast < long > (num_vertices), max_val))) + 1;
    cout << ' ';
    for (int j = 0; j < num_vertices; j++) {
        cout << setw(max_cell_width + 1) << static_cast < char > (j + 'A');
    }
    cout << endl;
    for (int i = 0; i < num_vertices; i++) {
        cout << static_cast < char > (i + 'A');
        for (int j = 0; j < num_vertices; j++) {
            cout << " " << setw(max_cell_width);
            if (matrix[i][j] == INF) {
                cout << "-";
            } else if (use_letters) {
                cout << char(matrix[i][j]);
            } else {
                cout << matrix[i][j];
            }
        }
        cout << endl;
    }
    cout << endl;
}

/**
 * Returns a string that contains the direction we took to get to a certain array.
 * This does it recursively.
 * @param backtrack_array The backtrack array
 * @param num_vertices The number of vertices in the graph
 * @param destination The node to arrive from
 * @param target The node we want to arrive at after the intermediate
*/
string get_directions(long** backtrack_array, int num_vertices, char destination, char target)
{
    // We assume things are in A -> D order
    // We will use i as horizontal index and j as vertical index

    int i = letter_to_int(destination) - 1;
    int j = letter_to_int(target) - 1;

    long intermediate_letter = backtrack_array[i][j];

    // If there is no intermediate letter, the recursion stops. Just return destination --> target
    if (intermediate_letter == INF)
    {   
        // We have 2 cases here. The first one is that we see something like A -> A (self loop)
        // Here, we rid duplicates. Just return the destination letter itself as a string
        if (destination == target)
        {
            return string(1, destination);
        }
        else
        {
            // The second case is normal. You just have A -> B. In that case, return it.
            string make_str = string(1, destination) + " -> " + string(1, target);
            return make_str;
        }
    }
    else
    {
        // Now, let's get the left and right directions of A -> D
        string dir_left = get_directions(backtrack_array, num_vertices, destination, char(intermediate_letter));
        string dir_right = get_directions(backtrack_array, num_vertices, char(intermediate_letter), target);

        // cout << dir_left << endl;
        // cout << dir_right << endl;

        // Now, at this step, we have "A -> B" and "B -> C". We need to get rid of duplicate Bs
        // Remember that we just need to cut B out. So just skip idx 0 on the right 
        // "B" is length 1. So we start from index 1
        // To capture "-> C -> D" (9) from "B -> C -> D" (11), we capture str_len - 1

        // Substring function obtained from https://cplusplus.com/reference/string/string/substr/
        string combined_dir = dir_left + dir_right.substr(1, dir_right.length() - 1);
        return combined_dir;
    }
}

/**
 * Prints the traceback onto the console
 * @param path_matrix The matrix that represents the path distances
 * @param backtrack_array The array that contains the intermediate vertices
 * @param num_vertices The number of vertices in the graph
*/
void print_traceback(long** path_matrix, long** backtrack_array, int num_vertices)
{
    // Loop through everything in path matrix. Then, return the necessary data.
    // Remember that the row is the destination
    for (int i = 0; i < num_vertices; i++)
    {
        // And the column is target
        for (int j = 0; j < num_vertices; j++)
        {
            // A -> A. Remember we add 1 to the number because we're indexing
            cout << int_to_letter(i + 1) << " -> " << int_to_letter(j + 1) << ", ";

            // If something's distance is infinity, we need special treatment
            if (path_matrix[i][j] == INF)
            {
                cout << "distance: infinity, path: none" << endl;
            }
            else
            {
                // distance: _
                cout << "distance: " << path_matrix[i][j] << ", ";

                // find the path:
                cout << "path: " << get_directions(backtrack_array, num_vertices, int_to_letter(i + 1), int_to_letter(j + 1)) << endl;
            }
        }
    }
}

int main(int argc, const char *argv[]) {
    // First, declare an istringstream (iss) that we will use to parse numbers
    istringstream iss;

    // Then, declare a weight matrix and a path matrix. We will change this later
    // We will use double pointers to represent the 2D array (since 2D arrays are an array that contains pointers to arrays)
    int num_vertices;
    long** weight_matrix;   // This is the initial state
    long** path_matrix;     // This is the path state

    // We will then declare a traceback matrix we will use to track back the paths in the Floyd Algorithm
    long** traceback_matrix;

    // ⭐ Create a string that represents the valid_char range. This will be used to report error messages.
    char valid_char_start = 'A';
    char valid_char_end;
    
    // Make sure the right number of command line arguments exist.
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <filename>" << endl;
        return 1;
    }
    // Create an ifstream object.
    ifstream input_file(argv[1]);
    // If it does not exist, print an error message.
    if (!input_file) {
        cerr << "Error: Cannot open file '" << argv[1] << "'." << endl;
        return 1;
    }
    // Add read errors to the list of exceptions the ifstream will handle.
    input_file.exceptions(ifstream::badbit);
    string line;
    try {
        unsigned int line_number = 1;

        while (getline(input_file, line)) {
            // ⭐ First, we do error checking

            // If the number of vertices is not between 1 and 26, throw an error
            if (line_number == 1)
            {
                iss.str(line);

                if (!(iss >> num_vertices))
                {
                    cerr << "Error: Invalid number of vertices '" << line <<"' on line 1." << endl;
                    return 1;
                }

                if (!(num_vertices >= 1 && num_vertices <= 26))
                {
                    cerr << "Error: Invalid number of vertices '" << line <<"' on line 1." << endl;
                    return 1;
                }

                iss.clear();

                // If the number is valid, make a weight matrix, path matrix, and the traceback matrix
                // This method of declaring 2D arrays is taken from
                // https://www.geeksforgeeks.org/how-to-declare-a-2d-array-dynamically-in-c-using-new-operator/
                weight_matrix = new long*[num_vertices];
                traceback_matrix = new long*[num_vertices];
                path_matrix = new long*[num_vertices];

                for (int i = 0; i < num_vertices; i++)
                {
                    weight_matrix[i] = new long[num_vertices];
                    traceback_matrix[i] = new long[num_vertices];
                    path_matrix[i] = new long[num_vertices];

                    // Then, we need to initialize all weights to INF when starting out (assuming no edges)
                    // This works for weight_matrix and path_matrix
                    for (int j = 0; j < num_vertices; j++)
                    {
                        // A self loop has a length of 0
                        if (i == j)
                        {
                            weight_matrix[i][j] = 0;
                            path_matrix[i][j] = 0;
                        }
                        else
                        {
                            // We use long int 
                            weight_matrix[i][j] = INF;
                            path_matrix[i][j] = INF;
                        }
                        // A traceback matrix should be infinite
                        traceback_matrix[i][j] = INF;
                    }
                }

                // ⭐ Create the valid chars (mainly, valid char end)
                valid_char_end = int_to_letter(num_vertices);
            }
            else
            {
                // Check invalid edge data - they should have 3 components
                // The trick is that these are space delimited, so we should expect 2 spaces

                int num_spaces = 0;
                for (size_t i = 0; i < line.length(); i++) 
                {
                    //cout << line[i] << endl;
                    if (line[i] == ' ')
                    {
                        num_spaces += 1;
                    }
                }
                if (num_spaces != 2)
                {
                    cerr << "Error: Invalid edge data '" << line << "' on line " << line_number << "." << endl;
                    deallocate_memory(num_vertices, weight_matrix, traceback_matrix, path_matrix);
                    return 1;
                }

                // Now that we know we have 3 items, chuck them into a line_data array
                string line_data[3];
                string current_str = "";

                for (size_t i = 0, idx = 0; i <= line.length(); i++)      
                {
                    // If we run into a delimiter or we reached the end of the string and need to chuck the last item into line_data
                    // Then, we chuck it in line_data and resets all tracker variables
                    if (line[i] == ' ' || i == line.length())
                    {
                        line_data[idx] = current_str;
                        current_str = "";
                        idx++;
                    }
                    else
                    {
                        // Otherwise, add to current line
                        current_str += line[i];
                    }
                }

                // The starting index must be in range. Check for that.
                // More specifically, the first two letters (that represent nodes) must be in range
                for (int i = 0; i < 2; i++)
                {
                    // if it's line_data[0], this is the starting vertex
                    // if it's the line_data[1], this is the ending vertex
                    string starting_or_ending;
                    if (i == 0)
                    {
                        starting_or_ending = "Starting";
                    }
                    else
                    {
                        starting_or_ending = "Ending";
                    }

                    // First, if there are multiple characters in the letters, that is invalid.
                    // There is no index that could be named "NYC"
                    if (line_data[i].length() != 1)      
                    {
                        cerr << "Error: " << starting_or_ending << " vertex '" << line_data[i] << "' on line " << line_number << " is not among valid values " << valid_char_start << "-" << valid_char_end << "." << endl;
                        deallocate_memory(num_vertices, weight_matrix, traceback_matrix, path_matrix);
                        return 1;
                    }
                    
                    // If the string item is not within range, it is invalid and throw an error
                    // We already checked that the string only has 1 char and hence, we can just call the 0th item
                    // That != 1 size also checked that the string is not empty (0 length char)
                    // Also we should make sure the letter to int is not < 0 because if that's the case, we are dealing with invalid chars like '.'
                    if (letter_to_int(line_data[i][0]) > num_vertices || letter_to_int(line_data[i][0]) < 0)
                    {
                        cerr << "Error: " << starting_or_ending << " vertex '" << line_data[i] << "' on line " << line_number << " is not among valid values " << valid_char_start << "-" << valid_char_end << "." << endl;
                        deallocate_memory(num_vertices, weight_matrix, traceback_matrix, path_matrix);
                        return 1;
                    }
                }

                // Finally, make sure the all edges are positive integers
                // The edges are contained in line_data[2] as a string
                // We will do the conversion using istringstream

                int edge_weight;
                iss.str(line_data[2]);

                // If it's not even a integer to begin with, we have problems.
                if (!(iss >> edge_weight))
                {
                    cerr << "Error: Invalid edge weight '" << line_data[2] << "' on line " << line_number << ".";
                    deallocate_memory(num_vertices, weight_matrix, traceback_matrix, path_matrix);
                    return 1;
                }

                // Else, make sure the weight is positive
                // Edge weight 0 will throw an error since 0 is neither positive nor negative
                if (edge_weight < 1)
                {
                    cerr << "Error: Invalid edge weight '" << line_data[2] << "' on line " << line_number << ".";
                    deallocate_memory(num_vertices, weight_matrix, traceback_matrix, path_matrix);
                    return 1;
                }

                // 📰 Now finally, update the weight_matrix and path_matrix
                // Remember weight_matrix items are initialized to undefined!!!!!!
                // We subtract 1 from the vertex number integer since we now calculating indices
                weight_matrix[letter_to_int(line_data[0][0]) - 1][letter_to_int(line_data[1][0]) - 1] = edge_weight;
                path_matrix[letter_to_int(line_data[0][0]) - 1][letter_to_int(line_data[1][0]) - 1] = edge_weight;

                // Clear istringstream
                iss.clear();
            }

            ++line_number;
        }

        // Don't forget to close the file.
        input_file.close();
    } catch (const ifstream::failure &f) {
        cerr << "Error: An I/O error occurred reading '" << argv[1] << "'.";
        return 1;
    }

    // If we get to the first step, print the original matrix
    display_table(weight_matrix, "Distance matrix:", num_vertices);
    //cout << endl;

    // Now, we run Floyd's algorithm and display the path matrix
    floyds_algorithm(path_matrix, num_vertices, traceback_matrix);
    display_table(path_matrix, "Path lengths:", num_vertices);
    //cout << endl;

    // Now, we print the intermediate vertices
    display_table(traceback_matrix, "Intermediate vertices:", num_vertices, true);
    //cout << endl;

    // Lastly, we printout the traceback
    print_traceback(path_matrix, traceback_matrix, num_vertices);

    // At the end, we deallocate memory so Valgrind doesn't yell at us
    deallocate_memory(num_vertices, weight_matrix, traceback_matrix, path_matrix);

    return 0;
}
