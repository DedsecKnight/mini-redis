#include "include/client.h"

#include <cstdio>
#include <cstdlib>

#include "include/protocol/request.h"
#include "utilities.cpp"

namespace mini_redis {
int client::connect_to(std::string_view hostname, std::string_view port) {
  return client_.connect_to(hostname, port);
}

void client::run() const noexcept {
  lib::protocol::request req, req2, req3, req4;
  std::vector<lib::protocol::request> requests{
      utilities::construct_request(std::string_view{"set"},
                                   std::string_view{"test_key"},
                                   std::string_view{"something different"}),
      utilities::construct_request(std::string_view{"get"},
                                   std::string_view{"test_key"}),
      utilities::construct_request(std::string_view{"del"},
                                   std::string_view{"test_key"}),
      utilities::construct_request(std::string_view{"get"},
                                   std::string_view{"test_key"}),
      utilities::construct_request(std::string_view{"hello"},
                                   std::string_view{"world"}),
      utilities::construct_request(
          std::string_view{"zadd"}, std::string_view{"test_set"},
          std::string_view{"12.50"}, std::string_view{"test_member"}),
      utilities::construct_request(std::string_view{"zscore"},
                                   std::string_view{"test_set"},
                                   std::string_view{"test_member"}),
      utilities::construct_request(std::string_view{"zrem"},
                                   std::string_view{"test_set"},
                                   std::string_view{"test_member"}),
      utilities::construct_request(std::string_view{"zscore"},
                                   std::string_view{"test_set"},
                                   std::string_view{"test_member"}),
  };
  for (const auto& req : requests) {
    if (client_.send_request(req) == -1) {
      fprintf(stderr, "something is wrong with req1\n");
      exit(1);
    }
    auto response = client_.get_next_response();
    if (!response.has_value()) {
      fprintf(stderr, "error receiving data for req1 from server");
      exit(1);
    }
    printf("from server: %s\n", response->to_string().data());
  }
}
}  // namespace mini_redis