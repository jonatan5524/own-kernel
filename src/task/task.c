#include "task.h"
#include "process.h"
#include "../memory/memory.h"
#include "../status.h"
#include "../kernel.h"
#include "../memory/heap/kernelHeap.h"

// The current task that is running 
struct task* current_task = 0;

// Task linked list 
struct task* task_tail = 0;
struct task* task_head = 0;

int task_init(struct task* task, struct process* process);

struct task* task_current()
{
  return current_task;
}

struct task* task_new(struct process* process)
{
  int res = 0;
  struct task* task = kernel_zalloc(sizeof(struct task));

  if (!task)
  {
    res = -NO_MEMORY_ERROR;
    goto out;
  }

  res = task_init(task, process);

  if (res != ALL_OK)
  {
    goto out;
  }

  if (task_head == 0)
  {
    task_head = task;
    task_tail = task;
    goto out;
  }

  task_tail->next = task;
  task->prev = task_tail;
  task_tail = task;

out:
  if (ISERR(res))
  {
    task_free(task);
    
    return ERROR(res);
  }

  return task;
}

struct task* task_get_next()
{
  if (!current_task->next)
  {
    return task_head;
  }

  return current_task->next;
}

static void task_list_remove(struct task* task)
{
  if (task->prev)
  {
    task->prev->next = task->next;
  }

  if (task == task_head)
  {
    task_head = task->next;
  }

  if (task == task_tail)
  {
    task_tail = task->prev;
  }

  if (task == current_task)
  {
    current_task = task_get_next();
  }
}

int task_free(struct task* task)
{
  paging_free_4gb(task->page_directory);
  task_list_remove(task);

  kernel_free(task);

  return 0;
}

int task_init(struct task* task, struct process* process)
{
  memset(task, 0x00, sizeof(struct task));

  // Map the entrie 4GB address space to itself
  task->page_directory = paging_new_4gb(PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL);

  if (!task->page_directory)
  {
    return -IO_ERROR;
  }

  task->process = process;
  task->registers.ip = OS_PROGRAM_VIRTUAL_ADDRESS;
  task->registers.ss = USER_DATA_SEGMENT;
  task->registers.esp = OS_USER_PROGRAM_VIRTUAL_STACK_ADDRESS_START;
  
  return 0;
}
