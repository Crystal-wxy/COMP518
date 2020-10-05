#ifndef _WINDOW_BIT_COUNT_APX_
#define _WINDOW_BIT_COUNT_APX_

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#include <string.h>

uint64_t N_MERGES = 0; // keep track of how many bucket merges occur

/*
    TODO: You can add code here.
*/

typedef struct {
    uint32_t timestamp;
    uint32_t size;
} Bucket;

typedef struct {
    uint32_t total_bucket;
    uint32_t wnd_size;
    Bucket* wnd_buffer;
    uint32_t count; // not sure if it is needed
    uint32_t index_oldest; // index pointing to the oldest element    
    uint32_t index_next;
    uint32_t now;
    uint32_t k;
} StateApx;

// k = 1/eps
// if eps = 0.01 (relative error 1%) then k = 100
// if eps = 0.001 (relative error 0.1%) the k = 1000
uint64_t wnd_bit_count_apx_new(StateApx* self, uint32_t wnd_size, uint32_t k) {
    assert(wnd_size >= 1);
    assert(k >= 1);
    // TODO: Fill me.
    // The function should return the total number of bytes allocated on the heap.
    self->wnd_size = wnd_size;
    self->index_oldest = 0;
    self->index_next = 0;
    self->k = k;
    self->now =0;
    self->count = 0;
    self->total_bucket = ceil(k * (log(wnd_size/k)/log(2)));
    uint64_t memory = ((uint64_t)self->total_bucket) * sizeof(Bucket);
    self->wnd_buffer = (Bucket*) malloc(memory);
    for (uint32_t i=0; i<wnd_size; i++) {
        self->wnd_buffer[i].size = 0;
        self->wnd_buffer[i].timestamp = 0;
    }
    printf("self->total_bucket = %d\n", self->total_bucket);
    return memory;
}

void wnd_bit_count_apx_destruct(StateApx* self) {
    // TODO: Fill me.
    // Make sure you free the memory allocated on the heap.
    free(self->wnd_buffer);
}

void wnd_bit_count_apx_print(StateApx* self) {
    // This is useful for debugging.
}

