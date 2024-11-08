#include<iostream>
using namespace std;
double a;
int main()
{
    for (int i = 0;i < 100000;i++)
        for (int j = 0;j < 100000;j++)
            a+=i*j;
}