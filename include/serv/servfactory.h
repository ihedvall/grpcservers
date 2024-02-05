/*
 * Copyright 2024 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#pragma once
#include <memory>
#include <workflow/irunnerfactory.h>

namespace serv {

class ServFactory : public workflow::IRunnerFactory {
 public:

  [[nodiscard]] std::unique_ptr<workflow::IRunner> CreateRunner(
      const workflow::IRunner& source) const override;


   static ServFactory& Instance();
 protected:
   ServFactory();
};

} // end namespace



