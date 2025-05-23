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
    vector<int> assignment;                 
    vector<unordered_set<int>> domains; 
};

bool is_safe(const vector<int>& assignment, int row, int col) {
    for (int r = 0; r < row; ++r) {
        int c = assignment[r];
        if (c == col || abs(c - col) == abs(r - row)) // checking for conflicts 
            // same col  || diagonally 
            return false;
    } return true;
}

// level 1 arc-consistency

//bool forward_check(CSPState& state, int row, int col) {
//    for (int r = row + 1; r < state.n; ++r) {
//        auto& domain = state.domains[r];
//        vector<int> to_remove;
//        for (int c : domain) {
//            if (c == col || abs(c - col) == abs(r - row)) {
//                to_remove.push_back(c);
//            }
//        }
//        for (int c : to_remove) {
//            domain.erase(c);
//		} 
//        if (domain.empty())
//            return false;
//    } return true;
//}


// level 2 arc-consistency

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
