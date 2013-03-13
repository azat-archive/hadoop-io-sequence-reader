
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

KeyValueType Reader::javaClassToKeyValueType(const std::string &clazz)
{
    if (clazz == "org.apache.hadoop.io.Text") {
        return TextType;
    }
    if (clazz == "org.apache.hadoop.io.BytesWritable") {
        return BytesType;
    }
    if (clazz == "org.apache.hadoop.io.ByteWritable") {
        return ByteType;
    }
    if (clazz == "org.apache.hadoop.io.BooleanWritable") {
        return BoolType;
    }
    if (clazz == "org.apache.hadoop.io.IntWritable") {
        return IntType;
    }
    if (clazz == "org.apache.hadoop.io.LongWritable") {
        return LongType;
    }
    if (clazz == "org.apache.hadoop.io.FloatWritable") {
        return FloatType;
    }
    if (clazz == "org.apache.hadoop.io.DoubleWritable") {
        return DoubleType;
    }
    if (clazz == "org.apache.hadoop.io.MD5Hash") {
        return MD5HashType;
    }
    return UnknownType;
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
