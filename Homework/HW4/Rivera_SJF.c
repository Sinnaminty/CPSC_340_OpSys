#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int ms = 0;

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

Job *txt_ready_queue = NULL;
Job *fp_ready_queue = NULL;
Job *mm_ready_queue = NULL;
Job *finished_queue = NULL;

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
void printAllQueues() {
  printQueue(txt_ready_queue, "TXT Ready Queue");
  printQueue(fp_ready_queue, "FP Ready Queue");
  printQueue(mm_ready_queue, "MM Ready Queue");
  printQueue(finished_queue, "Finished Queue");
}
// Function to create a new job
Job *createJob(char *name, int arrivalTime, int length, int deadline,
               char *priority) {
  Job *new_job = (Job *)malloc(sizeof(struct Job));
  strcpy(new_job->m_name, name);
  new_job->m_arrivalTime = arrivalTime;
  new_job->m_length = length;
  new_job->m_deadline = deadline;
  if (strcmp(priority, "TXT") == 0) {
    new_job->m_priority = TXT;

  } else if (strcmp(priority, "FP") == 0) {
    new_job->m_priority = FP;

  } else if (strcmp(priority, "MM") == 0) {
    new_job->m_priority = MM;

  } else {
    fprintf(stderr, "createJob - Priority is invalid!!\n");
    exit(1);
  }
  new_job->m_previous = new_job->m_next = NULL;
  return new_job;
}

void insertIntoTXTReadyQueue(Job *job) {
  if (!txt_ready_queue) {
    txt_ready_queue = job;
    return;
  }
  Job *temp = txt_ready_queue;
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
    txt_ready_queue = job;
  }
  if (temp) {
    temp->m_previous = job;
  }
}
void insertIntoFPReadyQueue(Job *job) {

  if (!fp_ready_queue) {
    fp_ready_queue = job;
    return;
  }
  Job *temp = fp_ready_queue;
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
    fp_ready_queue = job;
  }
  if (temp) {
    temp->m_previous = job;
  }
}

void insertIntoMMReadyQueue(Job *job) {
  if (!mm_ready_queue) {
    mm_ready_queue = job;
    return;
  }
  Job *temp = mm_ready_queue;
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
    mm_ready_queue = job;
  }
  if (temp) {
    temp->m_previous = job;
  }
}

void insertIntoFinishedQueue(Job *job) {
  printf("insertIntoReadyQueue\n");
  if (!finished_queue) {
    finished_queue = job;
    return;
  }

  Job *temp = finished_queue;
  while (temp->m_next) {
    temp = temp->m_next;
  }
  temp->m_next = job;
}

// sorted by arrivalTime
void insertIntoReadyQueue(Job *job) {
  switch (job->m_priority) {
  case TXT:
    insertIntoTXTReadyQueue(job);
    break;
  case FP:
    insertIntoFPReadyQueue(job);
    break;
  case MM:
    insertIntoMMReadyQueue(job);
    break;
  }
}

Job *findShortestJob(Job *head) {
  Job *shortest_job = head;
  Job *temp = head;
  Job *previous = NULL;
  Job *next = NULL;

  while (temp->m_next) {
    temp = temp->m_next;
    if (shortest_job->m_length > temp->m_length) {
      shortest_job = temp;
    }
  }
  // now we have the shortest Job!
  // connect previous and next together, and insert shortestJob in
  // finished_queue
  previous = shortest_job->m_previous;
  next = shortest_job->m_next;
  shortest_job->m_next = NULL;
  shortest_job->m_previous = NULL;

  // if next is null.. that means we are at the end of the list
  // if both are null.. that means we are the last item in this list

  // if neither are null.. that means we are in the middle of the list
  if (previous && next) {
    previous->m_next = next;
    next->m_previous = previous;

    // if previous is null.. that means we are at the beginning of the list
  } else if (!previous && next) {
    *head = *head->m_next;
  }

  printf("shortest_job returned!\n");
  printQueue(txt_ready_queue, "txt_ready_queue");
  return shortest_job;
}

void doJob(Job *job) {
  // need to keep track of global time
  // need to increment global time based on job time
  insertIntoFinishedQueue(job);
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

void start() {
  // find shortest job in each queue, do it, and then put it in the finished
  // print all queues every 500 ms

  while (1) {
    if (ms % 500 == 0) {
      printAllQueues();
    }

    if (txt_ready_queue) {
      doJob(findShortestJob(txt_ready_queue));
    } else if (fp_ready_queue) {
      doJob(findShortestJob(fp_ready_queue));
    } else if (mm_ready_queue) {
      doJob(findShortestJob(mm_ready_queue));
    } else {
      break;
    }
    ms++;
  }
}

int main() {
  loadJobs("jobs.txt"); // Load jobs from file
  // we start the scheduling process
  // jobs must be done in priority, and then inserted into the finished queue
  // when done print every 500 ms
  start();
  printf("Finished!\n");
  printAllQueues();
  return 0;
}
