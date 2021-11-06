#include "pathParser.h"
#include "../kernel.h"
#include "../string/string.h"
#include "../memory/heap/kernelHeap.h"
#include "../memory/memory.h"
#include "../status.h"

static int pathparser_path_valid_format(const char* filename)
{
  int len = strnlen(filename, MAX_PATH);

  return (len >= 3 && is_digit(filename[0]) && memcmp((void*) &filename[1], ":/", 2) == 0);
}

static int pathparser_extract_drive_by_path(const char** path)
{
  if(!pathparser_path_valid_format(*path))
  {
    return -BAD_PATH_ERROR;
  }

  int drive_number = to_numeric_digit(*path[0]);

  // add 3 bytes to skip drive number 0:/
  *path += 3;

  return drive_number;
}

static struct path_root* pathparser_create_root(int drive_number)
{
  struct path_root* path_root = (struct path_root*)kernel_zalloc(sizeof(struct path_root));
  path_root->drive_number = drive_number;
  path_root->first = 0;

  return path_root;
}

static const char* pathparser_extract_path_part(const char** path)
{
  char* result_path_part = (char*)kernel_zalloc(MAX_PATH);
  int i = 0;

  while(**path != '/' && **path != 0x00)
  {
    result_path_part[i] = **path;
    (*path)++;
    i++;
  }

  if (**path == '/')
  {
    // skip the forward slash
    (*path)++;
  }

  if (i == 0)
  {
    kernel_free(result_path_part);
    result_path_part = 0;
  }

  return result_path_part;
}

struct path_part* pathparser_parse_path_part(struct path_part* last_part, const char** path)
{
  const char* path_part_str = pathparser_extract_path_part(path);

  if (!path_part_str)
  {
    return 0;
  }

  struct path_part* part = (struct path_part*)kernel_zalloc(sizeof(struct path_part));
  part->part = path_part_str;
  part->next = 0x00;

  if (last_part)
  {
    last_part->next = part;
  }

  return part;
}

void pathparser_free(struct path_root* root)
{
  struct path_part* part = root->first;

  while (part)
  {
    struct path_part* next_part = part->next;
    kernel_free((void*) part->part);
    kernel_free(part);
    part = next_part;
  }

  kernel_free(root);
}

struct path_root* pathparser_parse(const char* path, const char* current_directory_path)
{
  int res = 0;
  const char* tmp_path = path;
  struct path_root* path_root = 0;

  if (strlen(path) > MAX_PATH)
  {
    return path_root;
  }

  res = pathparser_extract_drive_by_path(&tmp_path);
  
  if (res < 0)
  {
    return path_root;
  }

  path_root = pathparser_create_root(res);
  
  if (!path_root)
  {
    return path_root;
  }

  struct path_part* first_part = pathparser_parse_path_part(NULL, &tmp_path);

  if (!first_part)
  {
    return path_root;
  }

  path_root->first = first_part;

  struct path_part* part = pathparser_parse_path_part(first_part, &tmp_path);

  while (part)
  { 
    part = pathparser_parse_path_part(first_part, &tmp_path);
  }

  return path_root;
}
