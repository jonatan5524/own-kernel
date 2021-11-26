#include "fat16.h"
#include "../../kernel.h"
#include "../../string/string.h"
#include "../../memory/memory.h"
#include "../../memory/heap/kernelHeap.h"
#include "../../status.h"
#include "../../config.h"
#include "../../disk/streamer.h"
#include "../../disk/disk.h"
#include <stdint.h>

#define OS_FAT16_SIGNATURE 0x29
#define OS_FAT16_FAT_ENTRY_SIZE 0x02
#define OS_FAT16_BAD_SECTOR 0XF77
#define OS_FAT16_UNUSED 0X00

typedef unsigned int FAT_ITEM_TYPE;
#define FAT_ITEM_TYPE_DIRECTORY 0
#define FAT_ITEM_TYPE_FILE 1 

// Fat directory entry attributes bitmask
#define FAT_FILE_READ_ONLY 0x01
#define FAT_FILE_HIDDEN 0x02
#define FAT_FILE_SYSTEM 0x04
#define FAT_FILE_VOLUME_LABEL 0x08
#define FAT_FILE_SUBDIRECTORY 0x10
#define FAT_FILE_ARCHIVED 0x20
#define FAT_FILE_DEVICE 0x40
#define FAT_FILE_RESERVED 0x80 

struct fat_header_extended
{
  uint8_t driver_number;
  uint8_t win_nt_bit;
  uint8_t signature;
  uint32_t volume_id;
  uint8_t volume_id_string[11];
  uint8_t system_id_string[8];
} __attribute__((packed));

struct fat_header
{
  uint8_t short_jmp_ins[3];
  uint8_t oem_identifier[8];
  uint16_t bytes_per_sector;
  uint8_t sectors_per_cluster;
  uint16_t reserved_sectors;
  uint8_t fat_copies;
  uint16_t root_dir_entries;
  uint16_t number_of_sectors;
  uint8_t media_type;
  uint16_t sectors_per_fat;
  uint16_t sectors_per_track;
  uint16_t number_of_heads;
  uint32_t hidden_sectors;
  uint32_t sectors_big;
} __attribute__((packed));

struct fat_h
{
  struct fat_header primary_header;
  union fat_h_e
  {
    struct fat_header_extended extended_header;
  } shared;
};

struct fat_directory_item
{
  uint8_t filename[8];
  uint8_t ext[3];
  uint8_t attributes;
  uint8_t reserved;
  uint8_t creation_time_tenths_of_a_sec;
  uint16_t creation_time;
  uint16_t creation_date;
  uint16_t last_access;
  uint16_t high_16_bits_first_cluster;
  uint16_t last_mode_time;
  uint16_t last_mode_date;
  uint16_t low_16_bits_first_cluster;
  uint32_t filesize;
} __attribute__((packed));

struct fat_directory 
{
  struct fat_directory_item* item;
  int total;
  int sector_pos;
  int ending_sector_pos;
};

struct fat_item 
{
  union
  {
    struct fat_directory_item* item;
    struct fat_directory* directory;
  };

  FAT_ITEM_TYPE type;
};

struct fat_file_descriptor
{
  struct fat_item* item;
  uint32_t pos;
};

struct fat_private
{
  struct fat_h header;
  struct fat_directory root_directory;

  // Used to stream data clusters
  struct disk_stream* cluster_read_stream;
  // Used to stream the file allocation table
  struct disk_stream* fat_read_stream;

  // Used in situations where we stream the directory
  struct disk_stream* directory_stream;
};

int fat16_resolve(struct disk* disk);
void* fat16_open(struct disk* disk, struct path_part* path, FILE_MODE mode);
int fat16_read(struct disk* disk, void* descriptor, uint32_t size, uint32_t nmemb, char* out_ptr);
int fat16_seek(void* private, uint32_t offset, FILE_SEEK_MODE seek_mode);
int fat16_stat(struct disk* disk, void* private, struct file_stat* stat);

struct filesystem fat16_fs = 
{
  .resolve = fat16_resolve,
  .open = fat16_open,
  .read = fat16_read,
  .seek = fat16_seek,
  .stat = fat16_stat
};

