# CGI Module – Detailed Design

This document expands the original TODO list into a clear, defense-ready breakdown of responsibilities for the CGI part of the **webserv** project. The CGI module acts as a translator between parsed HTTP request data and external executable scripts.

---

## Expected Input (from Parser / Server Layer)
The CGI module **does not parse raw HTTP**. It receives already structured data:

- **method**: HTTP method (GET, POST)
- **path**: Requested URI path (e.g. `/cgi-bin/test.py`)
- **query**: Query string extracted from URI (e.g. `name=foo&x=42`)
- **body**: Request body (used mainly for POST)
- **headers**: Parsed HTTP headers (key-value map)

> These inputs may evolve depending on the parser, but the CGI module must remain independent from raw socket or HTTP text handling.

---

## Step 1: Detect CGI Execution

**Goal:** Decide whether the request should be handled as CGI.

**Responsibility:**
- Check if the requested resource matches a CGI rule
- Typical strategies:
  - File extension (`.py`, `.php`, `.cgi`, `.pl`)
  - Configured CGI locations (e.g. `/cgi-bin`)

**Output:**
- Boolean decision: `is_cgi`

**Notes:**
- This logic belongs entirely to the CGI module or CGI dispatcher
- No execution happens at this stage

---

## Step 2: Choose Interpreter (Multi-Script Support)

**Goal:** Select the correct interpreter for the script language.

**Responsibility:**
- Map file extensions to interpreter binaries

**Example Mapping:**
- `.py` → `/usr/bin/python3`
- `.php` → `/usr/bin/php-cgi`
- `.pl` → `/usr/bin/perl`

**Design Principle:**
- Language-agnostic execution
- Adding a new language should only require updating a map or config

**Failure Cases:**
- Unsupported extension
- Interpreter not found or not executable

---

## Step 3: Prepare Execution Context

**Goal:** Validate and prepare everything before forking.

**Responsibility:**
- Resolve filesystem path to script
- Check file existence
- Check execution permissions
- Ensure interpreter is executable

**Why it matters:**
- Failing early avoids unnecessary forks
- Clear error handling

---

## Step 4: Build Environment Variables

**Goal:** Translate request data into CGI-compatible environment variables.

**Responsibility:**
- Construct `char **envp` for `execve`

**Common Variables:**
- `REQUEST_METHOD`
- `QUERY_STRING`
- `CONTENT_LENGTH`
- `CONTENT_TYPE`
- `SCRIPT_FILENAME`
- `PATH_INFO`
- `SERVER_PROTOCOL`

**Important Notes:**
- CGI programs rely entirely on these variables
- Missing or incorrect values often cause silent failures

---

## Step 5: Create Pipes for IPC

**Goal:** Enable communication between server and CGI process.

**Responsibility:**
- Create pipe for stdin (server → CGI)
- Create pipe for stdout (CGI → server)
- Optionally pipe stderr for logging

**Key Rule:**
- No parsing here, only byte transfer

---

## Step 6: Fork the Process

**Goal:** Execute CGI in an isolated process.

**Responsibility:**
- Call `fork()`
- Separate parent and child logic

**Why fork is mandatory:**
- CGI must not block the server
- Isolation prevents crashes from affecting the server

---

## Step 7: Execute Script (Child Process)

**Goal:** Replace child process with the CGI program.

**Child Responsibilities:**
- Redirect stdin/stdout using `dup2`
- Call `execve(interpreter, args, envp)`
- Exit immediately on failure

**Important Rule:**
- No server logic must run after `execve`

---

## Step 8: Handle stdin / stdout (Parent Process)

**Goal:** Exchange data with the CGI process.

**Parent Responsibilities:**
- Write POST body to CGI stdin (if applicable)
- Read all data from CGI stdout until EOF
- Close unused pipe ends

**Notes:**
- Output may be larger than buffers
- Must handle partial reads

---

## Step 9: Wait and Handle Exit Status

**Goal:** Clean up and determine execution success.

**Responsibility:**
- Use `waitpid()` to collect child status
- Store exit code separately from HTTP status

**Important Distinction:**
- Exit status ≠ HTTP status
- Exit status indicates execution success only

---

## Step 10: Timeout Protection

**Goal:** Prevent infinite or long-running CGI scripts.

**Responsibility:**
- Track execution time
- Kill CGI process if it exceeds limit

**Why this matters:**
- Prevents server starvation
- Common defense question

---

## Step 11: Return Result to Server Layer

**Goal:** Provide clean output to HTTP layer.

**CGI Module Output:**
- Raw CGI stdout
- Exit status

**Server Layer Responsibility:**
- Wrap output into HTTP response
- Decide final HTTP status code

---

## Expected Output (from CGI Module)

- **cgi_output**: Raw stdout produced by the CGI script
- **exit_status**: Process exit code

> The CGI module must not write to sockets or format HTTP responses.

---

## Final Design Principle

The CGI module is a **pure execution engine**:
- No HTTP parsing
- No socket handling
- No response formatting

It converts structured request data into environment variables, executes scripts safely, and returns results to the server layer.