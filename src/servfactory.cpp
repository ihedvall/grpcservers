/*
 * Copyright 2024 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#include "serv/servfactory.h"

#include <memory>
#include <array>

#include <util/stringutil.h>

#include "syslogrpcserver.h"

#include "template_names.icc"

using namespace workflow;
using namespace util::string;

namespace serv {

ServFactory::ServFactory() {
  name_ = "Serv Factory";
  description_ = "Tasks that implements gRPC servers.";

  std::array<std::unique_ptr<IRunner>,1> template_list = {
      std::make_unique<SyslogRpcServer>(),
  };

  for ( auto& templ : template_list) {
    template_list_.emplace(templ->Name(), std::move(templ));
  }
}

ServFactory& ServFactory::Instance() {
  static ServFactory instance;
  return instance;
}

std::unique_ptr<workflow::IRunner> ServFactory::CreateRunner(const workflow::IRunner& source) const {
  std::unique_ptr<IRunner> runner;
  const auto& template_name = source.Template();
  if (IEquals(template_name, kSyslogRpcServer.data())) {
    auto temp = std::make_unique<SyslogRpcServer>(source);
    runner = std::move(temp);
  }
  return runner;
}
}