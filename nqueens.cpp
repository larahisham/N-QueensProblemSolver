#include <iostream>
#include <vector>
#include <chrono>
#include <fstream>
using namespace std;
using namespace std::chrono;
bool is_safe(const vector<int>& board, int row, int col) {
    for (int i = 0; i < row; ++i) {
        if (board[i] == col || 
            board[i] - i == col - row || 
            board[i] + i == col + row) {
            return false;
        }
    }
    return true;
}
bool solve(vector<int>& board, int row, int n) {
    if (row == n)
        return true;
    for (int col = 0; col < n; ++col) {
        if (is_safe(board, row, col)) {
            board[row] = col;
            if (solve(board, row + 1, n))
                return true;
        }
    }
    return false;
}
double dfs_blind(int n) {
    vector<int> board(n, -1);
    auto start = high_resolution_clock::now();
    solve(board, 0, n);
    auto end = high_resolution_clock::now();
    duration<double> duration = end - start;
    return duration.count();
}
int main() {
    vector<int> test_values = {4, 8, 15, 20, 25, 50, 100, 500, 1000, 1500, 5000, 5500, 10000}; // Try larger N if your machine allows
    ofstream csv("nqueens_dfs_results.csv");
    csv << "N,Time(seconds)\n";
    cout << "DFS - blindly searching...\n";
    for (int n : test_values) {
        cout << "Running for N = " << n << "...\n";
        double time_taken = dfs_blind(n);
        cout << "Time taken: " << time_taken << " seconds\n";
        csv << n << "," << time_taken << "\n";
    }
    csv.close();
    cout << "Results saved to nqueens_dfs_results.csv\n";
    return 0;
}