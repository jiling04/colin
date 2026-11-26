#include "server.h"

#define READ_BUFFER_SIZE 4096

static volatile sig_atomic_t stop_server = 0;

static void on_signal(int signal_number)
{
	(void)signal_number;
	stop_server = 1;
}

static int set_nonblocking(int fd)
{
	int flags = fcntl(fd, F_GETFL, 0);

	if (flags == -1) {
		return -1;
	}
	return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

static int refresh_max_fd(int server_sock, const int *clients,
			  int client_count)
{
	int i;
	int max_fd = server_sock;

	for (i = 0; i < client_count; i++) {
		if (clients[i] > max_fd) {
			max_fd = clients[i];
		}
	}
	return max_fd;
}

static void remove_client(int *clients, int *client_count, int index,
			  int client_sock, fd_set *monitor_set)
{
	FD_CLR(client_sock, monitor_set);
	close(client_sock);

	(*client_count)--;
	if (index != *client_count) {
		clients[index] = clients[*client_count];
	}
	printf("客户端断开: client_sock=%d, 当前客户端数=%d\n",
	       client_sock, *client_count);
}

/*
 * select 已报告该 socket 可读，因此这里循环 recv，把当前接收缓冲区中的
 * 数据全部取完。客户端 socket 已设为非阻塞，缓冲区读空时返回 EAGAIN。
 * 返回 0 表示连接仍然有效，返回 -1 表示对端关闭或发生错误。
 */
static int receive_client_data(int client_sock)
{
	char buffer[READ_BUFFER_SIZE];

	for (;;) {
		ssize_t received = recv(client_sock, buffer, sizeof(buffer), 0);

		if (received > 0) {
			printf("收到 client_sock=%d 的数据(%zd字节):\n",
			       client_sock, received);
			fwrite(buffer, 1, (size_t)received, stdout);
			if (buffer[received - 1] != '\n') {
				putchar('\n');
			}
			fflush(stdout);
			continue;
		}
		if (received == 0) {
			return -1;
		}
		if (errno == EINTR) {
			continue;
		}
		if (errno == EAGAIN || errno == EWOULDBLOCK) {
			return 0;
		}

		perror("recv失败");
		return -1;
	}
}

int selectModel(const char *ip, int port)
{
	int server_sock;
	int client_sock_array[MAX_CLIENTS];
	int client_count = 0;
	int max_fd;
	fd_set monitor_set; /* 专门保存所有需要长期监听的 socket */
	fd_set ready_set;   /* 每轮由 select 改写，只保存当前就绪的 socket */

	server_sock = net_init(ip, port);
	if (server_sock == -1) {
		return -1;
	}
	if (server_sock >= FD_SETSIZE) {
		fprintf(stderr, "server_sock=%d 超出fd_set支持范围\n",
			server_sock);
		close(server_sock);
		return -1;
	}

	FD_ZERO(&monitor_set);
	FD_SET(server_sock, &monitor_set);
	max_fd = server_sock;

	while (!stop_server) {
		int ready_count;
		int i;
		struct sockaddr_in client_addr;
		socklen_t client_addr_length = sizeof(client_addr);

		/*
		 * select 会把传入的集合改成“就绪集合”。必须从 monitor_set
		 * 重新拷贝，否则下一轮会漏掉本轮没有就绪的 socket。
		 */
		ready_set = monitor_set;
		ready_count = select(max_fd + 1, &ready_set, NULL, NULL, NULL);
		if (ready_count == -1) {
			if (errno == EINTR) {
				continue;
			}
			perror("select失败");
			break;
		}

		if (FD_ISSET(server_sock, &ready_set)) {
			int client_sock = accept(server_sock,
				(struct sockaddr *)&client_addr,
				&client_addr_length);

			if (client_sock == -1) {
				if (errno != EINTR) {
					perror("accept失败");
				}
			} else if (client_sock >= FD_SETSIZE) {
				fprintf(stderr,
					"client_sock=%d 超出fd_set支持范围\n",
					client_sock);
				close(client_sock);
			} else if (client_count >= MAX_CLIENTS) {
				const char *busy = "服务器连接数已满\n";
				send(client_sock, busy, strlen(busy), 0);
				close(client_sock);
			} else {
				char client_ip[INET_ADDRSTRLEN] = "unknown";

				inet_ntop(AF_INET, &client_addr.sin_addr,
					  client_ip, sizeof(client_ip));
				if (first_response(client_sock, client_ip) == -1 ||
				    set_nonblocking(client_sock) == -1) {
					perror("初始化新客户端失败");
					close(client_sock);
				} else {
					client_sock_array[client_count++] =
						client_sock;
					FD_SET(client_sock, &monitor_set);
					if (client_sock > max_fd) {
						max_fd = client_sock;
					}
					printf("新客户端: %s:%u, client_sock=%d, "
					       "当前客户端数=%d\n",
					       client_ip,
					       ntohs(client_addr.sin_port),
					       client_sock, client_count);
				}
			}
		}

		/*
		 * 用就绪集合遍历客户端数组。关闭一个客户端后，用数组末尾元素
		 * 补位，所以删除后保持 i 不变，继续检查补位过来的 socket。
		 */
		i = 0;
		while (i < client_count) {
			int client_sock = client_sock_array[i];

			if (FD_ISSET(client_sock, &ready_set) &&
			    receive_client_data(client_sock) == -1) {
				remove_client(client_sock_array, &client_count, i,
					      client_sock, &monitor_set);
				continue;
			}
			i++;
		}

		/*
		 * 删除的可能正好是最大文件描述符。重新计算后，传给
		 * select 的第一个参数仍应是“最大fd + 1”。
		 */
		max_fd = refresh_max_fd(server_sock, client_sock_array,
					client_count);
	}

	while (client_count > 0) {
		client_count--;
		close(client_sock_array[client_count]);
	}
	close(server_sock);
	printf("服务器已退出\n");
	return 0;
}

int main(int argc, char *argv[])
{
	const char *ip = SERVER_DEFAULT_IP;
	int port = SERVER_DEFAULT_PORT;
	struct sigaction action;

	if (argc > 3) {
		fprintf(stderr, "用法: %s [IPv4地址] [端口]\n", argv[0]);
		return EXIT_FAILURE;
	}
	if (argc >= 2) {
		ip = argv[1];
	}
	if (argc == 3) {
		char *end = NULL;
		long value = strtol(argv[2], &end, 10);

		if (end == argv[2] || *end != '\0' ||
		    value <= 0 || value > 65535) {
			fprintf(stderr, "无效端口: %s\n", argv[2]);
			return EXIT_FAILURE;
		}
		port = (int)value;
	}

	setvbuf(stdout, NULL, _IONBF, 0);

	memset(&action, 0, sizeof(action));
	action.sa_handler = on_signal;
	sigemptyset(&action.sa_mask);
	sigaction(SIGINT, &action, NULL);
	sigaction(SIGTERM, &action, NULL);
	signal(SIGPIPE, SIG_IGN);

	return selectModel(ip, port) == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
