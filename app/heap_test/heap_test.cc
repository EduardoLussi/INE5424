#include <utility/ostream.h>

using namespace EPOS;

OStream cout;

char* alloc(int bytes) {
    cout << "Allocating " << bytes << " bytes." << endl;
    return new char[bytes];
}

int main()
{
    cout << "################################" << endl;
    cout << "#       Entering main...       #" << endl;
    cout << "# Testing the heap allocation! #" << endl;
    cout << "################################" << endl;
    alloc(1 << 2);
    alloc(1 << 5);
    alloc(1 << 10);
    alloc(1 << 12);
    alloc(1 << 14);
    alloc(1 << 16);

    return 0;
}
