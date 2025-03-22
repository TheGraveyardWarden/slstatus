#if defined(__linux__)

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <net/if.h>
#include "../exec.h"
#include "../util.h"

const char * netip(const char *interface)
{
	int sockfd;
	struct ifreq ifr;

	if (interface == NULL)
		return NULL;

	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		return NULL;
	}

	memset(&ifr, 0, sizeof(ifr));
	strncpy(ifr.ifr_name, interface, IFNAMSIZ);

	if (ioctl(sockfd, SIOCGIFADDR, &ifr) < 0)
	{
		return NULL;
	}

	close(sockfd);

	return inet_ntoa(((struct sockaddr_in*)&ifr.ifr_addr)->sin_addr);
}

#define IP_MAX_BUFLEN 16
#define IF_MAX_BUFLEN 20

const char *net_info(void) {
	char ip[IP_MAX_BUFLEN];
	char if_net[IF_MAX_BUFLEN];
	struct exec_ret ret;
	
	ret = exec("ip route | grep default");
	if (ret.exit != 0) {
		goto err;
	}
	exec_ret_close(&ret);

	if (get_net_if(if_net, IF_MAX_BUFLEN) < 0)
		goto err;

	if (get_ip(ip, IP_MAX_BUFLEN) < 0)
		goto err;

	return bprintf("%s - %s", if_net, ip);

err:
		return bprintf("network not available");
}

#endif

