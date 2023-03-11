//
// Created by bruno on 22/05/2022.
//

#ifndef UNNECESSARYENGINE_UNNECESSARY_LOGGER_H
#define UNNECESSARYENGINE_UNNECESSARY_LOGGER_H

#include <sstream>
#include "cppast/diagnostic_logger.hpp"
#include "unnecessary/logging.h"

namespace un {
    class unnecessary_logger : public cppast::diagnostic_logger {
    private:
        std::string msg(const cppast::diagnostic& d) const;

        bool do_log(const char* source, const cppast::diagnostic& d) const override;
    };
}

#endif //UNNECESSARYENGINE_UNNECESSARY_LOGGER_H
