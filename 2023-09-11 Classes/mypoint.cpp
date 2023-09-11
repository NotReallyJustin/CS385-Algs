#include <iostream>
#include <vector>
#include <iomanip>
using namespace std;

class MyPoint {
    public:
        MyPoint(int x, int y, float z) : x_{x}, y_{y}, z_{z} { }

        void print_coords() const {
            cout << "(x, y, z) = (" << x_ << ", " << y_ << ", " << z_ << ")" << endl;
        }

        void set_x(int x) {
            x_ = x;
        }

        int get_x() const {
            return x_;
        }

    private:
        int x_;
        int y_;
        float z_;
};

//You can do it via iterators
void display_points(const vector<MyPoint> &points)
{
    // it++ - Tells iterator to move on to the next iteration
    for (auto it = points.cbegin(); it != points.cend(); it++)
    {
        //it stores the memory address (and is a pointer to) the vector object
        it -> print_coords();

        //Same as (*it).print_coords();
        // The * here is a dereference operator
    }
}

// Remember & is pass by reference
void display_points_v2(const vector<MyPoint> &points)
{
    // size_t is a good class to use to represent indices. It's guarenteed to always be big enough to count anything we want
    // This is useful because points.size() returns a size_t
    for (size_t i = 0; i < points.size(); i++)
    {
        points[i].print_coords();
    }
}

int main() {
    MyPoint point1(5, 7, 1.9823412);
    MyPoint point2(1, 2, 3);
    MyPoint point3(4, 5, 6);

    //Vectors are really important in C++!
    //<E> contains generics
    vector<MyPoint> points;
    points.push_back(point1);
    points.push_back(point2);
    points.push_back(point3);

    //Vector of all points what have a x value about 2
    vector<MyPoint> points_above_two;

    // For-each loop
    //const is a good convention so you don’t change point.
    //auto keyword will allow C++ to auto-detect the type of point based on what generic type is plugged into the vector.
    //The & here is passed by reference - and not memory address!!!

    for (const auto &point: points) {
        if (point.get_x() > 2)
        {
            points_above_two.push_back(point);
        }
    }
    
    display_points_v2(points);
    cout << "----" << endl;
    display_points(points_above_two);
    return 0;
}