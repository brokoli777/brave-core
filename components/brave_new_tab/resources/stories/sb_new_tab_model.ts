/* Copyright (c) 2024 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

import { createStore } from '../lib/store'
import { NewTabModel, defaultModel, defaultState } from '../models/new_tab_model'
import { getCurrentBackground } from '../models/backgrounds'

function delay(ms: number) {
  return new Promise((resolve) => {
    setTimeout(resolve, ms)
  })
}

export function createNewTabModel(): NewTabModel {
  const store = createStore(defaultState())

  store.update({
    currentBackground: getCurrentBackground(store.getState())
  })

  return {
    ...defaultModel(),

    getState: store.getState,
    addListener: store.addListener,

    setBackgroundsEnabled(enabled) {
      store.update({ backgroundsEnabled: enabled })
      store.update({
        currentBackground: getCurrentBackground(store.getState())
      })
    },

    setSponsoredImagesEnabled(enabled) {
      store.update({ sponsoredImagesEnabled: enabled })
    },

    selectBackground(type, value) {
      store.update({
        selectedBackgroundType: type,
        selectedBackground: value
      })
      store.update({
        currentBackground: getCurrentBackground(store.getState())
      })
    },

    async showCustomBackgroundChooser() {
      return true
    },

    async addCustomBackgrounds() {
      await delay(2000)

      const url =
        'https://brave.com/static-assets/images/brave-logo-sans-text.svg'

      store.update((state) => ({
        customBackgrounds: [...state.customBackgrounds, url],
        selectedBackground: url,
        selectedBackgroundType: 'custom'
      }))

      store.update({
        currentBackground: getCurrentBackground(store.getState())
      })
    },

    async removeCustomBackground(background) {
      store.update((state) => ({
        customBackgrounds:
          state.customBackgrounds.filter((elem) => elem !== background)
      }))
    }
  }
}
