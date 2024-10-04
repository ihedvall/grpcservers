/*
 * Copyright 2024 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#pragma once
#include <memory>
#include <workflow/itaskfactory.h>

namespace serv {

class ServFactory : public workflow::ITaskFactory {
 public:

  [[nodiscard]] std::unique_ptr<workflow::ITask> CreateTask(
      const workflow::ITask& source) const override;


   static ServFactory& Instance();
 protected:
   ServFactory();
};

} // end namespace



