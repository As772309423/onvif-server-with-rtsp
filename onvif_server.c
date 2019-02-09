#include "common.h"
#include "soapH.h"


static SOAP_SOCKET SoapBind(struct soap *pSoap, const char *pIp, bool flag)
{
    SOAP_SOCKET sockFD = SOAP_INVALID_SOCKET;
    if (flag)
    {
        sockFD = soap_bind(pSoap, ONVIF_UDP_IP, pSoap->port, 10);
        if (soap_valid_socket(sockFD))
        {
            printf("%s:%s:%d flag = %d, sockFD = %d, pSoap->master = %d\n", __FILE__, __func__, __LINE__, flag, sockFD, pSoap->master);
        }
        else
        {
            printf("%s:%s:%d flag = %d\n", __FILE__, __func__, __LINE__, flag);
            soap_print_fault(pSoap, stderr);
        }
    }
    else
    {
        sockFD = soap_bind(pSoap, pIp, pSoap->port, 10);
        if (soap_valid_socket(sockFD))
            printf("%s:%s:%d flag = %d, sockFD = %d, pSoap->master = %d\n", __FILE__, __func__, __LINE__, flag, sockFD, pSoap->master);
        else
        {
            printf("%s:%s:%d flag = %d\n", __FILE__, __func__, __LINE__, flag);
            soap_print_fault(pSoap, stderr);
        }
    }

    return sockFD;
}






//加入设备组，设备发现
static void *OnvifBeDiscovered(void *arg) {

    printf("[%s][%d][%s][%s] start \n", __FILE__, __LINE__, __TIME__, __func__);

    struct soap UDPserverSoap;
    struct ip_mreq mcast;

    soap_init1(&UDPserverSoap, SOAP_IO_UDP | SOAP_XML_IGNORENS);
    soap_set_namespaces(&UDPserverSoap,  namespaces);

    printf("[%s][%d][%s][%s] UDPserverSoap.version = %d \n", __FILE__, __LINE__, __TIME__, __func__, UDPserverSoap.version);

    int m = soap_bind(&UDPserverSoap, NULL, ONVIF_UDP_PORT, 10);
    if(!soap_valid_socket(m))
    {
        soap_print_fault(&UDPserverSoap, stderr);
        exit(1);
    }
    printf("socket bind success %d\n", m);

    mcast.imr_multiaddr.s_addr = inet_addr(ONVIF_UDP_IP);
    mcast.imr_interface.s_addr = htonl(INADDR_ANY);
    //开启route
    system("route add -net 224.0.0.0 netmask 224.0.0.0 eth0");
    //IP_ADD_MEMBERSHIP用于加入某个多播组，之后就可以向这个多播组发送数据或者从多播组接收数据
    if(setsockopt(UDPserverSoap.master, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&mcast, sizeof(mcast)) < 0)
    {
            printf("setsockopt error! error code = %d,err string = %s\n",errno,strerror(errno));
        return 0;
    }

    int fd = -1;
    while(1)
    {
        printf("socket connect %d\n", fd);
        fd = soap_accept(&UDPserverSoap);
        if (!soap_valid_socket(fd)) {
            soap_print_fault(&UDPserverSoap, stderr);
            exit(1);
        }

        printf("-----------------------------------\n\n");

        if( soap_serve(&UDPserverSoap) != SOAP_OK )
        {
            soap_print_fault(&UDPserverSoap, stderr);
            printf("soap_print_fault\n");
        }

        printf("IP = %u.%u.%u.%u\n", ((UDPserverSoap.ip)>>24)&0xFF, ((UDPserverSoap.ip)>>16)&0xFF, ((UDPserverSoap.ip)>>8)&0xFF,(UDPserverSoap.ip)&0xFF);
        soap_destroy(&UDPserverSoap);
        soap_end(&UDPserverSoap);
    }
    //分离运行时的环境
    soap_done(&UDPserverSoap);
    pthread_exit(0);
}


//监听soap报文
static void *OnvifWebServices(void *arg) {

    struct soap tcpSoap;
    soap_init(&tcpSoap);
    tcpSoap.port = ONVIF_TCP_PORT;
    tcpSoap.bind_flags = SO_REUSEADDR;
    //tcpSoap.accept_timeout = tcpSoap.recv_timeout = tcpSoap.send_timeout = 5;
    soap_set_namespaces(&tcpSoap, namespaces);

    char ip[20] = {0};
    getLocalIP(ip, 20);
    int tcpsocket = SoapBind(&tcpSoap, ip, false);
    if (!soap_valid_socket(tcpsocket)) {
        printf("tcpsocket SoapBind failed!\n");
        soap_print_fault(&tcpSoap, stderr);
        exit(1);
    }

    int tcp_fd = -1;
    while(1) {

        printf("socket connect %d\n", tcp_fd);
        tcp_fd = soap_accept(&tcpSoap);
        if (!soap_valid_socket(tcp_fd)) {
            soap_print_fault(&tcpSoap, stderr);
            exit(1);
        }

        printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n\n");

        if( soap_serve(&tcpSoap) != SOAP_OK )
        {
            soap_print_fault(&tcpSoap, stderr);
            printf("soap_print_fault\n");
        }

        printf("IP = %u.%u.%u.%u\n", ((tcpSoap.ip)>>24)&0xFF, ((tcpSoap.ip)>>16)&0xFF, ((tcpSoap.ip)>>8)&0xFF,(tcpSoap.ip)&0xFF);
        soap_destroy(&tcpSoap);
        soap_end(&tcpSoap);
    }

    //分离运行时的环境
    soap_done(&tcpSoap);
    pthread_exit(0);
}


int main(int argc,char ** argv)
{
    pthread_t discover = 0;
    pthread_t webservice = 0;
    pthread_create(&discover, NULL, OnvifBeDiscovered, NULL);
    pthread_create(&discover, NULL, OnvifWebServices, NULL);

    pthread_join(discover, 0);

    return 0;
}








