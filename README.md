# Sobel-Edge-Detector
Sobel Edge Detector in Serial C/MPI+OpenMP

| OpenMP                             | Hw-job.sh setting                                     | Total time (sec) |
| ---------------------------------- | ----------------------------------------------------- | ---------------- |
| Serial C                           | 1 node, 1 ranks/node                                  | 8.66856489       |
| static scheduling chunk=N/#threads | 1 node, 4 ranks/node, 4 threads/rank                  | 3.71714625       |
| static scheduling chunk=N/#threads | 2 nodes, 2 ranks/node, 2 ranks/socket, 4 threads/rank | 3.08335465       |
| static scheduling chunk=N/#threads | 2 nodes, 2 ranks/node, 1 ranks/socket,4 threads/rank  | 3.21714144       |
| static scheduling chunk=N/#threads | 4 nodes, 1 ranks/node, 8 threads/rank                 | 1.9979812        |
| static scheduling chunk=N/#threads | 4 nodes, 1 rank/node, 4 threads/rank                  | 2.02665157       |
| static scheduling chunk=N/#threads | 4 nodes, 1 rank/node, 2 threads/rank                  | 1.93180671       |
| dynamic scheduling, chunk=10       | 1 node, 4 ranks/node, 4 threads/rank                  | 4.88082666       |
| dynamic scheduling, chunk=10       | 2 nodes, 2 ranks/node, 2 ranks/socket,4 threads/rank  | 3.00606967       |
| dynamic scheduling, chunk=10       | 2 nodes, 2 ranks/node, 1 ranks/socket, 4 threads/rank | 3.21215067       |
| dynamic scheduling, chunk=10       | 4 nodes, 1 ranks/node, 8 threads/rank                 | 2.06328572       |
| dynamic scheduling, chunk=10       | 4 nodes, 1 rank/node, 4 threads/rank                  | 2.04782639       |
| dynamic scheduling, chunk=10       | 4 nodes, 1 rank/node, 2 threads/rank                  | 2.94740034       |
