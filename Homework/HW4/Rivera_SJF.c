#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Job {
  char m_name[10];
  int m_arrival_time;
  int m_job_length;
  int m_remaining_time;
  int m_deadline;
  int m_io_request_time;
  int m_io_duration;
  struct Job *m_next;
  struct Job *m_prev;
} Job;

////////////////////////////
/// DEBUG FUNCTIONS
////////////////////////////

void print_job(const Job *job) {
  printf("Job Name: %s | Arrival Time: %d | Length : %d | Remaining Time: %d | "
         "Deadline: %d | I/O Request Time: %d | I/O Duration: %d\n",
         job->m_name, job->m_arrival_time, job->m_job_length,
         job->m_remaining_time, job->m_deadline, job->m_io_request_time,
         job->m_io_duration);
}
// Head pointers for job queues
Job *ready_queue = NULL;
Job *waiting_queue = NULL;
Job *missed_deadlines = NULL;

// Function to create a new job node
Job *create_job(char *name, int arrival, int length, int deadline,
                int io_time) {
  Job *new_job = (Job *)malloc(sizeof(Job));
  strcpy(new_job->m_name, name);
  new_job->m_arrival_time = arrival;
  new_job->m_job_length = length;
  new_job->m_remaining_time = length;
  new_job->m_deadline = deadline;
  new_job->m_io_request_time = io_time;
  new_job->m_io_duration = 200; // I/O takes 200ms
  new_job->m_next = new_job->m_prev = NULL;
  return new_job;
}

// Function to insert job into Ready Queue (sorted by remaining time for SJF)
void insert_into_ready_queue(Job *job) {
  if (!ready_queue) {
    ready_queue = job;
    return;
  }
  Job *temp = ready_queue, *prev = NULL;
  while (temp && temp->m_remaining_time < job->m_remaining_time) {
    prev = temp;
    temp = temp->m_next;
  }
  job->m_next = temp;
  job->m_prev = prev;
  if (prev)
    prev->m_next = job;
  else
    ready_queue = job;
  if (temp)
    temp->m_prev = job;
}

// Function to insert job into Waiting Queue (sorted by I/O request time)
void insert_into_waiting_queue(Job *job) {
  if (!waiting_queue) {
    waiting_queue = job;
    return;
  }
  Job *temp = waiting_queue, *prev = NULL;
  while (temp && temp->m_io_request_time < job->m_io_request_time) {
    prev = temp;
    temp = temp->m_next;
  }
  job->m_next = temp;
  job->m_prev = prev;
  if (prev)
    prev->m_next = job;
  else
    waiting_queue = job;
  if (temp)
    temp->m_prev = job;
}

// Function to move a job to the missed deadline list
void mark_as_missed(Job *job) {
  job->m_next = missed_deadlines;
  missed_deadlines = job;
}

// Function to execute the scheduling
void run_scheduler(int time_quantum) {
  int current_time = 0;
  Job *current_job = NULL;

  while (ready_queue || waiting_queue) {
    // Move I/O completed jobs back to Ready Queue
    Job *io_job = waiting_queue;
    while (io_job) {
      if (io_job->m_io_request_time + io_job->m_io_duration <= current_time) {
        Job *m_next_job = io_job->m_next;
        insert_into_ready_queue(io_job);
        if (io_job->m_prev)
          io_job->m_prev->m_next = m_next_job;
        if (m_next_job)
          m_next_job->m_prev = io_job->m_prev;
        if (waiting_queue == io_job)
          waiting_queue = m_next_job;
        io_job = m_next_job;
      } else {
        io_job = io_job->m_next;
      }
    }

    // Select job with shortest remaining time
    if (ready_queue) {
      current_job = ready_queue;
      ready_queue = ready_queue->m_next;
      if (ready_queue)
        ready_queue->m_prev = NULL;

      // Process job for time quantum or until completion
      int execution_time = (current_job->m_remaining_time < time_quantum)
                               ? current_job->m_remaining_time
                               : time_quantum;
      current_job->m_remaining_time -= execution_time;
      current_time += execution_time;

      // Check for I/O request
      if (current_job->m_io_request_time == current_time) {
        insert_into_waiting_queue(current_job);
      } else if (current_job->m_remaining_time > 0) {
        // Reinsert into Ready Queue if job is not finished
        insert_into_ready_queue(current_job);
      }

      // Check for deadline miss
      if (current_time > current_job->m_deadline) {
        mark_as_missed(current_job);
      }
    } else {
      // If no job in Ready Queue, increment time
      current_time += time_quantum;
    }
  }
}

// Function to print Ready Queue
void print_queue(Job *queue, const char *title) {
  printf("\n%s:\n", title);
  printf("Job Name  | Remaining Time | Arrival Time | Deadline\n");
  printf("-----------------------------------------------\n");
  while (queue) {
    printf("%-10s | %-14d | %-12d | %-8d\n", queue->m_name,
           queue->m_remaining_time, queue->m_arrival_time, queue->m_deadline);
    queue = queue->m_next;
  }
}

// Function to load jobs from a file
void load_jobs(const char *filename) {
  FILE *file = fopen(filename, "r");
  if (!file) {
    perror("Error opening file");
    return;
  }

  char name[10];
  int arrival, length, deadline, io_time;
  while (fscanf(file, "%s %d %d %d %d", name, &arrival, &length, &deadline,
                &io_time) != EOF) {

    Job *new_job = create_job(name, arrival, length, deadline, io_time);
    print_job(new_job);
    insert_into_ready_queue(new_job);
  }

  fclose(file);
}

int main() {
  load_jobs("jobs.txt"); // Load jobs from file
  run_scheduler(100);    // Time quantum is 100ms
  print_queue(ready_queue, "Final Ready Queue");
  print_queue(waiting_queue, "Final Waiting Queue");
  print_queue(missed_deadlines, "Missed Deadline Jobs");
  return 0;
}
