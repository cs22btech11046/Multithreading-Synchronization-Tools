#include <iostream>
#include <vector>
#include <thread>
#include <fstream>
#include <chrono>
#include<atomic>
#include<cassert>

using namespace std;

vector<vector<int>> omatrix;	/*declaring out_put_matrix globally */
int n, k, rowInc,C;
atomic <int> lock(0);

struct thread_data {	/*struct to store thread_data*/
	const vector<vector<int>>& imt;
};
/*Compare and swap function
int compare_and_swap(int *value, int expected, int new_value) {
	int temp = *value;
	if (*value == expected){
		*value = new_value;
	}
	return temp;
}*/


/*Assigning rows to threads according to chunk and multiplying matrix*/
void *sq_multiplication(void* arg) {
	thread_data* thread = static_cast<thread_data*>(arg);	/*casting data to thread variable*/
	int start,temp=0;
	const vector<vector<int>>& im = thread->imt;

	while(true){
		while(lock.compare_exchange_strong(temp,1));
		if(C>=n){
			lock=0;
			break;
		}
		else{
			start = C;
			C = C + rowInc;
			lock=0;
		}
	
		/*multiplication*/
		if(start<n){
			if(start+rowInc >= n){
				for (int i = start; i <n; i++) {
					for (int j = 0; j < n; j++) {
						for (int l = 0; l < n; l++) {
							omatrix[i][j] = omatrix[i][j] + im[i][l] * im[l][j];
						}
					}
				}	
			}
			else{
				for (int i = start; i <start+rowInc; i++) {
					for (int j = 0; j < n; j++) {
						for (int l = 0; l < im[0].size(); l++) {
							omatrix[i][j] = omatrix[i][j] + im[i][l] * im[l][j];
						}
					}
				}
			}
		}
	}
	delete thread; // Free allocated memory
	return nullptr;
}

int main() {
	 C = 0;
	// Reading input file and storing the variables and matrix.
	ifstream i_file("inp.txt");
	i_file >> n >> k >> rowInc;

	vector<vector<int>> imatrix(n, vector<int>(n, 0));/*imatrix for storing input matrix*/

	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			i_file >> imatrix[i][j];
		}
	}

	i_file.close();

	omatrix.resize(n, vector<int>(n, 0));	/*fixing size for output_matrix*/
	
	auto st_time = std::chrono::high_resolution_clock::now();/*start time for overall threads perfomance*/

	vector<pthread_t>threads ;/*vector of p_threads*/

	for (int i = 0; i < k; i++) {
			pthread_t thread_id;
			pthread_create(&thread_id, nullptr, sq_multiplication, new thread_data{imatrix});
			threads.push_back(thread_id);
	}

	for (int i = 0; i < k; i++) {	/*All threads will join and terminated*/
		pthread_join(threads[i], nullptr);
	}

	auto en_time = std::chrono::high_resolution_clock::now();/*Ending time for overall threads perfomance*/
	auto time_taken = std::chrono::duration_cast<std::chrono::milliseconds>(en_time - st_time).count();

	printf("%ld \n",time_taken);

	/*Writing results to output_file*/
	ofstream o_file("out.txt");
	o_file << "SQUARING OF MATRIX USING CAS" << endl;
	o_file << endl;
	
	o_file << endl;

	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			o_file << omatrix[i][j] << " ";
		}
		o_file << endl;
	}
	o_file << endl;

	o_file << "Time taken: " << time_taken << " milliseconds" << endl;
	o_file << endl;

	o_file.close();

	return 0;
}
