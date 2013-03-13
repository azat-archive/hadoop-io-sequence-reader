
#pragma once

#include <string>
#include <ostream>
#include <iomanip>
#include <iostream>

struct Hex
{
    Hex(char *buffer, size_t size)
        : m_buffer(buffer)
        , m_size(size)
    {
    }

    friend std::ostream& operator <<(std::ostream &os, const Hex &obj)
    {
        unsigned char* aschar = (unsigned char*)obj.m_buffer;
        for (size_t i = 0; i < obj.m_size; ++i) {
            if (isprint(aschar[i]))  {
                os << aschar[i];
            } else {
                os << "\\x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int> (aschar[i]);
            }
        }
        return os;
    }

private:
    char *m_buffer;
    size_t m_size;
};
