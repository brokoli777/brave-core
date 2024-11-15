/* Copyright (c) 2024 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

import * as mojom from 'gen/ui/webui/resources/cr_components/searchbox/searchbox.mojom.m'

let instance: SearchBoxProxy | null = null

export class SearchBoxProxy {
  callbackRouter: mojom.PageCallbackRouter
  handler: mojom.PageHandlerRemote

  constructor(callbackRouter: mojom.PageCallbackRouter,
              handler: mojom.PageHandlerRemote) {
    this.callbackRouter = callbackRouter
    this.handler = handler
  }

  static getInstance(): SearchBoxProxy {
    if (!instance) {
      const callbackRouter = new mojom.PageCallbackRouter()
      const handler = mojom.PageHandler.getRemote()
      handler.setPage(callbackRouter.$.bindNewPipeAndPassRemote())
      instance = new SearchBoxProxy(callbackRouter, handler)
    }
    return instance
  }
}
