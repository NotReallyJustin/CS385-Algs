/*******************************************************************************
 * Name        : inversioncounter.cpp
 * Author      : Justin Chen
 * Version     : 1.0
 * Date        : 10/26/2023
 * Description : Counts the number of inversions in an array.
 * Pledge      : I pledge my honor that I have abided by the Stevens Honor System.
 ******************************************************************************/
#include <iostream>
#include <algorithm>
#include <sstream>
#include <vector>
#include <cstdio>
#include <cctype>
#include <cstring>

using namespace std;

// Function prototype.
static long mergesort(int array[], int scratch[], int low, int high);

/**
 * Counts the number of inversions in an array in Theta(n^2) time using two nested loops.
 * Inversions happen when a larger number appears before a smaller number.
 * Hence, we will iterate through 2 nested loops and check if arr[i] > arr[j]
 * @param array[] An array to count the inversion of
 * @param length The length of the array
 * @return The number of inversions
 */
long count_inversions_slow(int array[], int length) 
{
    long num_inversions = 0;

    //Loop through the array
    for (int i = 0; i < length; i++)    
    {
        //If the current index is > any future index, that's an inversion
        for (int j = i + 1; j < length; j++)
        {
            if (array[i] > array[j])
            {
                num_inversions += 1;
            }
        }
    }

    return num_inversions;
}

/**
 * Debugging tool I wrote to print an array
 * @param array[] The array to print
 * @parma length Length of array to print
*/
void print_arr(int array[], int length)
{
    for (int i = 0; i < length; i++)
    {
        cout << array[i] << endl;
    }
    cout << "---" << endl;
}

/**
 * Counts the number of inversions in an array in Theta(n lg n) time.
 * This uses merge sort and compares the number of swaps according to Meunier's formula
 * @param array[] The array to count inversions for
 * @param length The length of the array
 * @return Number of Inversions
 */
long count_inversions_fast(int array[], int length) {
    //Since merge sort counts the inversions, we will call merge sort and have that return our inversion #

    int* scratch = new int[length];                 //Create this temp scratch array
    long result = mergesort(array, scratch, 0, length - 1);

    //print_arr(array, length);

    delete[] scratch;
    return result;
}

/**
 * Implements merge sort in order to count inversions
 * @param array[] The array to "merge sort"
 * @param scratch[] Initialize this to length of array. We will use this to [low...high]. Heap preservation doesn't matter.
 * @param low The low index to merge
 * @param high The high index to merge
 * @return The number of inversions this instance of merge sort caught
 * The scratch array repeats, but it doesn't matter because it's temporary and we overwrite it
*/
static long mergesort(int array[], int scratch[], int low, int high) {
    if (low >= high)        //After debugging it a few times, you need the equal because mid = left sometimes
    {
        return 0;               //There are no merges if you don't merge sort
    }

    int mid = (low + high) / 2;
    
    //Tracks how many inversions happen on the left
    long left = mergesort(array, scratch, low, mid);

    //Tracks how many inversions happen on the right
    long right = mergesort(array, scratch, mid + 1, high);

    //Use this to find total number of inversions
    long num_inversions = left + right;

    //Now, we merge it while counting number of inversions
    int scratch_idx = low;
    int i = low;    //Tracks the left "sorted array" - this is i1
    int j = mid + 1;    //Tracks the right "sorted array" - this is i2

    while (i <= mid && j <= high)   //When both of them are in looping range
    {
        if (array[j] < array[i])    //If the right array is < left, we merge and add to inversion
        {
            scratch[scratch_idx] = array[j];
            num_inversions += (mid - i + 1);        //When we copy i2, the number of inversions is mid - i1 (i in this case) + 1

            j++;                    // Increment j and scratch_idx so we know we moved on
            scratch_idx++;
        }
        else    //Otherwise, we chuck the left into scratch and do nothing about inversion
        {
            scratch[scratch_idx] = array[i];
            i++;                        //Increment i and idx
            scratch_idx++;
        }
    }

    //Now, put the missing items into the array
    if (i <= mid)
    {
        while (i <= mid)
        {
            scratch[scratch_idx] = array[i];    //Chuck into scratch
            scratch_idx++;                      //Increment both
            i++;
        }
    }
    
    if (j <= high)
    {
        while (j <= high)
        {
            scratch[scratch_idx] = array[j];    //Chuck into scratch
            scratch_idx++;                      //Increment both
            j++;
        }
    }

    //Now, we put the items in the scratch array back into the original merged array
    for (int k = low; k <= high; k++)
    {
        array[k] = scratch[k];
    }

    return num_inversions;
}

int main(int argc, char *argv[]) {
    // Parse command-line argument
    //If there's too many (or too few) arguments
    if (argc < 0 || argc > 2)
    {
        cerr << "Usage: ./inversioncounter [slow]" << endl;
        return 1;
    }

    //If there is an argv[1] and it's now Slow
    //I got an error when using != because it says "comparison with string literal results in unspecified behavior"
    //I took the solution from https://stackoverflow.com/questions/2603039/warning-comparison-with-string-literals-results-in-unspecified-behaviour
    //and decided to use strcmp()
    if (argc == 2 && strcmp(argv[1], "slow") != 0)
    {
        cerr << "Error: Unrecognized option '" << argv[1] << "'." << endl;
        return 1;
    }

    cout << "Enter sequence of integers, each followed by a space: " << flush;

    istringstream iss;
    int value, index = 0;
    vector<int> values;
    string str;
    str.reserve(11);
    char c;
    while (true) {
        c = getchar();
        const bool eoln = c == '\r' || c == '\n';
        if (isspace(c) || eoln) {
            if (str.length() > 0) {
                iss.str(str);
                if (iss >> value) {
                    values.push_back(value);
                } else {
                    cerr << "Error: Non-integer value '" << str
                         << "' received at index " << index << "." << endl;
                    return 1;
                }
                iss.clear();
                ++index;
            }
            if (eoln) {
                break;
            }
            str.clear();
        } else {
            str += c;
        }
    }
    
    
    //ERROR CHECKING-------------------------------------
    //Check if we actually have a sequence of integers
    int arr_len = values.size();
    if (arr_len == 0)
    {
        cerr << "Error: Sequence of integers not received." << endl;
        return 1;
    }

    //This produces output. It runs fast inversion counter if there's no arguments, or slow inversion if specified

    long numInversions = 0;
    if (argc == 2 && strcmp(argv[1], "slow") == 0) // Run slow inversion
    {
        numInversions = count_inversions_slow(&values[0], arr_len);
        cout << "Number of inversions (slow): " << numInversions << endl;
    }
    else                //Run fast inversion
    {
        numInversions = count_inversions_fast(&values[0], arr_len);
        cout << "Number of inversions (fast): " << numInversions << endl;
    }

    return 0;
}
