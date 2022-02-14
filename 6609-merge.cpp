#include <iostream>
#include <pthread.h>
#include <ctime>
#include <fstream>
#include <sstream>

using namespace std;

// merge function to merge 2 arrays
void merge(int array[],int low, int mid, int high){
    int* left= new int[mid-low+1];
    int* right = new int[high-mid];
    //size of first and second array;
    int size1= mid-low+1, size2= high-mid;

    //copying values into first array
    for (int i = 0; i < size1; ++i)
        left[i] = array[i+low];
    for (int i = 0; i < size2; ++i)
        right[i] = array[i+mid+1];

    int k=low,i,j;
    i=j=0;

    //merging in order
    while(i<size1 && j<size2){
        if(left[i] <= right[j])
            array[k++] = left[i++];
        else array[k++] = right[j++];
    }
    //insert remaining values
    while(i < size1) array[k++] = left[i++];
    while(j < size2) array[k++] = right[j++];
}


typedef struct pmerge_args{
    int *array;
    int low;
    int high;
} pmerge_args;

//multithreading
void* thread_merge_sort(void* arg){
    pmerge_args pargs = *(pmerge_args*)arg;
    //calculating boundaries
    int low = pargs.low;
    int high = pargs.high;
    int *a   = pargs.array;

    //calculating mid point
    int mid = (low+high)/2;
    if(low == high) pthread_exit(0);
    else if (low < high){
        pthread_t upper,lower;
        pmerge_args uppera = {.array=a, .low=low,.high=mid};
        pmerge_args lowera = {.array=a, .low=mid+1,.high=high};

        pthread_create(&lower,NULL,thread_merge_sort,(void*)&lowera);
        pthread_create(&upper,NULL,thread_merge_sort,(void*)&uppera);

        pthread_join( lower, NULL);
        pthread_join( upper, NULL);

        merge(a,low,mid,high);
    }
}

int main(int argc, char ** argv) {
    if(argc < 2){
        cerr << "Please provide the file path as input.";
        exit(-1);
    }

    fstream input;
    input.open(argv[1]);
    string line;
    int n = 0;
    int *a;
    if(!input.is_open()){
        cerr << "Failed to open file. Ensure file exists.";
        exit(-1);
    }else {
        getline(input, line );
        n = stoi(line);
        if(!input.eof()) {
            getline(input, line);
            istringstream intstringstream(line);
            a = static_cast<int *>(malloc(sizeof(int) * n));
            for(int i =0 ; i < n ;i++){
                intstringstream >> a[i];
            }
        }else{
            cerr<< "invalid file format. first line must contain the number of entries and the second line the entries";
            exit(-1);
        }
    }
    for(int i =0;i <n ;i++){
        cout << a[i] << " ";
    }cout << endl;

    clock_t starting_time, end_time;
    starting_time = clock();
    pthread_t thread;
    pmerge_args pmergeArgs = {.array=a,.low=0,.high = n-1};
    pthread_create(&thread,NULL,thread_merge_sort,(void*)&pmergeArgs);
    pthread_join(thread,NULL);

    end_time =clock();
    cout << "Sorted Array\n";
    for (int i = 0; i < n; ++i) {
        cout << a[i] << " ";
    }cout <<  endl;
    cout << "Time: " << (end_time - starting_time)/(double)CLOCKS_PER_SEC << "\n";

    return 0;
}