uint32_t wnd_bit_count_apx_next(StateApx* self, bool item) {
    // add timestamp
    self->now += 1;

    // check if the oldest bucket is expired
    if (self->now - self->wnd_buffer[self->index_oldest].timestamp >= self->wnd_size){
        // self->index_next = self->index_oldest;
        if (self->index_oldest == self->total_bucket){
            self->index_oldest = 0;
        }else{
            self->index_oldest += 1;
        }
    }

    // Create new bucket and add to window
    if(item){
        self->count ++;
        self->wnd_buffer[self->index_next].timestamp = self->now;
        self->wnd_buffer[self->index_next].size = 1;
        self->index_next = (self->index_next + 1) % self->total_bucket; 
        printf("self->index_next = %u\n", self->index_next);
        // self->index_oldest = (self->index_oldest + 1) % self->total_bucket;
        
        int group_count = 1;
        int group_size = 0;
        // Merge
        size_t pointer_bucket = self->index_next - 1;
        // size_t pointer_bucket = (self->index_next - 1 + self->total_bucket)%self->total_bucket;
        for (size_t i=self->total_bucket; i>=0; i--) {
            pointer_bucket = (pointer_bucket + i) % self->total_bucket;
            printf("pointer_bucket = %u\n", pointer_bucket);
            if (self->wnd_buffer[pointer_bucket].size == group_count) {
                group_size += 1;
                printf("group_size = %d\n", group_size);
                if (group_size >= self->k+2) {
                    N_MERGES ++;
                    self->wnd_buffer[pointer_bucket].size *= 2;
                    self->wnd_buffer[pointer_bucket].timestamp = self->wnd_buffer[(pointer_bucket+1)%self->total_bucket].timestamp;
                    self->wnd_buffer[(pointer_bucket+1)%self->total_bucket].size = 0;
                    self->wnd_buffer[(pointer_bucket+1)%self->total_bucket].timestamp = 0;
                    
                    if (pointer_bucket <= self->index_next - 1){
                        memmove(self->wnd_buffer+((pointer_bucket+1)%self->total_bucket)*sizeof(Bucket), self->wnd_buffer+((pointer_bucket+2)%self->total_bucket)*sizeof(Bucket),(self->index_next-1-((pointer_bucket+1)%self->total_bucket))*sizeof(Bucket));
                    } else {
                        for (size_t j = pointer_bucket+1; j <= self->index_next - 2 +self->total_bucket; j++){
                            memmove(self->wnd_buffer+(j%self->total_bucket)*sizeof(Bucket),
                            self->wnd_buffer+((j+1)%self->total_bucket)*sizeof(Bucket), 1*sizeof(Bucket));
                        }
                    }
                    // memmove(self->wnd_buffer+(i+1)*sizeof(bucket), self->wnd_buffer+(i+2)*sizeof(bucket), (self->index_next-1-(i+1))*sizeof(bucket));
                    // self->index_next --;
                    self->index_next = (self->index_next + self->total_bucket-1)%self->total_bucket;
                    group_count *= 2;
                    group_size = 1;
                }    
            } else {
                // move all full buckets to 0
                // for (size_t j = 0; j < self->total_bucket; j ++){
                //     if(self->wnd_buffer[j].size == 0 && 
                //     self->wnd_buffer[(j+1)%self->total_bucket].size != 0){
                //         memmove(self->wnd_buffer+j*sizeof(Bucket), self->wnd_buffer+(j+1)*sizeof(Bucket),((self->index_next-(j+1))*sizeof(Bucket)));
                //         if (j < self->index_next){
                //             self->index_next -= 1;
                //         }
                //         if (j < self->index_oldest){
                //             self->index_oldest -= 1;
                //         }
                //     }
                // }
                break;
            }
        }
    }
    int count_total = 0;
    int bucket_max = 0;
    for(size_t i=0; i <= self->total_bucket; i++){
        if (self->wnd_buffer[i].size != 0) {
            count_total += self->wnd_buffer[i].size;
            if(self->wnd_buffer[i].size > bucket_max){
                printf("self->wnd_buffer[i].size = %d\n", self->wnd_buffer[i].size);
                bucket_max = self->wnd_buffer[i].size;
            }
        }
    }
    printf("self->count = %d\n", self->count);
    printf("bucket_max = %d\n", bucket_max);
    count_total = count_total - bucket_max + 1;
    return count_total;
} 

#endif // _WINDOW_BIT_COUNT_APX_



    // // add timestamp
    // self->now += 1;

    // // check if the oldest bucket is expired
    // if (self->wnd_buffer[index_oldest].timestamp + self->wnd_size <= self->now){
    //     self->index_next = self->index_oldest;
    //     if (self->index_oldest == self->total_bucket){
    //         self->index_oldest = 0;
    //     }else{
    //         self->index_oldest += 1;
    //     }
    // }

    // // if the new item is "1"
    // if (item){
    //     // add new bucket
    //     self->wnd_buffer[self->index_next].size = 1;
    //     self->wnd_buffer[self->index_next].timestamp = now;  
        
    //     // traverse the list of buckets
    //     uint32_t pointer_bucket = self->index_next;
    //     // pointer to the empty bucket
    //     uint32_t new_pointer_bucket = self->index_next;
    //     // traverse
    //     for (uint32_t i = self->total_bucket; i > 0; i = i - 1){
    //         pointer_bucket = pointer_bucket + i mod self->total_bucket;
    //         if(pointer_bucket > self->total_bucket){
    //             pointer_bucket = pointer_bucket - self->total_bucket;
    //         }

    //         if(self->wnd_buffer[pointer_bucket].size == 
    //         self->wnd_buffer[pointer_bucket+self->k+2].size & 
    //         self->wnd_buffer[pointer_bucket].size != 0){
    //             self->wnd_buffer[pointer_bucket+self->k+1].size = 
    //             self->wnd_buffer[pointer_bucket+self->k+1].size * 2;
    //             new_pointer_bucker = pointer_bucket +self->k+1
    //         }

    //     }

    //     while(pointer_bucket != self->index_oldest){
    //         tmp_size = self->wnd_buffer[pointer_bucket].size;
    //         if(self->wnd_buffer[pointer_bucket].size == 
    //         self->wnd_buffer[pointer_bucket+self->k+2].size & 
    //         self->wnd_buffer[pointer_bucket].size != 0){

    //         }
    //     }

        
        
    // }
