#include "file.h"
#include "../config.h"
#include "../disk/disk.h"
#include "../kernel.h"
#include "../memory/memory.h"
#include "../memory/heap/kernelHeap.h"
#include "../string/string.h"
#include "./fat/fat16.h"
#include "../status.h"
#include "pathParser.h"

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
  fs_insert_filesystem(fat16_init());
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

FILE_MODE file_get_mode_by_string(const char* str)
{
  FILE_MODE mode = FILE_MODE_INVALID;

  if (strncmp(str, "r", 1) == 0)
  {
    mode = FILE_MODE_READ;
  }
  else if (strncmp(str, "w", 1) == 0)
  {
    mode = FILE_MODE_WRITE;
  }
  else if (strncmp(str, "a", 1) == 0)
  {
    mode = FILE_MODE_APPEND;
  }

  return mode;
}

int fopen(const char* filename, const char* mode_str)
{
  int res = 0;
  struct path_root* root_path = pathparser_parse(filename, NULL);

  if (!root_path)
  {
    res = -INVALID_ARGUMENT_ERROR;
    goto out;
  }

  // We cannot have just a root path 0:/ 0:/text.txt
  if (!root_path->first)
  {
    res = -INVALID_ARGUMENT_ERROR;
    goto out;
  }

  struct disk* disk = disk_get(root_path->drive_number);

  if (!disk)
  {
    res = -IO_ERROR;
    goto out;
  }

  if (!disk->filesystem)
  {
    res = -IO_ERROR;
    goto out;
  }

  FILE_MODE mode = file_get_mode_by_string(mode_str);
  
  if (mode == FILE_MODE_INVALID)
  {
    res = -INVALID_ARGUMENT_ERROR;
    goto out;
  }

  void* descriptor_private_data = disk->filesystem->open(disk, root_path->first, mode);

  if (ISERR(descriptor_private_data))
  {
    res = ERROR_I(descriptor_private_data);
    goto out;
  }

  struct file_descriptor* descriptor = 0;
  res = file_new_descriptor(&descriptor);

  if (res < 0)
  {
    return res;
  }

  descriptor->filesystem = disk->filesystem;
  descriptor->private = descriptor_private_data;
  descriptor->disk = disk;
  res = descriptor->index;

out:
  if (res < 0)
  {
    res = 0;
  }
  return res;
}