#ifndef DISK_H
#define DISK_H

#include "../fileSystem/file.h"

typedef unsigned int DISK_TYPE;

// represent real physical hard disk 
#define DISK_TYPE_REAL 0

struct disk
{
  DISK_TYPE type;
  int sector_size;

  struct filesystem* filesystem;
};

void disk_search_and_init();
struct disk* disk_get(int index);
int disk_read_block(struct disk* idisk, unsigned int lba, int total, void* buffer);

#endif 
