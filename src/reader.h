
#pragma once

#include <fstream>
#include <ostream>
#include <string>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#ifdef DEBUG
#include <iostream>
#endif

#include <stdint.h> // uint_32

#include "hex_helper.h"

class Reader
{
public:
    Reader(std::ifstream *stream);

    bool next();
    friend std::ostream& operator<<(std::ostream& out, const Reader& reader);

private:
    std::ifstream *m_stream;
    std::string m_record;
    bool m_initialized;

    /**
     * Read header
     */
    void initialize();

    static uint32_t decodeVLongSize(int8_t ch) {
        if (ch >= -112) {
            return 1;
        } else if (ch < -120) {
            return -119 - ch;
        }
        return -111 - ch;
    }

    static uint32_t decodeVLongSize(const char *pos) {
        return decodeVLongSize(*pos);
    }

    static int64_t readVLong(const char* pos, uint32_t &len)
    {
        if (*pos >= (char)-112) {
            len = 1;
            return *pos;
        } else {
            return readVLongInner(pos, len);
        }
    }

    static int64_t readVLong(std::istream* stream)
    {
        char buff[10];
        if (!stream->read(buff, 1)) {
            throw std::string("readVLong: End of file reached");
        }
        uint32_t len = decodeVLongSize(buff);
        if (len > 1) {
            if (!stream->read(buff + 1, len - 1)) {
                throw std::string("readVLong + decodeVLongSize: End of file reached");
            }
        }
        return readVLong(buff, len);
    }

    static int64_t readVLongInner(const char *pos, uint32_t &len)
    {
        bool neg = *pos < -120;
        len = neg ? (-119 - *pos) : (-111 - *pos);
        const char * end = pos + len;
        int64_t value = 0;
        while (++pos < end) {
            value = (value << 8) | *(uint8_t*)pos;
        }
        return neg ? (value ^ -1LL) : value;
    }

    static std::string read(std::istream* stream);
};
