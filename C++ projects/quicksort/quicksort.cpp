#include <iostream>
#include <vector>
#include <cmath>
#include <ctime>
using namespace std;

/*
vector<double> quicksort(vector<double> passed) {
    vector<double> sort = passed;
    if(sort.size() < 2){
        return sort;
    }else{
        int pivotindex = (sort.size() - 1) / 2;
        int leftcheckindex = 0;

        while (leftcheckindex != pivotindex){ //iterate until pivot is one in front of the checked
            if(sort[leftcheckindex] >= sort[pivotindex]){
                double temp = sort[leftcheckindex];
                sort[leftcheckindex] = sort[pivotindex - 1];
                sort[pivotindex - 1] = sort[pivotindex];
                sort[pivotindex] = temp;
                pivotindex--;
            }else{
                leftcheckindex++;
            }
        }
        vector<double> partition1 = {};
        for (int i = 0; i < pivotindex; i++){
            partition1.push_back(sort[i]);
        }
        vector<double> partition2 = {};
        for (int i = pivotindex + 1; i < sort.size(); i++){
            partition2.push_back(sort[i]);
        }

        partition1 = quicksort(partition1);
        partition2 = quicksort(partition2);

        vector<double> sorted = {};

        sorted.insert(sorted.end(), partition1.begin(), partition1.end());
        sorted.push_back(sort[pivotindex]);
        sorted.insert(sorted.end(), partition2.begin(), partition2.end());

        return sorted;
    }
}

int main(){
    vector<double> unsorted = {};

    for (int i = 999; i >= 0; i--){ //Creates an inverted list to be sorted starting from 999 and ending at 0
        unsorted.push_back(i);
    }

    cout << "DEBUG" << endl;


    unsorted = {3,5,4,2,7,9,6,1,0,8};
    vector<double> sorted = quicksort(unsorted);
    for(int i = 0; i < sorted.size(); i++){
        cout << sorted[i] << " ";
    }
    return 0;
}

*/

int partition(vector<double>& arr, int low, int high)
{
    double pivot = arr[(low + high) / 2];   //Midpoint pivot

    int i = low;
    int j = high;

    while (i <= j)
    {
        while (arr[i] < pivot)
            i++;

        while (arr[j] > pivot)
            j--;

        if (i <= j)
        {
            swap(arr[i], arr[j]);
            i++;
            j--;
        }
    }

    return i;
}

void quicksort(vector<double>& arr, int low, int high)
{
    if (low >= high)
        return;

    int index = partition(arr, low, high);

    // Recursively sort the two halves
    quicksort(arr, low, index - 1);
    quicksort(arr, index, high);
}

int main(){
    time_t timestamp;
    time(&timestamp);
    vector<double> sort = {};

    for (int i = 99999999; i >= 0; i--){ //Creates an inverted list to be sorted starting from 999 and ending at 0
        sort.push_back(i);
    }

    cout << ctime(&timestamp) << endl;

    quicksort(sort, 0, sort.size()-1);
    for (int i = 0; i < sort.size(); i+=10000){
        cout << sort[i] << " ";
    }

    time(&timestamp);
    cout << endl << ctime(&timestamp) << endl;

    return 0;
}