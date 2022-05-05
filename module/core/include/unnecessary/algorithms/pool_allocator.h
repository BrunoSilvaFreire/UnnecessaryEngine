#ifndef UNNECESSARYENGINE_POOL_ALLOCATOR_H
#define UNNECESSARYENGINE_POOL_ALLOCATOR_H

#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <memory>
#include <unnecessary/algorithms/index_recycler.h>

namespace un {

    template<typename TElement>
    class PoolAllocator {
    private:
        class Page {
        private:
            TElement* block;
            std::size_t size;
            std::size_t head;
            un::IndexRecycler<std::size_t> recycler;
        public:
            bool operator<(const Page& rhs) const {
                return block < rhs.block;
            }

            bool operator>(const Page& rhs) const {
                return rhs < *this;
            }

            bool operator<=(const Page& rhs) const {
                return !(rhs < *this);
            }

            bool operator>=(const Page& rhs) const {
                return !(*this < rhs);
            }

        public:
            explicit Page(
                std::size_t numElements
            ) : size(numElements),
                head(0),
                block(static_cast<TElement*>(std::malloc(numElements * sizeof(TElement)))) {
            }

            ~Page() {
                std::free(block);
            }

            TElement* getBlock() const {
                return block;
            }

            TElement* allocate() {
                if (isFull()) {
                    throw std::runtime_error("Page is full, cannot allocate any more.");
                }
                std::size_t index;
                if (recycler.try_reuse(index)) {
                    return block + index;
                }
                return block + head++;
            }

            void free(TElement* allocation) {
                std::size_t index = allocation - block;
                recycler.recycle(index);
            }

            bool isFull() const {
                return size == head;
            }

            size_t getSize() const {
                return size;
            }
        };

        std::set<Page*> pages;
        std::queue<Page*> freePages;
        Page* root;
        Page* current;
        std::size_t pageSize;

        Page* constructPage() {
            Page* pPage = new Page(pageSize);
            pages.emplace(pPage);
            freePages.emplace(pPage);
            return pPage;
        }

    public:

        explicit PoolAllocator(
            std::size_t numElementsPerPage
        ) :
            pageSize(numElementsPerPage),
            pages(),
            freePages(),
            root(nullptr),
            current(nullptr) {
            current = root = constructPage();

        }

        ~PoolAllocator() {
            for (Page* page : pages) {
                delete page;
            }
        }

        template<typename ...Args>
        TElement* construct(Args... args) {
            if (current->isFull()) {
                auto next = constructPage();
                current = next;
            }
            TElement* ptr = current->allocate();
            return new(ptr) TElement(std::forward<Args>(args)...);
        }

        void dispose(TElement* element) {
            element->~TElement();
            Page* page = findPage(element);
            page->free(element);
        }

        Page* getRoot() const {
            return root;
        }

        Page* findPage(TElement* element) {
            // O(Log(N))
            auto it = std::lower_bound(
                pages.begin(),
                pages.end(),
                element,
                [](Page* page, TElement* element) {
                    TElement* start = page->getBlock();
                    TElement* end = start + page->getSize();
                    bool contains = element >= start && element < end;
                    std::cout << "start: " << start
                              << ", end: " << end
                              << ", element: " << element
                              << " contains: " << contains
                              << std::endl;
                    return page->getBlock() < element;
                }
            );
            if (it != pages.end()) {
                return *it;
            }
            return nullptr;
        }
    };
}
#endif
