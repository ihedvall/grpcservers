/*
* Copyright 2023 Ingemar Hedvall
* SPDX-License-Identifier: MIT
 */

#include "testsyslogrunner.h"
#include <string>
#include <filesystem>
#include <chrono>
#include <util/logconfig.h>
#include "serv/servfactory.h"
#include "ods/imodel.h"
#include "sysloginserter.h"
#include "syslogrpcserver.h"
#include "syslogrpcclient.h"
#include <source_location>
#include <ods/odsfactory.h>

using namespace std::filesystem;
using namespace std::chrono_literals;
using namespace util::log;
using namespace workflow;
using namespace util::syslog;
using namespace ods;

namespace {
  std::string kTestLogDir;
  std::string kTestDir;
  std::string kModelFile;
  std::string kDbFile;
  constexpr std::string_view kModelPath = "test/eventlogdb.xml";
  bool kSkipTest = false;
  std::unique_ptr<ods::IDatabase> kDatabase;
}

namespace serv::test {
void TestSyslogRunner::SetUpTestSuite() {
  try {
    path root_dir = temp_directory_path();
    std::cout << "Temp Path: " << root_dir.string() << std::endl;
    root_dir.append("test");
    create_directories(root_dir);
    kTestLogDir = root_dir.string();
    std::cout << "Log Path: " << kTestLogDir << std::endl;

    root_dir.append("ods");
    // Remove previous test directory is done here not in tear down
    // function, so we can inspect the generated files after the test is done.
    std::error_code err;
    remove_all(root_dir, err); // Don't want any exception here
    create_directories(root_dir);
    kTestDir = root_dir.string();
    std::cout << "Test Path: " << kTestDir << std::endl;

    auto location = std::source_location::current();
    path source_file(location.file_name());
    std::cout << "Source File: " << source_file.string() << std::endl;

    // Note 2 parent_path call to step up one level in source directory
    path source_path = source_file.parent_path().parent_path();
    std::cout << "Source Path: " << source_path.string() << std::endl;

    // Check that model file exists.

    path model_file = source_path;
    model_file.append(kModelPath);
    kModelFile = model_file.string();
    std::cout << "Model File: " << kModelFile << std::endl;
    if (!exists(kModelFile)) {
      throw std::runtime_error("Model file doesn't exist.");
    }
  } catch (const std::exception& err) {
    kSkipTest = true;
    std::cout << "Skip test is active due to file/dir error. Error: "
              << err.what();
  }
  auto &log_config = LogConfig::Instance();
  log_config.RootDir(kTestLogDir);
  log_config.BaseName("test_syslog_runner.log");
  log_config.Type(util::log::LogType::LogToFile);
  log_config.CreateDefaultLogger();

  log_config.AddLogger("Console", LogType::LogToConsole, {});

  // Create the database
  try {
    if (kSkipTest) {
      throw std::runtime_error("Didn't create the database due to skip test.");
    }
    path db_file(kTestDir);
    db_file.append("eventlogdb.sqlite");
    kDbFile = db_file.string();
    std::cout << "DB Name: " << kDbFile << std::endl;
    const auto& ods_factory = OdsFactory::Instance();
    kDatabase = std::move(ods_factory.CreateDatabase(DbType::TypeSqlite));
    kDatabase->ConnectionInfo(kDbFile);

    IModel model;
    const auto read = model.ReadModel(kModelFile);
    if (!read) {
      throw std::runtime_error("Failed to read the model file.");
    }
    std::cout << "Model Name: " << model.Name() << std::endl;

    const auto create = kDatabase->Create(model);
    if (!create) {
      throw std::runtime_error("Failed to create the database.");
    }
    if (!exists(kDbFile)) {
      throw std::runtime_error("The database file doesn't exist.");
    }
    std::cout << "Created: " << model.Name() << std::endl;
  } catch (const std::exception& err) {
    kSkipTest = true;
    std::cout << "Skip test is active due to database creation error. Error: "
      << err.what();
  }
}

void TestSyslogRunner::TearDownTestSuite() {
  kDatabase.reset();
  auto &log_config = LogConfig::Instance();
  log_config.DeleteLogChain();
}

TEST_F(TestSyslogRunner, TestInsert) {
  if (kSkipTest || !kDatabase) {
    GTEST_SKIP_("Skipped the inserter test");
  }
  SyslogInserter inserter;
  std::ostringstream arg;
  arg << "--connection=\"" << kDatabase->ConnectionInfo();
  inserter.Arguments(arg.str());
  inserter.Init();
  ASSERT_TRUE(inserter.IsOk()) << inserter.LastError();
  SyslogMessage msg1;
  msg1.Severity(SyslogSeverity::Informational);
  msg1.Message("Msg1");
  EXPECT_TRUE(inserter.AddOneMessage(msg1));
  const auto count = inserter.GetNofMessages();
  std::cout << "Nof Messages: " << count << std::endl;
  EXPECT_GT(inserter.GetNofMessages(),0);
  inserter.Exit();
}

TEST_F(TestSyslogRunner, TestRpcServer) {
  if (kSkipTest || !kDatabase) {
    GTEST_SKIP_("Skipped the inserter test");
  }
  SyslogInserter inserter;
  std::ostringstream arg;
  arg << "--connection=\"" << kDatabase->ConnectionInfo();
  inserter.Arguments(arg.str());
  inserter.Init();

  SyslogRpcServer server;
  server.Arguments(arg.str());
  server.Init();
  ASSERT_TRUE(server.IsOk()) << server.LastError();

  SyslogMessage msg;
  msg.Severity(SyslogSeverity::Informational);
  msg.Message("Test of RPC");

  SyslogRpcClient client;
  client.Start();

  EXPECT_TRUE(client.Operable());
  auto last1 = client.GetLastEvent();
  EXPECT_TRUE(client.Operable());

  EXPECT_TRUE(inserter.AddOneMessage(msg));

  auto last2 = client.GetLastEvent();
  EXPECT_TRUE(client.Operable());
  EXPECT_NE(last1.Index(), last2.Index());

  std::cout << "Last1: " << last1.Message() << std::endl;
  std::cout << "Last2: " << last2.Message() << std::endl;

  size_t db_count = inserter.GetNofMessages();
  size_t nof_msg = client.GetCount();
  EXPECT_GT(nof_msg, 0);
  EXPECT_EQ(nof_msg, db_count);
  std::cout << "Nof Msg: " << nof_msg << std::endl;

  SyslogList event_list;
  client.GetEventList(event_list);
  EXPECT_EQ(nof_msg, event_list.size());

  for (const auto& event : event_list) {
    std::cout << "ID: " << event.Index() << ", TEXT: "
              << event.Message() << std::endl;
  }

  SyslogList syslog_list;
  client.GetSyslogList(syslog_list);
  EXPECT_EQ(nof_msg, syslog_list.size());

  for (const auto& syslog : syslog_list) {
    std::cout << "ID: " << syslog.Index() << ", TEXT: "
              << syslog.Message() << std::endl;
  }

  client.AddEvent(msg);
  const auto nof_add_msg = client.GetCount();
  EXPECT_EQ(nof_add_msg, nof_msg + 1);

  client.Stop();
  server.Exit();
  inserter.Exit();
}

} // namespace ods::test