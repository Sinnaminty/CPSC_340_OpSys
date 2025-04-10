#include <stdio.h>
#include <stdlib.h>

#define PS1_SIZE 13
#define PS2_SIZE 33
#define PS3_SIZE 20
#define PS4_SIZE 20

const int ps1[PS1_SIZE] = {7, 0, 1, 2, 0, 3, 0, 4, 2, 3, 0, 3, 2};
const int ps2[PS2_SIZE] = {1, 0, 2, 2, 1, 7, 6, 7, 0, 1, 2, 0, 3, 0, 4, 5, 1,
                           5, 2, 4, 5, 6, 7, 6, 7, 2, 4, 2, 7, 3, 3, 2, 3};
const int ps3[PS3_SIZE] = {7, 0, 1, 2, 0, 3, 0, 4, 2, 3,
                           0, 3, 2, 1, 2, 0, 1, 7, 0, 1};
const int ps4[PS4_SIZE] = {1, 2, 3, 4, 2, 1, 5, 6, 2, 1,
                           2, 3, 7, 6, 3, 2, 1, 2, 3, 6};

typedef struct Node {
  int m_page;
  struct Node *m_prev;
  struct Node *m_next;
} Node;

Node *head = NULL;
Node *tail = NULL;

int frameCount = 3; /*the size of our Page table*/
int size = 0;

// search and move the page if it exists
int accessPage(int page) {
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
      return 1; /* page found!*/
    }
    curr = curr->m_next;
  }
  return 0; /* page not found.. :c*/
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

void simulateLRU(const int pages[], const int n) {
  int pageFaults = 0;
  for (int i = 0; i < n; i++) {
    system("clear");
    int page = pages[i];
    printf("Accessing page %d: ", page);
    if (!accessPage(page)) {
      pageFaults++;
      if (size == frameCount) {
        evictLRU();
      }
      insertPage(page);
      printf("Page Fault\n");
    } else {
      printf("Page hit!\n");
    }
    printMemory();
    for (int wait = 0; wait < 100000000; wait++)
      ;
  }
  printf("Total Page Faults: %d\n", pageFaults);
}

int main() {
  frameCount = 3;
  printf("3 Frame LRU\n");
  simulateLRU(ps1, PS1_SIZE);
  return 0;
}