struct filesystem* fat16_init()
{
  strcpy(fat16_fs.name, "FAT16");

  return &fat16_fs;
}

static void fat16_init_private(struct disk* disk, struct fat_private* private)
{
  memset(private, 0, sizeof(struct fat_private));
  private->cluster_read_stream = disk_stream_new(disk->id);
  private->fat_read_stream = disk_stream_new(disk->id);
  private->directory_stream = disk_stream_new(disk->id);
}

int fat16_sectore_to_absolute(struct disk* disk, int sector)
{
  return sector * disk->sector_size;
}

int fat16_get_total_items_for_directory(struct disk* disk, uint32_t directory_start_sector)
{
  struct fat_directory_item item;
  struct fat_directory_item empty_item;

  memset(&empty_item, 0, sizeof(empty_item));

  struct fat_private* fat_private = disk->fs_private;

  int i = 0;
  int directory_start_pos = directory_start_sector * disk->sector_size;
  struct disk_stream* stream = fat_private->directory_stream;

  if (disk_stream_seek(stream, directory_start_pos) != ALL_OK)
  {
    return -IO_ERROR;
  }

  while(1)
  {
    if (disk_stream_read(stream, &item, sizeof(item)) != ALL_OK)
    {
      return -IO_ERROR;
    }

    if (item.filename[0] == 0x00)
    {
      break;
    }

    if (item.filename[0] == 0xE5)
    {
      continue;
    }

    i++;
  }

  return i;
}

int fat16_get_root_directory(struct disk* disk, struct fat_private* fat_private, struct fat_directory* directory)
{
  struct fat_header* primary_header = &fat_private->header.primary_header;
  int root_dir_sector_pos = (primary_header->fat_copies * primary_header->sectors_per_fat) + primary_header->reserved_sectors;
  int root_dir_entries = fat_private->header.primary_header.root_dir_entries;
  int root_dir_size = (root_dir_entries * sizeof(struct fat_directory_item));
  int total_sectors = root_dir_size / disk->sector_size;

  if (root_dir_size % disk->sector_size)
  {
    total_sectors += 1;
  }

  int total_items = fat16_get_total_items_for_directory(disk, root_dir_sector_pos);

  struct fat_directory_item* dir = kernel_zalloc(root_dir_size);
  
  if (!dir)
  {
    return -NO_MEMORY_ERROR;
  }

  struct disk_stream* stream = fat_private->directory_stream;

  if (disk_stream_seek(stream, fat16_sectore_to_absolute(disk, root_dir_sector_pos)) != ALL_OK)
  {
    return -IO_ERROR;
  }

  if (disk_stream_read(stream, dir, root_dir_size) != ALL_OK)
  {
    return -IO_ERROR;
  }

  directory->item = dir;
  directory->total = total_items;
  directory->sector_pos = root_dir_sector_pos;
  directory->ending_sector_pos = root_dir_sector_pos + (root_dir_size / disk->sector_size);

  return 0;
}

int fat16_resolve(struct disk* disk)
{
  int res = 0;
  struct fat_private* fat_private = (struct fat_private*)kernel_zalloc(sizeof(struct fat_private));
  fat16_init_private(disk, fat_private);
  
  disk->fs_private = fat_private;
  disk->filesystem = &fat16_fs;
  
  struct disk_stream* stream = disk_stream_new(disk->id);
  
  if (!stream)
  {
    res = -NO_MEMORY_ERROR;
    goto out;
  }

  if (disk_stream_read(stream, &fat_private->header, sizeof(fat_private->header)) != ALL_OK)
  {
    res = -IO_ERROR;
    goto out;
  }

  if (fat_private->header.shared.extended_header.signature != 0x29)
  {
    res = -FILE_SYSTEM_NOT_US_ERROR; 
    goto out;
  }
  
  if (fat16_get_root_directory(disk, fat_private, &fat_private->root_directory) != ALL_OK)
  {
    res = -IO_ERROR;
    goto out;
  }

out:
  if (stream)
  {
    disk_stream_close(stream);
  }

  if (res < 0)
  {
    kernel_free(fat_private);
    disk->fs_private = 0;
  }

  return res;
}

