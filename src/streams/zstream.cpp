//
//  zstream.cpp
//  streams
//
//  Created by Dmitrii Torkhov <dmitriitorkhov@gmail.com> on 30.10.2020.
//  Copyright © 2020-2021 Dmitrii Torkhov. All rights reserved.
//

#define MINIZ_NO_STDIO
#define MINIZ_NO_TIME
#define MINIZ_NO_ARCHIVE_APIS
#define MINIZ_NO_ARCHIVE_WRITING_APIS

#include "miniz.h"

#include "streams/zstream.h"

// -- ozstream --

void oo::ozstream::operator>>(std::vector<u_int8_t> &value) {
    auto cmp_len = compressBound(m_buffer.size());
    value.resize(cmp_len);

    if (compress(value.data(), &cmp_len, m_buffer.data(), m_buffer.size()) != Z_OK) {
        value.resize(0);
        return;
    }

    value.resize(cmp_len);
}

// -- izstream --

namespace {

    const mz_uint32 c_default_buf_len = 1024;

}

void oo::izstream::operator<<(const std::vector<u_int8_t> &value) {
    mz_stream stream;
    memset(&stream, 0, sizeof(stream));

    stream.next_in = value.data();
    stream.avail_in = uint32_t(value.size());

    if (mz_inflateInit(&stream) != MZ_OK) {
        return;
    }

    const mz_uint32 buf_len = c_default_buf_len;
    u_int8_t buf[buf_len];

    int status;
    do {
        stream.next_out = buf;
        stream.avail_out = buf_len;

        status = mz_inflate(&stream, 0);

        m_buffer.insert(m_buffer.end(), buf, buf + (buf_len - stream.avail_out));
    } while (status == MZ_OK);

    mz_inflateEnd(&stream);
}