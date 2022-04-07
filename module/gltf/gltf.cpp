#include <unnecessary/jobs/job_system.h>
#include <unnecessary/jobs/workers/worker.h>
#include <unnecessary/jobs/job_chain.h>
#include <unnecessary/gltf/load_gltf_job.h>
#include <unnecessary/jobs/worker_chain.h>

int main() {
    un::SimpleJobSystem system(4, true);
    {
        un::WorkerChain<un::JobWorker> chain;
        auto dragon = std::filesystem::current_path() / "dragon.glb";
        LOG(INFO) << "Expecting dragon @ " << std::filesystem::absolute(dragon);
        chain.immediately<un::LoadGLTFJob>(dragon, un::GLTFType::eBinary);
    }
    system.finish(true);
}