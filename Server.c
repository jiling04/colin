#include "server.h"

static int send_all(int sock, const char *data, size_t length)
{
	size_t sent = 0;

	while (sent < length) {
		ssize_t result;
#ifdef MSG_NOSIGNAL
		result = send(sock, data + sent, length - sent, MSG_NOSIGNAL);
#else
		result = send(sock, data + sent, length - sent, 0);
#endif
		if (result > 0) {
			sent += (size_t)result;
			continue;
		}
		if (result == -1 && errno == EINTR) {
			continue;
		}
		return -1;
	}
	return 0;
}

int net_init(const char *ip, int port)
{
	int sockfd;
	int reuse = 1;
	struct sockaddr_in addr;

	if (port <= 0 || port > 65535) {
		fprintf(stderr, "端口号必须在 1~65535 之间\n");
		return -1;
	}

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
		perror("socket创建失败");
		return -1;
	}

	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR,
		       &reuse, sizeof(reuse)) == -1) {
		perror("设置SO_REUSEADDR失败");
		close(sockfd);
		return -1;
	}

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons((uint16_t)port);
	if (ip == NULL) {
		addr.sin_addr.s_addr = INADDR_ANY;
	} else if (inet_pton(AF_INET, ip, &addr.sin_addr) != 1) {
		fprintf(stderr, "IPv4地址无效: %s\n", ip);
		close(sockfd);
		return -1;
	}

	if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
		perror("网络绑定失败");
		close(sockfd);
		return -1;
	}

	if (listen(sockfd, 128) == -1) {
		perror("监听失败");
		close(sockfd);
		return -1;
	}

	printf("监听成功: %s:%d, server_sock=%d\n",
	       ip == NULL ? "0.0.0.0" : ip, port, sockfd);
	return sockfd;
}

int first_response(int sock, const char *cip)
{
	char response[256];
	int length;

	length = snprintf(response, sizeof(response), "连接成功 cip:%s\n",
			  cip == NULL ? "unknown" : cip);
	if (length < 0 || (size_t)length >= sizeof(response)) {
		return -1;
	}

	return send_all(sock, response, (size_t)length);
}
