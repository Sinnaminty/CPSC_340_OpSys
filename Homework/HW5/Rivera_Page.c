#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Sizes for predefined page streams
#define PS1_SIZE 13
#define PS2_SIZE 33
#define PS3_SIZE 20
#define PS4_SIZE 20

// Time delay for simulating processing (not functional, just slows output)
#define WAIT_TIME 100000000

// Constants for simulation
#define MAX_PAGES 4   // Max pages that can fit in memory
#define MAX_FRAMES 50 // Max number of frames in the table

// Predefined page request streams (used for testing)
const int ps1[PS1_SIZE] = {7, 0, 1, 2, 0, 3, 0, 4, 2, 3, 0, 3, 2};
const int ps2[PS2_SIZE] = {1, 0, 2, 2, 1, 7, 6, 7, 0, 1, 2, 0, 3, 0, 4, 5, 1,
                           5, 2, 4, 5, 6, 7, 6, 7, 2, 4, 2, 7, 3, 3, 2, 3};
const int ps3[PS3_SIZE] = {7, 0, 1, 2, 0, 3, 0, 4, 2, 3,
                           0, 3, 2, 1, 2, 0, 1, 7, 0, 1};
const int ps4[PS4_SIZE] = {1, 2, 3, 4, 2, 1, 5, 6, 2, 1,
                           2, 3, 7, 6, 3, 2, 1, 2, 3, 6};

/* Doubly linked list node used to simulate memory */
typedef struct Node {
  int m_page;          // Page number
  struct Node *m_prev; // Previous node in list
  struct Node *m_next; // Next node in list
} Node;

/* Frame stores current memory snapshot and whether a fault occurred */
typedef struct Frame {
  bool m_fault;           // Did a fault occur at this step?
  int m_frame[MAX_PAGES]; // Frame contents (up to MAX_PAGES pages)
} Frame;

/* Table stores entire simulation history */
typedef struct Table {
  int m_frameSize;           // Number of pages per frame
  int m_tableSize;           // Number of frames
  int m_head;                // Index of last frame inserted
  Frame m_table[MAX_FRAMES]; // Array of frames
} Table;

// Pointers to the front and back of the linked list
Node *head = NULL;
Node *tail = NULL;

// Current memory usage size (number of nodes in the list)
int size = 0;

/* Allocates and initializes a new table for the simulation */
Table *createTable(const int tableSize, const int frameSize) {
  Table *newTable = (Table *)malloc(sizeof(struct Table));
  newTable->m_tableSize = tableSize;
  newTable->m_frameSize = frameSize;
  newTable->m_head = -1;
  return newTable;
}

/* Adds a frame to the table, checking bounds */
void addFrame(Table *table, const Frame frame) {
  if (table->m_head < table->m_tableSize) {
    table->m_table[++table->m_head] = frame;
  } else {
    fprintf(stderr, "Head past max table size!\n");
    exit(1);
  }
}

/* Captures current memory state as a Frame, marking faults */
Frame captureState(const bool fault) {
  Frame frame;
  Node *curr = head;
  frame.m_fault = fault;

  for (int i = 0; i < MAX_PAGES; i++) {
    frame.m_frame[i] = (curr) ? curr->m_page : -1;
    if (curr)
      curr = curr->m_next;
  }
  return frame;
}

/* Tries to access a page in memory (linked list).
   If found, move it to the front (LRU policy).
   Returns true if page hit, false if page fault. */
bool accessPage(int page) {
  Node *curr = head;
  while (curr) {
    if (curr->m_page == page) {
      // Page hit: move to front
      if (curr != head) {
        if (curr->m_next)
          curr->m_next->m_prev = curr->m_prev;
        else
          tail = curr->m_prev;

        if (curr->m_prev)
          curr->m_prev->m_next = curr->m_next;

        curr->m_next = head;
        curr->m_prev = NULL;
        head->m_prev = curr;
        head = curr;
      }
      return true;
    }
    curr = curr->m_next;
  }
  return false;
}

/* Inserts a page to the front of the linked list */
void insertPage(int page) {
  Node *newNode = (Node *)malloc(sizeof(Node));
  newNode->m_page = page;
  newNode->m_prev = NULL;
  newNode->m_next = head;

  if (head)
    head->m_prev = newNode;
  head = newNode;

  if (!tail)
    tail = newNode;

  size++;
}

/* Evicts least recently used page (tail of the list) */
void evictLRU() {
  if (!tail)
    return;
  Node *oldTail = tail;
  if (tail->m_prev) {
    tail = tail->m_prev;
    tail->m_next = NULL;
  } else {
    head = tail = NULL;
  }
  free(oldTail);
  size--;
}

