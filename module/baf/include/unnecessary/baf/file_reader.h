#ifndef UNNECESSARYENGINE_FILE_READER_H
#define UNNECESSARYENGINE_FILE_READER_H
namespace un {
    class FileHelper {
    private:
        std::ifstream stream;
    public:
        explicit FileHelper(std::ifstream&& stream) : stream(std::move(stream)) {

        }

        void seek(std::size_t offset) {
            stream.seekg(offset, std::ios::beg);
        }

        std::size_t tell() {
            return stream.tellg();
        }

        template<typename T>
        T read() {
            T element;
            stream.read(reinterpret_cast<char*>(&element), sizeof(T));
            return element;
        }

        template<typename T>
        void read(T* into, std::size_t count) {
            stream.read(reinterpret_cast<char*>(&into), sizeof(T) * count);
        }
    };
}
#endif
