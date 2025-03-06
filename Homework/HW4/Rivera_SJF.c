#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum Priority {
  TXT, /* Highest Priority Job. */
  FP,
  MM
} Priority;

typedef struct Job {
  char m_name[10];
  int m_arrivalTime;
  int m_length;
  int m_deadline;
  Priority m_priority;

  struct Job *m_previous;
  struct Job *m_next;
} Job;

Job *ready_queue = NULL;

// Function to create a new job
Job *createJob(char *name, int arrivalTime, int length, int deadline,
               char *priority) {
  Job *new_job = (Job *)malloc(sizeof(struct Job));
  strcpy(new_job->m_name, name);
  new_job->m_arrivalTime = arrivalTime;
  new_job->m_length = length;
  new_job->m_deadline = deadline;
  if (strcmp(priority, "TXT")) {
    new_job->m_priority = TXT;

  } else if (strcmp(priority, "FP")) {
    new_job->m_priority = FP;

  } else if (strcmp(priority, "MM")) {
    new_job->m_priority = MM;

  } else {
    fprintf(stderr, "createJob - Priority is invalid!!\n");
    exit(1);
  }
  new_job->m_previous = new_job->m_next = NULL;
  return new_job;
}

// sorted by arrivalTime
void insertIntoReadyQueue(Job *job) {
  if (!ready_queue) {
    ready_queue = job;
    return;
  }
  Job *temp = ready_queue;
  Job *previous = NULL;
  while (temp && temp->m_arrivalTime < job->m_arrivalTime) {
    previous = temp;
    temp = temp->m_next;
  }
  job->m_next = temp;
  job->m_previous = previous;
  if (previous) {
    previous->m_next = job;
  } else {
    ready_queue = job;
  }
  if (temp) {
    temp->m_previous = job;
  }
}
////////////////////////////
/// DEBUG FUNCTIONS
////////////////////////////

void printJob(const Job *job) {

  printf("%-8s | %-12d | %-6d | %-8d | %-8d\n", job->m_name, job->m_arrivalTime,
         job->m_length, job->m_deadline, job->m_priority);
}

// Function to print Queues
void printQueue(const Job *queue, const char *title) {
  printf("\n%s:\n", title);
  printf("Job Name | Arrival Time | Length | Deadline | Priority\n");
  printf("------------------------------------------------------\n");
  while (queue) {
    printJob(queue);
    queue = queue->m_next;
  }
}

// Function to load jobs from a file
void loadJobs(const char *filename) {
  FILE *file = fopen(filename, "r");
  if (!file) {
    fprintf(stderr, "Error opening file\n");
    exit(1);
  }
  // txt, FP, MM
  char name[10];
  int arrival, length, deadline;
  char priority[4];

  while (fscanf(file, "%9s %d %d %d %3s", name, &arrival, &length, &deadline,
                priority) != EOF) {
    insertIntoReadyQueue(createJob(name, arrival, length, deadline, priority));
  }

  fclose(file);
}

int main() {
  loadJobs("jobs.txt"); // Load jobs from file
  printQueue(ready_queue, "Ready Queue");
  return 0;
}
