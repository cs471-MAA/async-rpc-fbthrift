// include "fb303/thrift/fb303_core.thrift"
include "common/fb303/if/fb303.thrift"

namespace cpp2 mock_message_board

// service MessageService extends fb303_core.BaseService {
service MessageService extends fb303.FacebookService {
  string find_last_message(1: string client_id);
  bool send_message(1:string client_id, 2:string message);  
}

service SanitizationService {
  bool sanitize_message(1: string client_id, 2 :string message);
}

service MockDatabase {
  string find_last_message(1: string client_id);
  bool store_message(1:string client_id, 2:string message);
}