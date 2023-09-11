/*******************************************************************************
 * Filename: sqrt.cpp
 * Author  : Justin Chen
 * Version : 1.0
 * Date    : September 6, 2023
 * Description: Computes the square root of a given number
 * Pledge  : I pledge my honor that I have abided by the Stevens Honor System.
 ******************************************************************************/

#include <iostream>
#include <sstream>
#include <limits>
#include <algorithm>
#include <iomanip>
using namespace std;

double sqrt(double num, double epsilon)
{
    if (num < 0)
    {
        return numeric_limits<double>::quiet_NaN();
    }
    else if (num == 1 || num == 0)
    {
        return num;
    }
    else
    {
        double last_guess = 0;
        double next_guess = num;

        while (!(abs(last_guess - next_guess) <= epsilon))
        {
            last_guess = next_guess;
            next_guess = (last_guess + num / last_guess) / 2;
        }

        return next_guess;
    }
}

int main(int argc, char* argv[])
{
    double value, epsilon;
    istringstream iss = istringstream();

    if (argc != 2 && argc != 3)
    {
        cerr << "Usage: " << argv[0] << " <value> [epsilon]" << endl;
        return 1;
    }
    else if (argc == 2)
    {
        epsilon = 1e-7;
        iss.str(argv[1]);

        if (!(iss >> value))
        {
            cerr << "Error: Value argument must be a double." << endl;
            return 1;
        }
    }
    else if (argc == 3)
    {
        iss.str(argv[1]);

        if (!(iss >> value))
        {
            cerr << "Error: Value argument must be a double." << endl;
            return 1;
        }

        iss.clear();

        iss.str(argv[2]);

        if (!(iss >> epsilon))
        {
            cerr << "Error: Epsilon argument must be a positive double." << endl;
            return 1;
        }

        if (epsilon <= 0)
        {
            cerr << "Error: Epsilon argument must be a positive double." << endl;
            return 1;
        }
    }

    double result = sqrt(value, epsilon);
    cout << fixed << setprecision(8) << result << endl;

    return 0;
}