void fat16_to_proper_string(char** out, const char* in)
{
  while(*in != 0x00 && *in != 0x20)
  {
    **out = *in;
    *out += 1;
    in += 1;
  }

  if (*in == 0x20)
  {
    **out = 0x00;
  }
}

void fat16_get_full_relative_filename(struct fat_directory_item* item, char* out, int max_len)
{
  memset(out, 0x00, max_len);
  char* out_tmp = out;
  
  fat16_to_proper_string(&out_tmp, (const char*) item->filename);

  if (item->ext[0] != 0x00 && item->ext[0] != 0x20)
  {
    *out_tmp++ = '.';
    fat16_to_proper_string(&out_tmp, (const char*) item->ext);
  }
}

struct fat_directory_item* fat16_clone_directory_item(struct fat_directory_item* item, int size)
{
  struct fat_directory_item* item_copy = 0;

  if (size < sizeof(struct fat_directory_item))
  {
    return 0;
  }

  item_copy = kernel_zalloc(size);

  if (!item_copy)
  {
    return 0;
  }

  memcpy(item_copy, item, size);

  return item_copy;
}

static uint32_t fat16_get_first_cluster(struct fat_directory_item* item)
{
  return item->high_16_bits_first_cluster | item->low_16_bits_first_cluster;
}

static int fat16_cluster_to_sector(struct fat_private* private, int cluster)
{
  return private->root_directory.ending_sector_pos + ((cluster - 2) * private->header.primary_header.sectors_per_cluster);
}

static uint32_t fat16_get_first_fat_sector(struct fat_private* private)
{
  return private->header.primary_header.reserved_sectors;
}

static int fat16_get_fat_entry(struct disk* disk, int cluster)
{
  int res = -1;
  struct fat_private* private = disk->fs_private;
  struct disk_stream* stream = private->fat_read_stream;

  if (!stream)
  {
    goto out;
  }

  uint32_t fat_table_position = fat16_get_first_fat_sector(private) * disk->sector_size;
  res = disk_stream_seek(stream, fat_table_position * (cluster * OS_FAT16_FAT_ENTRY_SIZE));

  if (res < 0)
  {
    goto out;
  }

  uint16_t result = 0;
  res = disk_stream_read(stream, &result, sizeof(result));

  if (res < 0)
  {
    goto out;
  }

  res = result;
out:
  return res;
}

static int fat16_get_cluster_for_offset(struct disk* disk, int starting_cluster, int offset)
{
  int res = 0;
  struct fat_private* private = disk->fs_private;
  int size_of_cluster_bytes = private->header.primary_header.sectors_per_cluster * disk->sector_size;
  int cluster_to_use = starting_cluster;
  int cluster_ahead = offset / size_of_cluster_bytes;

  for (int i = 0; i < cluster_ahead; i++)
  {
    int entry = fat16_get_fat_entry(disk, cluster_to_use);

    if (entry == 0xFF8 || entry == 0xFFF)
    {
      // we are at the last entry in the file
      res = -IO_ERROR;
      goto out;
    }

    if (entry == OS_FAT16_BAD_SECTOR)
    {
      res = -IO_ERROR;
      goto out;
    }

    // reserved sectors?
    if (entry == 0xFF0 || entry == 0xFF6)
    {
      res = -IO_ERROR;
      goto out;
    }

    if (entry == 0)
    {
      res = -IO_ERROR;
      goto out;
    }

    cluster_to_use = entry;
  }

  res = cluster_to_use;
out:
  return res;
}

