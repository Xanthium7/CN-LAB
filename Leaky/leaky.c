#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<time.h>

#define NOF_PACKETS 5

int main() {
    int packet_sz[NOF_PACKETS];
    int i, b_size, o_rate, p_sz_rm = 0;

    srand(time(NULL));

    // Generate random packet sizes
    for(i = 0; i < NOF_PACKETS; i++)
        packet_sz[i] = (rand() % 6 + 1) * 10;

    printf("Enter Bucket size: ");
    scanf("%d", &b_size);
    printf("Enter Output rate: ");
    scanf("%d", &o_rate);

    printf("\n--- Simulation Started ---\n");

    for(i = 0; i < NOF_PACKETS; i++) {
        printf("\nIncoming Packet Size: %d", packet_sz[i]);

        // Check for Overflow
        if (packet_sz[i] + p_sz_rm > b_size) {
            printf("\nBucket Overflow! Packet Rejected.");
        } else {
            p_sz_rm += packet_sz[i];
            printf("\nBucket Status: %d/%d occupied", p_sz_rm, b_size);
        }

        // The "Leaky" process: Transmission
        // We simulate one 'clock cycle' of leaking per packet arrival
        if (p_sz_rm > 0) {
            printf("\nTransmitting bytes...");
            sleep(1); // Simulate time passing

            if (p_sz_rm <= o_rate) {
                printf("\nTransmitted: %d", p_sz_rm);
                p_sz_rm = 0;
            } else {
                printf("\nTransmitted: %d", o_rate);
                p_sz_rm -= o_rate;
            }
            printf("\nRemaining in Bucket: %d", p_sz_rm);
        }
        printf("\n-----------------------");
    }

    // Empty the bucket after all packets have arrived
    while (p_sz_rm > 0) {
        printf("\nEmptying remaining bucket...");
        sleep(1);
        if (p_sz_rm <= o_rate) {
            printf("\nTransmitted: %d", p_sz_rm);
            p_sz_rm = 0;
        } else {
            printf("\nTransmitted: %d", o_rate);
            p_sz_rm -= o_rate;
        }
        printf("\nRemaining: %d", p_sz_rm);
    }

    printf("\nAll packets processed.\n");
    return 0;
}