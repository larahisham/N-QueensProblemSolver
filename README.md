# N-Queens Problem – Multi-Solver Implementation

This project presents a comparative implementation of several algorithms to solve the classic **N-Queens Problem**, where the goal is to place N queens on an N×N chessboard such that no two queens threaten each other. It is developed in both **Python** and **C++** for performance and algorithmic comparison purposes.

##  Implemented Algorithms

### 1. Blind Depth-First Search (DFS)
A simple backtracking approach that tries every possible configuration without using any heuristics.

### 2. Constraint Satisfaction Problem (CSP) Solver
Utilizes CSP strategies to efficiently prune the search space:
- **MRV (Minimum Remaining Values)**: Selects the variable (column) with the fewest legal values left.
- **LCV (Least Constraining Value)**: Chooses values that constrain future variables the least.
- **Forward Checking**: Prevents assignments that would leave a neighboring variable with no legal values.

### 3. Min-Conflicts Heuristic
A local search algorithm that starts with a complete assignment and iteratively repairs conflicts by moving queens to less conflicted positions.

## Metrics Tracked

- **Time to First Solution**: Useful for real-time system performance.
- **Number of Valid Solutions Found**: Validates correctness and completeness.
- **Number of Steps/Assignments**: Insight into algorithmic efficiency.

## Requirements

### C++
- used libraries:
  1. iostream
  2. vector
  3. chrono
  4. fstream
  5. algorithm
  6. unordered_set
- C++11 or higher
- `g++` or any modern C++ compiler

### Python
- used libraries:
  1.matplotlib - pyplot moduel
  2. pandas
- Python 3.7+
