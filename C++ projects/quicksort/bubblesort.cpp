#include <iostream>
#include <vector>
#include <ctime>
using namespace std;

void bubbleSort(vector<double>& arr) {
    int n = arr.size();
    bool swapped;

    for (int i = 0; i < n - 1; i++) {
        swapped = false;

        for (int j = 0; j < n - i - 1; j++) {
            if (arr[j] > arr[j + 1]) {
                swap(arr[j], arr[j + 1]);
                swapped = true;
            }
        }

        // If no swaps happened, the vector is already sorted
        if (!swapped)
            break;
    }
}

int main() {
    time_t timestamp;
    time(&timestamp);
    vector<double> sort = {};

    for (int i = 99999; i >= 0; i--){ //Creates an inverted list to be sorted starting from 999 and ending at 0
        sort.push_back(i);
    }

    cout << ctime(&timestamp) << endl;

    bubbleSort(sort);
    for (int i = 0; i < sort.size(); i+=10000){
        cout << sort[i] << " ";
    }

    time(&timestamp);
    cout << endl << ctime(&timestamp) << endl;

    return 0;
}
