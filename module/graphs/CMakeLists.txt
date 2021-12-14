add_unnecessary_module(
    unnecessary_graphs

    SOURCES
    include/unnecessary/graphs/dependency_graph.h

    DEPENDENCIES
    unnecessary_core
)


target_link_libraries(
    unnecessary_graphs
    PUBLIC
    grapphs
)
