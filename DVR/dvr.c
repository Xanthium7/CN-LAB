#include <stdio.h>

struct node {
    int dist[20];
    int from[20];
} rt[10];

int main() {
    int costmat[20][20];
    int nodes, i, j, k, count;

    printf("Enter the number of nodes: ");
    scanf("%d", &nodes);

    printf("Enter the cost matrix (use a large number like 999 for no direct connection):\n");
    for (i = 0; i < nodes; i++) {
        for (j = 0; j < nodes; j++) {
            scanf("%d", &costmat[i][j]);
            
            // Distance to self is always 0
            if (i == j) {
                costmat[i][j] = 0;
            }
            
            // Initialize the routing table with direct costs
            rt[i].dist[j] = costmat[i][j];
            rt[i].from[j] = j;
        }
    }

    // The Bellman-Ford logic: Iteratively update distances until no more changes occur
    do {
        count = 0;
        for (i = 0; i < nodes; i++) {
            for (j = 0; j < nodes; j++) {
                for (k = 0; k < nodes; k++) {
                    // If path through intermediate node 'k' is shorter
                    if (rt[i].dist[j] > costmat[i][k] + rt[k].dist[j]) {
                        rt[i].dist[j] = costmat[i][k] + rt[k].dist[j];
                        rt[i].from[j] = k;
                        count++; // Flag that a change happened
                    }
                }
            }
        }
    } while (count != 0); // Loop until the routing tables converge

    // Display the final routing tables
    printf("\n--- Optimized Routing Tables ---\n");
    for (i = 0; i < nodes; i++) {
        printf("\nFor Router %d:\n", i + 1);
        for (j = 0; j < nodes; j++) {
            printf("\tTo node %d via %d | Distance: %d\n", 
                   j + 1, rt[i].from[j] + 1, rt[i].dist[j]);
        }
    }
    printf("\n");

    return 0;
}