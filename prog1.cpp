// Name: Cathey, Creighton
// Email: ccathe4@lsu.edu
// Project: PA-1 (Multithreading)
// Instructor: Feng Chen
// Class: cs4103-sp23
// Login ID: cs410307

// include statements needed 
#include <iostream>
#include <fstream>
#include <vector>
#include <pthread.h>
using namespace std;

const int N = 9;  // Sudoku size
const int M = 3;  // Subgrid size

int sudoku[N][N];  // Sudoku grid
int valid_rows[N], valid_cols[N], valid_subs[N];  // Validation flags

struct thread_data {
    int id;
    pthread_t thread;
};

vector<thread_data> threads;  // Thread information

// function to check if a row is valid
void* check_row(void* arg) {
    int row = *((int*)arg);
    bool valid = true;
    for (int i = 0; i < N; i++) {
        for (int j = i + 1; j < N; j++) {
            if (sudoku[row][i] == sudoku[row][j]) {
                valid = false;
                break;
            }
        }
        if (!valid) {
            break;
        }
    }
    valid_rows[row] = valid;
    pthread_exit(NULL);
}
////////////////////////////////////////


// function to check if a column is valid
void* check_col(void* arg) {
    int col = *((int*)arg);
    bool valid = true;
    for (int i = 0; i < N; i++) {
        for (int j = i + 1; j < N; j++) {
            if (sudoku[i][col] == sudoku[j][col]) {
                valid = false;
                break;
            }
        }
        if (!valid) {
            break;
        }
    }
    valid_cols[col] = valid;
    pthread_exit(NULL);
}
////////////////////////////////////////


// function to check if a subgrid is valid
void* check_sub(void* arg) {
    int sub = *((int*)arg);
    bool valid = true;
    int row_offset = (sub / M) * M;
    int col_offset = (sub % M) * M;
    for (int i = row_offset; i < row_offset + M; i++) {
        for (int j = col_offset; j < col_offset + M; j++) {
            for (int k = row_offset; k < row_offset + M; k++) {
                for (int l = col_offset; l < col_offset + M; l++) {
                    if (i == k && j == l) {
                        continue;
                    }
                    if (sudoku[i][j] == sudoku[k][l]) {
                        valid = false;
                        break;
                    }
                }
                if (!valid) {
                    break;
                }
            }
            if (!valid) {
                break;
            }
        }
        if (!valid) {
            break;
        }
    }
    valid_subs[sub] = valid;
    pthread_exit(NULL);
}
////////////////////////////////////////


void read_sudoku(const char* filename) {
    ifstream file(filename);
    if (!file) {
        cerr << "Error: File " << filename << " not found." << endl;
        exit(1);
    }
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            file >> sudoku[i][j];
        }
    }
    file.close();
}
////////////////////////////////////////

// function to create p threads
void create_threads() {
    for (int i = 0; i < N; i++) {
        thread_data* data = new thread_data;
        data->id = i;
        pthread_create(&data->thread, NULL, check_row, &data->id);
        threads.push_back(*data);
    }
    for (int i = 0; i < N; i++) {
        thread_data* data = new thread_data;
        data->id = i;
        pthread_create(&data->thread, NULL, check_col, &data->id);
        threads.push_back(*data);
    }
    for (int i = 0; i < N; i++) {
        thread_data* data = new thread_data;
        data->id = i;
        pthread_create(&data->thread, NULL, check_sub, &data->id);
        threads.push_back(*data);
    }
}
////////////////////////////////////////


// function that waits for next thread 
void wait_threads() {
    for (size_t i = 0; i < threads.size(); i++) {
        // pthread_join(threads[i]->thread, NULL);
        // delete threads[i];
    }
    threads.clear();
}
////////////////////////////////////////


// function that writes the results to the output and console 
void write_results(const char* filename) {
    ofstream file(filename);
    if (!file) {
        cerr << "Error: File " << filename << " could not be opened for writing." << endl;
        exit(1);
    }
    int valid_row_count = 0, valid_col_count = 0, valid_sub_count = 0;
    for (int i = 0; i < N; i++) {
        file << "[Thread " << threads[i].id + 1 << "] Row " << i + 1 << ": " << (valid_rows[i] ? "Valid" : "Invalid") << endl;
        if (valid_rows[i]) {
            valid_row_count++;
        }
    }
    for (int i = N; i < 2 * N; i++) {
        file << "[Thread " << threads[i].id + 1 << "] Column " << i - N + 1 << ": " << (valid_cols[i - N] ? "Valid" : "Invalid") << endl;
        if (valid_cols[i - N]) {
            valid_col_count++;
        }
    }
    for (int i = 2 * N; i < 3 * N; i++) {
        int sub_row = (threads[i].id / M) * M + 1;
        int sub_col = (threads[i].id % M) * M + 1;
        file << "[Thread " << threads[i].id + 1 << "] Subgrid R" << sub_row << sub_row + M - 1 << "C" << sub_col << sub_col + M - 1 << ": " << (valid_subs[threads[i].id] ? "Valid" : "Invalid") << endl;
        if (valid_subs[threads[i].id]) {
            valid_sub_count++;
        }
    }
    bool valid = (valid_row_count == N && valid_col_count == N && valid_sub_count == N);
    file << "Valid rows: " << valid_row_count << endl;
    file << "Valid columns: " << valid_col_count << endl;
    file << "Valid subgrids: " << valid_sub_count << endl;
    if (valid) {
        file << "The solution is valid" << endl;
        cout << "\nThe solution is valid." << endl;
    } else {
        file << "The solution is invalid" << endl;
        cout << "\nThe solution is invalid." << endl;
    }
    file.close();
}
////////////////////////////////////////


// main function that calls other functions 
int main(int argc, char* argv[]) {
    if (argc != 3) {
        cerr << "Usage: " << argv[0] << " input_file output_file" << endl;
        exit(1);
    }
    read_sudoku(argv[1]);
    create_threads();
    wait_threads();
    write_results(argv[2]);
    cout << "Summary was written to output file.\n" << endl;
    return 0;
}
////////////////////////////////////////