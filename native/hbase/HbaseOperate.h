#ifndef __HBASE_OPERATE_H
#define __HBASE_OPERATE_H

#include <string>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>
#include "Hbase.h"

using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;
using namespace apache::hadoop::hbase::thrift;

typedef struct hbaseRet {
  std::string rowValue;
  time_t ts;

  hbaseRet() {
    ts = 0;
  }

} hbaseRet;

class CHbaseOperate
{
public:
  CHbaseOperate();
  virtual ~CHbaseOperate();

private:
  boost::shared_ptr<TTransport> socket;
  boost::shared_ptr<TTransport> transport;
  boost::shared_ptr<TProtocol> protocol;

  HbaseClient *client;

  std::string  hbaseServiceHost;
  int     hbaseServicePort;
  bool    isConnected;

public:
  bool  connect();

  bool  connect(std::string host, int port);

  bool  disconnect();

  bool  putRow(const std::string &tableName,
              const std::string &rowKey,
              const std::string &column,
              const std::string &rowValue);

  bool  getRow(hbaseRet &result,
              const std::string &tableName,
              const std::string &rowKey,
              const std::string &columnName);
};

#endif
