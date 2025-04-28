#include <iostream>
using namespace std;

int main() 
{
   int A, B, X, TEMPA, TEMPB, TEMPC, I, J;
   cout << "\t"<< "ENTER A:"; 
   cin >> A;
   cout << "\t"<< "ENTER B:"; 
   cin >> B;
   X = 0;
   TEMPA = 37707;
   TEMPB = A;
   TEMPC = B;
   if (A >= B)
   {
   goto END;
   cout << "\t" << A << " \n";
   }
   for (int TEMPB = A; TEMPB < TEMPC; TEMPB++)
   {
   cout << "\t" << (TEMPB * TEMPB) << " \n";
   }
   for (int I = 0; I < TEMPB; I++)
   {
   for (int J = 0; J < TEMPC; J++)
   {
   X = X + 1;
   }
   }
   cout << "\t" << X << " \n";
END:  
   cout << "\t" << TEMPA << " \n";
   return 0;
}
