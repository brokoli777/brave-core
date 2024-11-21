/* Copyright (c) 2024 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

import * as React from 'react'
import Dialog from '@brave/leo/react/dialog'
import Navigation from '@brave/leo/react/navigation'
import NavigationItem from '@brave/leo/react/navigationItem'

import { BackgroundPanel } from './background_panel'
import { SearchPanel } from './search_panel'
import { useLocale } from '../locale_context'

import { style } from './settings_dialog.style'

export type SettingsView = 'background' | 'search'

interface Props {
  initialView: SettingsView
  onClose: () => void
}

export function SettingsDialog(props: Props) {
  const { getString } = useLocale()

  const [currentView, setCurrentView] =
    React.useState<SettingsView>(props.initialView)

  function renderPanel() {
    switch (currentView) {
      case 'background': return <BackgroundPanel />
      case 'search': return <SearchPanel />
    }
  }

  function getNavItemText(view: SettingsView) {
    switch (view) {
      case 'background': return getString('backgroundSettingsTitle')
      case 'search': return getString('searchSettingsTitle')
    }
  }

  function renderNavItem(view: SettingsView) {
    return (
      <NavigationItem
        isCurrent={view === currentView}
        onClick={() => setCurrentView(view)}
      >
        {getNavItemText(view)}
      </NavigationItem>
    )
  }

  return (
    <div {...style}>
      <Dialog isOpen animate={false} showClose onClose={props.onClose}>
        <h3>
          {getString('settingsTitle')}
        </h3>
        <div className='panel-body'>
          <nav>
            <Navigation>
              {renderNavItem('background')}
              {renderNavItem('search')}
            </Navigation>
          </nav>
          <section>
            {renderPanel()}
          </section>
        </div>
      </Dialog>
    </div>
  )
}
