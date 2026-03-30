# System Tray Only — Windhawk Mod

> Hides everything except the system tray, turning the taskbar into a small floating tray in the bottom-right corner.

**システムトレイ以外を非表示にし、タスクバーを右下角の小さなフローティングトレイに変換します。**

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
[![Platform](https://img.shields.io/badge/Platform-Windows%2011%2025H2-blue)](https://www.microsoft.com/windows)
[![Windhawk](https://img.shields.io/badge/Windhawk-Mod-green)](https://windhawk.net)

---

## 動機 / Motivation

ObjectDock などのサードパーティランチャーを**タスクバーの代替**として使う場合、Windows タスクバーを非表示にして運用します。  
タスクバーやスタートメニューにピン留めしたアプリはランチャー側で代替できますが、  
**システムトレイ（通知領域）に常駐するアプリへのアクセス手段が失われてしまいます。**

現実的なワークアラウンドは次のいずれかです：

- タスクバーを常に表示したままにする（ランチャー運用の目的と矛盾する）
- 必要なたびにタスクバーを表示・非表示する（非常に煩雑）

このモッドはそのギャップを埋めるために開発しました。タスクバーをシステムトレイのみの最小フローティングオーバーレイに変換することで、ランチャーによるクリーンなデスクトップ運用を維持しながら、システムトレイアプリへのアクセスを保持します。

---

When using a third-party launcher (e.g. ObjectDock) as a **taskbar replacement**, the Windows taskbar is hidden entirely.  
Apps pinned to the taskbar or Start menu can be replaced by the launcher itself —  
but **system tray (notification area) resident apps become inaccessible**.

The practical workarounds are either keeping the taskbar always visible (which defeats the purpose of using a launcher) or tediously toggling its visibility whenever tray access is needed.

This mod was developed to fill that gap: it strips the taskbar down to just the system tray and presents it as a minimal floating overlay, letting you maintain a clean launcher-based desktop while keeping system tray apps fully accessible.

---

## 概要 / Overview

このモッドは Windows 11 のタスクバーからシステムトレイ（通知領域）以外のすべての要素を非表示にします。  
トレイは画面右下の小さなフローティングウィンドウとして表示され、サードパーティのドックなしで単独動作します。  
AppBar スペース予約を解除するため、最大化ウィンドウはフルスクリーンを使用できます。

This mod hides all taskbar elements except the system tray (notification area) and displays it as a small floating window in the bottom-right corner of the screen. It activates automatically on load — no third-party dock or external trigger is required. The AppBar space reservation is removed so that maximized windows use the full screen.

---

## 機能 / Features

| 機能 | 説明 |
|------|------|
| **オートハイド** | マウスが離れるとトレイを隠し、右下角でホバーすると再表示 |
| **オートハイド遅延** | 隠れるまでの待機時間をミリ秒で設定 |
| **時計の表示/非表示** | トレイ内の日時表示を切り替え |
| **「デスクトップの表示」ボタン** | 右端の細いストリップを表示/非表示 |
| **背景の不透明度** | 0（完全透明）〜 100（完全不透明）で調整（デフォルト: 80） |

---

## 要件 / Requirements

- **OS**: Windows 11 バージョン **25H2** (ビルド 26200) のみ対応
- **Windhawk**: [https://windhawk.net](https://windhawk.net) をインストール済みであること
- **アーキテクチャ**: x86-64

> [!WARNING]
> 他の Windows バージョン（24H2 以前）では動作しません。

---

## インストール / Installation

1. [Windhawk](https://windhawk.net) をインストールします。
2. Windhawk を起動し、**「Find mods」** から `systemtray-only-show` を検索してインストールします。

### 手動インストール（開発版）

1. `systemtray-only-show.wh.cpp` の内容をクリップボードにコピーします。
2. Windhawk の **「Create new mod」** → **「Advanced」** タブを開きます。
3. コードを貼り付けてコンパイル・有効化します。

---

## 設定 / Settings

Windhawk の設定パネルから以下のオプションを変更できます。

| 設定キー | デフォルト | 説明 |
|----------|-----------|------|
| `autoHide` | `true` | オートハイドの有効/無効 |
| `autoHideDelayMs` | `1000` | 隠れるまでの遅延（ミリ秒） |
| `showClock` | `true` | 時計の表示/非表示 |
| `showShowDesktop` | `false` | 「デスクトップの表示」ボタンの表示/非表示 |
| `backgroundOpacity` | `80` | 背景の不透明度（0–100） |

---

## 開発 / Development

### ファイル構成

```
systemtray-only-show.wh.cpp   # モッド本体（Windhawk C++ mod）
```

### ビルド

Windhawk の内蔵コンパイラを使用してください。外部ビルドツールは不要です。  
コンパイルオプション: `-lole32 -loleaut32 -lruntimeobject -lshcore`

### 対象プロセス

`explorer.exe` のみ

---

## ライセンス / License

[MIT License](LICENSE) © 2026 roflsunriz

---

## 作者 / Author

**roflsunriz** — [GitHub](https://github.com/roflsunriz)
