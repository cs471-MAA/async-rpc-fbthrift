namespace cpp2 mock_message_board

service MessageService {
  string find_last_message(1: string client_id);
  bool send_message(1:string client_id, 2:string message);  
}

service SanitizationService {
  bool sanitize_message(1: string client_id, 2 :string message);
}

service MockDatabase {
  string find_last_message(1: string client_id);
  bool store_message(1: string client_id, 2: string message);
}