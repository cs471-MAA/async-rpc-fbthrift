#pragma once

#include <glog/logging.h>
#include <regex>
#include <if/gen-cpp2/SanitizationService.h>

namespace mock_message_board {

    class SanitizationHandler : virtual public SanitizationServiceSvIf {
        public:
            bool sanitize_message(std::unique_ptr<::std::string> client_id, std::unique_ptr<::std::string> message) override {
                std::regex match_expr("^.*[\\/*$^].*$");
                if (std::regex_match(*message, match_expr)) {
                    LOG(INFO) << "server: message is invalid";
                    return false;
                }

                // TODO: bool store_message(1: string client_id, 2: string message)

                return true;
            }
    };

} // namespace mock_message_board