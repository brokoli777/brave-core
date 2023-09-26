// Copyright (c) 2023 The Brave Authors. All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

import * as React from 'react'

// types
import { BraveWallet } from '../../../../../constants/types'

// styles
import { ContentWrapper, Description, Name, StyledWrapper } from './dapp.styles'

interface Props {
  dapp: BraveWallet.Dapp
}

export const Dapp = ({ dapp }: Props) => {
  return (
    <StyledWrapper>
      {/* <DappImage src={dapp.logo} /> */}
      <div style={{ display: 'flex', flexShrink: 0, width: '40px', height: '40px', backgroundColor: 'blue', borderRadius: '50%'}} />
      <ContentWrapper>
        <Name>{dapp.name}</Name>
        <Description>{dapp.description}</Description>
      </ContentWrapper>
    </StyledWrapper>
  )
}
