#ifndef OSMIUM_IO_BZIP2_COMPRESSION_HPP
#define OSMIUM_IO_BZIP2_COMPRESSION_HPP

/*

This file is part of Osmium (http://osmcode.org/libosmium).

Copyright 2013,2014 Jochen Topf <jochen@topf.org> and others (see README).

Boost Software License - Version 1.0 - August 17th, 2003

Permission is hereby granted, free of charge, to any person or organization
obtaining a copy of the software and accompanying documentation covered by
this license (the "Software") to use, reproduce, display, distribute,
execute, and transmit the Software, and to prepare derivative works of the
Software, and to permit third-parties to whom the Software is furnished to
do so, all subject to the following:

The copyright notices in the Software and this entire statement, including
the above license grant, this restriction and the following disclaimer,
must be included in all copies of the Software, in whole or in part, and
all derivative works of the Software, unless such copies or derivative
works are solely in the form of machine-executable object code generated by
a source language processor.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.

*/

#define OSMIUM_LINK_WITH_LIBS_BZ2LIB -lbz2

#include <cstdio>
#include <stdexcept>
#include <string>

#include <bzlib.h>

#include <osmium/io/compression.hpp>
#include <osmium/io/file_compression.hpp>

namespace osmium {

    namespace io {

        class Bzip2Compressor : public Compressor {

            FILE* m_file;
            int m_bzerror;
            BZFILE* m_bzfile;

        public:

            explicit Bzip2Compressor(int fd) :
                Compressor(),
                m_file(fdopen(fd, "w")),
                m_bzerror(BZ_OK),
                m_bzfile(::BZ2_bzWriteOpen(&m_bzerror, m_file, 6, 0, 0)) {
                if (!m_bzfile) {
                    throw std::runtime_error("initialization of bzip2 compression failed");
                }
            }

            ~Bzip2Compressor() override final {
                this->close();
            }

            void write(const std::string& data) override final {
                int error;
                ::BZ2_bzWrite(&error, m_bzfile, const_cast<char*>(data.data()), data.size());
                // XXX check for error
            }

            void close() override final {
                if (m_bzfile) {
                    int error;
                    unsigned int nbytes_in_lo32;
                    unsigned int nbytes_in_hi32;
                    unsigned int nbytes_out_lo32;
                    unsigned int nbytes_out_hi32;
                    ::BZ2_bzWriteClose64(&error, m_bzfile, 0, &nbytes_in_lo32, &nbytes_in_hi32, &nbytes_out_lo32, &nbytes_out_hi32);
                    m_bzfile = nullptr;
                }
            }

        }; // class Bzip2Compressor

        class Bzip2Decompressor : public Decompressor {

            FILE* m_file;
            int m_bzerror;
            BZFILE* m_bzfile;

        public:

            Bzip2Decompressor(int fd) :
                Decompressor(),
                m_file(fdopen(fd, "r")),
                m_bzerror(BZ_OK),
                m_bzfile(::BZ2_bzReadOpen(&m_bzerror, m_file, 0, 0, nullptr, 0)) {
                if (!m_bzfile) {
                    throw std::runtime_error("initialization of bzip2 compression failed");
                }
            }

            ~Bzip2Decompressor() override final {
                this->close();
            }

            std::string read() override final {
                std::string buffer(osmium::io::Decompressor::input_buffer_size, '\0');
                int error;
                int nread = ::BZ2_bzRead(&error, m_bzfile, const_cast<char*>(buffer.data()), buffer.size());
//                if (error != BZ_OK && error != BZ_STREAM_END) {
//                    throw std::runtime_error("bzip2 read failed"); // XXX better error detection and reporting
//                }
                buffer.resize(nread);
                return buffer;
            }

            void close() override final {
                if (m_bzfile) {
                    int error;
                    ::BZ2_bzReadClose(&error, m_bzfile);
                    m_bzfile = nullptr;
                }
            }

        }; // class Bzip2Decompressor

        namespace {

            const bool registered_bzip2_compression = osmium::io::CompressionFactory::instance().register_compression(osmium::io::file_compression::bzip2,
                [](int fd) { return new osmium::io::Bzip2Compressor(fd); },
                [](int fd) { return new osmium::io::Bzip2Decompressor(fd); }
            );

        } // anonymous namespace

    } // namespace io

} // namespace osmium

#endif // OSMIUM_IO_BZIP2_COMPRESSION_HPP
