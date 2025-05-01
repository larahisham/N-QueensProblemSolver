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
    vector<int> assignment;             // each index represents a row, value represents the col       
    vector<unordered_set<int>> domains; // each row has a set of possible columns
};

bool is_safe(const vector<int>& assignment, int row, int col) {
    for (int r = 0; r < row; ++r) {
        int c = assignment[r];
        if (c == col || abs(c - col) == abs(r - row)) // checking for conflicts 
            // same col  || diagonally 
            return false;
    } return true;
}


bool forward_check(CSPState& state, int row, int col) {
    // for each row r after current one retrive domain -- which is a set of valid col. 
    for (int r = row + 1; r < state.n; ++r) {
        auto& domain = state.domains[r];
        vector<int> to_remove;
		// identify the conflicting col's 
        for (int c : domain) {
            if (c == col || abs(c - col) == abs(r - row)) {
                to_remove.push_back(c);
            }
        }
		// remove the conflicting col's from the domain
        for (int c : to_remove) {
            domain.erase(c);
		} // check if any domain is empty
		// if any domain is empty, the assignment is not valid -- false
        if (domain.empty())
            return false;
    } return true;
}

// LCV: least constraining value 
// will prioritize the col assignments that leave most options for others
vector<int> sorted_lcv(const CSPState& state, int row) {
	vector<pair<int, int>> col_constraints; // to store the number of constraints for each col
    for (int col : state.domains[row]) {
        int count = 0;
		// count the number of constraints for this col
        for (int r = row + 1; r < state.n; ++r) {
            for (int c : state.domains[r]) {
                if (c == col || abs(c - col) == abs(r - row))
                    ++count;
            }
        }
        col_constraints.push_back({ count, col });
    }
	// sort the col_constraints based on the number of constraints (ascending order)
    // lest constraining value first 
    sort(col_constraints.begin(), col_constraints.end());
    vector<int> sorted_cols;
    for (auto& pair : col_constraints)
        sorted_cols.push_back(pair.second);
    return sorted_cols;
}

int select_variable(const CSPState& state) {
    // selection is based on MRV and degree huristic as a tie breaker. 
    // MRV: minimum remaining values --> the row with the smallest domain size --> fewest valid col options
    int min_domain_size = state.n + 1; // initialize to a value larger than any possible domain size possible. 
    int best_row = -1; // to store the best row index "currently unassigned" --> used for tie breaking
    int max_constraints = -1; // initialize to a value smaller than any possible constraints (0 or more), help with tie breaking
    // as the, the degree huristic is used as a tie breaker when multiple rows have the same domain size
    // the row with the most constraints (most unassigned rows) will be selected
    for (int row = 0; row < state.n; ++row) {
        if (state.assignment[row] != -1)
            continue;
        // iterate through all rows, and check if the row is unassigned, skip if assigned       
        int domain_size = state.domains[row].size(); // get the domain size (represents the number of valid col options)

        // ensuring that the row with the fewest options is prioritized
        if (domain_size < min_domain_size) {
            min_domain_size = domain_size;
            best_row = row;
        }
        // the tie breaker
        else if (domain_size == min_domain_size) {
            int constraints = 0;
            // count the number of unassigned rows that are constrained by this row
            for (int other = 0; other < state.n; ++other) {
                if (other != row && state.assignment[other] == -1) { // ensure that the already assigned rows are not counted.
                    // as well as the row itself
                    constraints += state.domains[other].size();
                }
            }
            // if the number of constraints is greater than the current max, update the best row to be chosen
            if (constraints > max_constraints) {
                max_constraints = constraints;
                best_row = row;
            }
        }
        // best row to be assigned (selected)
    } return best_row;
}

bool solve(CSPState& state) {
    bool done = true;
    // check if all rows have been assigned A col.
    // if so, return true
    for (int i = 0; i < state.n; ++i)
        if (state.assignment[i] == -1)
            done = false;
    if (done) return true;
    int row = select_variable(state);
    // get the a list of col indicates for each row, ordered by LCV huristic
    // LCV huristic prioritizes the col assignments that leav most options for others 
    vector<int> values = sorted_lcv(state, row);
    // iterate over all possible assignments
    for (int col : values) {
        CSPState new_state = state; // to ensure that changes are made during the iteration
        new_state.assignment[row] = col;
        // to check if this assignment will not make the problem unsolvable soon
        if (!forward_check(new_state, row, col))
            continue;
        // the solve function will be called recursivly
        // if true, sol found. 
        // if false, backtrack. 
        if (solve(new_state)) {
            state = new_state;
            return true;
        }
    } return false;
}

double dfs_csp(int n) {
    CSPState state; // represents the current state of the CSP
    state.n = n;
    state.assignment = vector<int>(n, -1); // -1 indicates unassigned 
    state.domains = vector<unordered_set<int>>(n); // n rows, each with n possible columns (empty sets at the first)
    // initialize the domains with all possible columns 
    // (for each row i, with all col's j)
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            state.domains[i].insert(j);
    // calculate the time taken to solve the problem (from start to end)       
    auto start = high_resolution_clock::now();
    solve(state);
    auto end = high_resolution_clock::now();
    duration<double> elapsed = end - start;
    return elapsed.count();
}

int main() {
    vector<int> test_values = { 4, 5, 6, 7, 8, 9, 10,
 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
 21, 22, 23, 24, 25 };
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






/*
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
    vector<int> test_values = { 4, 5, 6, 7, 8, 9, 10,
                               11, 12, 13, 14, 15 }; // , 16, 17, 18, 19, 20,
                               //21, 22, 23, 24, 25 };
    ofstream csv("nqueens_dfs_results.csv");
    csv << "N,Time(seconds),Solutions\n";
    cout << "DFS - blindly searching all solutions for N-Queens...\n";
    // iterate through each test value
    // and run the dfs_blind function
    // to find all possible solutions
    // and measure the time taken
    // to find the total number of solutions as well as the first solution
    // and save the results to a csv file
    for (int n : test_values) {
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
*/
