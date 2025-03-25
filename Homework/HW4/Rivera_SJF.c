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
  int m_remainingTime;
  int m_deadline;
  int m_completionTime;
  Priority m_priority;

} Job;

typedef struct Node {
  struct Job *m_job;

  struct Node *m_previous;
  struct Node *m_next;
} Node;

Job *createJob(char *name, int arrivalTime, int length, int deadline,
               char *priority) {
  Job *newJob = (Job *)malloc(sizeof(struct Job));
  strcpy(newJob->m_name, name);

  newJob->m_arrivalTime = arrivalTime;
  newJob->m_length = length;
  newJob->m_remainingTime = length;
  newJob->m_deadline = deadline;
  newJob->m_completionTime = 0;

  if (strcmp(priority, "TXT") == 0) {
    newJob->m_priority = TXT;

  } else if (strcmp(priority, "FP") == 0) {
    newJob->m_priority = FP;

  } else if (strcmp(priority, "MM") == 0) {
    newJob->m_priority = MM;

  } else {
    fprintf(stderr, "createJob - Priority is invalid!!\n");
    exit(1);
  }
  return newJob;
}

//////////////////
/// NODE FUNCTIONS
//////////////////

Node *createNode(Job *job) {
  Node *newNode = (Node *)malloc(sizeof(struct Node));
  newNode->m_job = job;
  newNode->m_previous = newNode->m_next = NULL;
  return newNode;
}

Node *insertionSort(Node *head) {
  if (head == NULL)
    return head;
  struct Node *sorted = NULL;
  struct Node *curr = head;

  // Traverse the list to sort each element
  while (curr != NULL) {

    // Store the next node to process
    struct Node *next = curr->m_next;

    // Insert `curr` into the sorted part
    if (sorted == NULL ||
        sorted->m_job->m_remainingTime >= curr->m_job->m_remainingTime) {
      curr->m_next = sorted;

      // If sorted is not empty, set its `prev`
      if (sorted != NULL)
        sorted->m_previous = curr;

      // Update sorted to the new head
      sorted = curr;
      sorted->m_previous = NULL;

    } else {

      // Pointer to traverse the sorted part
      struct Node *current_sorted = sorted;

      // Find the correct position to insert
      while (current_sorted->m_next != NULL &&
             current_sorted->m_next->m_job->m_remainingTime <
                 curr->m_job->m_remainingTime) {
        current_sorted = current_sorted->m_next;
      }

      // Insert `curr` after `current_sorted`
      curr->m_next = current_sorted->m_next;

      // Set `prev` if `curr` is not inserted
      // at the end
      if (current_sorted->m_next != NULL)
        current_sorted->m_next->m_previous = curr;

      // Set `next` of `current_sorted` to `curr`
      current_sorted->m_next = curr;
      curr->m_previous = current_sorted;
    }

    curr = next;
  }

  return sorted;
}

void append(Node **head, Job *job) {
  Node *newNode = createNode(job);

  if (*head == NULL) {
    *head = createNode(job);
    return;
  }

  Node *temp = *head;
  while (temp->m_next != NULL) {
    temp = temp->m_next;
  }

  temp->m_next = newNode;
  newNode->m_previous = temp;
}

void appendWithSort(Node **head, Job *job) {
  append(head, job);

  // sort by m_remainingTime
  // if we are NOT timeQueue
  *head = insertionSort(*head);
}

Node *popFront(Node **head) {
  if (head == NULL) {
    fprintf(stderr, "Can't popFront, no head!!\n");
    exit(1);
  }

  Node *temp = *head;
  *head = (*head)->m_next;

  if (*head != NULL) {
    (*head)->m_previous = NULL;
  }

  return temp;
}

Node *timeQueue = NULL;

Node *txtReadyQueue = NULL;
Node *fpReadyQueue = NULL;
Node *mmReadyQueue = NULL;

Node *finishedQueue = NULL;
Node *missedQueue = NULL;

////////////////////////////
/// PRINT FUNCTIONS
////////////////////////////

