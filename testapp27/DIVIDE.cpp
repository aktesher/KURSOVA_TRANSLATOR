/*/
#include <iostream>
using namespace std;

int main()
{
	int A, B, C, TEMPA, TEMPB, TEMPC, TEMPD, TEMPE;
	TEMPA = -1000000;
	TEMPC = 1;
	TEMPD = 0;
	TEMPE = -1;
	cout << "\t" << "ENTER A:";
	cin >> A;
	cout << "\t" << "ENTER B:";
	cin >> B;
	cout << "\t" << "ENTER C:";
	cin >> C;
	if ((TEMPA <= A && TEMPA <= B && TEMPA <= C))
	{
		if (A >= B)
		{
			if (A >= C)
			{
				TEMPB = A;
				cout << "\t" << TEMPB << " \n";
			}
		}
		if (C >= B)
		{
			if (C >= A)
			{
				TEMPB = C;
				cout << "\t" << TEMPB << " \n";
			}
		}
		if (B >= C)
		{
			if (B >= A)
			{
				TEMPB = B;
				cout << "\t" << TEMPB << " \n";
			}
		}
	}
	if ((A == B && B == C && A == C))
	{
		cout << "\t" << TEMPC << " \n";
	}
	if ((A != B || B != C || A != C))
	{
		cout << "\t" << TEMPD << " \n";
	}
	if ((A <= 0 || B <= 0 || C <= 0))
	{
		cout << "\t" << TEMPE << " \n";
	}
	if ((A >= 0 || B >= 0 || A >= 0))
	{
		cout << "\t" << TEMPD << " \n";
	}
	return 0;
}
/*/