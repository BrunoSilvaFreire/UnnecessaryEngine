#ifndef UNNECESSARYENGINE_LIST_PRINTER_H
#define UNNECESSARYENGINE_LIST_PRINTER_H

#include <sstream>
#include <string>
#include <vector>

namespace un {
    template<typename TValue>
    std::string prettify(const std::string& header, const std::vector<TValue>& vector) {
        std::stringstream ss;
        ss << header;
        if (vector.empty()) {
            ss << " (Empty)";
            return ss.str();
        }
        ss << " (" << vector.size() << " entries)" << std::endl;
        for (std::size_t i = 0; i < vector.size(); ++i) {
            bool last = i == vector.size() - 1;
            if (i == 0) {
                if (last) {
                    ss << "─";
                }
                else {
                    ss << "┬";
                }
            }
            else if (last) {
                ss << "└";
            }
            else {
                ss << "├";
            }
            ss << "[" << i << "] " << vector[i];
            if (!last) {
                ss << std::endl;
            }
        }
        return ss.str();
    }
}
#endif //UNNECESSARYENGINE_LIST_PRINTER_H
