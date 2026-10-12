#include <bits/stdc++.h>
using namespace std;

struct FactorInt{
    vector<int> factors;
    FactorInt(int num, int primes){
        
    }
};

int main(){

    cout << "Which row of the triangle? (first is 0) ";
    long long n;
    cin >> n;
    cout << "\nWhich number at this row starting from the left? (first is 0) ";
    long long k;
    cin >> k;

    vector<bool> sieve(10000 + 1,true);
    sieve[0] = false;
    sieve[1] = false;
    int i = 0;
    while(i <= 10000){
        while(i <= 10000 && !sieve[i]){
            i++;
        }
        if(i > 10000) break;
        for(int j = 2*i; j <= 10000; j+=i){
            sieve[j]=false;
        }
        i++;
    }

    vector<int> primes = {};

    for(int i = 0; i <= 10000; i++){
        if(sieve[i]) primes.push_back(i);
    }
}