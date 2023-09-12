/*******************************************************************************
 * Name        : sieve.cpp
 * Author      : Justin Chen
 * Date        : September 11, 2023
 * Description : Sieve of Eratosthenes
 * Pledge      : I pledge my honor that I have abided by the Stevens Honor System.
 ******************************************************************************/
#include <cmath>
#include <iomanip>
#include <iostream>
#include <sstream>

using namespace std;

class PrimesSieve {
public:
    PrimesSieve(int limit);

    ~PrimesSieve() {
        delete [] is_prime_;
    }

    void display_primes() const;

private:
    // Instance variables
    bool * const is_prime_;
    const int limit_;
    int num_primes_, max_prime_;

    // Method declarations
    void sieve();
    static int num_digits(int num);
};

/**
 * This constructor initializes is_prime_ to contain the prime numbers from 2 to N
 * Justin Note: This also automatically runs sieve();
*/
PrimesSieve::PrimesSieve(int limit) :
        is_prime_{new bool[limit + 1]}, limit_{limit} {
    sieve();
}

/**
 * Prints out and displays the prime numbers
*/
void PrimesSieve::display_primes() const {
    //Display number of primes
    cout << "Number of primes found: " << this->num_primes_ << endl;
    cout << "Primes up to " << this->limit_ << ":" << endl;

    const int max_prime_width = num_digits(this->max_prime_), primes_per_row = 80 / (max_prime_width + 1);

    //A tally of how many primes we printed. Resets back to 0 once we new line.
    int primes_printed = 0;
    int total_primes_printed = 0;

    //Starts with 2
    for (int i = 2; i <= this->limit_; i++)
    {
        //Check if it's even prime to begin with so we can start printing
        if (this->is_prime_[i])
        {
            total_primes_printed++;
            primes_printed++;
            if ((total_primes_printed == this->num_primes_) || (primes_printed == primes_per_row))
            {
                // Printing is dumb. If it's multiline, setw. If not, don't.
                if (this->num_primes_ > primes_per_row)
                {
                    cout << setw(max_prime_width) << i << endl;
                }
                else
                {
                    cout << i << endl;
                }
            }
            else
            {
                // Printing is dumb. If it's multiline, setw. If not, don't.
                if (this->num_primes_ > primes_per_row)
                {
                    cout << setw(max_prime_width) << i << " ";
                }
                else
                {
                    cout << i << " ";
                }
            }

            //If we printed the max # of primes, restart the count
            if (primes_printed >= primes_per_row)
            {
                primes_printed = 0;
            }
        }
    }
}

/**
 * Perform the Sieve of Eratosthenes
*/
void PrimesSieve::sieve() {

    //Step 1: Set everything in the Sieve to true
    //It's <= limit_ as the array size is limit_ + 1
    //Start at 2 because index 0 and index 1 are not primes and are irrelevant
    for (int i = 2; i <= this->limit_; i++)
    {
        this->is_prime_[i] = true;
    }

    //Step 2: Loop through sqrt([2, n])
    for (int i = 2; i <= sqrt(this->limit_); i++)
    {
        if (this->is_prime_[i])
        {
            //Step 3: If the item is prime, square it, find all multiples after that until n, and flag it as not prime
            for (int j = i * i; j <= this->limit_; j += i )
            {
                this->is_prime_[j] = false;
            }
        }
    }

    // Step 3: Prime statistics

    //Set num_primes_ and max_prime_ to 0 for now
    this->num_primes_ = 0;
    this->max_prime_ = 2;   //Smallest max prime is 2

    //If something is prime, add it to num_primes
    for (int i = 2; i <= this->limit_; i++)
    {
        if (this->is_prime_[i])
        {
            this->num_primes_ += 1;

            //Now check if the num_primes_ is the max prime.
            if (i > this->max_prime_)
            {
                this->max_prime_ = i;
            }
        }
    }
}

/**
 * Determines how many digits are in an integer
 * Assumes num > 0 since we restricted user limit input to > 2
*/
int PrimesSieve::num_digits(int num) {
    int num_digits = 0;

    while (num > 0)
    {
        //Floor shouldn't be necessary but Justin case
        num = floor(num / 10);
        num_digits += 1;
    }

    return num_digits;
}

int main() {
    cout << "**************************** " <<  "Sieve of Eratosthenes" <<
            " ****************************" << endl;
    cout << "Search for primes up to: ";
    string limit_str;
    cin >> limit_str;
    int limit;

    // Use stringstream for conversion. Don't forget to #include <sstream>
    istringstream iss(limit_str);

    // Check for error.
    if ( !(iss >> limit) ) {
        cerr << "Error: Input is not an integer." << endl;
        return 1;
    }
    if (limit < 2) {
        cerr << "Error: Input must be an integer >= 2." << endl;
        return 1;
    }

    // Constructs a sieve class with the limit given
    PrimesSieve sieve = PrimesSieve(limit);

    //Displays the chat interface. But first pad some newlines
    cout << endl;

    sieve.display_primes();

    return 0;
}
