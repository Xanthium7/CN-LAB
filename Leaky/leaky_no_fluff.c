// Online C compiler to run C program online
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<time.h>

int main() {
    
    
    int packet_sz[20];
    
    for(int i =0; i< 10; i++){
        packet_sz[i] = (rand() % 6 + 1) * 10;
    }
    
    srand(time(NULL));
    
    int op_rate, b_size, p_sz_rem = 0;
    printf("Give bucket size and o/p rate: ");
    scanf("%d", &b_size);
    scanf("%d", &op_rate);
    
    for(int i = 0; i<10; i++){
        
        printf("incomeing packet: %d\n", packet_sz[i]);
        
        if(packet_sz[i] + p_sz_rem > b_size){
            printf("buket overflow cant incorporate\n");
        }else{
            p_sz_rem = packet_sz[i] + p_sz_rem;
            printf("Bucket size:  %d/%d\n", p_sz_rem, b_size );
        }
        
        printf("transmittinfg..\n");
        if(p_sz_rem <= op_rate){
            printf("TRansmitting %d\n", p_sz_rem);
            p_sz_rem = 0;
        }else{
            printf("TRansmitting %d\n", op_rate);
            p_sz_rem -= op_rate;
        }
        printf("Bucket status:  %d/%d\n\n", p_sz_rem, b_size );
        
    }
    
    while(p_sz_rem > 0){
        
        printf("MPTYING BUCKET..\n");
        printf("Bucket size:  %d/%d\n", p_sz_rem, b_size );
        if(p_sz_rem <= op_rate){
            printf("TRansmitting %d\n", p_sz_rem);
            p_sz_rem = 0;
        }else{
            printf("TRansmitting %d\n", op_rate);
            p_sz_rem -= op_rate;
        }
    }
    


    return 0;
}