void printJob(const Job *job) {
  char priority[4];
  switch (job->m_priority) {
  case TXT:
    strcpy(priority, "TXT");
    break;

  case FP:
    strcpy(priority, "FP");
    break;

  case MM:
    strcpy(priority, "MM");
    break;
  }

  printf("%-8s | %-12d | %-6d | %-14d | %-8d | %-15d | %-8s\n", job->m_name,
         job->m_arrivalTime, job->m_length, job->m_remainingTime,
         job->m_deadline, job->m_completionTime, priority);
}

void printQueue(const Node *queue, const char *title) {
  printf("\n%s:\n", title);
  printf("Job Name | Arrival Time | Length | Remaining Time | Deadline | "
         "Completion Time | Priority\n");
  printf("---------------------------------------------------------------------"
         "--------------------\n");
  while (queue) {
    printJob(queue->m_job);
    queue = queue->m_next;
  }
}

void printReadyQueues() {
  printf("Next incoming job:\n");
  if (timeQueue) {
    printJob(timeQueue->m_job);
  } else {
    printf("None!\n");
  }

  if (txtReadyQueue) {
    printQueue(txtReadyQueue, "TXT Ready Queue");
  }

  if (fpReadyQueue) {
    printQueue(fpReadyQueue, "FP Ready Queue");
  }

  if (mmReadyQueue) {
    printQueue(mmReadyQueue, "MM Ready Queue");
  }
}

void printFinalQueues() {
  printQueue(finishedQueue, "Finished Queue");
  printQueue(missedQueue, "Missed Queue");
}

////////////////////////////
/// QUEUE FUNCTIONS
////////////////////////////

void readyUp(Job *job) {
  switch (job->m_priority) {
  case TXT: {
    appendWithSort(&txtReadyQueue, job);
    break;
  }

  case FP: {
    appendWithSort(&fpReadyQueue, job);
    break;
  }

  case MM: {
    appendWithSort(&mmReadyQueue, job);
    break;
  }
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
    append(&timeQueue, createJob(name, arrival, length, deadline, priority));
  }
  printQueue(timeQueue, "timeQueue");

  fclose(file);
}

Job *popShortestJobWithPriority() {
  if (txtReadyQueue) {
    return popFront(&txtReadyQueue)->m_job;
  } else if (fpReadyQueue != NULL) {
    return popFront(&fpReadyQueue)->m_job;
  } else if (mmReadyQueue != NULL) {
    return popFront(&mmReadyQueue)->m_job;
  } else {
    return NULL;
  }
}

int queuesEmpty() {
  return (!txtReadyQueue && !mmReadyQueue && !fpReadyQueue && !timeQueue);
}

void spin() {
  // find shortest job in each queue, do it, and then put it in the finished
  // print all queues every 500 ms
  int currentTime = timeQueue->m_job->m_arrivalTime - 10;

  while (1) {
    system("clear");
    printf("Current Time: %d\n", currentTime);
    printReadyQueues();

    if (timeQueue) {
      if (timeQueue->m_job->m_arrivalTime <= currentTime) {
        // this queues up the front of the time queue into it's appropriate
        // queue
        readyUp(popFront(&timeQueue)->m_job);
      }
    } else {
      if (queuesEmpty()) {
        break;
      }
    }

    // select shortest time
    Job *job = popShortestJobWithPriority();
    if (job == NULL) {
      if (queuesEmpty()) {
        printf("Finished!\n");
        break;
      } else {
        currentTime++;
        continue;
      }
    }

    if (job->m_deadline < currentTime) {
      append(&missedQueue, job);
      continue;
    }

    // execute process
    job->m_remainingTime--;
    currentTime++;

    if (job->m_remainingTime == 0) {
      // we are done. add it to finishedQueue
      job->m_completionTime = currentTime;
      append(&finishedQueue, job);

    } else {
      // push it back into it's appropriate queue.
      readyUp(job);
    }

    // weeee im wasting time!!
    for (int i = 0; i < 20000; i++)
      ;
  }
}

int main() {
  loadJobs("jobs.txt"); // Load jobs from file
  // we start the scheduling process
  // jobs must be done in priority, and then inserted into the finished queue
  // when done print every 500 ms

  spin();
  system("clear");
  printf("Finished!\n");
  printFinalQueues();
  return 0;
}
