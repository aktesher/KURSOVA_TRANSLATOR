#include <iostream>
using namespace std;

int main() 
{
   int A, B, C, D, V;
   D = -5;
   V = -8;
   A = 0;
   B = 6;
   cout << "\t"<< "ENTER A:"; 
   cin >> A;
   if (A != 15)
   {
   B = (B / 3);
   cout << "\t" << B << " \n";
   }
   goto ABC;
ABC:  
   for (int C = 0; C < 5; C++)
   {
   cout << "\t" << (B * 5) << " \n";
   }
   if (V <= D)
   {
   cout << "\t" << (D % 2) << " \n";
   }
   if (V >= D)
   {
   cout << "\t" << (V % 2) << " \n";
   }
   return 0;
}
