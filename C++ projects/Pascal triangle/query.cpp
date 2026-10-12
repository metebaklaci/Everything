#include <bits/stdc++.h>
using namespace std;

long long choose(long long n, long long k){
    long long result = 1;
    if(k > n-k) k=n-k;
    for(long long i = n; i > n-k; i--){
        result*=i;
    }
    for(long long i = 2; i <= k; i++){
        result/=i;
    }
    return result;
}

int main(){
    cout << "Which row of the triangle? (first is 0) ";
    long long n;
    cin >> n;
    cout << "\nWhich number at this row starting from the left? (first is 0) ";
    long long k;
    cin >> k;
    cout << choose(n,k);
}