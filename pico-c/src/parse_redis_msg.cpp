#include <parse_redis_msg.hpp>

std::pair<std::optional<redis_value>, buffer_len> parse_redis_msg(const buffer_len buffer_in)
{
    const auto data = buffer_in.first;
    const auto data_len = buffer_in.second;
    if (data == nullptr || data_len <= 0)
    {
        return {std::nullopt, {nullptr, 0}};
    }

    std::string dataStr(reinterpret_cast<const char *>(data), data_len);

    if (dataStr[0] == ':')
    {
        size_t pos = dataStr.find("\r\n");
        if (pos == std::string::npos)
        {
            return {std::nullopt, buffer_in};
        }
        std::string parsed = dataStr.substr(1, pos - 1);
        return {std::stoi(parsed), {data + pos + 2, data_len - pos - 2}};
    }

    if (dataStr[0] == '-' || dataStr[0] == '+')
    {
        size_t pos = dataStr.find("\r\n");
        if (pos == std::string::npos)
        {
            return {std::nullopt, buffer_in};
        }
        std::string parsed = dataStr.substr(1, pos - 1);
        return {parsed, {data + pos + 2, data_len - pos - 2}};
    }

    if (dataStr[0] == '$')
    {
        size_t pos = dataStr.find("\r\n");
        if (pos == std::string::npos)
        {
            return {std::nullopt, buffer_in};
        }
        int num = std::stoi(dataStr.substr(1, pos - 1));
        std::string after = dataStr.substr(pos + 2);
        if (after.length() < static_cast<size_t>(num) + 2)
        {
            return {std::nullopt, buffer_in};
        }
        return {after.substr(0, num), {data + pos + num + 4, data_len - pos - num - 4}};
    }
    if (dataStr[0] == '*')
    {
        size_t pos = dataStr.find("\r\n");
        if (pos == std::string::npos)
        {
            return {std::nullopt, buffer_in};
        }
        int num = std::stoi(dataStr.substr(1, pos - 1));
        buffer_len remainder = {data + pos + 2, data_len - pos - 2};
        std::vector<std::variant<int, std::string>> arr;
        for (int i = 0; i < num; ++i)
        {
            auto [chunk, remainder_] = parse_redis_msg(remainder);
            remainder = remainder_;
            if (!chunk)
            {
                continue;
            }
            if (std::holds_alternative<std::vector<std::variant<int, std::string>>>(*chunk))
            {
                // If the chunk is a vector, expand it into the current array
                auto &subArr = std::get<std::vector<std::variant<int, std::string>>>(*chunk);
                arr.insert(arr.end(), subArr.begin(), subArr.end());
            }
            else if (std::holds_alternative<int>(*chunk))
            {
                arr.push_back(std::get<int>(*chunk));
            }
            else if (std::holds_alternative<std::string>(*chunk))
            {
                arr.push_back(std::get<std::string>(*chunk));
            }
        }
        return {arr, remainder};
    }

    return {std::nullopt, buffer_in};
}
