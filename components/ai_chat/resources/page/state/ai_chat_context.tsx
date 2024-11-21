// Copyright (c) 2024 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

import * as React from 'react'
import getAPI, * as mojom from '../api'

interface Props {
  // Whether there is a specific conversation selected
  isDefaultConversation: boolean
  // Create a new conversation and use it
  onNewConversation: () => unknown
  // Select a new conversation
  onSelectConversationUuid: (id: string | undefined) => unknown
}

export type AIChatContext = Props & mojom.UIState & {
  goPremium: () => void
  managePremium: () => void
  handleAgreeClick: () => void
  markStorageNoticeViewed: () => void
  dismissStorageNotice: () => void
  dismissPremiumPrompt: () => void
  userRefreshPremiumSession: () => void
  uiHandler?: mojom.AIChatUIHandlerRemote

  editingConversationId: string | null
  setEditingConversationId: (uuid: string | null) => void
}

const defaultContext: AIChatContext = {
  ...mojom.defaultUIState,
  isDefaultConversation: true,
  goPremium: () => { },
  managePremium: () => { },
  handleAgreeClick: () => { },
  markStorageNoticeViewed: () => { },
  dismissStorageNotice: () => { },
  dismissPremiumPrompt: () => { },
  userRefreshPremiumSession: () => { },
  onNewConversation: () => { },
  onSelectConversationUuid: () => { },

  editingConversationId: null,
  setEditingConversationId: () => { }
}

export const AIChatReactContext =
  React.createContext<AIChatContext>(defaultContext)

export function AIChatContextProvider(props: React.PropsWithChildren<Props>) {
  const [context, setContext] = React.useState<AIChatContext>({
    ...defaultContext,
    ...getAPI().UIState
  })
  const [editingConversationId, setEditingConversationId] = React.useState<string | null>(null)

  const setPartialContext = (partialContext: Partial<AIChatContext>) => {
    setContext((value) => ({
      ...value,
      ...partialContext
    }))
  }

  React.useEffect(() => {
    setPartialContext(getAPI().UIState)

    getAPI().addUIStateChangeListener((e) => {
      setPartialContext(e.detail)
    })
  }, [])

  const { Service, UIHandler } = getAPI()

  const store: AIChatContext = {
    ...context,
    ...props,
    goPremium: () => UIHandler.goPremium(),
    managePremium: () => UIHandler.managePremium(),
    markStorageNoticeViewed: () => Service.dismissStorageNotice(),
    dismissStorageNotice: () => {
      setPartialContext({
        isStorageNoticeDismissed: true
      })
      Service.dismissStorageNotice()
    },
    dismissPremiumPrompt: () => Service.dismissPremiumPrompt(),
    userRefreshPremiumSession: () => UIHandler.refreshPremiumSession(),
    handleAgreeClick: () => Service.markAgreementAccepted(),
    uiHandler: UIHandler,
    editingConversationId,
    setEditingConversationId
  }

  return (
    <AIChatReactContext.Provider value={store}>
      {props.children}
    </AIChatReactContext.Provider>
  )
}

export function useAIChat() {
  return React.useContext(AIChatReactContext)
}
