/**
 * @file
 * @ingroup ixlib
 * @brief Stream-file methods.
 *
 * Address range 444700-44490f. The six methods follow ten INT3 bytes after
 * the worker thread's helpers. The original file name is not in the
 * diagnostic strings.
 */
#include "ix.h"

/* Function start: 0x00444700 */
unsigned int IxStreamFile::ix_stream_file_read(void *newDestination,
                                                unsigned int bytes)
{
    ix_stream_file_wait();
    EnterCriticalSection(&csStreamerFileList);
    if (position + bytes > size)
        bytes = size - position;
    if (bytes > 0) {
        destination = (unsigned char *)newDestination;
        remaining = bytes;
        flags |= IX_STREAM_FILE_READING;
        if (previous != 0)
            previous->next = next;
        if (next == 0)
            pStreamIdleFiles = previous;
        else
            next->previous = previous;
        previous = pStreamReadRequests;
        next = 0;
        if (pStreamReadRequests != 0)
            pStreamReadRequests->next = this;
        pStreamReadRequests = this;
    }
    LeaveCriticalSection(&csStreamerFileList);
    SetEvent(hStreamerWakeupEvent);
    return bytes;
}

/* Function start: 0x00444809 */
void IxStreamFile::ix_stream_file_seek(unsigned int newPosition)
{
    ix_stream_file_wait();
    if (newPosition >= 0)
        position = size < newPosition ? size : newPosition;
    else
        position = 0;
}

/* Function start: 0x0044485B */
unsigned int IxStreamFile::ix_stream_file_tell(void)
{
    ix_stream_file_wait();
    return position;
}

/* Function start: 0x0044487F */
unsigned int IxStreamFile::ix_stream_file_size(void)
{
    return size;
}

/* Function start: 0x0044489B */
void IxStreamFile::ix_stream_file_wait(void)
{
    if ((flags & IX_STREAM_FILE_READING) != 0) {
        WaitForSingleObject(completionEvent, INFINITE);
        ResetEvent(completionEvent);
    }
}

/* Function start: 0x004448D9 */
int IxStreamFile::ix_stream_file_is_reading(void)
{
    return (flags & IX_STREAM_FILE_READING) != 0;
}
