#include <iostream>
#include <vector>
#include <chrono>
#include <fstream>
#include <algorithm>
#include <unordered_set>

using namespace std;
using namespace chrono;

struct CSPState {
    int n;
    vector<int> assignment;                 // board[row] = col
    vector<unordered_set<int>> domains;     // domains[row] = set of valid columns
};

// Check if placing (row, col) is safe based on current assignment
bool is_safe(const vector<int>& assignment, int row, int col) {
    for (int r = 0; r < row; ++r) {
        int c = assignment[r];
        if (c == col || abs(c - col) == abs(r - row))
            return false;
    }
    return true;
}

// Forward check: remove conflicting values from domains
bool forward_check(CSPState& state, int row, int col) {
    for (int r = row + 1; r < state.n; ++r) {
        auto& domain = state.domains[r];
        vector<int> to_remove;

        for (int c : domain) {
            if (c == col || abs(c - col) == abs(r - row)) {
                to_remove.push_back(c);
            }
        }

        for (int c : to_remove) {
            domain.erase(c);
        }

        if (domain.empty())
            return false;
    }
    return true;
}

// Least Constraining Value heuristic
vector<int> sorted_lcv(const CSPState& state, int row) {
    vector<pair<int, int>> col_constraints;

    for (int col : state.domains[row]) {
        int count = 0;
        for (int r = row + 1; r < state.n; ++r) {
            for (int c : state.domains[r]) {
                if (c == col || abs(c - col) == abs(r - row))
                    ++count;
            }
        }
        col_constraints.push_back({count, col});
    }

    sort(col_constraints.begin(), col_constraints.end());
    vector<int> sorted_cols;
    for (auto& pair : col_constraints)
        sorted_cols.push_back(pair.second);

    return sorted_cols;
}

// Select variable using MRV and Degree Heuristic
int select_variable(const CSPState& state) {
    int min_domain_size = state.n + 1;
    int best_row = -1;
    int max_constraints = -1;

    for (int row = 0; row < state.n; ++row) {
        if (state.assignment[row] != -1)
            continue;

        int domain_size = state.domains[row].size();
        if (domain_size < min_domain_size) {
            min_domain_size = domain_size;
            best_row = row;
        } else if (domain_size == min_domain_size) {
            // Tie-break with most constraining (highest degree)
            int constraints = 0;
            for (int other = 0; other < state.n; ++other) {
                if (other != row && state.assignment[other] == -1) {
                    constraints += state.domains[other].size();
                }
            }
            if (constraints > max_constraints) {
                max_constraints = constraints;
                best_row = row;
            }
        }
    }

    return best_row;
}

bool solve(CSPState& state) {
    // Check if complete
    bool done = true;
    for (int i = 0; i < state.n; ++i)
        if (state.assignment[i] == -1)
            done = false;
    if (done) return true;

    int row = select_variable(state);
    vector<int> values = sorted_lcv(state, row);

    for (int col : values) {
        CSPState new_state = state;
        new_state.assignment[row] = col;

        if (!forward_check(new_state, row, col))
            continue;

        if (solve(new_state)) {
            state = new_state;
            return true;
        }
    }

    return false;
}

double dfs_csp(int n) {
    CSPState state;
    state.n = n;
    state.assignment = vector<int>(n, -1);
    state.domains = vector<unordered_set<int>>(n);

    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            state.domains[i].insert(j);

    auto start = high_resolution_clock::now();
    solve(state);
    auto end = high_resolution_clock::now();

    duration<double> elapsed = end - start;
    return elapsed.count();
}

int main() {
    vector<int> test_values = {4, 8, 15, 20, 25, 50, 100, 500, 1000, 1500, 5000, 5500, 10000}; // Increase if needed
    ofstream csv("nqueens_csp_results.csv");
    csv << "N,Time(seconds)\n";

    cout << "DFS - CSP searching...\n";
    for (int n : test_values) {
        cout << "Running for N = " << n << "...\n";
        double time_taken = dfs_csp(n);
        cout << "Time taken: " << time_taken << " seconds\n";
        csv << n << "," << time_taken << "\n";
    }

    csv.close();
    cout << "Results saved to nqueens_csp_results.csv\n";
    return 0;
}
