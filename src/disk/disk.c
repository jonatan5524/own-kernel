#include "../io/io.h"

int disk_read_sector(int lba, int total, void* buffer)
{
  outb(0x1F6, (lba >> 24) | 0xE0);
  outb(0x1F2, total);
  outb(0x1F3, (unsigned char)(lba & 0xFF));
  outb(0x1F4, (unsigned char)(lba >> 8));
  outb(0x1F5, (unsigned char)(lba >> 16));
  outb(0x1F7, 0x20);

  unsigned short* ptr = (unsigned short*) buffer;

  for (int i = 0; i < total; i++)
  {
    // wait for the buffer to be ready
    char c = insb(0x1F7);
    
    while(!(c & 0x08))
    {
      c = insb(0x1F7);
    }

    // copy from hard disk to memory 
    for (int j = 0; j < 256; j++)
    {
      *ptr = insw(0x1F0);
      ptr++;
    }
  }

  return 0;
}
