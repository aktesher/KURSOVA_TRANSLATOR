/*/
#include <iostream>
using namespace std;

int main()
{
	int A, B, X, Y;
	cout << "\t" << "ENTER A:";
	cin >> A;
	cout << "\t" << "ENTER B:";
	cin >> B;
	cout << "\t" << (A + B) << " \n";
	cout << "\t" << (A - B) << " \n";
	cout << "\t" << (A * B) << " \n";
	cout << "\t" << (A / B) << " \n";
	cout << "\t" << (A % B) << " \n";
	X = (A - B) * 10 + (A + B) / 10;
	cout << "\t" << X << " \n";
	Y = X + X % 10;
	cout << "\t" << Y << " \n";
	return 0;
}
/*/