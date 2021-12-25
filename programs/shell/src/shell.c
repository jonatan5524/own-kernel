#include "shell.h"
#include "os.h"
#include "stdio.h"
#include "stdlib.h"

int main(int argc, char const *argv[]) {
  print("OS v1.0.0\n");

  while (1) {
    print("> ");
    char buff[1024];
    os_terminal_readline(buff, sizeof(buff), true);
    print("\n");
    os_system_run(buff);

    print("\n");
  }

  return 0;
}
