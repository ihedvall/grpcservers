/*
* Copyright 2023 Ingemar Hedvall
* SPDX-License-Identifier: MIT
 */

#pragma once

#include <workflow/itask.h>
#include "ods/imodel.h"
#include "ods/idatabase.h"
#include <util/syslogmessage.h>

#include "sysloginserter.h"
#include <thread>

namespace grpc {
class Server;
}
namespace serv {

class SyslogRpcServer : public workflow::ITask {
public:
  SyslogRpcServer();
  explicit SyslogRpcServer(const ITask& source);
  ~SyslogRpcServer() override;

  void Init() override;
  void Exit() override;
  [[nodiscard]] const ods::IModel& GetModel() { return model_;}
  [[nodiscard]] ods::IDatabase* GetDatabase() { return database_.get();}
  [[nodiscard]] util::syslog::SyslogMessage LastMessage() const;
private:
  size_t data_slot_ = 0; ///< The data slot stores the last syslog message.
  std::string db_type_ = "SQLite"; ///< Type of database.
  std::string connection_string_; ///< File name or connection string
  uint16_t server_port_ = 50600; ///< Default port is 50600.

  std::unique_ptr<ods::IDatabase> database_;
  ods::IModel model_;
  const ods::SyslogInserter* inserter_ = nullptr;
  std::unique_ptr<grpc::Server> server_;
  std::thread service_thread_;

  void ParseArguments();
  void StartThread();
  void StopThread();
  void WorkerThread(); ///< Thread that handle the server.
};


} // namespace ods