static int fat16_read_internal_from_stream(struct disk* disk, struct disk_stream* stream, int cluster, int offset, int total, void* out)
{
  int res = 0;
  struct fat_private* private = disk->fs_private;
  int size_of_cluster_bytes = private->header.primary_header.sectors_per_cluster * disk->sector_size;
  int cluster_to_use = fat16_get_cluster_for_offset(disk, cluster, offset);

  if (cluster_to_use < 0)
  {
    res = cluster_to_use;
    goto out;
  }

  int offset_from_cluster = offset % size_of_cluster_bytes;

  int starting_sector = fat16_cluster_to_sector(private, cluster_to_use);
  int starting_pos = (starting_sector * disk->sector_size) + offset_from_cluster;
  int total_to_read = total > size_of_cluster_bytes ? size_of_cluster_bytes : total;
  
  res = disk_stream_seek(stream, starting_pos);

  if (res != ALL_OK)
  {
    goto out;
  }

  res = disk_stream_read(stream, out, total_to_read);

  if (res != ALL_OK)
  {
    goto out;
  }

  total -= total_to_read;

  if (total > 0)
  {
    res = fat16_read_internal_from_stream(disk, stream, cluster, offset + total_to_read, total, out + total_to_read);
  }

out:
  return res;
}

static int fat16_read_internal(struct disk* disk, int starting_cluster, int offset, int total, void* out)
{
  struct fat_private* fs_private = disk->fs_private;
  struct disk_stream* stream = fs_private->cluster_read_stream;
  
  return fat16_read_internal_from_stream(disk, stream, starting_cluster, offset, total, out);
}

void fat16_free_directory(struct fat_directory* directory)
{
  if (!directory)
  {
    return;
  }

  if (directory->item)
  {
    kernel_free(directory->item);
  }

  kernel_free(directory);
}

void fat16_fat_item_free(struct fat_item* item)
{
  if (item->type == FAT_ITEM_TYPE_DIRECTORY)
  {
    fat16_free_directory(item->directory);
  }
  else if (item->type == FAT_ITEM_TYPE_FILE)
  {
    kernel_free(item->item);
  }

  kernel_free(item);
}

struct fat_directory* fat16_load_fat_directory(struct disk* disk, struct fat_directory_item* item)
{
  int res = 0;
  struct fat_directory* directory = 0;
  struct fat_private* fat_private = disk->fs_private;

  if (!(item->attributes & FAT_FILE_SUBDIRECTORY))
  {
    res = -INVALID_ARGUMENT_ERROR;
    goto out;
  }

  directory = kernel_zalloc(sizeof(struct fat_directory));

  if (!directory)
  {
    res = -NO_MEMORY_ERROR;
    goto out;
  }

  int cluster = fat16_get_first_cluster(item);
  int cluster_sector = fat16_cluster_to_sector(fat_private, cluster);
  int total_items = fat16_get_total_items_for_directory(disk, cluster_sector);
  directory->total = total_items;
  int directory_size = directory->total * sizeof(struct fat_directory_item);
  directory->item = kernel_zalloc(directory_size);

  if (!directory->item)
  {
    res = -NO_MEMORY_ERROR;
    goto out;
  }

  res = fat16_read_internal(disk, cluster, 0x00, directory_size, directory->item);

  if (res != ALL_OK)
  {
    goto out;
  }


out:
  if (res != ALL_OK)
  {
    fat16_free_directory(directory);
  }

  return directory;
}

struct fat_item* fat16_new_fat_item_for_directory_item(struct disk* disk, struct fat_directory_item* item)
{
  struct fat_item* f_item = kernel_zalloc(sizeof(struct fat_item));

  if (!f_item)
  {
    return 0;
  }

  if (item->attributes & FAT_FILE_SUBDIRECTORY)
  {
    f_item->directory = fat16_load_fat_directory(disk, item);
    f_item->type = FAT_ITEM_TYPE_DIRECTORY;
  }

  f_item->type = FAT_ITEM_TYPE_FILE;
  f_item->item = fat16_clone_directory_item(item, sizeof(struct fat_directory_item));

  return f_item;
}

struct fat_item* fat16_find_item_in_directory(struct disk* disk, struct fat_directory* directory, const char* name)
{
  struct fat_item* f_item = 0;
  char tmp_filename[OS_MAX_PATH];

