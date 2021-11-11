
#include "if/gen-cpp2/MessageService.h"

namespace mock_message_board {
    class MessageServiceHandler : public MessageServiceSvIf {
    public:
        MessageServiceHandler();
        virtual ~MessageServiceHandler();

        virtual void find_last_message(::std::string& /*_return*/, std::unique_ptr<::std::string> /*client_id*/);
        virtual bool send_message(std::unique_ptr<::std::string> /*client_id*/, std::unique_ptr<::std::string> /*message*/);
    private:
        bool bidule;
    };
}