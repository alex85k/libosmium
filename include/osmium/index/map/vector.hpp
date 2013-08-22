#ifndef OSMIUM_INDEX_MAP_VECTOR_HPP
#define OSMIUM_INDEX_MAP_VECTOR_HPP

/*

This file is part of Osmium (http://osmcode.org/osmium).

Copyright 2013 Jochen Topf <jochen@topf.org> and others (see README).

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

#include <algorithm>
#include <stdexcept>
#include <utility>

#include <osmium/index/map.hpp>
#include <osmium/io/detail/read_write.hpp>

namespace osmium {

    namespace index {

        namespace map {

            template <class TVector, typename TKey, typename TValue>
            class VectorBasedDenseMap : public Map<TKey, TValue> {

                TVector m_vector;

            public:

                VectorBasedDenseMap() :
                    m_vector() {
                }

                VectorBasedDenseMap(int fd) :
                    m_vector(fd) {
                }

                ~VectorBasedDenseMap() noexcept = default;

                void reserve(const size_t size) override final {
                    m_vector.reserve(size);
                }

                void set(const TKey key, const TValue value) override final {
                    if (size() <= key) {
                        m_vector.resize(key+1);
                    }
                    m_vector[key] = value;
                }

                const TValue get(const TKey key) const override final {
                    const TValue& value = m_vector.at(key);
                    if (value == TValue {}) {
                        throw std::out_of_range("out of range");
                    }
                    return value;
                }

                size_t size() const override final {
                    return m_vector.size();
                }

                size_t used_memory() const override final {
                    return sizeof(TValue) * size();
                }

                void clear() override final {
                    m_vector.clear();
                    m_vector.shrink_to_fit();
                }

            }; // class VectorBasedDenseMap


            template <typename TKey, typename TValue, template<typename...> class TVector>
            class VectorBasedSparseMap : public Map<TKey, TValue> {

            public:

                typedef typename std::pair<TKey, TValue> element_type;
                typedef TVector<element_type> vector_type;
                typedef typename vector_type::iterator iterator;
                typedef typename vector_type::const_iterator const_iterator;

            private:

                vector_type m_vector;

            public:

                VectorBasedSparseMap() :
                    m_vector() {
                }

                VectorBasedSparseMap(int fd) :
                    m_vector(fd) {
                }

                ~VectorBasedSparseMap() noexcept = default;

                void set(const TKey key, const TValue value) override final {
                    m_vector.push_back(element_type(key, value));
                }

                const TValue get(const TKey key) const override final {
                    const element_type element {key, TValue {}};
                    const auto result = std::lower_bound(m_vector.begin(), m_vector.end(), element, [](const element_type& a, const element_type& b) {
                        return a.first < b.first;
                    });
                    if (result == m_vector.end() || result->first != key) {
                        throw std::out_of_range("Unknown ID");
                    } else {
                        return result->second;
                    }
                }

                size_t size() const override final {
                    return m_vector.size();
                }

                size_t byte_size() const {
                    return m_vector.size() * sizeof(element_type);
                }

                size_t used_memory() const override final {
                    return sizeof(element_type) * size();
                }

                void clear() override final {
                    m_vector.clear();
                    m_vector.shrink_to_fit();
                }

                void sort() override final {
                    std::sort(m_vector.begin(), m_vector.end());
                }

                void dump_as_list(int fd) const {
                    osmium::io::detail::reliable_write(fd, reinterpret_cast<const char*>(m_vector.data()), byte_size());
                }

                iterator begin() {
                    return m_vector.begin();
                }

                iterator end() {
                    return m_vector.end();
                }

                const_iterator cbegin() const {
                    return m_vector.cbegin();
                }

                const_iterator cend() const {
                    return m_vector.cend();
                }

                const_iterator begin() const {
                    return m_vector.cbegin();
                }

                const_iterator end() const {
                    return m_vector.cend();
                }

            }; // class VectorBasedSparseMap

        } // namespace map

    } // namespace index

} // namespace osmium

#endif // OSMIUM_INDEX_MAP_VECTOR_HPP
