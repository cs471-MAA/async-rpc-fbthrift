
#include "if/gen-cpp2/MessageService.h"

namespace mock_message_board {
    class MessageServiceHandler : public MessageServiceSvIf {
    public:
        MessageServiceHandler(bool bite);
        ~MessageServiceHandler() override;

        void find_last_message(::std::string& result, std::unique_ptr<::std::string> client_id) override;

        bool send_message(std::unique_ptr<::std::string> client_id, std::unique_ptr<::std::string> message) override;
    private:
        bool bidule;
    };
}