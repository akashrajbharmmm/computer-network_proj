# 🔗 DNS Lookup Tool (C++)

This project is a simple **DNS Lookup Tool** written in **C++** that uses the **Windows Socket API (Winsock2)** to resolve domain names to their corresponding IP addresses and vice versa.

---

## 🧠 Features

- Resolves **domain name → IP address**
- Resolves **IP address → domain name (reverse lookup)**
- Uses the **Windows Sockets API (Winsock2)**
- Includes **error handling** for invalid inputs
- Lightweight and fast console-based program

---

## ⚙️ Requirements

- **Operating System:** Windows 7 or later  
- **Compiler:** MinGW / MSVC (Visual Studio)  
- **Libraries:**  
  - `ws2_32.lib` (Windows Socket Library)

---

## 🧩 Setup & Installation

### Step 1: Clone the Repository

```bash
git clone https://github.com/your-username/dns-lookup-tool.git
cd dns-lookup-tool


g++ dns_lookup.cpp -o dns_lookup -lws2_32


cl dns_lookup.cpp ws2_32.lib


dns_lookup
