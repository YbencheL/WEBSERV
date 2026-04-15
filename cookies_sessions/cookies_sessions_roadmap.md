# 🍪 Cookies & Sessions — Webserv (42) Roadmap

---

## 1. 🔥 Core Concept

### HTTP is Stateless
- Each request is independent
- Server does NOT remember previous requests

👉 Cookies + Sessions allow state persistence

---

## 2. 🍪 Cookies — Theory

### What is a Cookie?
- Small data sent from server → stored in browser
- Automatically sent back in future requests

### Flow
1. Server sends:
Set-Cookie: sessionId=abc123

2. Browser stores cookie

3. Browser sends:
Cookie: sessionId=abc123

---

### Types of Cookies

#### Session Cookie
Set-Cookie: sessionId=abc123
- Deleted when browser closes

#### Persistent Cookie
Set-Cookie: sessionId=abc123; Max-Age=3600
- Stored for a defined duration

---

## 3. 🧠 Sessions — Theory

### What is a Session?
- Data stored on the SERVER
- Identified by a session ID stored in a cookie

👉 Cookie = key  
👉 Session = data

### Example

Request:
Cookie: sessionId=abc123

Server:
session_map["abc123"] = { user: "yassin", logged_in: true };

---

## 4. ⚙️ Required Features (Webserv Bonus)

- Parse `Cookie` header
- Send `Set-Cookie` header
- Generate session IDs
- Store sessions server-side
- Reuse session across requests

---

## 5. 🏗️ Architecture

### Add Classes

Cookie  
Session  
SessionManager  

---

## 6. 🧩 Data Structures

### Session Storage

std::map<std::string, Session> sessions;

### Session Class

class Session {
public:
    std::string id;
    std::map<std::string, std::string> data;
    time_t last_access;
};

---

## 7. 🔧 Implementation Steps

### STEP 1 — Parse Cookies

Input:
Cookie: sessionId=abc123; theme=dark

Function:
std::map<std::string, std::string> parseCookies(std::string header);

---

### STEP 2 — Generate Session ID

std::string generateSessionId();

---

### STEP 3 — Session Manager

class SessionManager {
private:
    std::map<std::string, Session> sessions;

public:
    Session& getSession(const std::string& sessionId);
    Session& createSession();
    bool sessionExists(const std::string& id);
};

---

### STEP 4 — Get or Create Session

IF request has cookie:
    IF session exists:
        use it
    ELSE:
        create new session
ELSE:
    create new session

---

### STEP 5 — Attach Cookie

Set-Cookie: sessionId=abc123; Path=/; HttpOnly

---

### STEP 6 — Store Data

session.data["username"] = "yassin";

---

### STEP 7 — Expiration

if (current_time - session.last_access > TIMEOUT)
    delete session;

---

## 8. 🔄 Flow

Client Request → Parse → Cookies → Session → Response

---

## 9. 🧪 Testing

curl -i http://localhost:8080  
curl -i --cookie "sessionId=abc123" http://localhost:8080

---

## ✅ DONE
