#include <iostream>
#include <sstream>
#include <algorithm>
using namespace std;

//argc contains number of arguments (including name of cpp file)
//argv is an array of Strings
int main(int argc, char* argv[])
{
    //Initialize n and m
    int n, m;

    //Comes from sstream package
    istringstream iss;

    if (argc != 3)
    {
        cerr << "Usage: " << argv[0] << " <int> <int>" << endl;

        //Convention for programmers: Return 1 if error
        return 1;
    }

    //Chuck an item into istringstream.prototype
    iss.str(argv[1]);
    //iss >> n returns a "boolean-ish" value
    //print(iss >> n)
    if (!(iss >> n))
    {
        cerr << "Err: first command line arg must be an int" << endl;
        return 1;
    }

    //Make sure to clear the string because we're reusing the same input stream for both conversions
    iss.clear();
    
    iss.str(argv[2]);
    if (!(iss >> m))
    {
        cerr << "Err: first command line arg must be an int" << endl;
        return 1;
    }

    cout << "n is: " << n << endl;
    cout << "m is: " << m << endl;
    cout << "max(" << n << ", " << m << ") is " << max(n, m) << endl;
    
    return 0;
}