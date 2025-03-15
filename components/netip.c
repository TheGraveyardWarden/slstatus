#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <net/if.h>

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
