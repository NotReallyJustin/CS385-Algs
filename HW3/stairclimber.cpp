/*******************************************************************************
 * Name        : stairclimber.cpp
 * Author      : Justin Chen
 * Date        : 9/26/2023
 * Description : Lists the number of ways to climb n stairs.
 * Pledge      : I pledge my honor that I have abided by the Stevens Honor System.
 ******************************************************************************/
#include <iostream>
#include <vector>
#include <algorithm>
#include <sstream>
#include <iomanip>

using namespace std;

/**
 * Get the number of ways you can make num_stairs, and returns these combinations
 * @param num_stairs The number of stairs we need
*/
vector< vector<int> > get_ways(int num_stairs) {
    vector<vector<int>> ways;

    //Base case
    if (num_stairs <= 0)
    {
        vector<int> empty_vector;   //Return it with empty vector
        ways.push_back(empty_vector);
        return ways;
    }
    
    //Loop through 1, 2, and 3.
    //Use these indices to find get_ways(num_stairs - i)
    for (int i = 1; i < 4; i++)
    {
        if (num_stairs >= i)    //If the number of stairs exist
        {
            //Get the number of ways to reach (num_stairs - i) stairs
            vector<vector<int>> old_ways = get_ways(num_stairs - i);
            
            //Loop through all the vectors.
            for (size_t j = 0; j < old_ways.size(); j++)
            {
                vector<int> curr_way_to_make_num_stairs = old_ways.at(j);

                //Prepend i to the front of the vector
                curr_way_to_make_num_stairs.insert(curr_way_to_make_num_stairs.begin(), i);

                //Add it to current ways
                ways.push_back(curr_way_to_make_num_stairs);
            }
        }
    }

    return ways;
}

/**
 * Takes a 2D vector of ways and prints each combination
 * @param ways The 2D vector of ways to print
*/
void display_ways(const vector< vector<int> > &ways) {

    //Find the number of digits in the largest index in ways so we can setwidth to that later with iomanip
    int num_digits = 0;
    int size = (int)ways.size();

    while (size != 0)
    {
        num_digits += 1;
        size = size / 10;
    }

    for (size_t i = 0; i < ways.size(); i++)
    {
        vector<int> curr_way_to_make_num_stairs = ways.at(i);

        //Text-Align code (std::right) obtained from C++ documentation
        //@see https://cplusplus.com/reference/ios/right/
        cout << setw(num_digits) << right << i + 1 << ". " << setw(1) << left;

        //curr_way_to_make_num_stairs, but displayed as a list
        cout << "[";

        for (size_t j = 0; j < curr_way_to_make_num_stairs.size(); j++)
        {
            if (j != curr_way_to_make_num_stairs.size() - 1)
            {
                cout << curr_way_to_make_num_stairs.at(j) << ", ";
            }
            else
            {
                cout << curr_way_to_make_num_stairs.at(j) << "]" << endl;
            }
        }
    }
}

int main(int argc, char * const argv[]) {
    if (argc != 2)  //Invalid number of params
    {
        cerr << "Usage: ./stairclimber <number of stairs>" << endl;
        return 1;
    }

    //Use iss to parse ints
    int stair_num;
    istringstream iss;

    iss.str(argv[1]);

    if (!(iss >> stair_num)) //Not a number
    {
        cerr << "Error: Number of stairs must be a positive integer." << endl;
        return 1;
    }

    //Not a positive integer
    if (stair_num <= 0)
    {
        cerr << "Error: Number of stairs must be a positive integer." << endl;
        return 1;
    }

    vector<vector<int>> ways = get_ways(stair_num);

    //This is here because if there is 1 way to output, ways needs to be turned into ways and stairs --> stair
    string ways_output = "ways";
    string stairs_output = "stairs";

    if (ways.size() == 1)
    {
        ways_output = "way";
        stairs_output = "stair";
    }

    cout << ways.size() << " " << ways_output << " to climb " << stair_num << " " << stairs_output << "." << endl;
    display_ways(ways);

    return 0;
}
