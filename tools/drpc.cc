// Copyright (c) 2015 SHUMEI Inc. All rights reserved.
// Authors: Liang Kun <liangkun@ishumei.com>.
#include <cstdlib>
#include <iostream>
#include <string>
#include "boost/shared_ptr.hpp"
#include "thrift/Thrift.h"
#include "thrift/transport/TSocket.h"
#include "thrift/transport/TBufferTransports.h"
#include "thrift/protocol/TBinaryProtocol.h"
#include "DistributedRPC.h"

using std::atoi;
using std::cerr;
using std::cout;
using std::endl;
using std::getenv;
using std::string;
using boost::shared_ptr;
using apache::thrift::TException;
using apache::thrift::transport::TFramedTransport;
using apache::thrift::transport::TSocket;
using apache::thrift::protocol::TBinaryProtocol;

int main(int argc, const char **argv) {
    if (argc != 4) {
        cerr << "Usage: " << argv[0] << " server function args" << endl;
        return -1;
    }

    string server(argv[1]);
    string function(argv[2]);
    string args(argv[3]);
    int port = 3772;

    auto colon = server.find(':');
    if (colon != string::npos) {
        port = atoi(server.substr(colon + 1).c_str());
        server = server.substr(0, colon);
    }

    const char *conn_timeout = getenv("DRPC_CONN_TIMEOUT");
    const char *send_timeout = getenv("DRPC_SEND_TIMEOUT");
    const char *recv_timeout = getenv("DRPC_RECV_TIMEOUT");

    shared_ptr<TSocket> socket(new TSocket(server, port));
    socket->setConnTimeout(conn_timeout ? atoi(conn_timeout) : 1000);
    socket->setSendTimeout(send_timeout ? atoi(send_timeout) : 1000);
    socket->setRecvTimeout(recv_timeout ? atoi(recv_timeout) : 1000);
    shared_ptr<TFramedTransport> transport(new TFramedTransport(socket));
    shared_ptr<TBinaryProtocol> protocol(new TBinaryProtocol(transport));
    DistributedRPCClient client(protocol);

    try {
        socket->open();
        string response;
        client.execute(response, function, args);
        cout << response << endl;
        socket->close();
    } catch (TException &e) {
        cerr << e.what() << endl;
        return -1;
    }

    return 0;
}