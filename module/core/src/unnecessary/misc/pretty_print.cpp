#include <unnecessary/misc/pretty_print.h>

#include <utility>
#include <algorithm>

namespace un {

    RichMessage::RichMessage(
        std::string indent,
        std::string header
    ) : _indent(std::move(indent)),
        _header(std::move(header)) { }


    RichMessage::~RichMessage() {
        if (_header.empty()) {
            write(LOG(_level));
        } else {
            write(un::Log(_level, _header).stream());
        }

    }

    void TextMessage::write(std::ostream& stream) const {
        stream << _text.str();
    }

    void TreeMessage::prefix(
        std::ostream& stream,
        std::size_t i,
        std::size_t lineNbr,
        const std::unique_ptr<IMessage>& msg
    ) const {
        bool last = i == _children.size() - 1;
        bool isMidsection = lineNbr != 0;
        if (last && isMidsection) {
            stream << "    ";
            return;
        }
        if (isMidsection) {
            stream << "│   ";
            return;
        }
        if (i == 0) {
            if (last) {
                stream << "─";
            } else {
                stream << "┬";
            }
        } else if (last) {
            stream << "└";
        } else {
            stream << "├";
        }
        bool isSubtree = dynamic_cast<un::TreeMessage*>(msg.get()) != nullptr;
        stream << '[' << i << ']';
        if (!isSubtree) {
            stream << ' ';
        }
    }

    void CompositeMessage::write(std::ostream& stream) const {
        for (std::size_t i = 0; i < _children.size(); ++i) {
            const auto& child = _children[i];
            std::stringstream sub;
            child->write(sub);
            auto lines = un::split_string(sub.str(), un::new_line());
            for (std::size_t j = 0; j < lines.size(); ++j) {
                const auto& line = lines[j];
                prefix(stream, i, j, child);
                stream << line << std::endl;
            }
        }
    }

    un::TextMessage& CompositeMessage::text() {
        return dynamic_cast<TextMessage&>(*_children.emplace_back(std::make_unique<un::TextMessage>()));
    }

    std::size_t CompositeMessage::num_children() const {
        return _children.size();
    }

    un::TreeMessage& CompositeMessage::tree() {
        return dynamic_cast<TreeMessage&>(*_children.emplace_back(std::make_unique<un::TreeMessage>()));
    }

    void RichMessage::prefix(
        std::ostream& stream,
        std::size_t i,
        std::size_t lineNbr,
        const std::unique_ptr<IMessage>& msg
    ) const {
        stream << _indent;
    }

    void RichMessage::setLevel(const std::string& level) {
        _level = level;
    }

    void RichMessage::setHeader(const std::string& header) {
        _header = header;
    }

}