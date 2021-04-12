#ifndef LOCKFREEQUEUE_H
#define LOCKFREEQUEUE_H

#include <atomic>
#include <vector>
// 多读多写 
// read1 read2 -- 用一个原子readIndex_去避免冲突
// write1 write2 -- writeIndex_ 
// read和write 之间避免冲突 -- 定义是否是满的，如果有数据才能读
template <typename T, size_t N = 1024>
class LockFreeQueue {
public:
    struct Element {
        std::atomic<bool> full_;
        T data_;
    };
    LockFreeQueue()
        : data_(N),
        readIndex_(0),
        writeIndex_(0) {
    }
    bool Enqueue(T value) {
        // 这里使用memory_order_relaxed是因为宽松更容易优化，下一句又是依赖这一句，不可能放在这一排之前
        size_t writeIndex = writeIndex_.load(std::memory_order_relaxed);
        Element* e = nullptr;
        do {
            if(writeIndex >= readIndex_.load(std::memory_order_relaxed) + N) 
                return false;
            size_t index = writeIndex % N;
            e = &data_[index];
            if(e->full_.load(std::memory_order_relaxed)) return false;
        } while(!writeIndex_.compare_exchange_weak(writeIndex, writeIndex+1, 
                            std::memory_order_release, std::memory_order_relaxed));
                            
        e->data_ = std::move(value);
        e->full_.store(true, std::memory_order_release);
        return true;

    }
    bool Dequeue(T& value) {
        size_t readIndex = 0;
        Element* e = nullptr;
        do {
            readIndex  = readIndex_.load(std::memory_order_relaxed);
            if(readIndex >= writeIndex_.load(std::memory_order_relaxed)) {
                return false;
            }
            size_t index = readIndex % N;
            e = &data_[index];
            if(!e->full_.load(std::memory_order_relaxed)) {
                return false;
            }
        } while(!readIndex_.compare_exchange_weak(readIndex, readIndex+1, 
                            std::memory_order_release, std::memory_order_relaxed));
        value = std::move(e->data_);
        e->full_.store(false, std::memory_order_release);
        return true;
    }
private:
    std::vector<Element> data_;
    std::atomic<size_t> readIndex_;
    std::atomic<size_t> writeIndex_;
};

#endif // LOCKFREEQUEUE_H