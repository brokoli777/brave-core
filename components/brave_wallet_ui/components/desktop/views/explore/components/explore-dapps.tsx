// Copyright (c) 2023 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

import * as React from 'react'

// types
import { BraveWallet } from '../../../../../constants/types'

// api
import { useGetTopDappsQuery } from '../../../../../common/slices/api.slice'

// utils
import { getLocale } from '../../../../../../common/locale'

// selectors
import { useSafeUISelector } from '../../../../../common/hooks/use-safe-selector'
import { UISelectors } from '../../../../../common/selectors'

// components
import SearchBar from '../../../../shared/search-bar'
import DappCategoryList from './dapp-category-list'

// styles
import { Row, VerticalDivider, VerticalSpace } from '../../../../shared/style'
import { Title } from './explore-dapps.styles'
import {
  ButtonIcon,
  CircleButton,
  ContentWrapper,
  SearchBarWrapper
} from '../../portfolio/style'

export const ExploreDapps = () => {
  // state
  const [showSearchBar, setShowSearchBar] = React.useState<boolean>(false)
  const [searchValue, setSearchValue] = React.useState<string>('')

  // redux
  const isPanel = useSafeUISelector(UISelectors.isPanel)

  // queries
  const { data: dapps, isLoading: isLoadingDapps } = useGetTopDappsQuery()

  // methods
  const onCloseSearchBar = React.useCallback(() => {
    setShowSearchBar(false)
    setSearchValue('')
  }, [])

  // memos
  const categorizedDapps = React.useMemo(() => {
    const map: { [cateory: string]: BraveWallet.Dapp[] } = {}
    if (isLoadingDapps || !dapps) return map

    dapps.forEach((dapp) => {
      const { categories } = dapp
      categories.forEach((category) => {
        if (!map[category]) {
          map[category] = []
        }
        if (!map[category].some((d) => d.id === dapp.id)) {
          map[category].push(dapp)
        }
      })
    })

    return map
  }, [isLoadingDapps, dapps])

  return (
    <ContentWrapper
      fullWidth={true}
      fullHeight={isPanel}
      justifyContent='flex-start'
      isPanel={isPanel}
    >
      <Row
        justifyContent='space-between'
        alignItems='flex-end'
        width='100%'
        padding='0 0 0 8px'
      >
        {!showSearchBar ? (
          <Title>{getLocale('braveWalletTopNavExploreDapps')}</Title>
        ) : null}
        <Row width={showSearchBar ? '100%' : 'unset'}>
          {showSearchBar ? (
            <Row width='unset'>
              <SearchBarWrapper
                margin='0px 12px 0px 0px'
                showSearchBar={showSearchBar}
              >
                <SearchBar
                  placeholder={getLocale('braveWalletSearchText')}
                  action={(e) => setSearchValue(e.target.value)}
                  value={searchValue}
                  isV2={true}
                  autoFocus={true}
                />
              </SearchBarWrapper>
              <CircleButton onClick={onCloseSearchBar}>
                <ButtonIcon name='close' />
              </CircleButton>
            </Row>
          ) : (
            <Row width='unset'>
              <CircleButton
                marginRight={12}
                onClick={() => setShowSearchBar(true)}
              >
                <ButtonIcon name='search' />
              </CircleButton>

              <CircleButton onClick={() => console.log('filters')}>
                <ButtonIcon name='filter-settings' />
              </CircleButton>
            </Row>
          )}
        </Row>
      </Row>
      <VerticalSpace space='16px' />
      {isPanel ? <VerticalDivider /> : null}
      <DappCategoryList data={categorizedDapps} isLoading={isLoadingDapps} />
    </ContentWrapper>
  )
}
