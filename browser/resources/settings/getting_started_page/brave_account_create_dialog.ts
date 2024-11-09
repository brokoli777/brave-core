/* Copyright (c) 2024 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * you can obtain one at https://mozilla.org/MPL/2.0/. */

import { CrLitElement } from '//resources/lit/v3_0/lit.rollup.js'
import { getCss } from './brave_account_create_dialog.css.js'
import { getHtml } from './brave_account_create_dialog.html.js'

/**
 * @fileoverview
 * 'settings-brave-account-create-dialog'...
 */

export class SettingsBraveAccountCreateDialogElement extends CrLitElement {
  static get is() {
    return 'settings-brave-account-create-dialog'
  }

  static override get styles() {
    return getCss()
  }

  override render() {
    return getHtml.bind(this)()
  }

  static override get properties() {
    return {
      emailAddress: { type: String },
      emailAddressEndsWithBraveAlias: { type: Boolean },
      isEmailAddressValid: { type: Boolean },
      isAccountNameValid: { type: Boolean },
      isChecked: { type: Boolean },
      password: { type: String },
      passwordConfirmation: { type: String },
      percent: { type: Number },
    }
  }

  protected onEmailAddressInput(detail: { value: string }) {
    this.emailAddress = detail.value
    this.isEmailAddressValid = /^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,4}$/.test(this.emailAddress)
    this.emailAddressEndsWithBraveAlias = this.emailAddress.endsWith('@bravealias.com')
  }

  protected onAccountNameInput(detail: { value: string }) {
    this.isAccountNameValid = detail.value.length !== 0
  }

  protected onCreatePasswordInput(detail: { value: string }) {
    this.password = detail.value
    this.percent = this.regexps.filter(
      regexp => regexp[1] = regexp[0].test(this.password)
    ).length / this.regexps.length * 100
  }

  protected onConfirmPasswordInput(detail: { value: string }) {
    this.passwordConfirmation = detail.value
  }

  protected onChange(detail: { checked: boolean }) {
    this.isChecked = detail.checked
  }

  protected OnEyeIconClicked(event: Event) {
    event.preventDefault()
    const target = event.target as Element
    const isShowing = target.getAttribute('name') === 'eye-on'
    target.setAttribute('name', isShowing ? 'eye-off' : 'eye-on')
    target.parentElement!.setAttribute('type', isShowing ? 'password' : 'text')
  }

  protected getCategory() {
    return this.percent < 60 ? 'weak' : this.percent < 100 ? 'medium' : 'strong'
  }

  protected getIconName() {
    if (this.passwordConfirmation.length !== 0) {
      this.icon = this.passwordConfirmation === this.password ? 'check-circle-filled' : 'warning-triangle-filled'
    }
    
    return this.icon
  }

  protected emailAddress: string = ''
  protected emailAddressEndsWithBraveAlias: boolean = false
  protected isEmailAddressValid: boolean = false
  protected isAccountNameValid: boolean = false
  protected isChecked: boolean = false
  protected password: string = ''
  protected passwordConfirmation: string = ''
  protected icon: string = 'warning-triangle-filled'

  protected regexps: Array<[RegExp, boolean, string]> = [
    [/^.{5,}$/, false, 'At least 5 characters'],
    [/[a-z]/, false, 'At least 1 lowercase letter'],
    [/[A-Z]/, false, 'At least 1 uppercase letter'],
    [/[0-9]/, false, 'At least 1 number'],
    [/[^a-zA-Z0-9]/, false, 'At least 1 special character']
  ]
  protected percent: number = 0
}

declare global {
  interface HTMLElementTagNameMap {
    'settings-brave-account-create-dialog': SettingsBraveAccountCreateDialogElement
  }
}

customElements.define(
  SettingsBraveAccountCreateDialogElement.is, SettingsBraveAccountCreateDialogElement)
