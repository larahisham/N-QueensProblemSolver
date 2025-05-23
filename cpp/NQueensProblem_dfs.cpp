#include <iostream>
#include <vector>
#include <chrono> // for providing measurement of time utilities (for performance analysis)
#include <fstream>
using namespace std;
using namespace std::chrono;
// bool is_safe(const vector<int>& board, int row, int col) {
//     // const vector <int>& board : is a referance to the current board state
//     // each board index represents a row, and the value at that index represents the col
//     for (int i = 0; i < row; ++i) {
//         if (board[i] == col || // same col conflict
//             board[i] - i == col - row || // same diagonal conflict (top-left to bottom-right)
//             board[i] + i == col + row) { // same diagonal conflict (top-right to bottom-left)
//             // if there's any - at least one conflict - return false 
//             return false;
//         }
//     } return true;
// }

bool is_safe(const vector<int>& assignment, int row, int col) {
    for (int r = 0; r < row; ++r) {
        int c = assignment[r];
        if (c == col || abs(c - col) == abs(r - row)) // checking for conflicts 
            // same col  || diagonally 
            return false;
    } return true;
}

// backtracking function to find all possible solutions 
void solve_all(vector<int>& board, int row, int n, int& count) {
    // if all rows has been filled by queens with no conflicts -- indicates a valid solution
    if (row == n) {
        count++;
        return;
    }
    // iterate through each col in the current row, 
    // and check if placing a queen there is safe
    // if it is, place the queen and move to the next row
    // if not, try the next col
    for (int col = 0; col < n; ++col) {
        if (is_safe(board, row, col)) {
            board[row] = col;
            solve_all(board, row + 1, n, count);
        }
    }
}
double dfs_blind(int n, int& solution_count) {
    vector<int> board(n, -1);
    solution_count = 0;
    auto start = high_resolution_clock::now(); // record the start time
    solve_all(board, 0, n, solution_count);
    auto end = high_resolution_clock::now(); // record the end time
    duration<double> duration = end - start; // calculate the elapsed time
    return duration.count();
}
int main() {
   vector<int> TstValues = { 4, 8, 16, 32, 64, 128, 256, 512, 1024 };
    ofstream csv("nqueens_dfs_results.csv");
    csv << "N,Time(seconds),Solutions\n";
    cout << "DFS - blindly searching all solutions for N-Queens...\n";
    // iterate through each test value
    // and run the dfs_blind function
    // to find all possible solutions
    // and measure the time taken
    // to find the total number of solutions as well as the first solution
    // and save the results to a csv file
    for (int n : TstValues) {
        // range-based for loop, since we are not using the index 
        // or do not need to modify the content
        cout << "Running for N = " << n << "...\n";
        int solution_count = 0;
        double time_taken = dfs_blind(n, solution_count);
        cout << "Time taken: " << time_taken << " seconds, Solutions: " << solution_count << "\n";
        csv << n << "," << time_taken << "," << solution_count << "\n";
    }
    csv.close();
    cout << "Results saved to nqueens_dfs_results.csv\n";
    return 0;
}
