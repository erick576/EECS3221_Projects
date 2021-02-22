/**
 * Scheduler Implementation Using a First Come First Serve Algorithm
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "schedulers.h"
#include "list.h"
#include "task.h"
#include "cpu.h"

void add(char *name, int priority, int burst) {
    // Create Task
    Task * task = malloc(sizeof(struct task));
    task->name = name;
    task->priority = priority;
    task->burst = burst;

    // Add Totals
    struct node *temp;
    temp = scheduler;
    while (temp != NULL) {
        total_waiting_time += temp->task->burst;
        total_turnaround_time += temp->task->burst;
        total_response_time += temp->task->burst;
        temp = temp->next;
    }
    total_turnaround_time += burst;
    total_processes ++;

    // Add Task To Scheduler
    insert(&scheduler, task);
}

void schedule(){
    while (scheduler != NULL){
        struct node *temp;
        temp = scheduler;

        while (temp->next != NULL) {
            temp = temp->next;
        }
        if (temp != NULL){
            run(temp->task, temp->task->burst);
            delete(&scheduler, temp->task);
        }
    }

    printf("\n");
    printf("Average waiting time = %.2f\n", total_waiting_time / total_processes);
    printf("Average turnaround time = %.2f\n", total_turnaround_time / total_processes);
    printf("Average response time = %.2f\n", total_response_time / total_processes);
}