  for (int i = 0; i< directory->total; i++)
  {
    fat16_get_full_relative_filename(&directory->item[i], tmp_filename, sizeof(tmp_filename));

    if (istrncmp(tmp_filename, name, sizeof(tmp_filename)) == 0)
    {
      f_item = fat16_new_fat_item_for_directory_item(disk, &directory->item[i]);
    }
  }

  return f_item;
}

struct fat_item* fat16_get_directory_entry(struct disk* disk, struct path_part* path)
{
  struct fat_private* fat_private = disk->fs_private;
  struct fat_item* current_item = 0;
  struct fat_item* root_item = fat16_find_item_in_directory(disk, &fat_private->root_directory, path->part);

  if (!root_item)
  {
    goto out;
  }

  struct path_part* next_part = path->next;
  current_item = root_item;

  while(next_part != 0)
  {
    if (current_item->type != FAT_ITEM_TYPE_DIRECTORY)
    {
      current_item = 0;
      break;
    }

    struct fat_item* tmp_item = fat16_find_item_in_directory(disk, current_item->directory, next_part->part);
    fat16_fat_item_free(current_item);

    current_item = tmp_item;
    next_part = next_part->next;
  }

out:
  return current_item;
}

void* fat16_open(struct disk* disk, struct path_part* path, FILE_MODE mode)
{
  if (mode != FILE_MODE_READ)
  {
    return ERROR(-READ_ONLY_ERROR);
  }

  struct fat_file_descriptor* descriptor = 0;
  descriptor = kernel_zalloc(sizeof(struct fat_file_descriptor));
  
  if (!descriptor)
  {
    return ERROR(-NO_MEMORY_ERROR);
  }

  descriptor->item = fat16_get_directory_entry(disk, path);

  if (!descriptor->item)
  {
    return ERROR(-IO_ERROR);
  }

  descriptor->pos = 0;

  return descriptor;
}

int fat16_read(struct disk* disk, void* descriptor, uint32_t size, uint32_t nmemb, char* out_ptr)
{
  int res = 0;
  struct fat_file_descriptor* fat_descriptor = descriptor;
  struct fat_directory_item* item = fat_descriptor->item->item;
  int offset = fat_descriptor->pos;

  for(uint32_t i = 0; i < nmemb; i++)
  {
    res = fat16_read_internal(disk, fat16_get_first_cluster(item), offset, size, out_ptr);

    if (ISERR(res))
    {
      goto out;
    }

    out_ptr += size;
    offset += size;
  }

  res = nmemb;

out:
  return res;
}

int fat16_seek(void* private, uint32_t offset, FILE_SEEK_MODE seek_mode)
{
  int res = 0;
  struct fat_file_descriptor* descriptor = private;
  struct fat_item* descriptor_item = descriptor->item;

  if (descriptor_item->type != FAT_ITEM_TYPE_FILE)
  {
    res = -INVALID_ARGUMENT_ERROR;
    goto out;
  }
  
  struct fat_directory_item* root_item = descriptor_item->item;

  if (offset >= root_item->filesize)
  {
    res = -IO_ERROR;
    goto out;
  }

  switch(seek_mode)
  {
    case SEEK_SET:
      descriptor->pos = offset;

      break;

    case SEEK_END:
      res = -UNIMPLEMENTED_ERROR;

      break;

    case SEEK_CUR:
      descriptor->pos += offset;

      break;

    default:
      res = -INVALID_ARGUMENT_ERROR;

      break;
  }

out:
  return res;
}

int fat16_stat(struct disk* disk, void* private, struct file_stat* stat)
{
  int res = 0;
  struct fat_file_descriptor* descriptor = (struct fat_file_descriptor*) private;
  struct fat_item* descriptor_item = descriptor->item;

  if (descriptor_item->type != FAT_ITEM_TYPE_FILE)
  {
    res = -INVALID_ARGUMENT_ERROR;
    goto out;
  }

  struct fat_directory_item* root_item = descriptor_item->item;
  
  stat->filesize = root_item->filesize;
  stat->flags = 0x00;

  if (root_item->attributes & FAT_FILE_READ_ONLY)
  {
    stat->flags |= FILE_STAT_READ_ONLY;
  }

out:
  return res;
}
