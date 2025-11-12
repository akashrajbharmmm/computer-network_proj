#define _WIN32_WINNT 0x0601   // Ensure modern Winsock features
#include <iostream>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

using namespace std;

// ✅ Fallback implementations for inet_pton / inet_ntop (if missing)
#ifndef InetPtonA
int InetPtonA(int af, const char *src, void *dst) {
    sockaddr_storage ss;
    int size = sizeof(ss);
    char buf[INET6_ADDRSTRLEN + 1] = {0};

    snprintf(buf, sizeof(buf), "%s", src);
    ZeroMemory(&ss, sizeof(ss));

    if (WSAStringToAddressA((LPSTR)buf, af, nullptr, (LPSOCKADDR)&ss, &size) == 0) {
        if (af == AF_INET)
            *(IN_ADDR *)dst = ((sockaddr_in *)&ss)->sin_addr;
        else
            *(IN6_ADDR *)dst = ((sockaddr_in6 *)&ss)->sin6_addr;
        return 1;
    }
    return 0;
}
#endif

#ifndef InetNtopA
const char *InetNtopA(int af, const void *src, char *dst, socklen_t size) {
    sockaddr_storage ss;
    DWORD addrlen = sizeof(ss);

    ZeroMemory(&ss, sizeof(ss));
    if (af == AF_INET) {
        ((sockaddr_in *)&ss)->sin_family = AF_INET;
        memcpy(&((sockaddr_in *)&ss)->sin_addr, src, sizeof(IN_ADDR));
    } else if (af == AF_INET6) {
        ((sockaddr_in6 *)&ss)->sin6_family = AF_INET6;
        memcpy(&((sockaddr_in6 *)&ss)->sin6_addr, src, sizeof(IN6_ADDR));
    } else {
        return nullptr;
    }

    if (WSAAddressToStringA((LPSOCKADDR)&ss, addrlen, nullptr, dst, &addrlen) == 0) {
        return dst;
    }
    return nullptr;
}
#endif

// ✅ Function: Resolve domain → IP
void resolveDomain(const string &domain) {
    struct addrinfo hints, *res, *p;
    char ipstr[INET6_ADDRSTRLEN];

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    int status = getaddrinfo(domain.c_str(), nullptr, &hints, &res);
    if (status != 0) {
        cerr << "Error: Unable to resolve domain (" << gai_strerrorA(status) << ")" << endl;
        return;
    }

    cout << "[+] Domain Name: " << domain << endl;
    cout << "[+] IP Addresses:\n";

    for (p = res; p != nullptr; p = p->ai_next) {
        void *addr = nullptr;
        string ipver;

        if (p->ai_family == AF_INET) {
            struct sockaddr_in *ipv4 = reinterpret_cast<struct sockaddr_in *>(p->ai_addr);
            addr = &(ipv4->sin_addr);
            ipver = "IPv4";
        } else if (p->ai_family == AF_INET6) {
            struct sockaddr_in6 *ipv6 = reinterpret_cast<struct sockaddr_in6 *>(p->ai_addr);
            addr = &(ipv6->sin6_addr);
            ipver = "IPv6";
        } else {
            continue;
        }

        if (InetNtopA(p->ai_family, addr, ipstr, sizeof(ipstr)) != nullptr) {
            cout << "    " << ipver << ": " << ipstr << endl;
        } else {
            cout << "    " << ipver << ": (conversion failed)" << endl;
        }
    }

    freeaddrinfo(res);
}

// ✅ Function: Reverse lookup (IP → Domain)
void reverseLookup(const string &ip) {
    struct sockaddr_in sa;
    char host[NI_MAXHOST];

    memset(&sa, 0, sizeof(sa));
    sa.sin_family = AF_INET;

    if (InetPtonA(AF_INET, ip.c_str(), &sa.sin_addr) != 1) {
        cerr << "Invalid IP address format." << endl;
        return;
    }

    if (getnameinfo(reinterpret_cast<struct sockaddr *>(&sa),
                    sizeof(sa),
                    host, sizeof(host),
                    nullptr, 0, 0) == 0) {
        cout << "[+] Reverse DNS Lookup:\n";
        cout << "    IP Address : " << ip << endl;
        cout << "    Hostname   : " << host << endl;
    } else {
        cout << "[-] Reverse lookup failed for IP: " << ip << endl;
    }
}

// ✅ Main function (supports both CLI and automated input)
int main(int argc, char *argv[]) {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cerr << "WSAStartup failed!" << endl;
        return 1;
    }

    // If arguments are passed (used by frontend/Node bridge)
    if (argc == 3) {
        int choice = stoi(argv[1]);
        string query = argv[2];
        if (choice == 1)
            resolveDomain(query);
        else if (choice == 2)
            reverseLookup(query);
        else
            cout << "Invalid argument" << endl;
    }
    // Otherwise run in interactive (manual) mode
    else {
        cout << "=== DNS Lookup Tool ===\n";
        cout << "1. Domain to IP Lookup\n";
        cout << "2. IP to Domain (Reverse Lookup)\n";
        cout << "Choose an option: ";

        int choice;
        cin >> choice;

        if (choice == 1) {
            string domain;
            cout << "Enter domain name (e.g., www.google.com): ";
            cin >> domain;
            resolveDomain(domain);
        } else if (choice == 2) {
            string ip;
            cout << "Enter IP address (e.g., 8.8.8.8): ";
            cin >> ip;
            reverseLookup(ip);
        } else {
            cout << "Invalid option!" << endl;
        }
    }

    WSACleanup();
    return 0;
}

