#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PS1_SIZE 13
#define PS2_SIZE 33
#define PS3_SIZE 20
#define PS4_SIZE 20
#define WAIT_TIME 100000000
#define MAX_PAGES 4
#define MAX_FRAMES 50

const int ps1[PS1_SIZE] = {7, 0, 1, 2, 0, 3, 0, 4, 2, 3, 0, 3, 2};
const int ps2[PS2_SIZE] = {1, 0, 2, 2, 1, 7, 6, 7, 0, 1, 2, 0, 3, 0, 4, 5, 1,
                           5, 2, 4, 5, 6, 7, 6, 7, 2, 4, 2, 7, 3, 3, 2, 3};
const int ps3[PS3_SIZE] = {7, 0, 1, 2, 0, 3, 0, 4, 2, 3,
                           0, 3, 2, 1, 2, 0, 1, 7, 0, 1};
const int ps4[PS4_SIZE] = {1, 2, 3, 4, 2, 1, 5, 6, 2, 1,
                           2, 3, 7, 6, 3, 2, 1, 2, 3, 6};

/* DLL for our Pages*/
typedef struct Node {
  int m_page;
  struct Node *m_prev;
  struct Node *m_next;
} Node;

/* Holds the state of exactly one frame*/
typedef struct Frame {
  bool m_fault; /* did this frame have a fault?*/
  int m_frame[MAX_PAGES];
} Frame;

typedef struct Table {
  int m_frameSize; /* how many pages are in this frame?*/
  int m_tableSize; /* how many frames in this table? */
  int m_head;      /*top of table*/
  Frame m_table[MAX_FRAMES];
} Table;

Node *head = NULL;
Node *tail = NULL;

int size = 0;

Table *createTable(const int tableSize, const int frameSize) {
  Table *newTable = (Table *)malloc(sizeof(struct Table));
  newTable->m_tableSize = tableSize;
  newTable->m_frameSize = frameSize;
  newTable->m_head = -1;
  return newTable;
}

void addFrame(Table *table, const Frame frame) {
  if (table->m_head < table->m_tableSize) {
    table->m_table[++table->m_head] = frame;
  } else {
    fprintf(stderr, "Head past max table size!\n");
    fprintf(stderr, "head: %d | tableSize: %d", table->m_head,
            table->m_tableSize);
    exit(1);
  }
}

Frame captureState(const bool fault) {
  Frame frame;
  Node *curr = head;
  frame.m_fault = fault;

  for (int i = 0; i < MAX_PAGES; i++) {
    if (curr) {
      frame.m_frame[i] = curr->m_page;
      curr = curr->m_next;
    } else {
      frame.m_frame[i] = -1;
    }
  }
  return frame;
}

// search and move the page if it exists
bool accessPage(int page) {
  Node *curr = head;
  while (curr) {
    if (curr->m_page == page) {
      /* move the page to the front*/
      if (curr != head) { /* ..ofc if we're not at the front already :3c*/
        // unlink
        if (curr->m_next)
          curr->m_next->m_prev = curr->m_prev;
        else
          tail = curr->m_prev; /* if it was tail*/
        if (curr->m_prev)
          curr->m_prev->m_next = curr->m_next;

        // move that guy to the front!!!
        curr->m_next = head;
        curr->m_prev = NULL;
        head->m_prev = curr;
        head = curr;
      }
      return true; /* page found!*/
    }
    curr = curr->m_next;
  }
  return false; /* page not found.. :c*/
}

// add page to front
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

// remove the LRU page (tailpop)
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

void printTable(const Table *table, const int pages[], const int n) {
  printf("\n--- Page Table (Matrix View) ---\n");
  printf("Page   :");
  for (int i = 0; i < n; i++) {
    printf("%3d", pages[i]);
  }
  printf("\n------------------------------------------------------\n");

  for (int i = 0; i < table->m_frameSize; i++) {
    printf("Frame %d:", i);
    for (int j = 0; j < table->m_tableSize; j++) {
      int val = table->m_table[j].m_frame[i];
      if (val == -1) {
        printf("  -");
      } else {
        printf("%3d", val);
      }
    }
    printf("\n");
  }

  // Optional: print fault row
  printf("Faults :");
  for (int j = 0; j < table->m_tableSize; j++) {
    printf("  %c", table->m_table[j].m_fault ? ' ' : '*');
  }
  printf("\n");
}

void printMemory() {
  Node *curr = head;
  printf("-----------------------\n");
  printf("Memory:\n");
  while (curr) {
    printf("%d\n", curr->m_page);
    curr = curr->m_next;
  }
  printf("-----------------------\n");
}

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
      if (size == frameSize) {
        evictLRU();
      }
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

void simulateClock(const int pages[], const int n, const int frameSize) {
  int pageFaults = 0;
  int *frame = (int *)malloc(sizeof(int) * frameSize);
  bool *reference = (bool *)malloc(sizeof(bool) * frameSize);
  int hand = 0;

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

    // Update linked list from scratch (to reuse captureState)
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

void printHelp() {
  printf("------------------------------------------------\n");
  printf("ASSIGNMENT 5 - LRU PAGE REPLACEMENT USING STACK\n");
  printf("USAGE:\n");
  printf("./page (Page Stream) (Frame Size) (Algorithm)\n");
}

int main(int argc, char *argv[]) {
  // argv[1] == ps (1, 2, 3 or 4)
  // argv[2] == franeSize (3 or 4)
  // argv[3] == algo (LRU or Clock)
  if (argc != 4) {
    printHelp();
    exit(1);
  }

  int dataset = atoi(argv[1]);
  const int *pages = NULL;
  int pageCount = 0;

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

  // algo
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
