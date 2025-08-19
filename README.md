# ft\_irc

**A tiny IRC server in C++98** — multi‑client, non‑blocking I/O, channels, basic modes, and core commands. Built as part of the **École 42** curriculum.

---

## 📑 Table of Contents

1. [Overview](#overview)
2. [Protocol & Compliance](#protocol--compliance)
3. [Features](#features)
4. [Directory Structure](#directory-structure)
5. [Build](#build)
6. [Usage](#usage)
7. [Command Summary](#command-summary)
8. [Channel & User Modes](#channel--user-modes)
9. [Testing](#testing)
10. [Troubleshooting](#troubleshooting)
11. [Project Context](#project-context)

---

<a id="overview"></a>

## ✨ Overview

`ft_irc` implements a minimal yet practical IRC server. It accepts multiple clients, manages channels, and relays messages while respecting the IRC wire format (lines ≤ **512 bytes**, terminated by `\r\n`). The server uses **non‑blocking sockets** and a scalable event loop (e.g., `epoll`/`kqueue`/`poll`).

---

<a id="protocol--compliance"></a>

## 📜 Protocol & Compliance

* Base spec: **RFC 1459** (classic IRC). Some semantics inspired by **RFC 2812**.
* Lines are `CRLF` terminated and **≤ 512 bytes** (including CRLF).
* Nicknames must be unique and case‑insensitive per ASCII.
* Channels begin with `#` (no spaces, no control chars). One user is channel operator upon creation.
* Minimal numeric replies for common success/error states (e.g. `001`, `433`, `403`, `461`, …).

> This project targets a **subset** of the RFC sufficient to interoperate with standard clients for core flows.

---

<a id="features"></a>

## ⚙️ Features

* **Non‑blocking I/O** with a single event loop (backend decided at compile time).
* **Multi‑client** handling with per‑connection buffers and flood protection knobs.
* **Authentication** with server password (PASS) before registration.
* **Channels** with invite/key/limit/topic locks (subset).
* **Core commands**: `PASS`, `NICK`, `USER`, `PING`, `PONG`, `JOIN`, `PART`, `PRIVMSG`, `MODE` (subset), `TOPIC`, `KICK`, `INVITE`, `QUIT`.
* **Graceful shutdown** and cleanup.

---

<a id="directory-structure"></a>

## 🗂️ Directory Structure

```text
ft_irc/
├── includes/            # headers (Server.hpp, Client.hpp, Parser.hpp, ...)
├── object/                # implementation files
├── sources/             # sample run scripts / configs
├── Makefile
└── README.md           # you are here
```

---

<a id="build"></a>

## 🚀 Build

Requires **C++98** compiler and POSIX sockets.

```bash
# Clone repository
git clone https://github.com/<your-username>/ft_irc.git
cd ft_irc

# Build
make                # produces ./ircserv
```

Useful flags (already handled in the Makefile in most setups):

* Linux: `-std=c++98 -Wall -Wextra -Werror -O2`
* Event loop backend: `epoll` (Linux) / `kqueue` (macOS, \*BSD) / fallback `poll`

---

<a id="usage"></a>

## 📚 Usage

Start the server:

```bash
./ircserv <port> <password>
```

Examples:

```bash
./ircserv 6667 secret
./ircserv 4242 pass42
```

Connect with **netcat** (demo):

```text
$ nc 127.0.0.1 6667
PASS secret
NICK alice
USER alice 0 * :Alice Liddell
JOIN #garden
PRIVMSG #garden :Hello, world!
```

Connect with **irssi**:

```text
/CONNECT 127.0.0.1 6667 secret
/NICK alice
/JOIN #garden
/MSG #garden hi all
```

Connect with **WeeChat**:

```text
/server add local 127.0.0.1/6667 -password=secret
/connect local
```

---

<a id="command-summary"></a>

## 📬 Command Summary

| Command                       | Purpose                                                     |
| ----------------------------- | ----------------------------------------------------------- |
| `PASS <password>`             | Authenticate to the server (required before register).      |
| `NICK <nickname>`             | Set/Change nickname (unique).                               |
| `USER <user> 0 * :<real>`     | Complete registration with username & realname.             |
| `PING <token>`                | Liveness check; server replies `PONG <token>`.              |
| `JOIN <#chan> [key]`          | Join or create a channel. Optional key for locked channels. |
| `PART <#chan> [msg]`          | Leave a channel.                                            |
| `PRIVMSG <target> :text`      | Send private or channel message.                            |
| `TOPIC <#chan> [:text]`       | Get/set channel topic (if `+t`, op only).                   |
| `MODE <#chan> +/-itkol [arg]` | Channel modes subset (see below).                           |
| `INVITE <nick> <#chan>`       | Invite a user (works with `+i` invite‑only).                |
| `KICK <#chan> <nick> [msg]`   | Remove a user from a channel (op only).                     |
| `QUIT [msg]`                  | Disconnect with optional message.                           |

> Some numerics are sent to match client expectations (e.g., welcome `001`, topic replies `332/333`, names `353/366`, errors like `433` nickname in use, etc.).

---

<a id="channel--user-modes"></a>

## 🛡️ Channel & User Modes (subset)

**Channel modes** (via `MODE <#chan> +/-flag [arg]`):

* `+i` — invite‑only (only invited users can JOIN)
* `+t` — topic locked (only ops can set topic)
* `+k <key>` — channel key/password required to JOIN
* `+l <n>` — user limit
* `+o <nick>` — give/take operator status

**User modes** implemented minimally as needed by project (op status is per‑channel via `+o`).

---

<a id="testing"></a>

## 🧪 Testing

* **Basic registration**: `PASS` → `NICK` → `USER` → welcome numerics.
* **Join & message**: `JOIN #x` then `PRIVMSG #x :hello` (check broadcast).
* **Modes**: set `+k`, `+l`, `+t`, `+i` and verify behavior (`INVITE`, `KICK`).
* **Nickname collisions**: ensure `433` on duplicates.
* **Timeouts**: `PING/PONG` handling; idle clients pruned if enabled.

Scripted smoke test (netcat):

```bash
{ printf "PASS secret\r\nNICK bot\r\nUSER bot 0 * :robot\r\nJOIN #test\r\nPRIVMSG #test :ping!\r\n"; sleep 1; } | nc 127.0.0.1 6667
```

---

<a id="troubleshooting"></a>

## 🧯 Troubleshooting

* **Cannot connect**: check firewall, correct `<port>`, and password. Ports <1024 need root.
* **`433` Nickname is already in use**: choose a different nick.
* **Messages dropped**: remember IRC line **≤ 512 bytes** including CRLF.
* **High CPU**: verify non‑blocking sockets and backoff in the event loop.

---

<a id="project-context"></a>

## 🏛️ Project Context

Part of the **42** Network branch. Objectives:

* Event‑driven network programming (sockets, non‑blocking I/O).
* Parsing, state machines, and protocol compliance.
* Robust resource management and error handling in **C++98**.
