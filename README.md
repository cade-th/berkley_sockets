Notes:

void? connect(int sockfd, const struct *sockaddr, socklen_t addrlen)

int socket(int family, int type, int protocol)

int bind(int sockfd, const struct sockaddr *servaddr, socklen_t, addrlen)

int listen(int sockfd, int backlog)

int accpet (int sockfd, struct sockaddr *cliaddr, sockelen_t *addrlen)

Server-Side Flow:
1. Socket
2. Bind
3. Listen
4. Accept
5. Read
6. Close

Client-Side Flow:
1. Socket
2. Connect
3. Write
4. Close

getsockname() - retrieves port number bound to a socket
gethostbyname() - finds ip address for hostname
read-write loop, read does not always fill the buffer


