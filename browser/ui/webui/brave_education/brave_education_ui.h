// Copyright (c) 2024 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

#ifndef BRAVE_BROWSER_UI_WEBUI_BRAVE_EDUCATION_BRAVE_EDUCATION_UI_H_
#define BRAVE_BROWSER_UI_WEBUI_BRAVE_EDUCATION_BRAVE_EDUCATION_UI_H_

#include <memory>
#include <string>

#include "base/memory/raw_ptr.h"
#include "brave/components/brave_education/common/brave_education.mojom.h"
#include "mojo/public/cpp/bindings/pending_receiver.h"
#include "mojo/public/cpp/bindings/pending_remote.h"
#include "mojo/public/cpp/bindings/receiver.h"
#include "ui/webui/mojo_web_ui_controller.h"

namespace content {
class WebUI;
}

class Profile;

namespace brave_education {

class EducationCommandHandler;

// The Web UI controller for the brave://education page.
class BraveEducationUI : public ui::MojoWebUIController,
                         public mojom::CommandHandlerFactory {
 public:
  BraveEducationUI(content::WebUI* web_ui, const std::string& host_name);
  ~BraveEducationUI() override;

  BraveEducationUI(const BraveEducationUI&) = delete;
  BraveEducationUI& operator=(const BraveEducationUI&) = delete;

  // Instantiates the an instance of mojom::CommandHandlerFactory.
  void BindInterface(
      mojo::PendingReceiver<mojom::CommandHandlerFactory> pending_receiver);

 private:
  // mojom::CommandHandlerFactory
  void CreateEducationCommandHandler(
      mojo::PendingReceiver<mojom::CommandHandler> pending_handler) override;

  mojo::Receiver<mojom::CommandHandlerFactory> handler_factory_receiver_;
  std::unique_ptr<EducationCommandHandler> command_handler_;
  raw_ptr<Profile> profile_;

  WEB_UI_CONTROLLER_TYPE_DECL();
};

}  // namespace brave_education

#endif  // BRAVE_BROWSER_UI_WEBUI_BRAVE_EDUCATION_BRAVE_EDUCATION_UI_H_
