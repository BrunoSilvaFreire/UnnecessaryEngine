#ifndef UNNECESSARYENGINE_MAP_MESSAGE_H
#define UNNECESSARYENGINE_MAP_MESSAGE_H

#include <map>
#include <unnecessary/misc/rich_messages/message.h>

namespace un {

    class MapMessage final : public IMessage {
    private:
        std::vector<std::pair<std::string, std::unique_ptr<un::IMessage>>> _named;

        template<typename TChild>
        TChild& add_child(const std::string& name) {
            return static_cast<TChild&>(*_named.emplace_back(name, std::make_unique<TChild>()).second);
        }

        void prefix(
            std::ostream& stream,
            const std::string& name,
            std::size_t i,
            std::size_t lineNbr,
            const std::unique_ptr<IMessage>& msg
        ) const;

    protected:
        void write(std::ostream& stream) const override;

    public:
        bool is_node() override;

        un::TextMessage& text(const std::string& name);

        un::TreeMessage& tree(const std::string& name);
    };
}
#endif
