add_unnecessary_module(
    unnecessary_graphs

    SOURCES
    include/unnecessary/graphs/dependency_graph.h
    src/unnecessary/graphs/dependency_graph.cpp
    DEPENDENCIES
    unnecessary_core
)


target_link_libraries(
    unnecessary_graphs
    PUBLIC
    grapphs
)
