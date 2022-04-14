#include <unnecessary/nlohmann/json_archiver.h>
#include <unnecessary/nlohmann/adapter.h>
#include <nlohmann/json.hpp>


namespace un {


    un::Buffer JsonArchiver::write(const Serialized& serialized) {
        nlohmann::json obj = un::serialization::adapt(serialized);
        std::string string = nlohmann::to_string(obj);
        un::Buffer buf(string.size());
        buf.copy(reinterpret_cast<u8*>(string.data()));
        return buf;
    }

    void JsonArchiver::read(const Buffer& buffer, Serialized& into) {

    }
}