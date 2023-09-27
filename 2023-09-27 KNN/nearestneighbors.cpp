#include <iomanip>
#include <iostream>
#include <limits>
#include <math.h>

using namespace std;

//Write a struct. These are classes, but everything is public.
//Also think JSON
struct Neighbor {
    float x, y;

    string name;

    //Initialization list
    Neighbor(float x_, float y_, string name_) : x{x_}, y{y_}, name{name_} {};
};

float distance(Neighbor a, Neighbor b)
{
    float diff_x = b.x - a.x;
    float diff_y = b.y - a.y;

    return diff_x * diff_x + diff_y * diff_y;
}

/**
 * Brute force algorithm. Check all possible neighbors, and return the nearest one
 * Not fast - in quadratic time
*/
pair<Neighbor, Neighbor> nearest_neighbor(Neighbor neighbor[], int length)
{
    float min = numeric_limits<float>::max(); //Returns biggest floating point number in max
    int min_i = 0, min_j = 0;

    for (int i = 0; i < length; i++)
    {
        //Use this to make sure each pair is only looked at once
        //Because once you check (1, 2), there's no need to check (2, 1) again
        for (int j = i + 1; j < length; j++)
        {
            float d = distance(neighbor[i], neighbor[j]);

            if (d < min)
            {
                min = d;
                min_i = i;
                min_j = j;
            }
        }
    }

    return make_pair(neighbor[min_i], neighbor[min_j]);
}

int main()
{
    Neighbor neighbors[4] = {
        Neighbor(1.2, 3.4, "A"),
        Neighbor(5.6, 7.8, "B"),
        Neighbor(9.0, 1.6, "C"),
        Neighbor(5.1, 9.2, "D")
    };

    pair<Neighbor, Neighbor> p = nearest_neighbor(neighbors, 4);
    cout << "Pairs: " << p.first.name << " " << p.second.name << endl;
    cout << "Distance: " << sqrt(distance(p.first, p.second)) << endl;
}