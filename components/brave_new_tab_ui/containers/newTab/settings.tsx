// Copyright (c) 2020 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

import * as React from 'react'

import {
  SettingsContent,
  SettingsFeatureBody,
  SettingsMenu,
  SettingsSidebar,
  SettingsSidebarActiveButtonSlider,
  SettingsSidebarButton,
  SettingsSidebarButtonText,
  SettingsTitle,
  SettingsWrapper
} from '../../components/default'

import { getLocale } from '$web-common/locale'
import Button from '@brave/leo/react/button'
import Icon from '@brave/leo/react/icon'
import { useBraveNews } from '../../../brave_news/browser/resources/shared/Context'
import { loadTimeData } from '$web-common/loadTimeData'

// Tabs
const BackgroundImageSettings = React.lazy(() => import('./settings/backgroundImage'))
const BraveStatsSettings = React.lazy(() => import('./settings/braveStats'))
const TopSitesSettings = React.lazy(() => import('./settings/topSites'))
const ClockSettings = React.lazy(() => import('./settings/clock'))
const CardsSettings = React.lazy(() => import('./settings/cards'))
const SearchSettings = React.lazy(() => import('./settings/search'))

export interface Props {
  newTabData: NewTab.State
  textDirection: string
  showSettingsMenu: boolean
  featureCustomBackgroundEnabled: boolean
  onClose: () => void
  toggleShowBackgroundImage: () => void
  toggleShowTopSites: () => void
  setMostVisitedSettings: (show: boolean, customize: boolean) => void
  toggleShowRewards: () => void
  toggleShowBraveTalk: () => void
  toggleBrandedWallpaperOptIn: () => void
  toggleCards: (show: boolean) => void
  chooseNewCustomImageBackground: () => void
  setCustomImageBackground: (selectedBackground: string) => void
  removeCustomImageBackground: (background: string) => void
  setBraveBackground: (selectedBackground: string) => void
  setColorBackground: (color: string, useRandomColor: boolean) => void
  onEnableRewards: () => void
  showBackgroundImage: boolean
  showTopSites: boolean
  customLinksEnabled: boolean
  brandedWallpaperOptIn: boolean
  allowBackgroundCustomization: boolean
  showRewards: boolean
  showBraveTalk: boolean
  braveRewardsSupported: boolean
  braveTalkSupported: boolean
  setActiveTab?: TabType
  cardsHidden: boolean
}

export enum TabType {
  BackgroundImage = 'backgroundImage',
  BraveStats = 'braveStats',
  TopSites = 'topSites',
  BraveNews = 'braveNews',
  Clock = 'clock',
  Cards = 'cards',
  Search = 'search'
}

const tabTypes = Object.values(TabType)

type TabMap<T> = { [P in TabType]: T }
const tabIcons: TabMap<string> = {
  [TabType.BackgroundImage]: 'image',
  [TabType.BraveNews]: 'product-brave-news',
  [TabType.BraveStats]: 'bar-chart',
  [TabType.Clock]: 'clock',
  [TabType.TopSites]: 'window-content',
  [TabType.Cards]: 'browser-ntp-widget',
  [TabType.Search]: 'search'
}

const tabTranslationKeys: TabMap<string> = {
  [TabType.BackgroundImage]: 'backgroundImageTitle',
  [TabType.BraveNews]: 'braveNewsTitle',
  [TabType.BraveStats]: 'statsTitle',
  [TabType.Clock]: 'clockTitle',
  [TabType.TopSites]: 'topSitesTitle',
  [TabType.Cards]: 'cards',
  [TabType.Search]: 'searchTitle'
}

