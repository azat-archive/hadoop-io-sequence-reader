
#pragma once

#include <fstream>
#include <ostream>
#include <iostream>
#include <string>
#include <cstdint>
#include <cstdlib>
#include <cstring>

#include <stdint.h> // uint_32

#include "hex_helper.h"

enum KeyValueType {
    TextType = 0,
    BytesType = 1,
    ByteType = 2,
    BoolType = 3,
    IntType = 4,
    LongType = 5,
    FloatType = 6,
    DoubleType = 7,
    MD5HashType = 8,
    UnknownType = 9 
};

class Reader
{
public:
    Reader(std::ifstream *stream);

    bool next();
    friend std::ostream& operator<<(std::ostream& out, const Reader& reader);

    static KeyValueType javaClassToKeyValueType(const std::string &clazz);

private:
    std::ifstream *m_stream;
    std::string m_record;
    bool m_initialized;

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
        if (*pos>=(char)-112) {
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

    static std::string read(std::istream* stream)
    {
        char smallBuffer[1024];
        if (!stream->read(smallBuffer, 8)) {
            throw std::string("read: End of file reached");
        }
#ifdef DEBUG
        std::cerr << "In small buffer " << Hex(smallBuffer, 8) << std::endl;
#endif
        if (memcmp(smallBuffer, "\xff\xff\xff\xff", 4) == 0) {
            stream->read(smallBuffer, 12);
            return read(stream);
        }

        int32_t len = readVLong(stream);
#ifdef DEBUG
        std::cerr << "Readed " << std::dec << len << " bytes from stream" << std::endl;
#endif
        char *buffer = (char *)calloc(len, 1);

        if (!stream->read(buffer, len)) {
            free(buffer);
            throw std::string("read: End of file reached");
        }

        std::string ret = buffer;
        free(buffer);

        return ret;
    }
};
