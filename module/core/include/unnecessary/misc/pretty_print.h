#ifndef UNNECESSARYENGINE_PRETTY_PRINT_H
#define UNNECESSARYENGINE_PRETTY_PRINT_H

#include <string>
#include <unordered_map>
#include <unnecessary/logging.h>
#include <unnecessary/platform.h>
#include <unnecessary/misc/path_printer.h>
#include <unnecessary/misc/list_printer.h>

namespace un {
    class IMessage {
    public:
        IMessage() = default;

        IMessage(const IMessage&) = delete;

        virtual void write(std::ostream& stream) const = 0;
        virtual bool supports_children();
    };

    class TreeMessage;

    class MapMessage;

    class RichMessage;

    class TextMessage final : public IMessage {
    private:
        std::stringstream _text;
    public:
        void write(std::ostream& stream) const override;

        template<typename TAny>
        std::ostream& operator<<(TAny any) {
            return TextMessage::_text << any;
        }
    };


    class CompositeMessage : public IMessage {
    protected:
        std::vector<std::unique_ptr<IMessage>> _children;

        virtual void prefix(
            std::ostream& stream,
            std::size_t i,
            std::size_t lineNbr,
            const std::unique_ptr<IMessage>& msg
        ) const { }

        template<typename TChild>
        TChild& add_child() {
            return dynamic_cast<TChild&>(*_children.emplace_back(std::make_unique<TChild>()));
        }

    public:
        bool supports_children() override;

    public:
        CompositeMessage() = default;

        void write(std::ostream& stream) const override;

        std::size_t num_children() const;

        un::TextMessage& text();

        un::TreeMessage& tree();

        un::MapMessage& map();
    };

    class TreeMessage final : public CompositeMessage {
    protected:
        void prefix(
            std::ostream& stream,
            std::size_t i,
            std::size_t lineNbr,
            const std::unique_ptr<IMessage>& msg
        ) const override;
    };

    class MapMessage final : public IMessage {
    private:
        std::unordered_map<std::string, std::unique_ptr<un::IMessage>> _named;

        template<typename TChild>
        TChild& add_child(const std::string& name) {
            return dynamic_cast<TChild&>(*(_named[name] = std::make_unique<TChild>()));
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
        bool supports_children() override;

        un::TextMessage& text(const std::string& name);

        un::TreeMessage& tree(const std::string& name);
    };

    class RichMessage : public CompositeMessage {
    private:
        std::string _indent;
        std::string _header;
        std::string _level = INFO;
    public:
        explicit RichMessage(std::string indent, std::string header);

        ~RichMessage();

        void setLevel(const std::string& level);

        void setHeader(const std::string& header);

    protected:
        void prefix(
            std::ostream& stream,
            std::size_t i,
            std::size_t lineNbr,
            const std::unique_ptr<IMessage>& msg
        ) const override;
    };

    UN_AGGRESSIVE_INLINE un::RichMessage message() {
        return un::RichMessage("", "");
    }

    UN_AGGRESSIVE_INLINE un::RichMessage message(const std::string& header) {
        return un::RichMessage("", header);
    }
}
#endif
