//
// Created by bruno on 06/12/2021.
//

#include <gtest/gtest.h>

#include <unnecessary/misc/templates.h>
#include <unnecessary/memory/membuf.h>
#include <unnecessary/misc/types.h>
#include <unnecessary/logging.h>

TEST(templates, for_types_test) {
    un::for_types<un::byte_buffer, std::runtime_error, std::string, double, float, int>(
        []<typename T>() {
            LOG(INFO) << "Type: " << GREEN(un::type_name_of<T>());
        }
    );
}
