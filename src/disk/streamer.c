#include "streamer.h"
#include "../memory/heap/kernelHeap.h"
#include "../config.h"
#include "disk.h"

#include <stdbool.h>

struct disk_stream* disk_stream_new(int disk_id)
{
  struct disk* disk = disk_get(disk_id);

  if (!disk)
  {
    return 0;
  }

  struct disk_stream* streamer = (struct disk_stream*)kernel_zalloc(sizeof(struct disk_stream));
  streamer->position = 0;
  streamer->disk = disk;

  return streamer;
}

int disk_stream_seek(struct disk_stream* stream, int pos)
{
  stream->position = pos;

  return 0;
}

int disk_stream_read(struct disk_stream* stream, void* out, int total)
{
  int sector = stream->position / SECTOR_SIZE;
  int offset = stream->position % SECTOR_SIZE;
  int total_to_read = total;
  bool overflow = (offset + total_to_read) > SECTOR_SIZE;
  char buffer[SECTOR_SIZE];

  if (overflow)
  {
    total_to_read -= (offset + total_to_read) - SECTOR_SIZE;
  }

  int res = disk_read_block(stream->disk, sector, 1, buffer);

  if (res < 0)
  {
    return res;
  }

  for (int i = 0; i < total_to_read; i++)
  {
    *(char*)out++ = buffer[offset + i];
  }

  // adjust the stream
  stream->position += total_to_read;

  if (overflow)
  {
    res = disk_stream_read(stream, out, total - total_to_read);
  }

  return res;
}

void disk_stream_close(struct disk_stream* stream)
{
  kernel_free(stream);
}
