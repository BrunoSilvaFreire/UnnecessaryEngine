#ifndef UNNECESSARYENGINE_FILE_READER_H
#define UNNECESSARYENGINE_FILE_READER_H
namespace un {
    class file_helper {
    private:
        std::ifstream _stream;
    public:
        explicit file_helper(std::ifstream&& stream) : _stream(std::move(stream)) {

        }

        void seek(std::size_t offset) {
            _stream.seekg(offset, std::ios::beg);
        }

        std::size_t tell() {
            return _stream.tellg();
        }

        template<typename T>
        T read() {
            T element;
            _stream.read(reinterpret_cast<char*>(&element), sizeof(T));
            return element;
        }

        template<typename T>
        void read(T* into, std::size_t count) {
            _stream.read(reinterpret_cast<char*>(&into), sizeof(T) * count);
        }
    };
}
#endif
