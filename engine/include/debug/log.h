#pragma once
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <utility>

#ifdef ENGINE_DEBUG
#include <cassert>
#define ENGINE_ASSERT(x) assert(x)
#else
#define ENGINE_ASSERT(x) ((void)0)
#endif

namespace Debug
{
#ifdef ENGINE_DEBUG
inline std::ostream& prefix(std::ostream& os, const char* cat, const char* func)
{
    os << "[" << std::left << std::setw(5) << "DEBUG" << "]"
       << "[" << std::left << std::setw(10) << cat << "]"
       << "[" << std::left << std::setw(14) << func << "] ";
    return os;
}
#endif

template <typename... Args>
[[noreturn]] void
fatal(const char* cat, const char* file, int line, const char* func, Args&&... args)
{
    std::ostringstream oss;
    oss << "[" << std::left << std::setw(5) << "FATAL" << "]"
        << "[" << std::left << std::setw(10) << cat << "]"
        << "[" << std::left << std::setw(14) << func << "]"
        << "[" << file << ":" << line << "] ";

    (oss << ... << std::forward<Args>(args));

    std::cerr << oss.str() << '\n';
    std::abort();
}
} // namespace Debug

#ifdef ENGINE_DEBUG
#define DOUT(cat) Debug::prefix(std::cerr, cat, __func__)
#else
struct NullStream
{
    template <typename T> NullStream& operator<<(const T&)
    {
        return *this;
    }

    NullStream& operator<<(std::ostream& (*)(std::ostream&))
    {
        return *this;
    }
};

inline NullStream g_nullStream;

#define DOUT(cat) g_nullStream
#endif

#define ENGINE_FATAL(cat, ...) Debug::fatal(cat, __FILE__, __LINE__, __func__, __VA_ARGS__)