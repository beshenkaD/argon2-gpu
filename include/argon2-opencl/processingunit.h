#ifndef ARGON2_OPENCL_PROCESSINGUNIT_H
#define ARGON2_OPENCL_PROCESSINGUNIT_H

#include <memory>

#include "kernelrunner.h"

namespace argon2 {
namespace opencl {

class ProcessingUnit
{
private:
    const ProgramContext *programContext;
    const Argon2Params *params;
    const Device *device;

    KernelRunner runner;
    std::uint32_t bestLanesPerBlock;
    std::uint32_t bestJobsPerBlock;

public:
    size_t getBatchSize() const { return runner.getBatchSize(); }

    ProcessingUnit(
            const ProgramContext *programContext, const Argon2Params *params,
            const Device *device, size_t batchSize,
            bool bySegment = true, bool precomputeRefs = false);

    void setPassword(size_t index, const void *pw, size_t pwSize);
    void getHash(size_t index, void *hash);

    void beginProcessing();
    void endProcessing();
};

} // namespace opencl
} // namespace argon2

#endif // ARGON2_OPENCL_PROCESSINGUNIT_H
