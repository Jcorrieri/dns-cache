#ifndef JC_PCQ_RINGBUF
#define JC_PCQ_RINGBUF

#include <array>
#include <condition_variable>
#include <cstddef>
#include <optional>

// Enqueue at the back, dequeue at the front.
template<typename T>
class RequestQueue {
public:
    const std::size_t m_capacity;

    RequestQueue(std::size_t capacity) : m_capacity{capacity} {
        m_queue.reserve(capacity);
    };

    bool produce(T value);

    T consume();

private:
    bool enqueue(T value);

    std::optional<T> dequeue();

    std::size_t size() const;

    bool empty() const;

    std::mutex m_mutex;
    std::condition_variable m_queue_has_space;
    std::condition_variable m_queue_not_empty;
    std::vector<T> m_queue;
    std::size_t m_front{0};
    std::size_t m_back{0};
    std::size_t m_size{0};
};

template<typename T>
bool RequestQueue<T>::produce(T value) {
    {
        std::unique_lock lock{m_mutex};
        m_queue_has_space.wait(lock, [this] {
            return size() < m_capacity;
        });

        if (!enqueue(value)) {
            return false;
        }
    }

    m_queue_not_empty.notify_one();

    return true;
}

template<typename T>
T RequestQueue<T>::consume() {
    T item;
    {
        std::unique_lock lock{m_mutex};
        m_queue_not_empty.wait(lock, [this] {
            return !empty();
        });

        item = *dequeue();
    }

    m_queue_has_space.notify_one();

    return item;
}

template<typename T>
bool RequestQueue<T>::enqueue(T value) {
    if (m_size >= m_capacity) {
        return false;
    }

    m_queue[m_back] = value;
    m_back = (m_back + 1) % m_capacity;
    ++m_size;

    return true;
}

template<typename T>
std::optional<T> RequestQueue<T>::dequeue() {
    if (m_size == 0) {
        return std::nullopt;
    }

    const T ret_val{m_queue[m_front]};
    m_front = (m_front + 1) % m_capacity;
    --m_size;

    return ret_val;
}

template<typename T>
std::size_t RequestQueue<T>::size() const {
    return m_size;
}

template<typename T>
bool RequestQueue<T>::empty() const {
    return m_size == 0;
}

#endif
