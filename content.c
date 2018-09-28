#include <stdio.h>
#include <ucontext.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/pthread.h>

ucontext_t mm;

void test(void *arg) {
    puts("I am in test");
   // printf("%d\n", a);
}

typedef struct _Task {
    char *stackptr;
    size_t stack_size;
    void (*func_ptr)(void*);
    void *argv;
    ucontext_t mm;
}Task;

void exec(int x, int y) {
    unsigned long addr;
    addr = (unsigned long)y;
    addr <<= 32;
    addr |= (unsigned long)x;
    Task *task;
    task = (Task *)addr;
    task->func_ptr(task->argv);
    setcontext(&mm);
}

Task* createTask(void(*fn)(void*), size_t stack_size, void *arg){
    Task *task;
    task = malloc(sizeof(*task)+stack_size);
    if (task == NULL) {
      puts("errr");exit(0);
    }
    memset(task, 0, sizeof(*task));
    task->stackptr = (char *)(task+1);
    task->stack_size = stack_size;
    getcontext(&task->mm);
    task->mm.uc_stack.ss_sp = task->stackptr;
    task->mm.uc_stack.ss_size = task->stack_size;
    task->func_ptr = fn;
    task->argv= arg;
    unsigned long addr = (unsigned long)task;
    int x,y;
    x = (int) addr;
    addr >>=32;
    y = (int)addr;    
    makecontext(&task->mm, (void(*)())exec,2, x, y);
    return task;
}

int main(int argc, char *argv[]) {
    Task *task = createTask(test, 8604, NULL);
    swapcontext(&mm, &task->mm);
    puts("main end");
    return 0;
}
