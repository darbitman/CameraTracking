#include "ResizablePriorityQueue.h"
#include "ConstSizeMinBinaryHeap.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <queue>

using namespace std;
using namespace std::chrono;
using std::priority_queue;
using std::vector;

typedef priority_queue<int32_t, vector<int32_t>, std::greater<int32_t> > MinPQ;

int main() {
  MinPQ vpq;
  int32_t entries = 50;
  ct::ResizablePriorityQueue<int32_t, vector<int32_t>, std::greater<int32_t> > rpq(entries);


  auto start = high_resolution_clock::now();
  auto stop = high_resolution_clock::now();
  auto duration = duration_cast<microseconds>(stop - start);


  start = high_resolution_clock::now();
  for (int32_t i = 0; i < entries; i++) {
    vpq.push(i);
  }
  stop = high_resolution_clock::now();
  duration = duration_cast<microseconds>(stop - start);


  start = high_resolution_clock::now();
  for (int32_t i = 0; i < entries; i++) {
    rpq.push(i);
  }
  stop = high_resolution_clock::now();
  duration = duration_cast<microseconds>(stop - start);

  start = high_resolution_clock::now();
  ConstSizeMinBinaryHeap<int32_t> csmbp(entries);
  
  for (int32_t i = 0; i < entries; i++) {
    csmbp.push(i);
  }
  stop = high_resolution_clock::now();
  duration = duration_cast<microseconds>(stop - start);

  return 0;
}