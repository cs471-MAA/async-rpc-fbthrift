#pragma once

#include <glog/logging.h>
#include "if/gen-cpp2/ExampleService.h"


namespace example {
    class ExampleHandler : public ExampleServiceSvIf {
    public:
        int32_t get_number(int32_t n) override {
            LOG(INFO) << "server: receive " << n;
            return n * 2;
        }
    };
}
