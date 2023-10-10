/*******************************************************************************
 * Name        : unique.cpp
 * Author      : Justin Chen
 * Date        : 9/25/2023
 * Description : Determining uniqueness of chars with int as bit vector.
 * Pledge      : I pledge my honor that I have abided by the Stevens Honor System.
 ******************************************************************************/
#include <iostream>
#include <cctype>

using namespace std;

//ASCII lower case values go from 97 to 122
bool is_all_lowercase(const string &s) {

    //Idea for using s.length() comes from Meunier's BitManipulation.txt file on Canvas
    for (size_t i = 0; i < s.length(); i++)
    {
        //For something to be a lower case letter, its ASCII value must be >= 97 ('a')
        //and <= 122 ('z')
        bool within_a_bounds = s[i] - 'a' >= 0;
        bool within_z_bounds = s[i] - 'z' <= 0;

        //If it's not within either bounds, it's not lowercase.
        if (!(within_a_bounds && within_z_bounds))
        {
            return false;
        }
    }

    return true;
}

bool all_unique_letters(const string &s) {
    //The unsigned int that will act as a dictionary for us to run bitwise operations on
    //In order to determine uniqueness
    unsigned int bitset = 0;

    for (size_t i = 0; i < s.length(); i++)
    {
        unsigned int curr_letter_bitset = 1 << (s[i] - 'a');

        if ((curr_letter_bitset & bitset) != 0)
        {
            //This means we have a duplicate
            return false;
        }

        //Update the bitset with new curr_letter_bitset
        bitset = bitset | curr_letter_bitset;
    }

    return true;
}

int main(int argc, char * const argv[]) {
    if (argc != 2) //Error: No arguments and too many args
    {
        cerr << "Usage: " << argv[0] << " <string>" << endl;
        return 1;
    }

    if (!(is_all_lowercase(argv[1])))
    {
        cerr << "Error: String must contain only lowercase letters." << endl;
        return 1;
    }

    if (!(all_unique_letters(argv[1])))
    {
        cout << "Duplicate letters found." << endl;
    }
    else
    {
        cout << "All letters are unique." << endl;
    }

    return 0;
}
