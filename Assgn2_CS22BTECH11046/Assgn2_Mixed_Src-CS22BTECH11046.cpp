#include <iostream>
#include <vector>
#include <thread>
#include <fstream>
#include <ctime>
#include <cassert>
#include <chrono>

using namespace std;

vector<vector<int>> omatrix;  /*declaring out_put_matrix globally */  
vector<int> times;             /*times vector for storing time of each thread*/
int n, k, b, c, p, BT;
struct thread_data {    /*struct to store thread_data*/
    const vector<vector<int>>& imt;
    int index;
};

/*Assigning rows to threads according to mixed and multiplying matrix*/
void *mixed_multiplication(void* arg) {
    auto start_time = std::chrono::high_resolution_clock::now();/*start time of each thread*/
    thread_data* thread = static_cast<thread_data*>(arg);/*casting data to thread variable*/
    int point =thread-> index;
    const vector<vector<int>>& im = thread->imt;

    /*multiplication*/
    for(int i = point;i< n;i= i+ k){
        for (int j=0; j<n;j++){
            for (int l=0;l<n;l++){
                omatrix[i][j] = omatrix[i][j] + im[i][l]*im[l][j];
            }	
        }
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    auto time_spent = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
    times[point] = time_spent;
    delete thread; // Free allocated memory
    return nullptr;
}

int main() {
    int l = 0;
    // Reading input file and storing the variables and matrix.
    ifstream i_file("inp.txt");
    i_file >> n >> k >> c >> BT;
    p = (n / k);
    b = (k / c);
    if(k>c){
		l = (BT / b);
	}else{
		l= k/2;
		b=1;
	}

    vector<vector<int>> imatrix(n, vector<int>(n, 0));/*imatrix for storing input matrix*/

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            i_file >> imatrix[i][j];
        }
    }

    i_file.close();

    /* Creating cpu sets*/
    cpu_set_t cpu_sets[l];

    for (int i = 0; i < l; i++) {
        CPU_ZERO(&cpu_sets[i]);
        CPU_SET(i, &cpu_sets[i]);
    }

    omatrix.resize(n, vector<int>(n, 0));   /*fixing size for output_matrix*/
    times.resize(k);                        /*fixing size of stroring times to k*/

    auto st_time = std::chrono::high_resolution_clock::now();/*start time for overall threads perfomance*/

    vector<pthread_t> mixed;/*mixed is vector of p_threads*/

    int j = 0;
    for (int i = 0; i < l; i++) {   /*fot BT threads*/
        for (int m = 0; m < b; m++) {
            int ind = j;
            pthread_t thread_id;
            assert(pthread_create(&thread_id, nullptr, mixed_multiplication, new thread_data{imatrix, ind}) == 0);
            assert(pthread_setaffinity_np(thread_id, sizeof(cpu_set_t), &cpu_sets[i]) == 0);
            mixed.push_back(thread_id);
            j++;
        }
    }

    for (int i = BT; i < k; i++) {  /*for Non-BT threads*/
        int ind = i;
        pthread_t thread_id;
        assert(pthread_create(&thread_id, nullptr, mixed_multiplication, new thread_data{imatrix, ind}) == 0);
        mixed.push_back(thread_id);
    }

    for (int i = 0; i < k; i++) {   /*All threads will join and terminated*/
        pthread_join(mixed[i], nullptr);
    }

    auto en_time = std::chrono::high_resolution_clock::now();/*Ending time for overall threads perfomance*/
    auto time_taken = std::chrono::duration_cast<std::chrono::milliseconds>(en_time - st_time).count();

    int Time_taken_by_all=0, Time_by_BT=0, Time_by_normal=0;/*Declaring variables for exp2 times*/

    /*Average time execution without threads being bound to cores*/
    if(BT==0){
        for(int i=0;i<k;i++){
            Time_taken_by_all = Time_taken_by_all+ times[i]; 
        }
        Time_taken_by_all = Time_taken_by_all/k;
    }
    /*Average time execution of Core-bounded threads*/		
    if(BT != 0){
        for(int i=0;i<BT;i++){
            Time_by_BT= Time_by_BT+ times[i];
        }
        Time_by_BT = Time_by_BT/BT;	
    }
    /*Average time execution of Normal threads*/
    if(k != BT){
        for(int i=BT;i<k;i++){
            Time_by_normal = Time_by_normal + times[i];
        }
        Time_by_normal = Time_by_normal /(k-BT);	
    }

    /*Writing results to output_file*/
    ofstream o_file("out.txt");
    o_file << "The mixed matrix multiplication" << endl;
    o_file << endl;
    o_file << "Experiment 1:" << endl;
    o_file << endl;
    o_file << "Time taken: " << time_taken << " milliseconds" << endl;
    o_file << endl;
    o_file << endl;
    o_file << "Experiment 2:" << endl;
    o_file << "1." << " " << Time_taken_by_all << endl;
    o_file << endl;
    o_file << "2." << " " << Time_by_BT << endl;
    o_file << endl;
    o_file << "3." << " " << Time_by_normal << endl;
    o_file << endl;

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            o_file << omatrix[i][j] << " ";
        }
        o_file << endl;
    }
    o_file << endl;



    o_file.close();

    return 0;
}

