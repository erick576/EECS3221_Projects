/**
 * Scheduler Implementation Using a Round Robin Algorithm
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
    task->tid = total_processes;
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
    int values_behind[(int) total_processes];
    for (int i = 0; i < total_processes; i++) {
        values_behind[i] = 0;
    }

    int curr_processes = total_processes;
    int stop_point = curr_processes - 1;
    while (scheduler != NULL){
        int counter = 0;
        struct node *temp;
        temp = scheduler;

        while (temp->next != NULL && counter != stop_point) {
            temp = temp->next;
            counter ++;
        }

        // Run Process (A Slice Of The Quantum)
        if (temp->task->burst < 10){
            run(temp->task, temp->task->burst);
            values_behind[temp->task->tid] += temp->task->burst;
            temp->task->burst = 0;
        } else{
            run(temp->task, 10);
            values_behind[temp->task->tid] += 10;
            temp->task->burst -= 10;
        }

        if (temp->task->burst == 0){
            // Add Totals
            for (int i = 0; i < total_processes; i++) {
                if (i != temp->task->tid){
                    total_waiting_time += values_behind[i];
                    total_turnaround_time += values_behind[i];
                    total_response_time += values_behind[i];
                }
            }

            // Delete Task
            delete(&scheduler, temp->task);
            curr_processes--;
        }
        stop_point --;
        if (stop_point < 0){
            stop_point = curr_processes - 1;
        }
    }

    printf("\n");
    printf("Average waiting time = %.2f\n", total_waiting_time / total_processes);
    printf("Average turnaround time = %.2f\n", total_turnaround_time / total_processes);
    printf("Average response time = %.2f\n", total_response_time / total_processes);
}