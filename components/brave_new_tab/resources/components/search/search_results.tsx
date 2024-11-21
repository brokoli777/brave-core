/* Copyright (c) 2024 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

import * as React from 'react'
import Icon from '@brave/leo/react/icon'

import { SearchResultMatch, ClickEvent } from '../../models/search_model'
import { useLocale } from '../locale_context'
import { placeholderImageSrc } from '../../lib/image_loader'
import { faviconURL } from '../../lib/favicon_url'
import { Optional } from '../../lib/optional'
import { PcdnImage } from '../pcdn_image'

import { style } from './search_results.style'

function MatchImage(props: { match: SearchResultMatch }) {
  const { getString } = useLocale()
  const { imageUrl, iconUrl } = props.match
  if (props.match.description === getString('searchAskLeoDescription')) {
    return <Icon name='product-brave-leo' className='brave-leo-icon' />
  }
  if (!imageUrl) {
    return <img className='icon' src={iconUrl || placeholderImageSrc} />
  }
  if (imageUrl.startsWith('chrome:')) {
    return <img src={imageUrl} />
  }
  return <PcdnImage src={imageUrl} />
}

interface URLResultOption {
  kind: 'url'
  url: string
}

interface MatchResultOption {
  kind: 'match'
  matchIndex: number
  match: SearchResultMatch
}

export type ResultOption = URLResultOption | MatchResultOption

interface Props {
  options: ResultOption[]
  selectedOption: Optional<number>
  onOptionClick: (option: ResultOption, event: ClickEvent) => void
}

export function SearchResults(props: Props) {
  const { selectedOption, options } = props

  if (options.length === 0) {
    return null
  }

  return (
    <div {...style}>
      {options.map((option, index) => {
        const isSelected = selectedOption.valueOr(-1) === index
        const className = isSelected ? 'selected' : ''
        const onClick = (event: React.MouseEvent) => {
          props.onOptionClick(option, event)
        }

        if (option.kind === 'url') {
          return (
            <button key={option.url} className={className} onClick={onClick}>
              <span className='result-image'>
                <img src={faviconURL(option.url)} />
              </span>
              <span className='content'>
                {option.url}
              </span>
            </button>
          )
        }

        const { match } = option

        return (
          <button
            key={option.matchIndex}
            className={className}
            onClick={onClick}
          >
            <span className='result-image'>
              <MatchImage match={match} />
            </span>
            <span className='content'>
              {match.contents}
              <span className='description'>{match.description}</span>
            </span>
          </button>
        )
      })}
    </div>
  )
}
