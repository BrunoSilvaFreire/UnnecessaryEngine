//
// Created by bruno on 06/12/2021.
//

#include <gtest/gtest.h>
#include <unnecessary/misc/templates.h>
#include "unnecessary/memory/membuf.h"
#include "unnecessary/misc/types.h"

TEST(templates, for_types_test) {
    un::for_types<un::Buffer, std::runtime_error, std::string, double, float, int>(
        []<typename T>() {
            std::cout << "Type: " << un::type_name_of<T>() << std::endl;
        }
    );
}