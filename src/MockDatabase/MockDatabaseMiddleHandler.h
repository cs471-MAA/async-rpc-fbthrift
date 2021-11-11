#pragma once

#include <glog/logging.h>
#include <thread>
#include "if/gen-cpp2/MockDatabase.h"
#include "Utils.h"


using namespace std;

namespace mock_message_board {
    class MockDatabaseMiddleHandler : public MockDatabaseSvIf {
    private:
        unordered_map<string, string> last_messages;
        unique_ptr<MockDatabaseAsyncClient> middle_client;
    public:
        
        MockDatabaseMiddleHandler() {
            folly::EventBase eb;
            folly::SocketAddress addr("mock-database", 10001, true);
            // folly::SocketAddress addr("127.0.0.1", 10001, true);

            // creating client
            middle_client = newMockDatabaseRocketClient(&eb, addr);
            
        }

        void find_last_message(string& result, unique_ptr<string> client_id) {
            // this_thread::sleep_for(1000ms);
            middle_client->sync_find_last_message(result, *client_id);
        }

        // folly::Future<std::unique_ptr<::std::string>> future_find_last_message(std::unique_ptr<::std::string> p_client_id) {
        //     this_thread::sleep_for(1000ms);
        //     return middle_client->future_find_last_message(p_client_id);
        // }
        // folly::SemiFuture<std::unique_ptr<::std::string>> semifuture_find_last_message(std::unique_ptr<::std::string> p_client_id) override;
        // void async_tm_find_last_message(std::unique_ptr<apache::thrift::HandlerCallback<std::unique_ptr<::std::string>>> callback, std::unique_ptr<::std::string> p_client_id) override;
        
        bool store_message(unique_ptr<string> client_id, unique_ptr<string> message) {
            // this_thread::sleep_for(1000ms);
            return middle_client->sync_store_message(*client_id, *message);
        }
        
        // folly::Future<bool> future_store_message(std::unique_ptr<::std::string> p_client_id, std::unique_ptr<::std::string> p_message) {
        //     this_thread::sleep_for(1000ms);
        //     return middle_client->future_store_message(p_client_id, p_message);
        // }
        // folly::SemiFuture<bool> semifuture_store_message(std::unique_ptr<::std::string> p_client_id, std::unique_ptr<::std::string> p_message) override;
        // void async_tm_store_message(std::unique_ptr<apache::thrift::HandlerCallback<bool>> callback, std::unique_ptr<::std::string> p_client_id, std::unique_ptr<::std::string> p_message) override;
        
    };
}
