#include <stdio.h>
#include <cassert>
#include <cstdlib>
#include <cstring>
#define heap_CAP 640000
#define chunk_list_CAP 1024

#define UNIMPLEMENTED \
    do { \
        fprintf(stderr, "%s:%d: TODO: %s is not implemented yet\n", \
                __FILE__, __LINE__, __func__); \
        abort(); \
    } while (0)

typedef struct {
    void *start;
    size_t size;
} Chunk;

// Separate data structure for chunk operations
typedef struct {
    size_t count;
    Chunk chunks[chunk_list_CAP];
} chunk_List;



int chunk_start_comper(const Chunk *a,const Chunk * b){
   const Chunk * a_Chunk=a;
    const Chunk * b_Chunk=b;

    return  (char *)  a_Chunk->start- (char *) b_Chunk->start;
}
int chunk_list_find(const chunk_List *list, void *ptr) {

    Chunk key={
        .start=ptr
    };
    Chunk * result=(Chunk *) bsearch(&key,
                           list->chunks,
                           list->count,
                           sizeof(list->chunks[0]),
                                     (int (*)(const void *, const void *))chunk_start_comper);;
    if (result !=0){
    assert(list->chunks<=result);
        return (result-list->chunks)/ sizeof (list->chunks[0]);

    } else{
        return -1;
    }
}

int chunk_list_insert(chunk_List *list, void *start, size_t size) {
    assert(list->count < chunk_list_CAP);
    list->chunks[list->count].start = start;
    list->chunks[list->count].size = size;

    for (int i = list->count; i > 0
    && list->chunks[i].start<list->chunks[i - 1].start;
    --i) {
        //swap them for sorting purpose
        Chunk temp = list->chunks[i];
        list->chunks[i] = list->chunks[i - 1];
        list->chunks[i - 1] = temp;
    }

    list->count++;
    return 0;
}

int chunk_list_remove(chunk_List *list, size_t index) {
    assert(index<=list->count);
    for (int i = 0; i < list->count-1; ++i) {
        list->chunks[i]=list->chunks[i+1];
    }
    list->count-=1;
    return 0;
}

void chunk_list_dump(const chunk_List *list) {
    printf("Chunks (%zu):\n", list->count);
    for (size_t i = 0; i < list->count; ++i) {
        printf("    Start: %p, Size: %zu\n",
               list->chunks[i].start,
               list->chunks[i].size);
    }
}

char heap[heap_CAP] = {0};

chunk_List alloced_Chunks = {0};
chunk_List freed_chunks = {
        .count=1,
        .chunks={
                [0]={.start=heap,.size=sizeof (heap)}
        },
};

void *heap_alloc(size_t size) {
    if (size>0){
        for(int i=0;i<freed_chunks.count;i++){
            const Chunk chunk=freed_chunks.chunks[i];
            if(chunk.size>=size){
                chunk_list_remove(&freed_chunks,i);
                const size_t tail_size=chunk.size-size;
                chunk_list_insert(&alloced_Chunks,chunk.start,size);

                if (tail_size>0){
                    chunk_list_insert(&freed_chunks,(int *)chunk.start+size,tail_size);
                }
            }
            return chunk.start;
        }
        return NULL;
    }

}

void heap_free(void *ptr) {

    if (ptr!=NULL){
        const int index = chunk_list_find(&alloced_Chunks,ptr);
        assert(index>=0);
        chunk_list_insert(&freed_chunks,
                          alloced_Chunks.chunks[index].start,
                          alloced_Chunks.chunks[index].size );
        chunk_list_remove(&alloced_Chunks,(size_t)index);
    }
}

void heap_collect() {
    UNIMPLEMENTED;
}

void chunk_list_merge(chunk_List *dst, const chunk_List *src)
{
    dst->count = 0;

    for (size_t i = 0; i < src->count; ++i) {
        const Chunk chunk = src->chunks[i];

        if (dst->count > 0) {
            Chunk *top_chunk = &dst->chunks[dst->count - 1];

            if ((int *)top_chunk->start + top_chunk->size == chunk.start) {
                top_chunk->size += chunk.size;
            } else {
                chunk_list_insert(dst, chunk.start, chunk.size);
            }
        } else {
            chunk_list_insert(dst, chunk.start, chunk.size);
        }
    }
}
int main() {
    // Dump the initial state of allocated and freed chunks
    chunk_list_dump(&alloced_Chunks);
    chunk_list_dump(&freed_chunks);

    // Allocate memory for the string
    char *s = (char *)heap_alloc(strlen("MIDO LOVES MALOKY") + 1);
    if (s != NULL) {
        // Copy the string into the allocated memory
        strcpy(s, "MIDO LOVES MALOKY");
        printf("Allocated string: %s\n", s);

        // Dump the state after allocation
        chunk_list_dump(&alloced_Chunks);
        chunk_list_dump(&freed_chunks);

        // Free the allocated memory
        heap_free(s);

        // Dump the state after freeing
        chunk_list_dump(&alloced_Chunks);
        chunk_list_dump(&freed_chunks);
    } else {
        printf("Allocation failed.\n");
    }

    return 0;
}