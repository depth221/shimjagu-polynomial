#include <iostream>

#include "polynomial.h"

using namespace std;

int main() {
    Polynomial a, b;

    // (a) istream& operator >>
    cin >> a;
    cin >> b;

    // (b) ostream& operator <<
    cout << a << "\n";
    cout << b << "\n";

    // (c) Copy Constructor
    Polynomial *c = new Polynomial(a);
    cout << *c << "\n";

    // (d) Assignment
    *c = b;
    cout << *c << "\n";

    // (e) Destructor
    delete c;

    // (f) Addition
    cout << a + b << "\n";

    // (g) Substraction
    cout << a - b << "\n";

    // (h) Multiplication
    cout << a * b << "\n";

    // (i) Evaluate
    float x = 2.5;
    cout << a.Evaluate(x) << "\n";
    cout << b.Evaluate(x) << "\n";
}
