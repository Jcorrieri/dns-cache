#ifndef JC_PCQ_RINGBUF
#define JC_PCQ_RINGBUF

#include <array>
#include <condition_variable>
#include <cstddef>
#include <mutex>
#include <optional>

// Enqueue at the back, dequeue at the front.
class RequestQueue {
public:
    static constexpr std::size_t s_max_size{10};

    bool produce(int value, int thread_id);

    int consume(int thread_id);

private:
    bool enqueue(int value);

    std::optional<int> dequeue();

    std::size_t size() const;

    bool empty() const;

    std::mutex m_mutex;
    std::condition_variable m_queue_has_space;
    std::condition_variable m_queue_not_empty;
    std::array<int, s_max_size> m_queue{};
    std::size_t m_front{0};
    std::size_t m_back{0};
    std::size_t m_size{0};
};

#endif
