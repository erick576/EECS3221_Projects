/**
 * Scheduler Implementation Using a Shortest Job First Algorithm
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "schedulers.h"
#include "list.h"
#include "task.h"
#include "cpu.h"

double curr_time_sum = 0;

void add(char *name, int priority, int burst) {
    // Create Task
    Task * task = malloc(sizeof(struct task));
    task->name = name;
    task->priority = priority;
    task->burst = burst;

    // Add Totals
    total_turnaround_time += burst;
    total_processes ++;

    // Add Task To Scheduler
    insert(&scheduler, task);
}

void schedule(){
    while (scheduler != NULL){
        struct node *temp, *min_node;
        temp = scheduler;
        min_node = scheduler;
        int min = scheduler->task->burst;

        while (temp != NULL) {
            if (temp->task->burst <= min){
                min = temp->task->burst;
                min_node = temp;
            }
            temp = temp->next;
        }

        if (min_node != NULL){
            // Add Totals
            total_waiting_time += curr_time_sum;
            total_turnaround_time += curr_time_sum;
            total_response_time += curr_time_sum;

            curr_time_sum += min;

            // Run Process
            run(min_node->task, min_node->task->burst);
            delete(&scheduler, min_node->task);
        }
    }

    printf("\n");
    printf("Average waiting time = %.2f\n", total_waiting_time / total_processes);
    printf("Average turnaround time = %.2f\n", total_turnaround_time / total_processes);
    printf("Average response time = %.2f\n", total_response_time / total_processes);
}