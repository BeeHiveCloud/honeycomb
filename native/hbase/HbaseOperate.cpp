#include "HbaseOperate.h"

using namespace std;

CHbaseOperate::CHbaseOperate() :
socket((TSocket*)NULL), transport((TBufferedTransport*)NULL), protocol((TBinaryProtocol*)NULL), client(NULL), hbaseServicePort(9090), isConnected(false)
{
}

CHbaseOperate::~CHbaseOperate()
{
  if (isConnected) {
    disconnect();
  }
  if (NULL != client) {
    delete client;
    client = NULL;
  }
}

/**
 * Connect Hbase.
 *
 */
bool CHbaseOperate::connect()
{
  if (isConnected) {
    printf("%s \n", "Already connected, don't need to connect it again");
    return true;
  }

  try {
    socket.reset(new TSocket(hbaseServiceHost, hbaseServicePort));
    transport.reset(new TBufferedTransport(socket));
    protocol.reset(new TBinaryProtocol(transport));

    client = new HbaseClient(protocol);

    transport->open();
  } catch (const TException &tx) {
    printf("Connect Hbase error : %s \n", tx.what());
    return false;
  }

  isConnected = true;
  return isConnected;
}

/**
 * Connect Hbase.
 *
 */
bool CHbaseOperate::connect(std::string host, int port)
{
  hbaseServiceHost = host;
  hbaseServicePort = port;

  return connect();
}

/**
 * Disconnect from Hbase.
 *
 */
bool CHbaseOperate::disconnect()
{
  if (!isConnected) {
    printf("%s \n", "Haven't connected to Hbase yet, can't disconnect from it");
    return false;
  }

  if (NULL != transport) {
    try {
      transport->close();
    } catch (const TException &tx) {
      printf("Disconnect Hbase error : %s \n", tx.what());
      return false;
    }
  } else {
    return false;
  }

  isConnected = false;
  return true;
}

/**
 * Put a row to Hbase.
 *
 * @param tableName   [IN] The table name.
 * @param rowKey      [IN] The row key.
 * @param column      [IN] The "column family : qualifier".
 * @param rowValue    [IN] The row value.
 * @return True for successfully put the row, false otherwise.
 */
bool CHbaseOperate::putRow(const string &tableName, const string &rowKey, const string &column, const string &rowValue)
{
  if (!isConnected) {
    printf("%s \n", "Haven't connected to Hbase yet, can't put row");
    return false;
  }

  try {
    std::vector<Mutation> mutations;
    mutations.push_back(Mutation());
    mutations.back().column = column;
    mutations.back().value = rowValue;
	const std::map<Text, Text> attributes;
	client->mutateRow(tableName, rowKey, mutations, attributes);

  } catch (const TException &tx) {
    printf("Operate Hbase error : %s \n", tx.what());
    return false;
  }

  return true;
}

/**
 * Get a Hbase row.
 *
 * @param result      [OUT] The object which contains the returned data.
 * @param tableName   [IN] The Hbase table name, e.g. "MyTable".
 * @param rowKey      [IN] The Hbase row key, e.g. "kdr23790".
 * @param columnName  [IN] The "column family : qualifier".
 * @return True for successfully get the row, false otherwise.
 */
bool CHbaseOperate::getRow(hbaseRet &result, const std::string &tableName, const std::string &rowKey, const std::string &columnName)
{
  if (!isConnected) {
    printf("%s \n", "Haven't connected to Hbase yet, can't read data from it");
    return false;
  }

  std::vector<std::string> columnNames;
  columnNames.push_back(columnName);
  const std::map<Text, Text> attributes;

  std::vector<TRowResult> rowResult;
  try {
	  client->getRowWithColumns(rowResult, tableName, rowKey, columnNames, attributes);
  } catch (const TException &tx) {
    printf("Operate Hbase error : %s \n", tx.what());
    return false;
  }

  if (0 == rowResult.size()) {
    printf("Got no record with the key : [%s] \n",  rowKey);
    return false;
  }

  std::map<std::string, TCell>::const_iterator it = rowResult[rowResult.size() -1].columns.begin();
  result.rowValue = it->second.value;
  result.ts = it->second.timestamp;

  return true;
}
