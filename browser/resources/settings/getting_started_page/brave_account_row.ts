/* Copyright (c) 2024 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

import { CrLitElement } from '//resources/lit/v3_0/lit.rollup.js'
import { getCss } from './brave_account_row.css.js'
import { getHtml } from './brave_account_row.html.js'

/**
 * @fileoverview
 * 'settings-brave-account-row'...
 */

export enum DialogType {
  NONE,
  CREATE,
  ENTRY,
  FORGOT_PASSWORD,
  SIGN_IN
}

export class SettingsBraveAccountRow extends CrLitElement {
  static get is() {
    return 'settings-brave-account-row'
  }

  static override get styles() {
    return getCss()
  }

  override render() {
    return getHtml.bind(this)()
  }

  static override get properties() {
    return {
      dialogType: { type: DialogType },
      signedIn: { type: Boolean, reflect: true },
    }
  }

  protected onBackButtonClicked() {
    switch (this.dialogType) {
      case DialogType.CREATE:
        this.dialogType = DialogType.ENTRY
        break
      case DialogType.FORGOT_PASSWORD:
        this.dialogType = DialogType.SIGN_IN
        break
      case DialogType.SIGN_IN:
        this.dialogType = DialogType.ENTRY
        break
    }
  }

  protected dialogType: DialogType = DialogType.NONE
  protected signedIn: boolean = false
}

declare global {
  interface HTMLElementTagNameMap {
    'settings-brave-account-row': SettingsBraveAccountRow
  }
}

customElements.define(SettingsBraveAccountRow.is, SettingsBraveAccountRow)
