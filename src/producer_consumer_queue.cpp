#include <format>
#include <cstddef>
#include <iostream>
#include "producer_consumer_queue.h"

bool RequestQueue::produce(int value, int thread_id) {
    {
        std::unique_lock lock{m_mutex};
        m_queue_has_space.wait(lock, [this] {
            return size() < s_max_size;
        });

        if (!enqueue(value)) {
            return false;
        }

        std::cout << std::format("[LOG] producer {} added {} to the queue.\n", thread_id, value);

        if (size() == s_max_size) {
            std::cout << "[LOG] queue is full.\n";
        }
    }

    m_queue_not_empty.notify_one();

    return true;
}

int RequestQueue::consume(int thread_id) {
    int item;
    {
        std::unique_lock lock{m_mutex};
        m_queue_not_empty.wait(lock, [this] {
            return !empty();
        });

        item = *dequeue();

        std::cout << std::format("[LOG] consumer {} consumed {} from the queue.\n", thread_id, item);

        if (empty()) {
            std::cout << "[LOG] Queue is empty\n";
        }
    }

    m_queue_has_space.notify_one();

    return item;
}

bool RequestQueue::enqueue(int value) {
    if (m_size >= s_max_size) {
        return false;
    }

    m_queue[m_back] = value;
    m_back = (m_back + 1) % s_max_size;
    ++m_size;

    return true;
}

std::optional<int> RequestQueue::dequeue() {
    if (m_size == 0) {
        return std::nullopt;
    }

    const int ret_val{m_queue[m_front]};
    m_front = (m_front + 1) % s_max_size;
    --m_size;

    return ret_val;
}

std::size_t RequestQueue::size() const {
    return m_size;
}

bool RequestQueue::empty() const {
    return m_size == 0;
}
