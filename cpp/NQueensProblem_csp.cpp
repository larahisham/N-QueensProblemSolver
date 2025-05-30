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

// level 1 

//bool forward_check(CSPState& state, int row, int col) {
//    // for each row r after current one retrive domain -- which is a set of valid col. 
//    for (int r = row + 1; r < state.n; ++r) {
//        auto& domain = state.domains[r];
//        vector<int> to_remove;
//		// identify the conflicting col's 
//        for (int c : domain) {
//            if (c == col || abs(c - col) == abs(r - row)) {
//                to_remove.push_back(c);
//            }
//        }
//		// remove the conflicting col's from the domain
//        for (int c : to_remove) {
//            domain.erase(c);
//		} // check if any domain is empty
//		// if any domain is empty, the assignment is not valid -- false
//        if (domain.empty())
//            return false;
//    } return true;
//}

// level 2 

bool forward_check(CSPState& state, int row, int col) {
    vector<pair<int, int>> removed;
    vector<int> queue;
    for (int r1 = row + 1; r1 < state.n; ++r1) {
        auto& domain1 = state.domains[r1];
        vector<int> to_remove;
        for (int c1 : domain1) {
            if (c1 == col || abs(c1 - col) == abs(r1 - row)) {
                to_remove.push_back(c1);
            }
        }
        for (int c1 : to_remove) {
            domain1.erase(c1);
            removed.emplace_back(r1, c1);
        }
        if (domain1.empty())
            return false;
        if (!to_remove.empty())
            queue.push_back(r1);
    }
    while (!queue.empty()) {
        int r1 = queue.back();
        queue.pop_back();
        for (int r2 = r1 + 1; r2 < state.n; ++r2) {
            auto& domain2 = state.domains[r2];
            vector<int> to_remove;
            for (int c2 : domain2) {
                for (int c1 : state.domains[r1]) {
                    if (c2 == c1 || abs(c2 - c1) == abs(r2 - r1)) {
                        to_remove.push_back(c2);
                        break;
                    }
                }
            }
            for (int c2 : to_remove) {
                domain2.erase(c2);
                removed.emplace_back(r2, c2);
            }
            if (domain2.empty())
                return false;
            if (!to_remove.empty())
                queue.push_back(r2);
        }
    }
    return true;
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
	vector <int> TstValues = { 4, 8, 16, 32, 64, 128, 256, 512, 1024 };
    ofstream csv("nqueens_csp_results.csv");
    csv << "N,Time(seconds)\n";
    cout << "DFS - CSP searching...\n";
    for (int n : TstValues) {
        cout << "Running for N = " << n << "...\n";
        double time_taken = dfs_csp(n);
        cout << "Time taken: " << time_taken << " seconds\n";
        csv << n << "," << time_taken << "\n";
    }
    csv.close();
    cout << "Results saved to nqueens_csp_results.csv\n";
    return 0;
}
