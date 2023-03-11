#include <unnecessary/nlohmann/json_archiver.h>
#include <unnecessary/nlohmann/adapter.h>
#include <nlohmann/json.hpp>

namespace un {
    byte_buffer json_archiver::write(const serialized& serialized) {
        nlohmann::json obj = serialization::adapt(serialized);
        std::string string = nlohmann::to_string(obj);
        byte_buffer buf(string.size());
        buf.copy_from(string.data());
        return buf;
    }

    void json_archiver::read(const byte_buffer& buffer, serialized& into) {
        std::string jsonStr(reinterpret_cast<const char*>(buffer.data()), buffer.size());
        nlohmann::json obj = nlohmann::json::parse(jsonStr);
        serialization::adapt(obj, into);
    }
}
