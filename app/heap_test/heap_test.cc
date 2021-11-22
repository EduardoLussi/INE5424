#include <utility/ostream.h>

using namespace EPOS;

OStream cout;

void stress_test()
{

}

void method_test()
{

}

int main()
{
    cout << "Running stress test" << endl;
    stress_test();

    cout << "Running First-Fit correctness test" << endl;
    method_test();

    return 0;
}
