#ifndef PARSE_REDIS_MSG_HPP
#define PARSE_REDIS_MSG_HPP

#include <string>
#include <cstdint>
#include <variant>
#include <optional>
#include <vector>

using buffer_len = std::pair<const uint8_t *, uint16_t>;
using redis_value = std::variant<int, std::string, std::vector<std::variant<int, std::string>>>;
std::pair<std::optional<redis_value>, buffer_len> parse_redis_msg(const buffer_len buffer_in);

#endif
