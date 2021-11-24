#include <utility/ostream.h>

using namespace EPOS;

OStream cout;

void stress_test()
{

}

void method_test()
{

}

char* alloc(int bytes) {
    cout << "Allocating " << bytes << "bytes." << endl;
    return new char[bytes];
}

int main()
{
    cout << "Hello World!" << endl;
    alloc(1 << 2);
    alloc(1 << 5);
    alloc(1 << 10);
    alloc(1 << 12);
    alloc(1 << 14);
    alloc(1 << 16);
  
    //cout << "Running stress test" << endl;
    //stress_test();

    //cout << "Running First-Fit correctness test" << endl;
    //method_test();

    return 0;
}
