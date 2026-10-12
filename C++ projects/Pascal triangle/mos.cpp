#include <bits/stdc++.h>
using namespace std;

struct Query {
    int id;
    int n;
    int k;
};

int main(){
    int N,Q;
    cin >> N;
    cin >> Q;

    int sqrtN = ceil(sqrt(N));

    vector<Query> queries;
    for(int i = 0; i < Q; i++){
        int n,k;
        cin >> n >> k;
        queries.push_back({i,n,k});
    }
    sort(queries.begin(), queries.end(), [](const Query& a, const Query& b) {return a.n < b.n;});
    vector<int> answers(Q);
    vector<vector<Query>> partitions(sqrtN+1, vector<Query>());
    for(int i = 0; i < Q; i++){
        partitions[queries[i].k/sqrtN].push_back(queries[i]);
    }
    for(int i = 0; i < sqrtN+1; i++){
        int answer = 1;
        int tempn = 0;
        int tempk = 0;
        for(int j = 0; j < partitions[i].size(); j++){
            
        }
    }
}