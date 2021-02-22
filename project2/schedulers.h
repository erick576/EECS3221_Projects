#define MIN_PRIORITY 1
#define MAX_PRIORITY 10

struct node * scheduler;
double total_waiting_time;
double total_turnaround_time;
double total_response_time;
double total_processes;

// add a task to the list 
void add(char *name, int priority, int burst);

// invoke the scheduler
void schedule();
