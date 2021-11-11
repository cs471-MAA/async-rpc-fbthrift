#pragma once

#include <glog/logging.h>
#include <unordered_map>
#include <thread>
#include "if/gen-cpp2/MockDatabase.h"
#include <iostream>
#include <chrono>
#include <ctime>
using namespace std;

namespace mock_message_board {
    class MockDatabaseHandler : public MockDatabaseSvIf {
    private:
        unordered_map<string, string> last_messages;
        chrono::time_point<chrono::system_clock> start;
    public:
        
        MockDatabaseHandler() : start(std::chrono::system_clock::now()){}

        void find_last_message(string& result, unique_ptr<string> client_id){
            auto end = std::chrono::system_clock::now();
            std::chrono::duration<double> elapsed_seconds = end - start;
            cout << "mock-database|find_last_message: received find last message rpc with " << *client_id << " at " << elapsed_seconds.count() << "s." << endl;
            
            this_thread::sleep_for(10000ms);

            if (last_messages.find(*client_id) == last_messages.end()){
            // if (last_messages.contains(*client_id)){
                result = "WARNING: This user " + *client_id + " didn't post any message or doesn't exit...";
            } else {
                result = last_messages[*client_id];
            }


            end = std::chrono::system_clock::now();
            elapsed_seconds = end - start;
            cout << "mock-database|find_last_message: rpc treated - to find " << *client_id << "'s message at " << elapsed_seconds.count() << "s." << endl;
        }
        // folly::Future<std::unique_ptr<::std::string>> future_find_last_message(std::unique_ptr<::std::string> p_client_id) override;
        // folly::SemiFuture<std::unique_ptr<::std::string>> semifuture_find_last_message(std::unique_ptr<::std::string> p_client_id) override;
        // void async_tm_find_last_message(std::unique_ptr<apache::thrift::HandlerCallback<std::unique_ptr<::std::string>>> callback, std::unique_ptr<::std::string> p_client_id) override;
        
        bool store_message(unique_ptr<string> client_id, unique_ptr<string> message) {
            cout << "mock-database|store_message: received store message rpc with " << *client_id << endl;
            this_thread::sleep_for(10000ms);

            last_messages[*client_id] = *message;
            
            cout << "mock-database|store_message: rpc treated - to store " << *message  << " of " << *client_id <<  endl;
            return true;
        }
        // folly::Future<bool> future_store_message(std::unique_ptr<::std::string> p_client_id, std::unique_ptr<::std::string> p_message) override;
        // folly::SemiFuture<bool> semifuture_store_message(std::unique_ptr<::std::string> p_client_id, std::unique_ptr<::std::string> p_message) override;
        // void async_tm_store_message(std::unique_ptr<apache::thrift::HandlerCallback<bool>> callback, std::unique_ptr<::std::string> p_client_id, std::unique_ptr<::std::string> p_message) override;
        
    };
}
