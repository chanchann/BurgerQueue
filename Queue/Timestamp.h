
#ifndef TIMESTAMP_H
#define TIMESTAMP_H

#include "copyable.h"
#include <iostream>
#include <boost/operators.hpp>
#include <chrono>
#include <string>
#include <sstream>
#include <iomanip> // put_time

using namespace std::chrono;

class Timestamp : public copyable,   
                  public boost::less_than_comparable<Timestamp> {    // 只需要实现<，自动实现>,>=,<=
public:
    Timestamp();
    explicit Timestamp(uint64_t microSecondsSinceEpoch);
    void swap(Timestamp& that);
    std::string toString() const;
    std::string toFormatTime() const;
    uint64_t microSecondsSinceEpoch() const;
    bool valid() const { return microSecondsSinceEpoch_ > 0; }

    static Timestamp now();   
    static Timestamp invalid() { return Timestamp(); }
    static const int kMicroSecondsPerSecond = 1000 * 1000;
private:
    uint64_t microSecondsSinceEpoch_;
};

inline bool operator<(Timestamp lhs, Timestamp rhs) {
    return lhs.microSecondsSinceEpoch() < rhs.microSecondsSinceEpoch();
}


inline double timeDifference(Timestamp high, Timestamp low) {
    uint64_t diff = high.microSecondsSinceEpoch() - low.microSecondsSinceEpoch();
    return static_cast<double>(diff) / Timestamp::kMicroSecondsPerSecond;    // 结果/（1000*1000），返回的是秒数
}

// 时间戳加上秒数生成新的时间戳
inline Timestamp addTime(Timestamp timestamp, double seconds) {
    uint64_t delta = static_cast<uint64_t>(seconds * Timestamp::kMicroSecondsPerSecond);
    return Timestamp(timestamp.microSecondsSinceEpoch() + delta);
}

#endif // TIMESTAMP_H