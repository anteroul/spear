#include <spear/util.hh>

#include <ctime>
#include <random>

namespace spear
{

uint64_t Util::generateRandomUint64()
{
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<uint64_t> dist(0, std::numeric_limits<uint64_t>::max());
    return dist(gen);
}

std::string Util::generateRandomString(size_t n)
{
    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"; // Allowed characters
    const size_t charsetSize = sizeof(charset) - 1;                                // Exclude null terminator

    // Random engine seeded with current time
    std::mt19937 generator(static_cast<uint32_t>(std::time(nullptr)));
    std::uniform_int_distribution<uint64_t> distribution(0, charsetSize - 1);

    std::string result;
    result.reserve(n);

    for (size_t i = 0; i < n; ++i)
    {
        result += charset[distribution(generator)];
    }

    return result;
}

} // namespace spear
