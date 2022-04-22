add_unnecessary_library(
    unnecessary_application

    MODULE engine
    SOURCES
    include/unnecessary/application/application.h
    include/unnecessary/application/validator.h
    src/unnecessary/application/application.cpp
    src/unnecessary/application/validator.cpp

    DEPENDENCIES
    unnecessary_core
)