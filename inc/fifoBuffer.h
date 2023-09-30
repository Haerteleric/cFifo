/**
 * Author:    Haerteleric
 * Created:   11.07.23
 * 
 * (c) Copyright by Eric Haertel
 * 
 * Portable Fifo Buffer template implementation
 * 
 **/
#define _FIFO_BUFFER_TEMPLATE_HEADER_
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef FIFO_BUFFER_CONFIG_HEADER_INCLUDE
    #include FIFO_BUFFER_CONFIG_HEADER_INCLUDE
#endif

#ifndef FIFO_BUFFER_ENTRY_DATA_TYPE
    #define FIFO_BUFFER_ENTRY_DATA_TYPE unsigned int
#endif

#ifndef FIFO_BUFFER_ARITHMETIC_DATA_TYPE
    #define FIFO_BUFFER_ARITHMETIC_DATA_TYPE unsigned int
#endif

typedef FIFO_BUFFER_ENTRY_DATA_TYPE fifoBufferEntryDataType_t;
typedef FIFO_BUFFER_ARITHMETIC_DATA_TYPE fifoBufferArithmeticDataType_t;

typedef struct fifoBuffer_s
{
    fifoBufferEntryDataType_t  *buffer;
    const fifoBufferArithmeticDataType_t bufferSize;
    fifoBufferArithmeticDataType_t readHeadPos;
    fifoBufferArithmeticDataType_t writeHeadPos;
}fifoBuffer_t;

inline static fifoBufferArithmeticDataType_t fifo_getCurrentSize(fifoBuffer_t * handle)
{
    if(handle->readHeadPos <= handle->writeHeadPos) 
    {
        return handle->writeHeadPos - handle->readHeadPos;
    }
    return handle->writeHeadPos + handle->bufferSize - handle->readHeadPos;
}

inline static fifoBufferArithmeticDataType_t fifo_getFreeSize(fifoBuffer_t * handle)
{
    return handle->bufferSize - 1 - fifo_getCurrentSize(handle);
}

inline static fifoBufferArithmeticDataType_t fifo_getMaxSize(fifoBuffer_t * handle)
{
    return handle->bufferSize - 1;
}

inline static void fifo_reset(fifoBuffer_t * handle)
{
    handle->readHeadPos = 0;
    handle->writeHeadPos = 0;
}

#ifdef FIFO_BUFFER_CLEAR_MEMORY_ON_ALLOC
#include <string.h>
#endif

inline static fifoBufferEntryDataType_t * fifo_alloc(fifoBuffer_t * handle)
{
    fifoBufferArithmeticDataType_t nextWriteHeadPos = ( handle->writeHeadPos + 1 ) % handle->bufferSize;
    if( nextWriteHeadPos == handle->readHeadPos)
    {
        return NULL;
    }
    fifoBufferEntryDataType_t * allocedEntry = &handle->buffer[handle->writeHeadPos];

#ifdef FIFO_BUFFER_CLEAR_MEMORY_ON_ALLOC
    memset(allocedEntry, 0x00, sizeof(fifoBufferEntryDataType_t));
#endif

    handle->writeHeadPos = nextWriteHeadPos;
    return allocedEntry;
}

inline static bool fifo_put(fifoBuffer_t * handle, fifoBufferEntryDataType_t * entry)
{
    fifoBufferArithmeticDataType_t nextWriteHeadPos = ( handle->writeHeadPos + 1 ) % handle->bufferSize;
    if( nextWriteHeadPos == handle->readHeadPos)
    {
        return false;
    }
    handle->buffer[handle->writeHeadPos] = *entry; 
    handle->writeHeadPos = nextWriteHeadPos;
    return true;
}


inline static fifoBufferArithmeticDataType_t fifo_write(fifoBuffer_t * handle, fifoBufferEntryDataType_t * data, fifoBufferArithmeticDataType_t numEntries)
{
    fifoBufferArithmeticDataType_t nWritten = 0;
    while (nWritten < numEntries)
    {
        if(!fifo_put(handle, &data[nWritten]))
        {
            break;
        }
        nWritten++;
    }
    return nWritten;
}

inline static fifoBufferEntryDataType_t * fifo_peakFirst(fifoBuffer_t * handle)
{
    if( handle->writeHeadPos == handle->readHeadPos)
    {
        return NULL;
    }
    return &handle->buffer[handle->readHeadPos];
}

inline static fifoBufferEntryDataType_t * fifo_peak(fifoBuffer_t * handle)
{
    return fifo_peakFirst(handle);
}

inline static fifoBufferEntryDataType_t * fifo_peakLast(fifoBuffer_t * handle)
{
    if( handle->writeHeadPos == handle->readHeadPos)
    {
        return NULL;
    }

    if(handle->writeHeadPos == 0)
    {
        return &handle->buffer[(handle->bufferSize - 1)];
    }
    
    return &handle->buffer[(handle->writeHeadPos - 1) % handle->bufferSize];
}

inline static void fifo_discardFirst(fifoBuffer_t * handle)
{
    if( handle->writeHeadPos != handle->readHeadPos)
    {
        handle->readHeadPos = ( handle->readHeadPos + 1 ) % handle->bufferSize;
    }
}

inline static void fifo_discard(fifoBuffer_t * handle)
{
    fifo_discardFirst(handle);
}

inline static void fifo_discardLast(fifoBuffer_t * handle)
{
    if( handle->writeHeadPos != handle->readHeadPos)
    {
        if(handle->writeHeadPos == 0)
        {
            return &handle->buffer[(handle->bufferSize - 1)];
        }
        handle->writeHeadPos = ( handle->writeHeadPos - 1 ) % handle->bufferSize;
    }
}

inline static bool fifo_getFirst(fifoBuffer_t * handle, fifoBufferEntryDataType_t * buffer)
{
    //find pending
    fifoBufferEntryDataType_t * temp = fifo_peakFirst(handle);

    if( temp == NULL )
    {
        return false;
    }
    //Copy
    *buffer = *temp;
    
    //delete pending
    fifo_discardFirst(handle);
    return true;
}

inline static bool fifo_get(fifoBuffer_t * handle, fifoBufferEntryDataType_t * buffer)
{
    return fifo_getFirst(handle, buffer);
}

inline static bool fifo_getLast(fifoBuffer_t * handle, fifoBufferEntryDataType_t * buffer)
{
    //find latest
    fifoBufferEntryDataType_t * temp = fifo_peakLast(handle);

    if( temp == NULL )
    {
        return false;
    }
    //Copy
    *buffer = *temp;
    
    //delete latest
    fifo_discardLast(handle);
    return true;
}


inline static fifoBufferArithmeticDataType_t fifo_read(fifoBuffer_t * handle, fifoBufferEntryDataType_t * buffer, fifoBufferArithmeticDataType_t bufferSize)
{
    fifoBufferArithmeticDataType_t nRead = 0;
    while (nRead < bufferSize)
    {
        if(!fifo_get(handle,&buffer[nRead]))
            break;
    }
    return nRead;
}
