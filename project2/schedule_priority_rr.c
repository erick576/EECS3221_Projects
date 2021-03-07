/**
 * Scheduler Implementation Using a Priority With Round Robin Algorithm
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
    // Sort Out Priorities
    int max_priority = 0;
    struct node *pri;
    pri = scheduler;
    while (pri != NULL) {
        if (pri->task->priority >= max_priority){
            max_priority = pri->task->priority;
        }
        pri = pri->next;
    }
    int priorities[(int) max_priority + 1];
    for (int i = 0; i < max_priority + 1; i++) {
        priorities[i] = 0;
    }
    pri = scheduler;
    while (pri != NULL) {
        priorities[pri->task->priority] = priorities[pri->task->priority] + 1;
        pri = pri->next;
    }


    int values_behind[(int) total_processes];
    for (int i = 0; i < total_processes; i++) {
        values_behind[i] = 0;
    }

    int stay_in_rr = 1;

    // Priority
    while (scheduler != NULL){
        struct node *temp, *max_node;
        temp = scheduler;
        max_node = scheduler;
        stay_in_rr = 1;
        int max = scheduler->task->priority;

        while (temp != NULL) {
            if (temp->task->priority >= max){
                max = temp->task->priority;
                max_node = temp;
            }
            temp = temp->next;
        }

        if (max_node != NULL){
            // Round Robin
            struct  node *temp0;
            temp0 = scheduler;
            int num_of = 0;
            while (temp0 != NULL){
                if (temp0->task->priority == max){
                    num_of ++;
                }
                temp0 = temp0->next;
            }

            int curr_processes = num_of;
            int stop_point = num_of;

            while (stay_in_rr == 1){
                int counter = 0;
                struct node *temp1;
                temp1 = scheduler;
                int response_yet = 0;

                while (temp1->next != NULL) {
                    if (temp1->task->priority == max){
                        counter ++;
                    }
                    if (counter == stop_point){
                        break;
                    } else{
                        temp1 = temp1->next;
                    }
                }

                response_yet = 0;
                if (values_behind[temp1->task->tid] == 0){
                    response_yet = 1;
                }
                if (priorities[temp1->task->priority] > 1){
                    // Run Process (A Slice Of The Quantum)
                    if (temp1->task->burst < 10){
                        run(temp1->task, temp1->task->burst);
                        values_behind[temp1->task->tid] += temp1->task->burst;
                        temp1->task->burst = 0;
                    } else{
                        run(temp1->task, 10);
                        values_behind[temp1->task->tid] += 10;
                        temp1->task->burst -= 10;
                    }
                }
                else {
                    run(temp1->task, temp1->task->burst);
                    values_behind[temp1->task->tid] += temp1->task->burst;
                    temp1->task->burst = 0;
                }

                if (values_behind[temp1->task->tid] != 0 && response_yet == 1){
                    // Add Response Time
                    for (int i = 0; i < total_processes; i++) {
                        if (i != temp1->task->tid){
                            total_response_time += values_behind[i];
                        }
                    }
                }

                if (temp1->task->burst == 0){
                    priorities[temp1->task->priority] --;
                    // Add Totals
                    for (int i = 0; i < total_processes; i++) {
                        if (i != temp1->task->tid){
                            total_waiting_time += values_behind[i];
                            total_turnaround_time += values_behind[i];
                        }
                    }

                    // Delete Task
                    delete(&scheduler, temp1->task);
                    curr_processes--;
                }
                stop_point --;
                if (stop_point == 0){
                    stop_point = curr_processes;
                }

                struct node *temp2;
                temp2 = scheduler;
                stay_in_rr = 0;
                while (temp2 != NULL) {
                    if (temp2->task->priority == max){
                        stay_in_rr = 1;
                    }
                    temp2 = temp2->next;
                }
            }
        }
    }

    printf("\n");
    printf("Average waiting time = %.2f\n", total_waiting_time / total_processes);
    printf("Average turnaround time = %.2f\n", total_turnaround_time / total_processes);
    printf("Average response time = %.2f\n", total_response_time / total_processes);
}