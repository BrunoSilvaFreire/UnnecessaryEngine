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
        stream << '[' << i << ']';
        if (!msg->supports_children()) {
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

    std::size_t CompositeMessage::num_children() const {
        return _children.size();
    }

    un::TextMessage& CompositeMessage::text() {
        return add_child<un::TextMessage>();
    }

    un::TreeMessage& CompositeMessage::tree() {
        return add_child<un::TreeMessage>();
    }

    un::MapMessage& CompositeMessage::map() {
        return add_child<un::MapMessage>();
    }

    bool CompositeMessage::supports_children() {
        return true;
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

    un::TextMessage& MapMessage::text(const std::string& name) {
        return add_child<un::TextMessage>(name);
    }

    un::TreeMessage& MapMessage::tree(const std::string& name) {
        return add_child<un::TreeMessage>(name);
    }

    void MapMessage::write(std::ostream& stream) const {
        std::size_t i = 0;
        for (const auto& [name, child] : _named) {
            std::stringstream sub;
            child->write(sub);
            auto lines = un::split_string(sub.str(), un::new_line());
            for (std::size_t j = 0; j < lines.size(); ++j) {
                const auto& line = lines[j];
                prefix(stream, name, i, j, child);
                stream << line << std::endl;
            }
            i++;
        }
    }

    void MapMessage::prefix(
        std::ostream& stream,
        const std::string& name,
        std::size_t i,
        std::size_t lineNbr,
        const std::unique_ptr<IMessage>& msg
    ) const {
        bool last = i == _named.size() - 1;
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
        stream << ' ' << name << ": ";
    }

    bool MapMessage::supports_children() {
        return true;
    }

    bool IMessage::supports_children() {
        return false;
    }
}