#include <data_buffer.hpp>

void DataBuffer::add_new_data(buffer_len data)
{
    // Append new data to the end of the buffer
    buffer.insert(buffer.end(), data.first, data.first + data.second);
}

void DataBuffer::did_consume_prefix(buffer_len newPointer)
{
    // Calculate the offset based on the new pointer
    size_t offset = newPointer.first - buffer.data();
    if (offset == 0)
        return;

    // If all data has been consumed, clear the buffer
    if (offset >= buffer.size())
    {
        buffer.clear();
        return;
    }

    // Shift the remaining data towards the beginning
    std::copy(buffer.begin() + offset, buffer.end(), buffer.begin());
    buffer.resize(buffer.size() - offset); // Shrink the buffer

    // Optionally shrink the vector's capacity to fit its size
    // to avoid holding unnecessary memory
    std::vector<uint8_t>(buffer).swap(buffer);
}

buffer_len DataBuffer::get_current_data() const
{
    return {buffer.data(), buffer.size()};
}
