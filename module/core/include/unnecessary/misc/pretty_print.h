
#ifndef UNNECESSARYENGINE_PRETTY_PRINT_H
#define UNNECESSARYENGINE_PRETTY_PRINT_H

#include <string>
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
    };


    class TextMessage : public IMessage {
    private:
        std::stringstream _text;
    public:
        void write(std::ostream& stream) const override;

        template<typename TAny>
        std::ostream& operator<<(TAny any) {
            return TextMessage::_text << any;
        }
    };

    class TreeMessage;

    class CompositeMessage : public IMessage {
    protected:
        std::vector<std::unique_ptr<IMessage>> _children;

        virtual void prefix(
            std::ostream& stream,
            std::size_t i,
            std::size_t lineNbr,
            const std::unique_ptr<IMessage>& msg
        ) const { }

    public:
        CompositeMessage() = default;

        void write(std::ostream& stream) const override;

        std::size_t num_children() const;

        un::TextMessage& text();

        un::TreeMessage& tree();
    };

    class TreeMessage : public CompositeMessage {
    protected:
        void prefix(
            std::ostream& stream,
            std::size_t i,
            std::size_t lineNbr,
            const std::unique_ptr<IMessage>& msg
        ) const override;
    };

    class RichMessage : public CompositeMessage {
    private:
        std::string _indent;
        std::string _header;
        std::string _level = INFO;
    public:
        explicit RichMessage(std::string indent , std::string header);

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
