#ifndef DATA_BUFFER_HPP
#define DATA_BUFFER_HPP

#include <iostream>
#include <vector>
#include <cstdint>
#include <algorithm> // For std::copy
#include "parse_redis_msg.hpp"

class DataBuffer
{
private:
    std::vector<uint8_t> buffer = std::vector<uint8_t>();

public:
    void add_new_data(buffer_len);
    void did_consume_prefix(buffer_len);
    buffer_len get_current_data() const;
};
#endif
