#include "config_manager.hpp"

// Define the magic word
const std::string MAGIC_WORD = "config";
const size_t pageSize = FLASH_PAGE_SIZE;

ConfigManager::ConfigManager()
{
    load();
}

void ConfigManager::readFlash()
{
    uintptr_t program_end = ((uintptr_t)&__flash_binary_end - XIP_BASE);
    if (program_end % pageSize)
    {
        program_end += pageSize;
        program_end -= program_end % pageSize;
    }
    targetOffset = program_end + 4096;
    const uint8_t *flash_contents = (const uint8_t *)(XIP_BASE + targetOffset);
    // std::cout
    //     << "offset set: " << targetOffset << std::endl;
    // std::cout << "program end: " << program_end << std::endl;

    // Check for the magic word
    std::string magic = MAGIC_WORD + ";";
    uint32_t ints = save_and_disable_interrupts();
    for (uint i = 0; i < MAGIC_WORD.length(); i++)
    {
        if (static_cast<char>(flash_contents[i]) != magic[i])
        {
            restore_interrupts(ints);

            // std::cout << "magic word off at i = " << i << ":" << std::endl;
            // std::cout << static_cast<char>(flash_contents[i]) << "!=" << magic[i] << std::endl;
            return;
        }
    }

    // Read the size of the JSON data
    std::string sizeStr;
    for (size_t i = magic.length(); flash_contents[i] != ';' && i < pageSize; ++i)
    {
        sizeStr += static_cast<char>(flash_contents[i]);
    }
    int dataSize = std::stoi(sizeStr);

    // Read the JSON data
    std::string configData(reinterpret_cast<const char *>(flash_contents) + magic.length() + sizeStr.length() + 1, dataSize);
    restore_interrupts(ints);

    // std::cout << "size str " << sizeStr << std::endl;
    // std::cout << "size " << dataSize << std::endl;
    // std::cout << "read " << configData << std::endl;

    try
    {
        config = nlohmann::json::parse(configData);
        // std::cout << "config " << config << std::endl;
    }
    catch (nlohmann::json::parse_error &e)
    {
        // std::cout << "can't parse " << e.what() << std::endl;
        // Handle parse error or use a default config
    }
}

void ConfigManager::writeFlash()
{
    std::string serialized = config.dump();

    // Prepare the initial buffer with the magic word and size
    std::ostringstream bufferStream;
    bufferStream.write((MAGIC_WORD + ";").c_str(), MAGIC_WORD.length() + 1);
    bufferStream << serialized.size() << ';';
    bufferStream << serialized;

    std::string buffer = bufferStream.str();
    size_t bufferSize = buffer.size();
    size_t remainingSize = bufferSize;

    // Write data in pageSize chunks
    for (size_t offset = 0; offset < bufferSize; offset += pageSize)
    {
        size_t chunkSize = std::min(pageSize, remainingSize);
        std::vector<uint8_t> pageBuffer(pageSize, 0);
        std::memcpy(pageBuffer.data(), buffer.data() + offset, chunkSize);
        // std::cout << "writing " << targetOffset + offset << ", " << pageSize << std::endl;
        // std::cout << pageBuffer.data() << std::endl;
        uint32_t ints = save_and_disable_interrupts();
        flash_range_erase(targetOffset + offset, pageSize);
        flash_range_program(targetOffset + offset, pageBuffer.data(), pageSize);
        restore_interrupts(ints);
        // std::cout << "done" << std::endl;
        remainingSize -= chunkSize;
    }
}

int ConfigManager::get_int(const std::string &key, int defaultValue)
{
    // Check if the key exists and is not null
    if (config.contains(key) && !config[key].is_null())
    {
        return config[key].get<int>();
    }
    return defaultValue;
}

std::string ConfigManager::get_str(const std::string &key, const std::string &defaultValue)
{
    // Check if the key exists and is not null
    if (config.contains(key) && !config[key].is_null())
    {
        return config[key].get<std::string>();
    }
    return defaultValue;
}

void ConfigManager::set_int(const std::string &key, int value)
{
    config[key] = value;
    save();
}

void ConfigManager::set_str(const std::string &key, const std::string &value)
{
    config[key] = value;
    save();
}

void ConfigManager::load()
{
    readFlash();
}

void ConfigManager::save()
{
    writeFlash();
}
