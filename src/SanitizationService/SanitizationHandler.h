#pragma once

#include <glog/logging.h>
#include <if/gen-cpp2/SanitizationService.h>

namespace mock_message_board {

    class SanitizationHandler : virtual public SanitizationServiceSvIf {
        public:
            bool sanitize_message(std::unique_ptr<::std::string> client_id, std::unique_ptr<::std::string> message) override {
                LOG(INFO) << "server: received call: client_id=" << *client_id << " | message=" << *message;
                return true;
            }
    };

} // namespace mock_message_board