/* Displays the table with all memory states during simulation */
void printTable(const Table *table, const int pages[], const int n) {
  printf("\n--- Page Table (Matrix View) ---\n");
  printf("Page   :");
  for (int i = 0; i < n; i++)
    printf("%3d", pages[i]);
  printf("\n------------------------------------------------------\n");

  for (int i = 0; i < table->m_frameSize; i++) {
    printf("Frame %d:", i);
    for (int j = 0; j < table->m_tableSize; j++) {
      int val = table->m_table[j].m_frame[i];
      printf(val == -1 ? "  -" : "%3d", val);
    }
    printf("\n");
  }

  // Indicate which accesses caused faults
  printf("Faults :");
  for (int j = 0; j < table->m_tableSize; j++)
    printf("  %c", table->m_table[j].m_fault ? ' ' : '*');
  printf("\n");
}

/* Print current memory layout (linked list view) */
void printMemory() {
  Node *curr = head;
  printf("-----------------------\nMemory:\n");
  while (curr) {
    printf("%d\n", curr->m_page);
    curr = curr->m_next;
  }
  printf("-----------------------\n");
}

/* Simulates LRU page replacement using linked list */
void simulateLRU(const int pages[], const int n, const int frameSize) {
  int pageFaults = 0;
  Table *table = createTable(n, frameSize);

  for (int i = 0; i < n; i++) {
    system("clear");
    int page = pages[i];
    printf("Accessing page %d: ", page);
    bool accessed = accessPage(page);

    if (!accessed) {
      pageFaults++;
      if (size == frameSize)
        evictLRU();
      insertPage(page);
      printf("Page fault!\n");
    } else {
      printf("Page hit!\n");
    }

    addFrame(table, captureState(accessed));
    printMemory();
    for (int wait = 0; wait < WAIT_TIME; wait++)
      ;
  }

  printTable(table, pages, n);
  printf("Total Page Faults: %d\n", pageFaults);
}

/* Simulates Clock page replacement algorithm */
void simulateClock(const int pages[], const int n, const int frameSize) {
  int pageFaults = 0;
  int *frame = (int *)malloc(sizeof(int) * frameSize);
  bool *reference = (bool *)malloc(sizeof(bool) * frameSize);
  int hand = 0; // Clock hand

  // Initialize frame and reference bits
  for (int i = 0; i < frameSize; i++) {
    frame[i] = -1;
    reference[i] = false;
  }

  Table *table = createTable(n, frameSize);

  for (int i = 0; i < n; i++) {
    system("clear");
    int page = pages[i];
    bool hit = false;

    // Check for page hit
    for (int j = 0; j < frameSize; j++) {
      if (frame[j] == page) {
        reference[j] = true;
        hit = true;
        break;
      }
    }

    // Page fault handling using Clock algorithm
    if (!hit) {
      pageFaults++;
      while (true) {
        if (!reference[hand]) {
          frame[hand] = page;
          reference[hand] = true;
          hand = (hand + 1) % frameSize;
          break;
        } else {
          reference[hand] = false;
          hand = (hand + 1) % frameSize;
        }
      }
    }

    // Sync linked list with clock frame
    while (head)
      evictLRU();
    for (int j = frameSize - 1; j >= 0; j--) {
      if (frame[j] != -1)
        insertPage(frame[j]);
    }

    addFrame(table, captureState(hit));
    printf("Accessing page %d: %s\n", page, hit ? "Page hit!" : "Page fault!");
    printMemory();
    for (int wait = 0; wait < WAIT_TIME; wait++)
      ;
  }

  printTable(table, pages, n);
  printf("Total Page Faults: %d\n", pageFaults);

  free(frame);
  free(reference);
}

/* Prints program usage instructions */
void printHelp() {
  printf("------------------------------------------------\n");
  printf("ASSIGNMENT 5 - LRU PAGE REPLACEMENT USING STACK\n");
  printf("USAGE:\n");
  printf("./page (Page Stream) (Frame Size) (Algorithm)\n");
}

/* Main entry: parses input arguments and runs simulation */
int main(int argc, char *argv[]) {
  if (argc != 4) {
    printHelp();
    exit(1);
  }

  int dataset = atoi(argv[1]);
  const int *pages = NULL;
  int pageCount = 0;

  // Load selected page stream
  switch (dataset) {
  case 1:
    pages = ps1;
    pageCount = PS1_SIZE;
    break;
  case 2:
    pages = ps2;
    pageCount = PS2_SIZE;
    break;
  case 3:
    pages = ps3;
    pageCount = PS3_SIZE;
    break;
  case 4:
    pages = ps4;
    pageCount = PS4_SIZE;
    break;
  default:
    printf("Invalid dataset number. Select 1-4.\n");
    printHelp();
    exit(1);
  }

  int frameSize = atoi(argv[2]);
  if (frameSize != 3 && frameSize != 4) {
    printf("Invalid frame size. Must be either 3 or 4.");
    printHelp();
    exit(1);
  }

  // Run selected algorithm
  if (strcmp(argv[3], "LRU") == 0) {
    simulateLRU(pages, pageCount, frameSize);
  } else if (strcmp(argv[3], "Clock") == 0) {
    simulateClock(pages, pageCount, frameSize);
  } else {
    printf("Invalid Algorithm.\n");
    printHelp();
    exit(1);
  }

  return 0;
}
