#include "ResizablePriorityQueue.h"
#include "ConstSizeMinBinaryHeap.h"
#include <gtest/gtest.h>

using std::priority_queue;
using std::vector;

typedef ct::ResizablePriorityQueue<int32_t, std::greater<int32_t>> MinPQ;

struct ResizablePriorityQueueTest : testing::Test
{
  MinPQ* pq;

  ResizablePriorityQueueTest()
  {
    pq = new MinPQ();
  }

  virtual ~ResizablePriorityQueueTest()
  {
    delete pq;
  }
};

TEST_F(ResizablePriorityQueueTest, ContainerSize)
{
  // initial capacity should be 0
  EXPECT_EQ(0, pq->GetContainerCapacity());
  
  // reserve memory for NewCapacity elements
  int32_t NewCapacity = 100;
  EXPECT_EQ(true, pq->SetCapacity(NewCapacity));
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

TEST(ResizableMinPriorityQueueTest, InitialNonZeroCapacity)
{
  // create a new Min Oriented Priority Queue with space for NewCapacity elements
  int32_t NewCapacity = 100;
  MinPQ* pq = new MinPQ(NewCapacity);
  EXPECT_EQ(pq->size(), 0);
  EXPECT_EQ(pq->GetContainerCapacity(), NewCapacity);

  // number of elements should be NewCapacity
  // container capacity should be NewCapacity
  for (int32_t n = 0; n < NewCapacity; n++)
  {
    pq->push(NewCapacity - n);
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
  // all elements added were 0 thru 100 in descending order
  // elements removed should be 0 thru 100 in ascending order
  // the current top should be greater than the previous top
  {
    int32_t top = pq->top();
    while (pq->size() > 0)
    {
      pq->pop();
      if (pq->size())
      {
        EXPECT_GE(pq->top(), top);
        top = pq->top();
      }
    }
  }
  EXPECT_EQ(pq->size(), 0);
  EXPECT_GE(pq->GetContainerCapacity(), NewCapacity + 1);

  delete pq;
}


int main(int argc, char* argv[]) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
