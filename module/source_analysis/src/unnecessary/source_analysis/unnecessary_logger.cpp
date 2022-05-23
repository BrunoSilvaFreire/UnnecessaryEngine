#include "unnecessary/source_analysis/unnecessary_logger.h"

namespace un {


    std::string UnnecessaryLogger::msg(const cppast::diagnostic& d) const {
        std::stringstream ss;
        const type_safe::optional<std::string>& file = d.location.file;
        ss << "cppast: " << d.message;
        ss << " (";
        if (file.has_value()) {
            ss << file.value();
        }
        ss << ")";
        const auto& line = d.location.line;
        if (line.has_value()) {
            ss << "#" << line.value();
        }
        return ss.str();
    }

    bool UnnecessaryLogger::do_log(const char* source, const cppast::diagnostic& d) const {

        switch (d.severity) {

            case cppast::severity::debug:
                LOG(VERBOSE) << msg(d);
                break;
            case cppast::severity::info:
                LOG(INFO) << msg(d);
                break;
            case cppast::severity::warning:
                LOG(WARN) << msg(d);
                break;
            case cppast::severity::error:
            case cppast::severity::critical:
                LOG(FUCK) << msg(d);
                break;
        }
        return true;
    }
}