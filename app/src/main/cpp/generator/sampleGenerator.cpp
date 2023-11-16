//
// Created by eszdman on 16.11.2023.
//
#include "Halide.h"

using namespace Halide;

class SampleGenerator : public Halide::Generator<SampleGenerator> {
public:
    Input<Buffer<uint16_t, 1>> input{"input"};
    Output<Buffer<uint16_t, 1>> output{"output"};

    void generate() {
        Var x, y;
        output(x) = input(x)*2;
        output.set_estimates({{0, 256}});
        input.set_estimates({{0, 256}});
        if(!using_autoscheduler()) {
            output.compute_root();
        }
    }
};

HALIDE_REGISTER_GENERATOR(SampleGenerator, sample)