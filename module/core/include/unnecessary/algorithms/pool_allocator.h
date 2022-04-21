#ifndef UNNECESSARYENGINE_POOL_ALLOCATOR_H
#define UNNECESSARYENGINE_POOL_ALLOCATOR_H

#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <memory>

namespace un {
    template<typename TElement>
    class PoolAllocator {
    private:
        class Page {
        private:
            std::shared_ptr<Page> nextPage;
            TElement* block;
            std::size_t size;
            std::size_t head;
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

            TElement* allocate() {
                if (isFull()) {
                    throw std::runtime_error("Page is full, cannot allocate any more.");
                }
                return block + head++;
            }

            bool isFull() const {
                return size == head;
            }

            size_t getSize() const {
                return size;
            }

            std::shared_ptr<Page> newPage() {
                return nextPage = std::make_shared<Page>(size);
            }
        };

        std::shared_ptr<Page> root;
        std::weak_ptr<Page> current;
    public:

        explicit PoolAllocator(std::size_t numElementsPerPage) : root(std::make_shared<Page>(numElementsPerPage)) {
            current = root;
        }

        template<typename ...Args>
        TElement& construct(Args... args) {
            TElement* element;
            if (std::shared_ptr<Page> page = current.lock()) {
                if (page->isFull()) {
                    current = page = page->newPage();
                }
                TElement* allocated = page->allocate();
                element = new(allocated) TElement(std::forward<Args>(args)...);

            } else {
                throw std::runtime_error("Page was not available for allocation");
            }
            return *element;
        }
    };
}
#endif
