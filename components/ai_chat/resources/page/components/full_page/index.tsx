/* Copyright (c) 2024 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

import Button from '@brave/leo/react/button'
import Icon from '@brave/leo/react/icon'
import useMediaQuery from '$web-common/useMediaQuery'
import { useAIChat } from '../../state/ai_chat_context'
import ConversationsList from '../conversations_list'
import { NavigationHeader } from '../header'
import Main from '../main'
import styles from './style.module.scss'
import { useConversation } from '../../state/conversation_context'
import * as React from 'react'
import Dropdown from '@brave/leo/react/dropdown'
import styled from 'styled-components'
import Flex from '$web-common/Flex'
import { color, radius, spacing } from '@brave/leo/tokens/css/variables'
import { Url as MojomUrl } from 'gen/url/mojom/url.mojom.m'
import * as mojom from '../../api'

const TabEntryListItem = styled.li`
  display: flex;
  flex-direction: row;
  align-items: center;
  width: 100%;
  gap: ${spacing.m};
  & span {
    flex: 1;
    max-height: 1.2rem;
    overflow: clip;
  }
  & leo-button {
    flex: 0;
  }
`

const FavIconImage = styled.div<{ url: string }>`
  display: inline-block;
  background-image: url("${p => p.url}");
  background-repeat: no-repeat;
  background-size: contain;
  background-origin: content-box;
  padding: 2px;
  border-radius: ${radius.s};
  border: 1px solid ${color.divider.subtle};
  width: 24px;
  height: 24px;
  flex-shrink: 0;
`

function TabImage(props: { url: MojomUrl }) {
  const aiChatContext = useAIChat()
  const [imgUrl, setImgUrl] = React.useState<string | null>()
  React.useEffect(() => {
    if (!aiChatContext.uiHandler) {
      return
    }
    aiChatContext.uiHandler.getFaviconImageDataForContent(props.url)
      .then(({ faviconImageData }) => {
        if (!faviconImageData) {
          setImgUrl(null)
          return
        }
        const blob = new Blob([new Uint8Array(faviconImageData)], { type: 'image/*' })
        setImgUrl(URL.createObjectURL(blob))
      })
  }, [props.url.url])

  if (imgUrl === null) {
    return <FavIconImage url='//resources/brave-icons/file-text.svg' />
  }

  if (!imgUrl) {
    return null
  }

  return <FavIconImage url={imgUrl} />
}

function TabEntry(props: {
  site: mojom.WebSiteInfoDetail
  canRemove: boolean
}) {
  const context = useConversation()

  return <TabEntryListItem>
    <TabImage url={props.site.url} />
    <span>{props.site.title}</span>
    {props.canRemove &&
      <Button fab kind="plain-faint" title={'Remove this tab from the conversation'} onClick={() => context.conversationHandler?.removeAssociatedTab(props.site.url)}>
        <Icon name='close' />
      </Button>
    }
  </TabEntryListItem>
}

function SitePicker(props: { disabled: boolean }) {
  const conversation = useConversation()
  const aiChat = useAIChat()

  const availableSites = React.useMemo(() => {
    const used = new Set(conversation.associatedContentInfo?.detail?.multipleWebSiteInfo?.sites.map(a => a.url.url))
    return aiChat.availableAssociatedContent.filter(w => !used.has(w.url.url))
  }, [aiChat.availableAssociatedContent, conversation.associatedContentInfo])

  return <Dropdown value='' disabled={props.disabled} placeholder='Add a tab to the conversation' onChange={e => conversation.conversationHandler?.addAssociatedTab({ url: e.value ?? '' })}>
    <span slot="value">Add a tab to the conversation</span>
    {availableSites.map((a, i) => <leo-option key={i} value={a.url.url}>
      <Flex align='center' gap={8} style={{ maxWidth: '500px' }} title={a.title}>
        <TabImage url={a.url} /> <span>{a.title}</span>
      </Flex>
    </leo-option>)}
  </Dropdown>
}

export default function FullScreen() {
  const aiChatContext = useAIChat()
  const chat = useConversation();
  const asideAnimationRef = React.useRef<Animation | null>()
  const controllerRef = React.useRef(new AbortController())
  const isSmall = useMediaQuery('(max-width: 1024px)')
  const [isNavigationCollapsed, setIsNavigationCollapsed] = React.useState(isSmall)
  const [isNavigationRendered, setIsNavigationRendered] = React.useState(!isSmall)

  const associatedTabs: mojom.WebSiteInfoDetail[] =
    chat.associatedContentInfo?.detail?.multipleWebSiteInfo?.sites
    ?? (chat.associatedContentInfo?.detail?.webSiteInfo
      ? [chat.associatedContentInfo.detail.webSiteInfo]
      : [])
  // Can't add or remove Tabs if conversation has history
  // TODO(petemill): ignore staged content
  const canModifyTabAssociation = chat.conversationHistory.length === 0 && !chat.isGenerating

  const initAsideAnimation = React.useCallback((node: HTMLElement | null) => {
    if (!node) return
    const open = { width: '340px', opacity: 1 }
    const close = { width: '0px', opacity: 0 }
    const animationOptions: KeyframeAnimationOptions = {
      duration: 200,
      easing: 'ease-out',
      fill: 'forwards'
    }
    asideAnimationRef.current = new Animation(
      new KeyframeEffect(node, [open, close], animationOptions)
    )

    // Make sure we're in the right state for our screen size when
    asideAnimationRef.current.playbackRate = isSmall ? 1 : -1
    asideAnimationRef.current.finish()
  }, [])

  const toggleAside = () => {
    const asideAnimation = asideAnimationRef.current

    if (asideAnimation) {
      if (isNavigationCollapsed) {
        controllerRef.current.abort()
        controllerRef.current = new AbortController()
        asideAnimation.ready.then(() => setIsNavigationRendered(true))
        asideAnimation.playbackRate = -1
      } else {
        // 'finish' triggers in both directions, so we only need this once per close animation
        // user may rapidly toggle the aside, so we need to abort scheduled listener in open animation
        asideAnimation.addEventListener(
          'finish',
          () => setIsNavigationRendered(false),
          { once: true, signal: controllerRef.current.signal }
        )
        asideAnimation.playbackRate = 1
      }

      asideAnimation.play()
      setIsNavigationCollapsed(!isNavigationCollapsed)
    }
  }

  React.useEffect(() => {
    const isOpen = asideAnimationRef.current?.playbackRate === 1
    if (aiChatContext.editingConversationId && isOpen) {
      toggleAside()
    }
  }, [aiChatContext.editingConversationId, isNavigationCollapsed]);

  React.useEffect(() => {
    const isOpen = asideAnimationRef.current?.playbackRate === 1

    // We've just changed to small and the sidebar was open, so close it
    if (isSmall && !isOpen) {
      toggleAside()
    }

    // We've just changed to big and the sidebar was closed, so open it
    if (!isSmall && isOpen) {
      toggleAside()
    }

  }, [isSmall])

  return (
    <div className={styles.fullscreen}>
      <div className={styles.left}>
        <div className={styles.controls}>
          <Button
            fab
            kind='plain-faint'
            onClick={toggleAside}
          >
            <Icon name={asideAnimationRef.current?.playbackRate === 1 ? 'sidenav-expand' : 'sidenav-collapse'} />
          </Button>
          {!isNavigationRendered && (
            <>
              <Button
                fab
                kind='plain-faint'
                onClick={aiChatContext.onNewConversation}
              >
                <Icon name='edit-box' />
              </Button>
            </>
          )}
        </div>
        <aside
          ref={initAsideAnimation}
          className={styles.aside}
        >
          {isNavigationRendered && (
            <>
              <NavigationHeader />
              <ConversationsList setIsConversationsListOpen={setIsNavigationCollapsed} />
            </>
          )}
        </aside>
      </div>
      <div className={styles.content}>
        <Main />
      </div>
      <div className={styles.right}>
        <div className={styles.headerSpacer} />
        <h3>Tabs used in this conversation</h3>
        <SitePicker disabled={!canModifyTabAssociation} />
        {associatedTabs.length > 0 ? (
          <ul>
            {associatedTabs.map((t, i) => (
              <TabEntry
                key={i}
                site={t}
                canRemove={canModifyTabAssociation}
              />
            ))}
          </ul>
        ) : (
          <p>No tabs are associated with this conversation</p>
        )}
      </div>
    </div>
  )
}
