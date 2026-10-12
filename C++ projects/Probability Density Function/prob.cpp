#include <bits/stdc++.h>

using namespace std;

double sampleRand(){
    double rand1 = (rand()*1.0)/RAND_MAX;
    double rand2 = (rand()*1.0)/RAND_MAX;
    //double randvalue = pow(rand01_1, rand01_2);
    //double randvalue = rand01_1 / rand01_2;
    //double randvalue = pow(2,rand01_1);
    double randvalue = 3*pow((rand1 - 0.5),2) + 0.2;
    return randvalue;
}

int main(){
    cout << fixed << setprecision(3);

    srand(time(0));


    int bucketcount = 20;

    double bucket_lower = 0;
    double bucket_upper = 1;


    vector<int> buckets = {};
    for(int i = 0; i < bucketcount; ++i){
        buckets.push_back(0);
    }
    

    int samplecount = 10000000;


    for(int i = 0; i < samplecount; ++i){
        double sample = sampleRand();
        int targetbucket = floor(((sample - bucket_lower) / (bucket_upper - bucket_lower)) * bucketcount);
        if(targetbucket >= 0 && targetbucket < bucketcount){
            buckets[targetbucket]++;
        }
    }


    double samplesperbar = 30000;


    cout << "\n";
    for(int i = 0; i < bucketcount; ++i){
        cout << i*(bucket_upper - bucket_lower)/bucketcount + bucket_lower << "-" << (i+1)*(bucket_upper - bucket_lower)/bucketcount + bucket_lower << ": ";
        for(int j = 0; j < buckets[i]; j += samplesperbar){
            cout << "#";
        }
        cout << "\n";
    }
}