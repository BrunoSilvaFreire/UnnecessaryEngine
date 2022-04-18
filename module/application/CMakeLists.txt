add_unnecessary_library(
    unnecessary_application

    MODULE engine
    SOURCES
    include/unnecessary/application/application.h
    src/unnecessary/application/application.cpp

    DEPENDENCIES
    unnecessary_core
)