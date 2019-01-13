#include "ResizablePriorityQueue.h"
#include "ConstSizeMinBinaryHeap.h"
#include <gtest/gtest.h>

using std::priority_queue;
using std::vector;

typedef ct::ResizablePriorityQueue<int32_t, std::greater<int32_t> > MinPQ;

struct ResizeablePriorityQueueTest : testing::Test
{
  MinPQ* pq;

  ResizeablePriorityQueueTest()
  {
    pq = new MinPQ();
  }

  virtual ~ResizeablePriorityQueueTest()
  {
    delete pq;
  }
};

TEST_F(ResizeablePriorityQueueTest, ContainerSize)
{
  // initial capacity should be 0
  EXPECT_EQ(0, pq->GetContainerCapacity());
  
  // reserve memory for NewCapacity elements
  int32_t NewCapacity = 100;
  EXPECT_EQ(true, pq->SetNonzeroCapacity(NewCapacity));
  EXPECT_EQ(NewCapacity, pq->GetContainerCapacity());

  // number of elements should be NewCapacity
  // container capacity should be NewCapacity
  for (int32_t n = 0; n < NewCapacity; n++)
  {
    pq->push(n);
  }
  EXPECT_EQ(pq->size(), NewCapacity);
  EXPECT_EQ(pq->GetContainerCapacity(), NewCapacity);

  // number of elements should increase by 1
  // container capacity should AT LEAST increase by 1
  pq->push(0);
  EXPECT_EQ(pq->size(), NewCapacity + 1);
  EXPECT_GE(pq->GetContainerCapacity(), NewCapacity + 1);

  // remove all elements from the queue, so size should be 0
  // capacity shall not change
  while (pq->size() > 0)
  {
    pq->pop();
  }
  EXPECT_EQ(pq->size(), 0);
  EXPECT_GE(pq->GetContainerCapacity(), NewCapacity + 1);
}

int main(int argc, char* argv[]) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
