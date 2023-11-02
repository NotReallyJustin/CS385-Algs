/*******************************************************************************
 * Name          : quickselect.cpp
 * Author        : Justin Chen
 * Pledge        : I pledge my honor that I have abided by the Stevens Honor System
 * Date          : 10/26/2023
 * Description   : Implements the quickselect algorithm.
 ******************************************************************************/
#include <iostream>
#include <sstream>
#include <algorithm>
#include <vector>

using namespace std;

size_t lomuto_partition(int array[], size_t left, size_t right) {
    // TODO
    // DO NOT change the function header in any way, otherwise you will lose points.

    //Do the median of 3 method. From the left, right, and middle, pick the middle item.
    //We will use insertion sort to do this
    //Using insertion sort increases the runtime a tiny bit. However, since it's an array of length 3, it doesn't
    //make much difference in the long run
    int medianArr[3] = {array[left], array[(left + right) / 2], array[right]};
    size_t arrIdx[3] = {left, (left + right) / 2, right};

    for (int i = 1; i < 3; i++)         //Array length 3
    {
        for (int j = i - 1; j >= 0; j--)
        {
            if (medianArr[j + 1] < medianArr[j])            //Swap
            {
                int temp = medianArr[j + 1];
                medianArr[j + 1] = medianArr[j];
                medianArr[j] = temp;

                //Also swap the arr indices too
                temp = arrIdx[j + 1];
                arrIdx[j + 1] = arrIdx[j];
                arrIdx[j] = temp;
            }
        }
    }

    size_t middleidx = arrIdx[1];          //Middle of the 3 elements
    int middleVal = array[middleidx];
    array[middleidx] = array[left];
    array[left] = middleVal;           // Set that middle to be the pivot

    //cout << middleVal << endl;

    //Now, we want to begin lomuto partition
    size_t s = left;                       // Start both S and i being at the pivot index    
    for (size_t i = left + 1; i <= right; i++) //Start running quick sort
    {
        if (array[i] < array[left])     //if the current item is < the pivot, s++ and swap.
        {
            s += 1;                     // s++

            //Swap
            int temp = array[s];
            array[s] = array[i];
            array[i] = temp;
        }
    }

    //At the end, swap pivot and arr[s]
    int temp = array[left];
    array[left] = array[s];
    array[s] = temp;

    //Return the middle index
    return s;
}

/**
 * Quick select finds the kth smallest element
 * A returned s is the kth smallest element in the array
 * because of how indexing works, s = k + 1
*/
int quick_select(int array[], size_t left, size_t right, size_t k) {
    // TODO
    // DO NOT change the function header in any way, otherwise you will lose points.

    size_t s = lomuto_partition(array, left, right);
    if (s == k - 1)
    {
        return array[s];
    }
    else if (s > k - 1)         //Our index exceeded the kth smallest element we want. Look to the left now
    {
        return quick_select(array, left, s - 1, k);
    }
    else                        //Our index is less than the kth smallest element we want. Look to the right now
    {
        return quick_select(array, s + 1, right, k);
    }
}

int quick_select(int array[], const size_t length, size_t k) {
    return quick_select(array, 0, length - 1, k);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <k>" << endl;
        return 1;
    }

    int k;
    istringstream iss;
    iss.str(argv[1]);
    if ( !(iss >> k) || k <= 0 ) {
        cerr << "Error: Invalid value '" << argv[1] << "' for k." << endl;
        return 1;
    }

    cout << "Enter sequence of integers, each followed by a space: " << flush;
    int value, index = 0;
    vector<int> values;
    string str;
    str.reserve(11);
    char c;
    iss.clear();
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

    int num_values = values.size();
    if (num_values == 0) {
        cerr << "Error: Sequence of integers not received." << endl;
        return 1;
    }

    // TODO - error checking k against the size of the input
    if (k > num_values)
    {
        if (num_values == 1)
        {
            cerr << "Error: Cannot find smallest element " << k << " with only " << num_values << " value." << endl;
        }
        else
        {
            cerr << "Error: Cannot find smallest element " << k << " with only " << num_values << " values." << endl;
        }

        return 1;
    }
    
    // TODO - call the quick_select function and display the result

    int smallest_el = quick_select(&values[0], num_values, k);
    cout << "Smallest element " << k << ": " << smallest_el << endl;
    
    return 0;
}
