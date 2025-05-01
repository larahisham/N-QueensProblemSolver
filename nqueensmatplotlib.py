import matplotlib.pyplot as plt
import pandas as pd
csp_df = pd.read_csv(r"C:\Users\user\source\repos\nqueenscsp\nqueens_csp_results.csv")
dfs_df = pd.read_csv(r"C:\Users\user\source\repos\nqueenscsp\nqueens_dfs_results.csv")
plt.plot(dfs_df["N"], dfs_df["Time(seconds)"], marker='o', label="Blind DFS (C++)", color='purple')
plt.plot(csp_df["N"], csp_df["Time(seconds)"], marker='x', label="CSP with MRV+LCV+FC (C++)", color='green')
plt.xlabel("N (Board Size)")
plt.ylabel("Time (seconds)")
plt.title("Performance of Blind DFS vs CSP (N-Queens, C++)")
plt.yscale('log')
plt.legend()
plt.grid(True, which="both", linestyle='--', linewidth=0.5)
plt.tight_layout()
plt.show()
