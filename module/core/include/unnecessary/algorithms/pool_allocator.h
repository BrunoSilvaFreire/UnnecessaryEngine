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
        class Page;

        struct Allocation {
            /**
             * Which page is this allocated in?
             */
            std::size_t page;
            TElement block;
        };

        class Page {
        private:
            Page* nextPage;
            Allocation* block;
            std::size_t size;
            std::size_t head;
            un::IndexRecycler<std::size_t> recycler;
        public:
            explicit Page(
                std::size_t numElements,
                std::size_t pageIndex
            ) : size(numElements),
                head(0),
                block(static_cast<Allocation*>(std::malloc(numElements * sizeof(Allocation)))),
                nextPage(nullptr) {
            }

            ~Page() {
                std::free(block);
            }

            Page* getNextPage() const {
                return nextPage;
            }

            Allocation* allocate() {
                if (isFull()) {
                    throw std::runtime_error("Page is full, cannot allocate any more.");
                }
                std::size_t index;
                if (recycler.try_reuse(index)) {
                    return block + index;
                }
                return block + head++;
            }

            void free(Allocation* allocation) {
                std::size_t index = allocation - block;
                recycler.recycle(index);
            }

            bool isFull() const {
                return size == head;
            }

            size_t getSize() const {
                return size;
            }

            void setNextPage(Page* next) {
                nextPage = next;
            }
        };

        Page root;
        Page* current;
        std::size_t numPages;
        std::size_t pageSize;

        Page* constructPage() {
            return new Page(pageSize, numPages++);
        }

        Page* findPage(std::size_t index) {
            Page* page = &root;
            std::size_t i = 0;
            while (i++ < index) {
                page = page->getNextPage();
            }
            return page;
        }

    public:

        explicit PoolAllocator(
            std::size_t numElementsPerPage
        ) : numPages(1),
            pageSize(numElementsPerPage),
            root(numElementsPerPage, 0),
            current(&root) {
        }

        ~PoolAllocator() {
            Page* page = root.getNextPage();
            for (std::size_t i = 0; i < numPages - 1; ++i) {
                auto next = page->getNextPage();
                delete page;
                page = next;
            }
        }

        template<typename ...Args>
        TElement* construct(Args... args) {
            if (current->isFull()) {
                auto next = constructPage();
                current->setNextPage(next);
                current = next;
            }
            Allocation* allocation = current->allocate();
            TElement* allocated = &allocation->block;
            return new(allocated) TElement(std::forward<Args>(args)...);
        }

        void free(TElement* element) {
            auto allocation = reinterpret_cast<Allocation*>(element - sizeof(Allocation::page));
            (&allocation->block)->~TElement();
            Page* page = findPage(allocation->page);
            page->free(allocation);
        }

    };
}
#endif
