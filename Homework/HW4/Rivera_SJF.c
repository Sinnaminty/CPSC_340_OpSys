/*
 * @struct Job
 * @param m_jobName
 * @param m_arrivalTime
 * @param m_jobLength
 * @param m_deadline
 * @param m_ioTime
 */
#include <stdint.h>
typedef struct Job {
  char *m_jobName;
  uint16_t m_arrivalTime;
  uint16_t m_jobLength;
  uint16_t m_deadline;
  uint16_t m_ioTime;
} job;

int main() { return 0; }
