/**
 * Scheduler Implementation Using a Priority Scheduling Algorithm
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
        struct node *temp, *max_node;
        temp = scheduler;
        max_node = scheduler;
        int max = scheduler->task->priority;

        while (temp != NULL) {
            if (temp->task->priority >= max){
                max = temp->task->priority;
                max_node = temp;
            }
            temp = temp->next;
        }

        if (max_node != NULL){
            // Add Totals
            total_waiting_time += curr_time_sum;
            total_turnaround_time += curr_time_sum;
            total_response_time += curr_time_sum;

            curr_time_sum += max_node->task->burst;

            // Run Process
            run(max_node->task, max_node->task->burst);
            delete(&scheduler, max_node->task);
        }
    }

    printf("\n");
    printf("Average waiting time = %.2f\n", total_waiting_time / total_processes);
    printf("Average turnaround time = %.2f\n", total_turnaround_time / total_processes);
    printf("Average response time = %.2f\n", total_response_time / total_processes);
}