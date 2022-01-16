add_unnecessary_module(
    unnecessary_application

    SOURCES
    include/unnecessary/application/application.h
    src/unnecessary/application/application.cpp

    DEPENDENCIES
    unnecessary_core
)