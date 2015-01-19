#include "TFC\TFC.h"

static char StringBuffer[256];

void InitByteQueue(ByteQueue *BQ,uint16_t Size,uint8_t * Storage) {
    uint16_t i;

    BQ->QueueSize = Size;
    BQ->ReadPtr=0;
    BQ->WritePtr=0;
    BQ->QueueStorage = Storage;

    for (i=0;i<BQ->QueueSize;i++) {
        BQ->QueueStorage[i] = 0;
    }
}

uint16_t BytesInQueue(ByteQueue *BQ) {
    if (BQ->ReadPtr > BQ->WritePtr) {
        return (BQ->QueueSize - BQ->ReadPtr + BQ->WritePtr);
    } else if (BQ->WritePtr > BQ->ReadPtr) {
        return     (BQ->WritePtr - BQ->ReadPtr);
    } else {
        return 0;
    }
}

int16_t ByteEnqueue(ByteQueue *BQ,uint8_t Val) {
    if (BytesInQueue(BQ) == BQ->QueueSize) {
        return QUEUE_FULL;
    } else {
        BQ->QueueStorage[BQ->WritePtr] = Val;
        BQ->WritePtr++;

        if (BQ->WritePtr >= BQ->QueueSize) {
            BQ->WritePtr = 0;
        }
        return QUEUE_OK;
    }
}

int16_t ByteArrayEnqueue(ByteQueue *BQ,uint8_t *Buf,uint16_t Len) {
    uint16_t i;
    for (i=0;i<Len;i++) {
        ByteEnqueue(BQ,Buf[i]);
    }
    return QUEUE_OK;
}


int16_t Qprintf(ByteQueue *BQ, const char *FormatString,...)
{
 
     va_list argptr; 
     va_start(argptr,FormatString); 
     vsprintf((char *)StringBuffer,FormatString,argptr);
     va_end(argptr);   
           
    return ByteArrayEnqueue(BQ,(uint8_t *)StringBuffer,strlen(StringBuffer));
}


int16_t ByteDequeue(ByteQueue *BQ,uint8_t *Val) {

    if (BytesInQueue(BQ) == 0) {
        return QUEUE_EMPTY;
    } else {
        *Val  = BQ->QueueStorage[BQ->ReadPtr];

        BQ->ReadPtr++;

        if (BQ->ReadPtr >= BQ->QueueSize) {
            BQ->ReadPtr = 0;
        }
        return QUEUE_OK;
    }
}

uint8_t ForcedByteDequeue(ByteQueue *BQ)
{
	uint8_t RetVal;

    if (BytesInQueue(BQ) == 0) {
        return 0;
    } else {
    	RetVal  = BQ->QueueStorage[BQ->ReadPtr];

        BQ->ReadPtr++;

        if (BQ->ReadPtr >= BQ->QueueSize) {
            BQ->ReadPtr = 0;
        }
        return RetVal;
    }
}
