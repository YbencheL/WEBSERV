# Configuration File Parsing – Detailed Design

This document describes the responsibilities and TODO list for the **configuration file parsing** part of the **webserv** project. The goal of the config parser is to read the configuration file, validate it, and expose a clean, structured configuration to the rest of the server.

The config parser **does not**:
- Handle sockets
- Handle HTTP requests
- Execute CGI

It only translates text configuration into structured data.

---

## Expected Input

- **Configuration file path** (e.g. `webserv.conf`)
- Raw text content of the configuration file

The configuration file follows an **Nginx-like syntax** with nested blocks:

```
server {
    listen 8080;
    root /var/www/html;

    location /cgi-bin {
        cgi_pass .py /usr/bin/python3;
        cgi_pass .php /usr/bin/php-cgi;
    }
}
```

---

## Expected Output

A structured, validated representation of the configuration, for example:

- Global configuration
- One or more **server blocks**
- Per-server **location blocks**
- Key-value settings accessible by the server logic

The output must be independent of the raw text format.

---

## Step 1: Read Configuration File

**Goal:** Load the configuration file into memory.

**Responsibilities:**
- Open the file
- Read entire content into a string or buffer
- Handle file access errors (missing file, permissions)

**Failure cases:**
- File not found
- File unreadable

---

## Step 2: Tokenization (Lexical Analysis)

**Goal:** Convert raw text into tokens.

**Responsibilities:**
- Ignore comments
- Handle whitespace and newlines
- Extract meaningful tokens:
  - Keywords (`server`, `location`, `listen`, etc.)
  - Symbols (`{`, `}`, `;`)
  - Values (paths, numbers, strings)

**Important Rule:**
- Tokenization does not understand meaning, only structure

---

## Step 3: Syntax Validation

**Goal:** Ensure the configuration follows valid grammar.

**Responsibilities:**
- Match opening and closing braces
- Ensure directives end with `;`
- Enforce correct block nesting (`location` inside `server`)

**Failure cases:**
- Missing `;`
- Unmatched `{}`
- Invalid block placement

---

## Step 4: Parse Global Context

**Goal:** Handle directives outside any `server` block.

**Responsibilities:**
- Parse global settings (if allowed)
- Store them separately from server-specific config

**Notes:**
- Some projects skip global context; design should still allow it

---

## Step 5: Parse Server Blocks

**Goal:** Build server-level configuration objects.

**Responsibilities:**
- Detect `server { ... }` blocks
- Parse directives inside server block:
  - `listen`
  - `server_name`
  - `root`
  - `error_page`
  - `client_max_body_size`

**Output:**
- One config object per server

---

## Step 6: Parse Location Blocks

**Goal:** Handle per-path configuration.

**Responsibilities:**
- Detect `location /path { ... }` blocks
- Attach location blocks to their parent server
- Parse location-specific directives:
  - `root`
  - `index`
  - `allow_methods`
  - `autoindex`
  - `cgi_pass`

**Important Rule:**
- Location blocks cannot exist outside server blocks

---

## Step 7: Directive Validation

**Goal:** Validate values and detect conflicts.

**Responsibilities:**
- Validate numeric values (ports, sizes)
- Validate paths
- Detect duplicate or conflicting directives

**Failure cases:**
- Invalid port number
- Multiple roots where not allowed
- Unsupported directive

---

## Step 8: Default Values Handling

**Goal:** Ensure missing directives have sane defaults.

**Responsibilities:**
- Apply default root
- Default allowed methods
- Default error pages

**Why this matters:**
- Simplifies server logic
- Prevents runtime ambiguity

---

## Step 9: Build Final Configuration Model

**Goal:** Produce a clean configuration API.

**Responsibilities:**
- Convert parsed data into structs/classes
- Remove dependency on tokens or raw text

**Example:**
```cpp
Config
 ├── GlobalConfig
 └── vector<ServerConfig>
       └── vector<LocationConfig>
```

---

## Step 10: Error Reporting

**Goal:** Provide clear feedback for invalid configs.

**Responsibilities:**
- Include line numbers in errors
- Provide descriptive messages

**Defense Tip:**
- Clear config errors show parser maturity

---

## Step 11: Expose Read-Only Access

**Goal:** Prevent runtime modification.

**Responsibilities:**
- Provide const accessors
- Prevent accidental mutation by server logic

---

## Final Design Principles

- Config parsing happens **once at startup**
- Errors must fail fast
- No runtime reloading required
- Parser is independent from server, CGI, and sockets

---

## One-Sentence Defense Summary

> "The configuration parser reads and validates an Nginx-like configuration file, builds a structured configuration model, and provides safe read-only access to the server components."