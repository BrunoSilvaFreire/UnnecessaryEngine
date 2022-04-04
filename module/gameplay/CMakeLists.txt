add_unnecessary_module(
        unnecessary_gameplay

        SOURCES
        include/unnecessary/simulation/world.h
        include/unnecessary/simulation/simulation_graph.h
        include/unnecessary/simulation/jobs/simulation_worker.h
        include/unnecessary/simulation/transforms/components.h
        include/unnecessary/simulation/transforms/compute_matrices.h

        src/unnecessary/simulation/world.cpp
        src/unnecessary/simulation/simulation_graph.cpp
        src/unnecessary/simulation/jobs/simulation_worker.cpp
        src/unnecessary/simulation/transforms/components.cpp
        src/unnecessary/simulation/transforms/compute_matrices.cpp
        src/unnecessary/simulation/systems/transform_system.cpp

        DEPENDENCIES
        unnecessary_jobs
        unnecessary_application
        EnTT::EnTT
        glm::glm
)