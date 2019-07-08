#include "rpcuri.h"

namespace arch {

// bool RpcUri::Parse(const std::string &uri, RpcUri *result) {
//   typedef std::string::const_iterator iterator_t;

//   if (uri.length() == 0 || !result)
//     return false;

//   iterator_t uri_end = uri.end();

//   // get query start
//   iterator_t query_start = std::find(uri.begin(), uri_end, L'?');

//   // protocol
//   iterator_t protocol_start = uri.begin();
//   iterator_t protocol_end = std::find(protocol_start, uri_end, L':'); //"://");

//   if (protocol_end != uri_end) {
//     std::string prot = &*(protocol_end);
//     if ((prot.length() > 3) && (prot.substr(0, 3) == L"://")) {
//       result->protocol = std::string(protocol_start, protocol_end);
//       protocol_end += 3; //      ://
//     } else
//       protocol_end = uri.begin(); // no protocol
//   } else
//     protocol_end = uri.begin(); // no protocol

//   // host
//   iterator_t hostStart = protocol_end;
//   iterator_t path_start = std::find(hostStart, uri_end, L'/'); // get path_start

//   iterator_t host_end = std::find(
//       protocol_end, (path_start != uri_end) ? path_start : query_start,
//       L':'); // check for port

//   result->host = std::string(hostStart, host_end);

//   // port
//   if ((host_end != uri_end) && ((&*(host_end))[0] == L':')) {
//     host_end++;
//     iterator_t port_end = (path_start != uri_end) ? path_start : query_start;
//     result->port = std::string(host_end, port_end);
//   }

//   // path
//   if (path_start != uri_end)
//     result->service = std::string(path_start, query_start);

//   // query
//   if (query_start != uri_end)
//     result->query = std::string(query_start, uri.end());

//   return bool;
// }

} // namespace arch