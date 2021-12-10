#ifndef _STRING_EXT_HPP_
#define _STRING_EXT_HPP_

#include <algorithm>
#include <string>
#include <string_view>
#include <type_traits>

// trim from begin (in place)
inline void lTrim(std::string &s) {
    s.erase(s.begin(),
            std::find_if(s.begin(), s.end(), [](unsigned char ch) { return !std::isspace(ch); }));
}

// trim from end (in place)
inline void rTrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) { return !std::isspace(ch); })
                .base(),
            s.end());
}

// trim from both ends (in place)
inline void trim(std::string &s) {
    lTrim(s);
    rTrim(s);
}

// trim from both ends (copy)
inline std::string trimCopy(std::string s) {
    trim(s);
    return s;
}

constexpr bool endsWith(std::string_view str, std::string_view suffix) {
    return str.size() >= suffix.size() &&
           0 == str.compare(str.size() - suffix.size(), suffix.size(), suffix);
}

constexpr bool startsWith(std::string_view str, std::string_view prefix) {
    return str.size() >= prefix.size() && 0 == str.compare(0, prefix.size(), prefix);
}

template <typename ContainerT>
void split(std::string_view str, ContainerT &tokens, std::string_view delimiters,
           bool trimEmpty = false) {
    std::string::size_type pos, last_pos = 0, length = str.length();

    using value_type = typename ContainerT::value_type;
    using size_type = typename ContainerT::size_type;

    while (last_pos < length + 1) {
        pos = str.find_first_of(delimiters, last_pos);
        if (pos == std::string_view::npos) pos = length;

        if (pos != last_pos || !trimEmpty) {
            tokens.push_back(value_type(str.data() + last_pos, (size_type) pos - last_pos));
        }
        last_pos = pos + 1;
    }
}

template <typename ContainerT>
void split(std::string_view sv, ContainerT &tokens, char delimiters = ' ',
           bool trimEmpty = false) {
    std::string::size_type pos, last_pos = 0, length = sv.length();

    using value_type = typename ContainerT::value_type;
    using size_type = typename ContainerT::size_type;

    while (last_pos < length + 1) {
        pos = sv.find_first_of(delimiters, last_pos);
        if (pos == std::string_view::npos) pos = length;

        if (pos != last_pos || !trimEmpty) {
            tokens.push_back(value_type(sv.data() + last_pos, (size_type) pos - last_pos));
        }
        last_pos = pos + 1;
    }
}

#endif // _STRING_EXT_HPP_
