/* Copyright (c) 2023 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

import * as React from 'react'

import classnames from 'classnames'
import { getLocale } from '$web-common/locale'
import Icon from '@brave/leo/react/icon'
import Button from '@brave/leo/react/button'

import styles from './style.module.scss'
import DataContext from '../../state/context'
import getPageHandlerInstance from '../../api/page_handler'

const MAX_INPUT_CHAR = 2000
const CHAR_LIMIT_THRESHOLD = MAX_INPUT_CHAR * 0.80

function InputBox () {
  const [inputText, setInputText] = React.useState('')
  const context = React.useContext(DataContext)

  const isCharLimitExceeded = inputText.length >= MAX_INPUT_CHAR
  const isCharLimitApproaching = inputText.length >= CHAR_LIMIT_THRESHOLD

  if (!context.hasAcceptedAgreement) {
    return (
      <div className={styles.actionsContainer}>
        <Button onClick={context.handleAgreeClick}>
          {getLocale('acceptButtonLabel')}
        </Button>
      </div>
    )
  }

  const onInputChange = (e: React.ChangeEvent<HTMLTextAreaElement>) => {
    setInputText(e.target.value)
  }

  const submitInputTextToAPI = () => {
    if (!inputText) return
    if (isCharLimitExceeded) return
    if (context.shouldDisableUserInput) return

    getPageHandlerInstance().pageHandler.submitHumanConversationEntry(inputText)
    setInputText('')
  }

  const handleSubmit = (e: React.MouseEvent<HTMLButtonElement, MouseEvent>) => {
    e.preventDefault()
    submitInputTextToAPI()
  }

  const onUserPressEnter = (e: React.KeyboardEvent<HTMLTextAreaElement>) => {
    if (e.key === 'Enter') {
      if (!e.repeat) {
        submitInputTextToAPI()
      }

      e.preventDefault()
    }
  }

  return (
    <form className={styles.form}>
      <div className={styles.textareaBox}>
        <textarea
          className={styles.textarea}
          placeholder={getLocale('placeholderLabel')}
          onChange={onInputChange}
          onKeyDown={onUserPressEnter}
          value={inputText}
          autoFocus
        />
        <div className={classnames({
          [styles.counterText]: true,
          [styles.counterTextVisible]: isCharLimitApproaching,
          [styles.counterTextError]: isCharLimitExceeded
        })}>
          {`${inputText.length} / ${MAX_INPUT_CHAR}`}
        </div>
      </div>
      <div>
        <button
          className={styles.buttonSend}
          onClick={handleSubmit}
          disabled={context.shouldDisableUserInput}
          title={getLocale('sendChatButtonLabel')}
        >
          <Icon name='send' />
        </button>
      </div>
    </form>
  )
}

export default InputBox
