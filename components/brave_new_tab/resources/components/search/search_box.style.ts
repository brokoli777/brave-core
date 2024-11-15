/* Copyright (c) 2024 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

import { color } from '@brave/leo/tokens/css/variables'
import { scoped } from '../../lib/scoped_css'

export const style = scoped.css`

  & {
    --self-transition-time: 200ms;

    padding-top: 0;
    display: flex;
    justify-content: center;
    transition: padding-top var(--self-transition-time);
    color: ${color.text.primary};
  }

  .input-container {
    anchor-name: --search-input-container;
    position: relative;
    z-index: 2;
    flex: 0 1 392px;
    margin-inline-start: 16px;
    margin-inline-end: 16px;
    display: flex;
    align-items: center;
    gap: 8px;
    transition: flex-basis var(--self-transition-time);
    padding: 8px;
    border-radius: 12px;
    background: #fff;
  }

  input {
    flex: 1 1 auto;
    order: 2;
    border: none;
    padding: 0;
    font: inherit;
    outline: none;
  }

  .engine-picker-button {
    --leo-icon-size: 16px;

    anchor-name: --engine-picker-button;
    order: 1;
    padding: 7px;
    border-radius: 4px;
    border: solid 1px transparent;

    &:hover {
      background-color: ${color.container.interactive};
    }

    &.open {
      background-color: ${color.container.interactive};
      border-color: ${color.divider.interactive};
    }
  }

  .search-button {
    --leo-icon-size: 24px;

    order: 3;
    padding: 4px;
    border-radius: 4px;
    visibility: hidden;
    opacity: 0;
    transition: opacity var(--self-transition-time);
    color: ${color.neutral['40']};

    &:hover {
      background-color: ${color.container.interactive};
    }
  }

  .engine-options {
    --leo-icon-size: 20px;

    position: absolute;
    position-anchor: --engine-picker-button;
    position-area: block-end span-inline-end;
    z-index: 4;

    margin-top: 2px;
    min-width: 232px;
    display: flex;
    flex-direction: column;
    gap: 4px;
    background: ${color.container.background};
    border-radius: 8px;
    border: solid 1px ${color.divider.subtle};
    box-shadow: 0px 1px 0px 0px rgba(0, 0, 0, 0.05);
    padding: 4px;

    button {
      padding: 10px;
      display: flex;
      gap: 8px;
      align-items: center;
      border-radius: 6px;

      .engine-icon {
        width: 20px;
      }

      &:hover {
        background: ${color.container.highlight};
      }
    }

    .divider {
      height: 1px;
      background: ${color.divider.subtle};
    }
  }

  .results-container {
    position: absolute;
    position-anchor: --search-input-container;
    position-area: bottom center;
    z-index: 2;

    width: anchor-size(width);
    margin-top: 12px;
    display: flex;
    flex-direction: column;
    visibility: hidden;
    opacity: 0;
    transition: opacity var(--self-transition-time);
  }

  .menu-backdrop {
    position: fixed;
    inset: 0;
    z-index: 3;
  }

  .backdrop {
    position: fixed;
    inset: 0;
    z-index: 1;
    display: none;
    opacity: 0;
    background: #000;
  }

  &.expanded {
    position: relative;
    padding-top: 100px;

    .input-container {
      flex-basis: 540px;
    }

    .search-button {
      visibility: visible;
      opacity: 1;
    }

    .results-container {
      visibility: visible;
      opacity: 1;
    }

    .backdrop {
      display: block;
      opacity: 0.25;
    }
  }

`
