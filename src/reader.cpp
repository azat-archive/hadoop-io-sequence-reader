
/**
 * This file is part of the hadoop-io-sequence-reader package.
 *
 * (c) Azat Khuzhin <a3at.mail@gmail.com>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

#include "reader.h"

Reader::Reader(std::ifstream *stream)
    : m_stream(stream)
    , m_initialized(false)
{
}

bool Reader::next()
{
    if (!m_initialized) {
        initialize();
    }

    m_record = read(m_stream);

    return true;
}

std::ostream& operator<<(std::ostream& out, const Reader& reader)
{
    out << reader.m_record << std::endl;
    return out;
}

void Reader::initialize()
{
    const uint8_t SEQFILE_VERSION_HEADER[4] = {'S', 'E', 'Q', 6};
    const char* const SEQFILE_VALUE_CLASS_NAME = "org.apache.hadoop.io.Text";

    char buffer[1024];

    if (!m_stream->read(buffer, sizeof(SEQFILE_VERSION_HEADER) + 1)) {
        throw std::string("Can't read SEQFILE_VERSION_HEADER");
    }
    if (memcmp(SEQFILE_VERSION_HEADER, buffer, sizeof(SEQFILE_VERSION_HEADER)) != 0) {
        throw std::string("SEQFILE_VERSION_HEADER is malformed");
    }

    if (!m_stream->read(buffer, sizeof(SEQFILE_VALUE_CLASS_NAME))) {
        throw std::string("Can't read SEQFILE_VALUE_CLASS_NAME");
    }
    if (memcmp(SEQFILE_VALUE_CLASS_NAME, buffer, sizeof(SEQFILE_VALUE_CLASS_NAME)) != 0) {
        throw std::string("SEQFILE_VALUE_CLASS_NAME is malformed");
    }
    m_stream->read(buffer,      85
                                - sizeof(SEQFILE_VALUE_CLASS_NAME)
                                - sizeof(SEQFILE_VERSION_HEADER));

    m_initialized = true;
}

std::string Reader::read(std::istream* stream)
{
    char smallBuffer[1024];
    if (!stream->read(smallBuffer, 8)) {
        throw std::string("read: End of file reached");
    }
#ifdef DEBUG
    std::cerr << "In small buffer " << Hex(smallBuffer, 8) << std::endl;
#endif
    // Seems that it is "sync"
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
#ifdef DEBUG
    std::cerr << "Read in hex '" << Hex(buffer, len) << "'" << std::endl;
#endif

    std::string ret(buffer, len);
    free(buffer);

    return ret;
}
