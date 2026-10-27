#ifndef JC_BP_WORKER
#define JC_BP_WORKER

#include "cache.h"
#include "producer_consumer_queue.h"

void spawn_worker(int id, RequestQueue& queue, KVCache& cache);

#endif

