#pragma once
#include <cstdint>
#include <string>

namespace remmy {
    namespace _detail {
        // encodes ASCII characters to 6bit encoding
        constexpr unsigned char encoding_table[] = {
            /*     ..0 ..1 ..2 ..3 ..4 ..5 ..6 ..7 ..8 ..9 ..A ..B ..C ..D ..E ..F  */
            /* 0.. */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
            /* 1.. */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
            /* 2.. */  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
            /* 3.. */  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 0,  0,  0,  0,  0,  0,
            /* 4.. */  0, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
            /* 5.. */ 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36,  0,  0,  0,  0, 37,
            /* 6.. */  0, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52,
            /* 7.. */ 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63,  0,  0,  0,  0,  0 };

        // decodes 6bit characters to ASCII
        constexpr char decoding_table[] =
            " 0123456789"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ_"
            "abcdefghijklmnopqrstuvwxyz";

        constexpr uint64_t next_interim(uint64_t current, size_t char_code) {
            return (current << 6) | encoding_table[(char_code <= 0x7F) ? char_code : 0];
        }

        constexpr uint64_t atom_val(const char* cstr, uint64_t interim = 0xF) {
            return (*cstr == '\0') ?
                interim :
                atom_val(cstr + 1,
                    next_interim(interim, static_cast<size_t>(*cstr)));
        }
    }

    /// Creates an atom from given string literal.
    template <size_t Size>
    constexpr uint64_t UniqueId(char const (&str)[Size]) {
        // last character is the NULL terminator
        static_assert(Size <= 11, "only 10 characters are allowed");
        return static_cast<uint64_t>(_detail::atom_val(str));
    }

    inline std::string DecodeUniqueId(const uint64_t x) {
        std::string result;
        result.reserve(11);
        // don't read characters before we found the leading 0xF
        // first four bits set?
        bool read_chars = ((x & 0xF000000000000000) >> 60) == 0xF;
        uint64_t mask = 0x0FC0000000000000;
        for (int bitshift = 54; bitshift >= 0; bitshift -= 6, mask >>= 6) {
            if (read_chars)
                result += _detail::decoding_table[(x & mask) >> bitshift];
            else if (((x & mask) >> bitshift) == 0xF)
                read_chars = true;
        }
        return result;
    }
}