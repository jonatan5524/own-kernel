#include "file.h"
#include "../config.h"
#include "../kernel.h"
#include "../memory/memory.h"
#include "../memory/heap/kernelHeap.h"
#include "../status.h"

struct filesystem* filesystems[OS_MAX_FILESYSTEMS];
struct file_descriptor* file_descriptors[OS_MAX_FILE_DESCRIPTORS];

static struct filesystem** fs_get_free_filesystem()
{
  int i = 0;

  for (i = 0; i < OS_MAX_FILESYSTEMS; i++)
  {
    if(filesystems[i] == 0)
    {
      return &filesystems[i];
    }
  }

  return 0;
}

void fs_insert_filesystem(struct filesystem* filesystem)
{
  struct filesystem** fs;
  fs = fs_get_free_filesystem();

  if (!fs)
  {
    print("Problem inserting filesystem");
    while(1)
    {}
  }

  *fs = filesystem;
}

static void fs_static_load()
{
 // fs_insert_filesystem(fat16_init());
}

void fs_load()
{
  memset(filesystems, 0, sizeof(filesystems));
  fs_static_load();
}

void fs_init()
{
  memset(file_descriptors, 0, sizeof(file_descriptors));
  fs_load();
}

static int file_new_descriptor(struct file_descriptor** descriptor_out)
{
  int res = -NO_MEMORY_ERROR;

  for(int i = 0; i <OS_MAX_FILE_DESCRIPTORS; i++)
  {
    if (file_descriptors[i] == 0)
    {
      struct file_descriptor* descriptor = kernel_zalloc(sizeof(struct file_descriptor));
      descriptor->index = i + 1;
      file_descriptors[i] = descriptor;
      *descriptor_out = descriptor;

      return 0;
    }
  }

  return res;
}

static struct file_descriptor* file_get_descriptor(int file_descriptor_index)
{
  if (file_descriptor_index <= 0 || file_descriptor_index >= OS_MAX_FILE_DESCRIPTORS)
  {
    return 0;
  }

  return file_descriptors[file_descriptor_index - 1];
}

struct filesystem* fs_resolve(struct disk* disk)
{
  struct filesystem* fs = 0;

  for(int i = 0; i < OS_MAX_FILESYSTEMS; i++)
  {
    if ( filesystems[i] != 0 && filesystems[i]->resolve(disk) == 0 )
    {
      fs = filesystems[i];
      break;
    }
  }

  return fs;
}

int fopen(const char* filename, const char* mode)
{
  return -IO_ERROR;
}
