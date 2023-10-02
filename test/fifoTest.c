#include <stdio.h>
#include <assert.h>
#include <time.h>
/*****************************TEMPLATE INCLUDE**************************************/
#define FIFO_BUFFER_ENTRY_DATA_TYPE size_t
#define FIFO_BUFFER_INLINE_IMPLEMENTATION
#define FIFO_BUFFER_STATIC_IMPLEMENTATION

//following just for testing
#define FIFO_BUFFER_ONLY_PROTOTYPE_DECLARATION
#include "fifoBuffer_t.h" //Prototype
#undef FIFO_BUFFER_ONLY_PROTOTYPE_DECLARATION
#include "fifoBuffer_t.h" //Implementation
/***********************************************************************************/


static fifoBufferEntryDataType_t s_buffer[1000000];
static fifoBuffer_t s_fifoBuffer = 
{
    .buffer = s_buffer,
    .bufferSize = sizeof(s_buffer)/sizeof(s_buffer[0]),
    .readHeadPos = 0,
    .writeHeadPos = 0
};

int main(void)
{
    clock_t startTime = clock();
    fifo_reset(&s_fifoBuffer);

    for (size_t i = 0; i < fifo_getMaxSize(&s_fifoBuffer); i++)
    {
        fifo_put(&s_fifoBuffer, &i);
        assert(*(fifo_peakLast(&s_fifoBuffer)) == i);
        assert(fifo_getCurrentSize(&s_fifoBuffer) == ( i + 1 ));
        assert((fifo_getFreeSize(&s_fifoBuffer)) == ( fifo_getMaxSize(&s_fifoBuffer) - ( i + 1 )));
    }

    assert(fifo_getFreeSize(&s_fifoBuffer) == 0);
    assert(fifo_getCurrentSize(&s_fifoBuffer) == (fifo_getMaxSize(&s_fifoBuffer)));

    for (size_t i = 0; i < (fifo_getMaxSize(&s_fifoBuffer)/2); i++)
    {
        size_t j;
        fifo_getFirst(&s_fifoBuffer, &j);
        assert(j == i);
    }
    
    assert(fifo_getFreeSize(&s_fifoBuffer) == (fifo_getMaxSize(&s_fifoBuffer)/2));

    for (size_t i = 0; i < ((fifo_getMaxSize(&s_fifoBuffer)/2)); i++)
    {
        size_t temp = i + 100;
        fifo_put(&s_fifoBuffer, &temp);
        assert(*(fifo_peakLast(&s_fifoBuffer)) == temp);
    } 
    
    assert(fifo_getFreeSize(&s_fifoBuffer) == 0);
    assert(fifo_getCurrentSize(&s_fifoBuffer) == (fifo_getMaxSize(&s_fifoBuffer)));

    for (size_t i = (fifo_getMaxSize(&s_fifoBuffer)/2); i < (fifo_getMaxSize(&s_fifoBuffer)); i++)
    {
        size_t j;
        fifo_getFirst(&s_fifoBuffer, &j);
        assert(j == i);
    }
    
    for (size_t i = 0; i < ((fifo_getMaxSize(&s_fifoBuffer)/2)); i++)
    {
        size_t temp = i + 100;
        size_t entry = 0;
        fifo_getFirst(&s_fifoBuffer, &entry);
        assert(entry == temp);
    } 

    assert(fifo_getFreeSize(&s_fifoBuffer) == (fifo_getMaxSize(&s_fifoBuffer)));
    assert(fifo_getCurrentSize(&s_fifoBuffer) == 0);

    double elapsedTime = (double)(clock() - startTime) / CLOCKS_PER_SEC;
    printf("Done in %f seconds\n", elapsedTime);
}