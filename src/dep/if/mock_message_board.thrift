include "common/fb303/if/fb303.thrift"

namespace cpp2 mock_message_board

struct MessageResponse {
  1: string message;
  2: i64 query_uid;
}

struct StatusResponse {
  1: bool ok;
  2: i64 query_uid;
}

service MessageService extends fb303.FacebookService {
  MessageResponse find_last_message(1: string client_id, 2:i64 query_uid);
  StatusResponse send_message(1:string client_id, 2:string message, 3:i64 query_uid);  
}

service SanitizationService {
  bool sanitize_message(1: string client_id, 2 :string message, 3:i64 query_uid);
}

service MockDatabase {
  string find_last_message(1: string client_id, 2:i64 query_uid);
  bool store_message(1:string client_id, 2:string message, 3:i64 query_uid);
}