const featureFlagSearchWidget = loadTimeData.getBoolean('featureFlagSearchWidget')
export default function Settings(props: Props) {
  const settingsMenuRef = React.createRef<any>()
  const allowedTabTypes = React.useMemo(() => tabTypes.filter(t =>
    (props.allowBackgroundCustomization || t !== TabType.BackgroundImage) &&
    (featureFlagSearchWidget || t !== TabType.Search)), [props.allowBackgroundCustomization])
  const [activeTab, setActiveTab] = React.useState(props.allowBackgroundCustomization
    ? TabType.BackgroundImage
    : TabType.BraveStats)
  const { customizePage, setCustomizePage } = useBraveNews()

  const changeTab = React.useCallback((tab: TabType) => {
    if (tab === TabType.BraveNews) {
      setCustomizePage('news')
      return
    }

    setActiveTab(tab)
  }, [])

  // When the outside world tells us to update the active tab, do so.
  React.useEffect(() => {
    if (!props.setActiveTab || !allowedTabTypes.includes(props.setActiveTab)) return
    setActiveTab(props.setActiveTab)
  }, [props.setActiveTab])

  // Set the global listeners
  React.useEffect(() => {
    const handleClickOutside = (event: Event) => {
      if (settingsMenuRef.current &&
        !settingsMenuRef.current.contains(event.target) &&
        // Don't close the settings dialog for a click outside if we're in the
        // Brave News modal - the user expects closing that one to bring them back
        // to this one.
        !customizePage
      ) {
        props.onClose()
      }
    }

    const handleKeyPress = (event: KeyboardEvent) => {
      if (event.key === 'Escape') {
        props.onClose()
      }
    }

    document.addEventListener('mousedown', handleClickOutside)
    document.addEventListener('keydown', handleKeyPress)

    return () => {
      document.removeEventListener('mousedown', handleClickOutside)
      document.removeEventListener('keydown', handleKeyPress)
    }
  }, [props.onClose, customizePage])

  if (!props.showSettingsMenu) return null

  return <SettingsWrapper textDirection={props.textDirection}>
    <SettingsMenu
      ref={settingsMenuRef}
      textDirection={props.textDirection}
      title={getLocale('dashboardSettingsTitle')}
    >
      <SettingsTitle id='settingsTitle'>
        <h1>{getLocale('dashboardSettingsTitle')}</h1>
        <Button fab kind='plain-faint' onClick={props.onClose}>
          <Icon name='close' />
        </Button>
      </SettingsTitle>
      <SettingsContent id='settingsBody'>
        <SettingsSidebar id='sidebar'>
          <SettingsSidebarActiveButtonSlider
            translateTo={allowedTabTypes.indexOf(activeTab)}
          />
          {
            allowedTabTypes.map((tabType) => {
              const titleKey = tabTranslationKeys[tabType]
              const isActive = activeTab === tabType
              return (
                <SettingsSidebarButton
                  tabIndex={0}
                  key={tabType}
                  data-active={isActive ? '' : null}
                  onClick={() => changeTab(tabType)}
                >
                  <Icon name={tabIcons[tabType]} />
                  <SettingsSidebarButtonText
                    data-text={getLocale(titleKey)}>
                    {getLocale(titleKey)}
                  </SettingsSidebarButtonText>
                </SettingsSidebarButton>
              )
            })
          }
        </SettingsSidebar>
        <SettingsFeatureBody id='content'>
          {/* Empty loading fallback is ok here since we are loading from local disk. */}
          <React.Suspense fallback={(<div />)}>
            {activeTab === TabType.BackgroundImage && <BackgroundImageSettings
              newTabData={props.newTabData}
              toggleBrandedWallpaperOptIn={props.toggleBrandedWallpaperOptIn}
              toggleShowBackgroundImage={props.toggleShowBackgroundImage}
              chooseNewCustomImageBackground={props.chooseNewCustomImageBackground}
              setCustomImageBackground={props.setCustomImageBackground}
              removeCustomImageBackground={props.removeCustomImageBackground}
              setBraveBackground={props.setBraveBackground}
              setColorBackground={props.setColorBackground}
              brandedWallpaperOptIn={props.brandedWallpaperOptIn}
              showBackgroundImage={props.showBackgroundImage}
              featureCustomBackgroundEnabled={props.featureCustomBackgroundEnabled}
              onEnableRewards={props.onEnableRewards}
              braveRewardsSupported={props.braveRewardsSupported}
            />}
            {activeTab === TabType.BraveStats && <BraveStatsSettings />}
            {activeTab === TabType.TopSites && <TopSitesSettings
              toggleShowTopSites={props.toggleShowTopSites}
              showTopSites={props.showTopSites}
              customLinksEnabled={props.customLinksEnabled}
              setMostVisitedSettings={props.setMostVisitedSettings}
            />}
            {activeTab === TabType.Clock && <ClockSettings />}
            {activeTab === TabType.Cards && <CardsSettings
              toggleCards={props.toggleCards}
              cardsHidden={props.cardsHidden}
              toggleShowBraveTalk={props.toggleShowBraveTalk}
              showBraveTalk={props.showBraveTalk}
              braveTalkSupported={props.braveTalkSupported}
              toggleShowRewards={props.toggleShowRewards}
              braveRewardsSupported={props.braveRewardsSupported}
              showRewards={props.showRewards}
            />}
            {activeTab === TabType.Search && <SearchSettings />}
          </React.Suspense>
        </SettingsFeatureBody>
      </SettingsContent>
    </SettingsMenu>
  </SettingsWrapper>
}
