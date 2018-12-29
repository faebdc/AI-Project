#include<iostream>
using namespace std;

int main()
{
    double eps = 0.1;
    double prob[7];
    prob[1] = 0;
    prob[2] = 1;
    for(int i = 3; i < 7; i++)
    {
        prob[i] = (prob[i-1] - (1 + eps)*prob[i-2]/2)/(1 - eps) * 2;
    }
    cout<<1/prob[6]<<endl;
    return 0;
}