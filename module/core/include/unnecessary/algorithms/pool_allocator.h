#ifndef UNNECESSARYENGINE_POOL_ALLOCATOR_H
#define UNNECESSARYENGINE_POOL_ALLOCATOR_H

#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <memory>
#include <unnecessary/algorithms/index_recycler.h>

namespace un {

    template<typename t_element>
    class pool_allocator {
    private:
        class page {
        private:
            t_element* _block;
            std::size_t _size;
            std::size_t _head;
            un::index_recycler<std::size_t> _recycler;
        public:
            bool operator<(const page& rhs) const {
                return _block < rhs._block;
            }

            bool operator>(const page& rhs) const {
                return rhs < *this;
            }

            bool operator<=(const page& rhs) const {
                return *this <= rhs;
            }

            bool operator>=(const page& rhs) const {
                return *this >= rhs;
            }

        public:
            explicit page(
                std::size_t numElements
            ) : _size(numElements),
                _head(0),
                _block(static_cast<t_element*>(std::malloc(numElements * sizeof(t_element)))) {
            }

            ~page() {
                std::free(_block);
            }

            t_element* get_block() const {
                return _block;
            }

            t_element* allocate() {
                if (is_full()) {
                    throw std::runtime_error("Page is full, cannot allocate any more.");
                }
                std::size_t index;
                if (_recycler.try_reuse(index)) {
                    return _block + index;
                }
                return _block + _head++;
            }

            void free(t_element* allocation) {
                std::size_t index = allocation - _block;
                _recycler.recycle(index);
            }

            bool is_full() const {
                return _size == _head;
            }

            size_t get_size() const {
                return _size;
            }
        };

        std::set<page*> _pages;
        std::queue<page*> _freePages;
        page* _root;
        page* _current;
        std::size_t _pageSize;

        page* construct_page() {
            page* pPage = new page(_pageSize);
            _pages.emplace(pPage);
            _freePages.emplace(pPage);
            return pPage;
        }

    public:

        explicit pool_allocator(
            std::size_t numElementsPerPage
        ) :
            _pageSize(numElementsPerPage),
            _pages(),
            _freePages(),
            _root(nullptr),
            _current(nullptr) {
            _current = _root = construct_page();

        }

        ~pool_allocator() {
            for (page* page : _pages) {
                delete page;
            }
        }

        template<typename ...Args>
        t_element* construct(Args... args) {
            if (_current->is_full()) {
                auto next = construct_page();
                _current = next;
            }
            t_element* ptr = _current->allocate();
            return new(ptr) t_element(std::forward<Args>(args)...);
        }

        void dispose(t_element* element) {
            element->~t_element();
            page* page = find_page(element);
            page->free(element);
        }

        page* get_root() const {
            return _root;
        }

        page* find_page(t_element* element) {
            // O(Log(N))
            auto it = std::lower_bound(
                _pages.begin(),
                _pages.end(),
                element,
                [](page* page, t_element* element) {
                    t_element* start = page->get_block();
                    t_element* end = start + page->get_size();
                    bool contains = element >= start && element < end;
                    std::cout << "start: " << start
                              << ", end: " << end
                              << ", element: " << element
                              << " contains: " << contains
                              << std::endl;
                    return page->get_block() < element;
                }
            );
            if (it != _pages.end()) {
                return *it;
            }
            return nullptr;
        }
    };
}
#endif
