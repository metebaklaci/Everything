#include <bits/stdc++.h>

using namespace std;

const vector<int> chars = {'a', 'b', 'c', 'ç', 'd', 'e', 'f', 'g', 'ğ', 'h', 'ı', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'ö', 'p', 'q', 'r', 's', 'ş', 't', 'u', 'ü', 'v', 'w', 'x', 'y', 'z'};
const vector<int> nums = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};

double score(string pw){
    int charrunsquaresum = 0;
    int numrunsquaresum = 0;
    int charruncount;
    int numruncount;

    int ongoingcharrun = 0;
    int ongoingnumrun = 0;
    for(int i = 0; i < pw.size(); ++i){
        if(find(chars.begin(), chars.end(), pw[i]) != chars.end()){
            if(ongoingcharrun == 0){
                if(ongoingnumrun != 0){
                    numruncount++;
                    numrunsquaresum += ongoingnumrun * ongoingnumrun;
                    ongoingnumrun = 0;
                }
            }
            ongoingcharrun++;
        }
        
    }
}