#include<iostream>
#include<vector>
#include<chrono>
#include<fstream>
#include<algorithm>
#include<unordered_set>
using namespace std;
using namespace chrono;


int numOfConflicts(const vector<int>& board, int row, int col) {
    int conflicts = 0;
    for (int i = 0; i < board.size(); ++i) {
        if (i == row) continue;
        if (board[i] == col || abs(board[i] - col) == abs(i - row))
            conflicts++;
    }
    return conflicts;
}
bool hillClimb(vector<int>& board, int max_steps) {
    int n = board.size();
    srand(time(nullptr));
    for (int i = 0; i < n; ++i)
        board[i] = rand() % n;
    for (int step = 0; step < max_steps; ++step) {
        vector<int> conflicted_rows;
        for (int row = 0; row < n; ++row) {
            if (numOfConflicts(board, row, board[row]) > 0)
                conflicted_rows.push_back(row);
        }
        if (conflicted_rows.empty()) return true;
        int row = conflicted_rows[rand() % conflicted_rows.size()];
        int best_col = board[row];
        int min_conflict = numOfConflicts(board, row, best_col);
        for (int col = 0; col < n; ++col) {
            int conflicts = numOfConflicts(board, row, col);
            if (conflicts < min_conflict) {
                min_conflict = conflicts;
                best_col = col;
            }
        }
        if (best_col == board[row]) return false;
        board[row] = best_col;
    }
    return false;
}
double runHillClimbing(int n, int max_steps = 1000000) {
    vector<int> board(n);
    auto start = high_resolution_clock::now();
    hillClimb(board, max_steps);
    auto end = high_resolution_clock::now();
    return duration<double>(end - start).count();
}
int main() {
    srand(time(nullptr));
    ofstream file("nqueens_hillclimbing_results.csv");
    vector<int> TstValues = { 4, 8, 16, 32, 64, 128, 256, 512, 1024 };
    file << "N,Time(seconds)\n";
    cout << "Pure Hill Climbing Results:\n";
    for (int n : TstValues) {
        cout << "Running for N = " << n << "...\n";
        double time_taken = runHillClimbing(n);
        file << n << "," << time_taken << "\n";
        cout << "Time = " << time_taken << " seconds\n";
    }
    file.close();
    return 0;
}
