#include "common.h"
#include "wsaapi.h"
#include "soapH.h"
#include "soapStub.h"
#include "wsdd.nsmap"

//执行系统命令
static int readInfoByPipe(const char *order, char *buff, unsigned int readSize)
{
  //1.打开管道，并将命令执行结果保存到文本
  FILE  *stream = popen(order, "r");
  if( !stream ) {
    printf("popen error : %s",strerror(errno));
    return -1;
  }
  //2.读取管道数据
  bzero(buff, readSize);
  fread(buff, readSize, 1, stream);
  //3.关闭管道
  pclose(stream);

  return 0;
}

//获取本地IP
static void getLocalIP(char *ip, unsigned int readSize)
{
	char IP[20];
	char CMD[] = "ifconfig eth0 | grep 'inet addr' | awk '{print $2}' | cut -f 2 -d ':'";
	memset(IP, '\0', 20);
	readInfoByPipe(CMD, IP, readSize);
	//去除\n
	int len = strlen(IP) > 0 ? strlen(IP) : 1;
	if ( len == 1 ) {
		printf("get ip failed!\n");
		return ;
	}
	IP[len - 1] = '\0';
	//判断是否还未分配IP
    if ( strstr(IP, "117.117.117.117") ) {
    	memset(IP, '\0', readSize);
    	memcpy(IP, ONVIF_TCP_IP, strlen(ONVIF_TCP_IP)+1);
    }
	memcpy(ip, IP, len);
	return ;
}

SOAP_FMAC5 int SOAP_FMAC6 SOAP_ENV__Fault(struct soap* soap, char *faultcode, char *faultstring, char *faultactor,
struct SOAP_ENV__Detail *detail, struct SOAP_ENV__Code *_SOAP_ENV__Code,
struct SOAP_ENV__Reason *SOAP_ENV__Reason, char *SOAP_ENV__Node,
	char *SOAP_ENV__Role, struct SOAP_ENV__Detail *SOAP_ENV__Detail)
{
 
	return 0;
}

SOAP_FMAC5 int SOAP_FMAC6 __wsdd__Hello(struct soap* soap, struct wsdd__HelloType *wsdd__Hello)
{
 
	return 0;
}
 
SOAP_FMAC5 int SOAP_FMAC6 __wsdd__Bye(struct soap* soap, struct wsdd__ByeType *wsdd__Bye)
{
 
	return 0;
}
 
SOAP_FMAC5 int SOAP_FMAC6 __wsdd__ProbeMatches(struct soap* soap, struct wsdd__ProbeMatchesType *wsdd__ProbeMatches)
{

    printf("-----------__wsdd__ProbeMatches-----------------\n");

    return SOAP_OK;
}
 
SOAP_FMAC5 int SOAP_FMAC6 __wsdd__Resolve(struct soap* soap, struct wsdd__ResolveType *wsdd__Resolve)
{
 
	return 0;
}
 
SOAP_FMAC5 int SOAP_FMAC6 __wsdd__ResolveMatches(struct soap* soap, struct wsdd__ResolveMatchesType *wsdd__ResolveMatches)
{
 
	return 0;
}
 
 
#define IP 192.168.20.1					//服务器IP
#define ONVIF_TCP_PORT 5000 			//服务器访问端口（随意设置）
SOAP_FMAC5 int SOAP_FMAC6  __wsdd__Probe(struct soap* soap, struct wsdd__ProbeType *wsdd__Probe)
{
        struct wsdd__ScopesType         *pScopes = NULL;
        char                            str_tmp[256] = {0};

        char scopes_message[] =
            "onvif://www.onvif.org/type/NetworkVideoTransmitter\r\n"
            "onvif://www.onvif.org/Profile/Streaming\r\n"
            "onvif://www.onvif.org/Profile/Q/Operational\r\n"
            "onvif://www.onvif.org/hardware/HD1080P\r\n"
            "onvif://www.onvif.org/name/discover_test\r\n"
            "onvif://www.onvif.org/location/city/GuangZhou\r\n"
            "onvif://www.onvif.org/location/country/China\r\n";

        // response ProbeMatches
        struct wsdd__ProbeMatchesType   wsdd__ProbeMatches = {0};
        struct wsdd__ProbeMatchType     *pProbeMatchType = NULL;
        struct wsa__Relationship        *pWsa__RelatesTo = NULL;

        pProbeMatchType = (struct wsdd__ProbeMatchType*)soap_malloc(soap, sizeof(struct wsdd__ProbeMatchType));
        soap_default_wsdd__ProbeMatchType(soap, pProbeMatchType);

        sprintf(str_tmp, "http://%s:%d/onvif/device_service", IP, ONVIF_TCP_PORT);
        pProbeMatchType->XAddrs = soap_strdup(soap, str_tmp);
        if( wsdd__Probe->Types && strlen(wsdd__Probe->Types) )
            pProbeMatchType->Types  = soap_strdup(soap, wsdd__Probe->Types);
        else
            pProbeMatchType->Types  = soap_strdup(soap, "dn:NetworkVideoTransmitter tds:Device");

        pProbeMatchType->MetadataVersion = 1;

        // Build Scopes Message
        pScopes = (struct wsdd__ScopesType *)soap_malloc(soap, sizeof(struct wsdd__ScopesType));
        soap_default_wsdd__ScopesType(soap, pScopes);
        pScopes->MatchBy = NULL;
        pScopes->__item  = soap_strdup(soap, scopes_message);
        pProbeMatchType->Scopes = pScopes;

		char g_uuid[64];
		snprintf(g_uuid, 64, "%s", soap_wsa_rand_uuid(soap));
        pProbeMatchType->wsa__EndpointReference.Address = soap_strdup(soap, g_uuid);

        wsdd__ProbeMatches.__sizeProbeMatch = 1;
        wsdd__ProbeMatches.ProbeMatch       = pProbeMatchType;

        // Build SOAP Header
        pWsa__RelatesTo = (struct wsa__Relationship*)soap_malloc(soap, sizeof(struct wsa__Relationship));
        soap_default__wsa__RelatesTo(soap, pWsa__RelatesTo);
        pWsa__RelatesTo->__item = soap->header->wsa__MessageID;
        soap->header->wsa__RelatesTo = pWsa__RelatesTo;
        soap->header->wsa__Action      = soap_strdup(soap, "http://schemas.xmlsoap.org/ws/2005/04/discovery/ProbeMatches");
        soap->header->wsa__To          = soap_strdup(soap, "http://schemas.xmlsoap.org/ws/2004/08/addressing/role/anonymous");

        soap_send___wsdd__ProbeMatches(soap, "http://", NULL, &wsdd__ProbeMatches);

	return SOAP_OK;
    
}


/** Web service operation '__tdn__Hello' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tdn__Hello(struct soap* soap, struct wsdd__HelloType tdn__Hello, struct wsdd__ResolveType *tdn__HelloResponse)
{
    return 0;
}

/** Web service operation '__tdn__Bye' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tdn__Bye(struct soap* soap, struct wsdd__ByeType tdn__Bye, struct wsdd__ResolveType *tdn__ByeResponse)
{
    return 0;
}

/** Web service operation '__tdn__Probe' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tdn__Probe(struct soap* soap, struct wsdd__ProbeType tdn__Probe, struct wsdd__ProbeMatchesType *tdn__ProbeResponse)
{
    return 0;
}

/** Web service operation '__tds__GetServices' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__GetServices(struct soap* soap, struct _tds__GetServices *tds__GetServices, struct _tds__GetServicesResponse *tds__GetServicesResponse)
{
    printf("-------------------------------__tds__GetServices-------------------------------------\n");
    int size = 2;
    tds__GetServicesResponse->__sizeService = size;
    tds__GetServicesResponse->Service = (struct tds__Service *)soap_malloc(soap, sizeof(struct tds__Service) * size);

    //get IP
    char IP[20] = {0};
    getLocalIP(IP, 20);

    //device
    int i = 0;
    tds__GetServicesResponse->Service[i].Namespace = (char *)soap_malloc(soap, sizeof(char)* 100);
    strcpy(tds__GetServicesResponse->Service[i].Namespace, "http://www.onvif.org/ver10/device/wsdl");
    tds__GetServicesResponse->Service[i].XAddr     = (char *)soap_malloc(soap, sizeof(char)* 100);
    sprintf(tds__GetServicesResponse->Service[i].XAddr, "http://%s:%d/onvif/device_service", IP, ONVIF_TCP_PORT);

    //media
    i = 1;
    tds__GetServicesResponse->Service[i].Namespace = (char *)soap_malloc(soap, sizeof(char)* 100);
    strcpy(tds__GetServicesResponse->Service[i].Namespace, "http://www.onvif.org/ver10/media/wsdl");
    tds__GetServicesResponse->Service[i].XAddr     = (char *)soap_malloc(soap, sizeof(char)* 100);
    sprintf(tds__GetServicesResponse->Service[i].XAddr, "http://%s:%d/onvif/media_service", IP, ONVIF_TCP_PORT);

    for(i=0; i<tds__GetServicesResponse->__sizeService; i++) {
        tds__GetServicesResponse->Service[i].Capabilities = NULL;
        tds__GetServicesResponse->Service[i].Version = (struct tt__OnvifVersion *)soap_malloc(soap, sizeof(struct tt__OnvifVersion));
        tds__GetServicesResponse->Service[i].Version->Major = 1;
        tds__GetServicesResponse->Service[i].Version->Minor = 10;
        tds__GetServicesResponse->Service[i].__size = 0;
        tds__GetServicesResponse->Service[i].__any = NULL;
        tds__GetServicesResponse->Service[i].__anyAttribute = NULL;
    }


    printf("__tds__GetServices is over\n");

    return 0;
}

/** Web service operation '__tds__GetServiceCapabilities' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__GetServiceCapabilities(struct soap* soap, struct _tds__GetServiceCapabilities *tds__GetServiceCapabilities, struct _tds__GetServiceCapabilitiesResponse *tds__GetServiceCapabilitiesResponse)
{
    return 0;
}

/** Web service operation '__tds__GetDeviceInformation' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__GetDeviceInformation(struct soap* soap, struct _tds__GetDeviceInformation *tds__GetDeviceInformation, struct _tds__GetDeviceInformationResponse *tds__GetDeviceInformationResponse)
{
    printf("-------------------------__tds__GetDeviceInformation------------------------------\n");
    tds__GetDeviceInformationResponse->Manufacturer = (char *)soap_malloc(soap, sizeof(char) * 32);
    tds__GetDeviceInformationResponse->Model = (char *)soap_malloc(soap, sizeof(char) * 32);
    tds__GetDeviceInformationResponse->FirmwareVersion = (char *)soap_malloc(soap, sizeof(char) * 32);
    tds__GetDeviceInformationResponse->SerialNumber = (char *)soap_malloc(soap, sizeof(char) * 32);
    tds__GetDeviceInformationResponse->HardwareId = (char *)soap_malloc(soap, sizeof(char) * 32);

    strcpy(tds__GetDeviceInformationResponse->Manufacturer, "STONKAM");
    strcpy(tds__GetDeviceInformationResponse->Model, "HD1080P");
    strcpy(tds__GetDeviceInformationResponse->FirmwareVersion, "v1.0.9");
    strcpy(tds__GetDeviceInformationResponse->SerialNumber, "IPC123456");
    strcpy(tds__GetDeviceInformationResponse->HardwareId, "1.0");

    return 0;
}

/** Web service operation '__tds__SetSystemDateAndTime' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__SetSystemDateAndTime(struct soap* soap, struct _tds__SetSystemDateAndTime *tds__SetSystemDateAndTime, struct _tds__SetSystemDateAndTimeResponse *tds__SetSystemDateAndTimeResponse)
{
    return 0;
}

/** Web service operation '__tds__GetSystemDateAndTime' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__GetSystemDateAndTime(struct soap* soap, struct _tds__GetSystemDateAndTime *tds__GetSystemDateAndTime, struct _tds__GetSystemDateAndTimeResponse *tds__GetSystemDateAndTimeResponse)
{
    return 0;
}

/** Web service operation '__tds__SetSystemFactoryDefault' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__SetSystemFactoryDefault(struct soap* soap, struct _tds__SetSystemFactoryDefault *tds__SetSystemFactoryDefault, struct _tds__SetSystemFactoryDefaultResponse *tds__SetSystemFactoryDefaultResponse)
{
    return 0;
}

/** Web service operation '__tds__UpgradeSystemFirmware' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__UpgradeSystemFirmware(struct soap* soap, struct _tds__UpgradeSystemFirmware *tds__UpgradeSystemFirmware, struct _tds__UpgradeSystemFirmwareResponse *tds__UpgradeSystemFirmwareResponse)
{
    return 0;
}

/** Web service operation '__tds__SystemReboot' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__SystemReboot(struct soap* soap, struct _tds__SystemReboot *tds__SystemReboot, struct _tds__SystemRebootResponse *tds__SystemRebootResponse)
{
    return 0;
}

/** Web service operation '__tds__RestoreSystem' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__RestoreSystem(struct soap* soap, struct _tds__RestoreSystem *tds__RestoreSystem, struct _tds__RestoreSystemResponse *tds__RestoreSystemResponse)
{
    return 0;
}

/** Web service operation '__tds__GetSystemBackup' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__GetSystemBackup(struct soap* soap, struct _tds__GetSystemBackup *tds__GetSystemBackup, struct _tds__GetSystemBackupResponse *tds__GetSystemBackupResponse)
{
    return 0;
}

/** Web service operation '__tds__GetSystemLog' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__GetSystemLog(struct soap* soap, struct _tds__GetSystemLog *tds__GetSystemLog, struct _tds__GetSystemLogResponse *tds__GetSystemLogResponse)
{
    return 0;
}

/** Web service operation '__tds__GetSystemSupportInformation' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__GetSystemSupportInformation(struct soap* soap, struct _tds__GetSystemSupportInformation *tds__GetSystemSupportInformation, struct _tds__GetSystemSupportInformationResponse *tds__GetSystemSupportInformationResponse)
{
    return 0;
}

/** Web service operation '__tds__GetScopes' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__GetScopes(struct soap* soap, struct _tds__GetScopes *tds__GetScopes, struct _tds__GetScopesResponse *tds__GetScopesResponse)
{
    printf("--------------------------__tds__GetScopes---------------------------\n");

    tds__GetScopesResponse->__sizeScopes = 7;
    tds__GetScopesResponse->Scopes = (struct tt__Scope *)soap_malloc(soap, sizeof(struct tt__Scope) * tds__GetScopesResponse->__sizeScopes);
    memset(tds__GetScopesResponse->Scopes, 0, sizeof(struct tt__Scope) * tds__GetScopesResponse->__sizeScopes);

    tds__GetScopesResponse->Scopes[0].ScopeDef = tt__ScopeDefinition__Fixed;
    tds__GetScopesResponse->Scopes[1].ScopeDef = tt__ScopeDefinition__Configurable;
    tds__GetScopesResponse->Scopes[2].ScopeDef = tt__ScopeDefinition__Configurable;
    tds__GetScopesResponse->Scopes[3].ScopeDef = tt__ScopeDefinition__Configurable;
    tds__GetScopesResponse->Scopes[4].ScopeDef = tt__ScopeDefinition__Fixed;
    tds__GetScopesResponse->Scopes[5].ScopeDef = tt__ScopeDefinition__Configurable;
    tds__GetScopesResponse->Scopes[6].ScopeDef = tt__ScopeDefinition__Configurable;

    tds__GetScopesResponse->Scopes[0].ScopeItem = (char *)soap_malloc(soap, sizeof(char) * 100);
    tds__GetScopesResponse->Scopes[1].ScopeItem = (char *)soap_malloc(soap, sizeof(char) * 100);
    tds__GetScopesResponse->Scopes[2].ScopeItem = (char *)soap_malloc(soap, sizeof(char) * 100);
    tds__GetScopesResponse->Scopes[3].ScopeItem = (char *)soap_malloc(soap, sizeof(char) * 100);
    tds__GetScopesResponse->Scopes[4].ScopeItem = (char *)soap_malloc(soap, sizeof(char) * 100);
    tds__GetScopesResponse->Scopes[5].ScopeItem = (char *)soap_malloc(soap, sizeof(char) * 100);
    tds__GetScopesResponse->Scopes[6].ScopeItem = (char *)soap_malloc(soap, sizeof(char) * 100);
    memset(tds__GetScopesResponse->Scopes[0].ScopeItem, '\0', sizeof(char) * 100);
    memset(tds__GetScopesResponse->Scopes[1].ScopeItem, '\0', sizeof(char) * 100);
    memset(tds__GetScopesResponse->Scopes[2].ScopeItem, '\0', sizeof(char) * 100);
    memset(tds__GetScopesResponse->Scopes[3].ScopeItem, '\0', sizeof(char) * 100);
    memset(tds__GetScopesResponse->Scopes[4].ScopeItem, '\0', sizeof(char) * 100);
    memset(tds__GetScopesResponse->Scopes[5].ScopeItem, '\0', sizeof(char) * 100);
    memset(tds__GetScopesResponse->Scopes[6].ScopeItem, '\0', sizeof(char) * 100);

    strcpy(tds__GetScopesResponse->Scopes[0].ScopeItem, "onvif://www.onvif.org/type/Network_Video_Transmitter");
    strcpy(tds__GetScopesResponse->Scopes[1].ScopeItem, "onvif://www.onvif.org/Profile/Streaming");
    strcpy(tds__GetScopesResponse->Scopes[2].ScopeItem, "onvif://www.onvif.org/Profile/Q/Operational");
    strcpy(tds__GetScopesResponse->Scopes[3].ScopeItem, "onvif://www.onvif.org/hardware/HD1080P");
    strcpy(tds__GetScopesResponse->Scopes[4].ScopeItem, "onvif://www.onvif.org/name/discover_test");
    strcpy(tds__GetScopesResponse->Scopes[5].ScopeItem, "onvif://www.onvif.org/location/city/GuangZhou");
    strcpy(tds__GetScopesResponse->Scopes[6].ScopeItem, "onvif://www.onvif.org/location/country/China");

    return 0;
}

/** Web service operation '__tds__SetScopes' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__SetScopes(struct soap* soap, struct _tds__SetScopes *tds__SetScopes, struct _tds__SetScopesResponse *tds__SetScopesResponse)
{
    return 0;
}

/** Web service operation '__tds__AddScopes' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__AddScopes(struct soap* soap, struct _tds__AddScopes *tds__AddScopes, struct _tds__AddScopesResponse *tds__AddScopesResponse)
{
    return 0;
}

/** Web service operation '__tds__RemoveScopes' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__RemoveScopes(struct soap* soap, struct _tds__RemoveScopes *tds__RemoveScopes, struct _tds__RemoveScopesResponse *tds__RemoveScopesResponse)
{
    return 0;
}

/** Web service operation '__tds__GetDiscoveryMode' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__GetDiscoveryMode(struct soap* soap, struct _tds__GetDiscoveryMode *tds__GetDiscoveryMode, struct _tds__GetDiscoveryModeResponse *tds__GetDiscoveryModeResponse)
{
    return 0;
}

/** Web service operation '__tds__SetDiscoveryMode' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__SetDiscoveryMode(struct soap* soap, struct _tds__SetDiscoveryMode *tds__SetDiscoveryMode, struct _tds__SetDiscoveryModeResponse *tds__SetDiscoveryModeResponse)
{
    return 0;
}

/** Web service operation '__tds__GetRemoteDiscoveryMode' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__GetRemoteDiscoveryMode(struct soap* soap, struct _tds__GetRemoteDiscoveryMode *tds__GetRemoteDiscoveryMode, struct _tds__GetRemoteDiscoveryModeResponse *tds__GetRemoteDiscoveryModeResponse)
{
    return 0;
}

/** Web service operation '__tds__SetRemoteDiscoveryMode' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__SetRemoteDiscoveryMode(struct soap* soap, struct _tds__SetRemoteDiscoveryMode *tds__SetRemoteDiscoveryMode, struct _tds__SetRemoteDiscoveryModeResponse *tds__SetRemoteDiscoveryModeResponse)
{
    return 0;
}

/** Web service operation '__tds__GetDPAddresses' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__GetDPAddresses(struct soap* soap, struct _tds__GetDPAddresses *tds__GetDPAddresses, struct _tds__GetDPAddressesResponse *tds__GetDPAddressesResponse)
{
    return 0;
}

/** Web service operation '__tds__GetEndpointReference' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__GetEndpointReference(struct soap* soap, struct _tds__GetEndpointReference *tds__GetEndpointReference, struct _tds__GetEndpointReferenceResponse *tds__GetEndpointReferenceResponse)
{
    return 0;
}

/** Web service operation '__tds__GetRemoteUser' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__GetRemoteUser(struct soap* soap, struct _tds__GetRemoteUser *tds__GetRemoteUser, struct _tds__GetRemoteUserResponse *tds__GetRemoteUserResponse)
{
    return 0;
}

/** Web service operation '__tds__SetRemoteUser' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__SetRemoteUser(struct soap* soap, struct _tds__SetRemoteUser *tds__SetRemoteUser, struct _tds__SetRemoteUserResponse *tds__SetRemoteUserResponse)
{
    return 0;
}

/** Web service operation '__tds__GetUsers' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__GetUsers(struct soap* soap, struct _tds__GetUsers *tds__GetUsers, struct _tds__GetUsersResponse *tds__GetUsersResponse)
{
    printf("--------------------__tds__GetUsers--------------------");
    tds__GetUsersResponse->__sizeUser = 1;
    tds__GetUsersResponse->User = (struct tt__User *)soap_malloc(soap, sizeof(struct tt__User));
    memset(tds__GetUsersResponse->User, 0, sizeof(struct tt__User));
    tds__GetUsersResponse->User->Username = (char *)soap_malloc(soap, sizeof(char) * 32);
    memset(tds__GetUsersResponse->User->Username, 0, sizeof(char) * 32);
    tds__GetUsersResponse->User->Password = (char *)soap_malloc(soap, sizeof(char) * 32);
    memset(tds__GetUsersResponse->User->Password, 0, sizeof(char) * 32);
    tds__GetUsersResponse->User->UserLevel = tt__UserLevel__User;

    strcpy(tds__GetUsersResponse->User->Username, "admin");
    strcpy(tds__GetUsersResponse->User->Password, "123456");

    return SOAP_OK;
}

/** Web service operation '__tds__CreateUsers' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__CreateUsers(struct soap* soap, struct _tds__CreateUsers *tds__CreateUsers, struct _tds__CreateUsersResponse *tds__CreateUsersResponse)
{
    return 0;
}

/** Web service operation '__tds__DeleteUsers' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__DeleteUsers(struct soap* soap, struct _tds__DeleteUsers *tds__DeleteUsers, struct _tds__DeleteUsersResponse *tds__DeleteUsersResponse)
{
    return 0;
}

/** Web service operation '__tds__SetUser' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__SetUser(struct soap* soap, struct _tds__SetUser *tds__SetUser, struct _tds__SetUserResponse *tds__SetUserResponse)
{
    return 0;
}

/** Web service operation '__tds__GetWsdlUrl' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__GetWsdlUrl(struct soap* soap, struct _tds__GetWsdlUrl *tds__GetWsdlUrl, struct _tds__GetWsdlUrlResponse *tds__GetWsdlUrlResponse)
{
    return 0;
}

/** Web service operation '__tds__GetCapabilities' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__GetCapabilities(struct soap* soap, struct _tds__GetCapabilities *tds__GetCapabilities, struct _tds__GetCapabilitiesResponse *tds__GetCapabilitiesResponse)
{
    printf("---------------------------__tds__GetCapabilities---------------------------------------\n");
    //get ip
    char IP[20] = {0};
    getLocalIP(IP, 20);

    if (tds__GetCapabilities->Category[0] == tt__CapabilityCategory__Device ||
        tds__GetCapabilities->Category[0] == tt__CapabilityCategory__All) {
        //<Capabilities>
        tds__GetCapabilitiesResponse->Capabilities = (struct tt__Capabilities *)soap_malloc(soap, sizeof(struct tt__Capabilities));
        memset(tds__GetCapabilitiesResponse->Capabilities, 0, sizeof(struct tt__Capabilities));

        //<Device>
        tds__GetCapabilitiesResponse->Capabilities->Device = (struct tt__DeviceCapabilities *)soap_malloc(soap, sizeof(struct tt__DeviceCapabilities));
        memset(tds__GetCapabilitiesResponse->Capabilities->Device, 0, sizeof(struct tt__DeviceCapabilities));
        tds__GetCapabilitiesResponse->Capabilities->Device->XAddr = (char *)soap_malloc(soap, sizeof(char) * 100 );
        memset(tds__GetCapabilitiesResponse->Capabilities->Device->XAddr, 0, sizeof(char) * 100);
        sprintf(tds__GetCapabilitiesResponse->Capabilities->Device->XAddr, "http://%s:%d/onvif/device_service", IP, ONVIF_TCP_PORT);
        //<Device><Network>
        tds__GetCapabilitiesResponse->Capabilities->Device->Network = (struct tt__NetworkCapabilities *)soap_malloc(soap, sizeof(struct tt__NetworkCapabilities ));
        memset(tds__GetCapabilitiesResponse->Capabilities->Device->Network, 0, sizeof(struct tt__NetworkCapabilities ));
        tds__GetCapabilitiesResponse->Capabilities->Device->Network->IPFilter = (enum xsd__boolean *)soap_malloc(soap, sizeof(enum xsd__boolean));
        *(tds__GetCapabilitiesResponse->Capabilities->Device->Network->IPFilter) = xsd__boolean__false_;                //关闭功能 xsd__boolean__true_
        tds__GetCapabilitiesResponse->Capabilities->Device->Network->ZeroConfiguration= (enum xsd__boolean *)soap_malloc(soap, sizeof(enum xsd__boolean));
        *(tds__GetCapabilitiesResponse->Capabilities->Device->Network->ZeroConfiguration) = xsd__boolean__false_;        //打开功能 xsd__boolean__false_
        tds__GetCapabilitiesResponse->Capabilities->Device->Network->IPVersion6 = (enum xsd__boolean *)soap_malloc(soap, sizeof(enum xsd__boolean));
        *(tds__GetCapabilitiesResponse->Capabilities->Device->Network->IPVersion6) = xsd__boolean__false_;              //关闭功能 xsd__boolean__true_
        tds__GetCapabilitiesResponse->Capabilities->Device->Network->DynDNS = (enum xsd__boolean *)soap_malloc(soap, sizeof(enum xsd__boolean));
        *(tds__GetCapabilitiesResponse->Capabilities->Device->Network->DynDNS) = xsd__boolean__false_;                   //打开功能 xsd__boolean__false_
        //<Device><Network><Extension>
        tds__GetCapabilitiesResponse->Capabilities->Device->Network->Extension = (struct tt__NetworkCapabilitiesExtension *)soap_malloc(soap, sizeof(struct tt__NetworkCapabilitiesExtension));
        memset(tds__GetCapabilitiesResponse->Capabilities->Device->Network->Extension, 0, sizeof(struct tt__NetworkCapabilitiesExtension ));
        tds__GetCapabilitiesResponse->Capabilities->Device->Network->Extension->__size = 1;
        tds__GetCapabilitiesResponse->Capabilities->Device->Network->Extension->Dot11Configuration = (enum xsd__boolean *)soap_malloc(soap, sizeof(enum xsd__boolean));
        *(tds__GetCapabilitiesResponse->Capabilities->Device->Network->Extension->Dot11Configuration) = xsd__boolean__false_;

        //<Device><System>
        tds__GetCapabilitiesResponse->Capabilities->Device->System = (struct tt__SystemCapabilities *)soap_malloc(soap, sizeof(struct tt__SystemCapabilities));
        memset( tds__GetCapabilitiesResponse->Capabilities->Device->System, 0, sizeof(struct tt__SystemCapabilities));
        tds__GetCapabilitiesResponse->Capabilities->Device->System->DiscoveryResolve = xsd__boolean__true_;
        tds__GetCapabilitiesResponse->Capabilities->Device->System->DiscoveryBye     = xsd__boolean__true_;
        tds__GetCapabilitiesResponse->Capabilities->Device->System->RemoteDiscovery  = xsd__boolean__true_;
        tds__GetCapabilitiesResponse->Capabilities->Device->System->SystemBackup     = xsd__boolean__true_;
        tds__GetCapabilitiesResponse->Capabilities->Device->System->SystemLogging    = xsd__boolean__false_;
        tds__GetCapabilitiesResponse->Capabilities->Device->System->FirmwareUpgrade  = xsd__boolean__true_;
        tds__GetCapabilitiesResponse->Capabilities->Device->System->__sizeSupportedVersions = 1;
        tds__GetCapabilitiesResponse->Capabilities->Device->System->SupportedVersions = (struct tt__OnvifVersion *)soap_malloc(soap, sizeof(struct tt__OnvifVersion));
        tds__GetCapabilitiesResponse->Capabilities->Device->System->SupportedVersions->Major = 1;
        tds__GetCapabilitiesResponse->Capabilities->Device->System->SupportedVersions->Minor = 10;
        tds__GetCapabilitiesResponse->Capabilities->Device->System->Extension = (struct tt__SystemCapabilitiesExtension *)soap_malloc(soap, sizeof(struct tt__SystemCapabilitiesExtension));
        memset( tds__GetCapabilitiesResponse->Capabilities->Device->System->Extension, 0, sizeof(struct tt__SystemCapabilitiesExtension));
        tds__GetCapabilitiesResponse->Capabilities->Device->System->Extension->HttpFirmwareUpgrade = (enum xsd__boolean *)soap_malloc(soap, sizeof(enum xsd__boolean));
        *(tds__GetCapabilitiesResponse->Capabilities->Device->System->Extension->HttpFirmwareUpgrade) = xsd__boolean__true_;
        tds__GetCapabilitiesResponse->Capabilities->Device->System->Extension->HttpSystemBackup = (enum xsd__boolean *)soap_malloc(soap, sizeof(enum xsd__boolean));
        *(tds__GetCapabilitiesResponse->Capabilities->Device->System->Extension->HttpSystemBackup) = xsd__boolean__true_;
        tds__GetCapabilitiesResponse->Capabilities->Device->System->Extension->HttpSystemLogging = (enum xsd__boolean *)soap_malloc(soap, sizeof(enum xsd__boolean));
        *(tds__GetCapabilitiesResponse->Capabilities->Device->System->Extension->HttpSystemLogging) = xsd__boolean__false_;
        tds__GetCapabilitiesResponse->Capabilities->Device->System->Extension->HttpSupportInformation = (enum xsd__boolean *)soap_malloc(soap, sizeof(enum xsd__boolean));
        *(tds__GetCapabilitiesResponse->Capabilities->Device->System->Extension->HttpSupportInformation) = xsd__boolean__true_;

        //<Device><Security>
        tds__GetCapabilitiesResponse->Capabilities->Device->Security = (struct tt__SecurityCapabilities *)soap_malloc(soap, sizeof(struct tt__SecurityCapabilities));
        memset(tds__GetCapabilitiesResponse->Capabilities->Device->Security, 0, sizeof(struct tt__SecurityCapabilities));
        tds__GetCapabilitiesResponse->Capabilities->Device->Security->TLS1_x002e1          = xsd__boolean__false_;
        tds__GetCapabilitiesResponse->Capabilities->Device->Security->TLS1_x002e2          = xsd__boolean__false_;
        tds__GetCapabilitiesResponse->Capabilities->Device->Security->OnboardKeyGeneration = xsd__boolean__false_;
        tds__GetCapabilitiesResponse->Capabilities->Device->Security->AccessPolicyConfig   = xsd__boolean__false_;
        tds__GetCapabilitiesResponse->Capabilities->Device->Security->X_x002e509Token      = xsd__boolean__false_;
        tds__GetCapabilitiesResponse->Capabilities->Device->Security->SAMLToken            = xsd__boolean__false_;
        tds__GetCapabilitiesResponse->Capabilities->Device->Security->KerberosToken        = xsd__boolean__false_;
        tds__GetCapabilitiesResponse->Capabilities->Device->Security->RELToken             = xsd__boolean__false_;
        tds__GetCapabilitiesResponse->Capabilities->Device->Security->Extension = (struct tt__SecurityCapabilitiesExtension *)soap_malloc(soap, sizeof(struct tt__SecurityCapabilitiesExtension));
        memset(tds__GetCapabilitiesResponse->Capabilities->Device->Security->Extension, 0, sizeof(struct tt__SecurityCapabilitiesExtension));
        tds__GetCapabilitiesResponse->Capabilities->Device->Security->Extension->Extension =
                (struct tt__SecurityCapabilitiesExtension2 *)soap_malloc(soap, sizeof(struct tt__SecurityCapabilitiesExtension2));
        memset(tds__GetCapabilitiesResponse->Capabilities->Device->Security->Extension->Extension, 0, sizeof(struct tt__SecurityCapabilitiesExtension2));
        tds__GetCapabilitiesResponse->Capabilities->Device->Security->Extension->Extension->Dot1X = xsd__boolean__false_;
        tds__GetCapabilitiesResponse->Capabilities->Device->Security->Extension->Extension->RemoteUserHandling = xsd__boolean__false_;
    }
    
    //Media
    if (tds__GetCapabilities->Category[0] == tt__CapabilityCategory__Media ||
        tds__GetCapabilities->Category[0] == tt__CapabilityCategory__All) {
        tds__GetCapabilitiesResponse->Capabilities->Media = (struct tt__MediaCapabilities *)soap_malloc(soap, sizeof(struct tt__MediaCapabilities));
        memset(tds__GetCapabilitiesResponse->Capabilities->Media, 0, sizeof(struct tt__MediaCapabilities));
        tds__GetCapabilitiesResponse->Capabilities->Media->XAddr = (char *)soap_malloc(soap, sizeof(char) * 100 );
        memset(tds__GetCapabilitiesResponse->Capabilities->Media->XAddr, '\0', sizeof(char) * 100);
        sprintf(tds__GetCapabilitiesResponse->Capabilities->Media->XAddr, "http://%s:%d/onvif/media_service", IP, ONVIF_TCP_PORT);
        //<Media><StreamingCapabilities>
        tds__GetCapabilitiesResponse->Capabilities->Media->StreamingCapabilities = (struct tt__RealTimeStreamingCapabilities *)soap_malloc(soap, sizeof(struct tt__RealTimeStreamingCapabilities));
        memset(tds__GetCapabilitiesResponse->Capabilities->Media->StreamingCapabilities, 0, sizeof(struct tt__RealTimeStreamingCapabilities));
        tds__GetCapabilitiesResponse->Capabilities->Media->StreamingCapabilities->RTPMulticast = (enum xsd__boolean *)soap_malloc(soap, sizeof(enum xsd__boolean));
        *(tds__GetCapabilitiesResponse->Capabilities->Media->StreamingCapabilities->RTPMulticast) = xsd__boolean__false_;
        tds__GetCapabilitiesResponse->Capabilities->Media->StreamingCapabilities->RTP_USCORERTSP_USCORETCP = (enum xsd__boolean *)soap_malloc(soap, sizeof(enum xsd__boolean));
        *(tds__GetCapabilitiesResponse->Capabilities->Media->StreamingCapabilities->RTP_USCORERTSP_USCORETCP) = xsd__boolean__true_;
        tds__GetCapabilitiesResponse->Capabilities->Media->StreamingCapabilities->RTP_USCORETCP = (enum xsd__boolean *)soap_malloc(soap, sizeof(enum xsd__boolean));
        *(tds__GetCapabilitiesResponse->Capabilities->Media->StreamingCapabilities->RTP_USCORETCP) = xsd__boolean__true_;
    }

    return 0;
}

/** Web service operation '__tds__SetDPAddresses' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__SetDPAddresses(struct soap* soap, struct _tds__SetDPAddresses *tds__SetDPAddresses, struct _tds__SetDPAddressesResponse *tds__SetDPAddressesResponse)
{
    return 0;
}

/** Web service operation '__tds__GetHostname' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__GetHostname(struct soap* soap, struct _tds__GetHostname *tds__GetHostname, struct _tds__GetHostnameResponse *tds__GetHostnameResponse)
{
    return 0;
}

/** Web service operation '__tds__SetHostname' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__SetHostname(struct soap* soap, struct _tds__SetHostname *tds__SetHostname, struct _tds__SetHostnameResponse *tds__SetHostnameResponse)
{
    return 0;
}

/** Web service operation '__tds__SetHostnameFromDHCP' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__SetHostnameFromDHCP(struct soap* soap, struct _tds__SetHostnameFromDHCP *tds__SetHostnameFromDHCP, struct _tds__SetHostnameFromDHCPResponse *tds__SetHostnameFromDHCPResponse)
{
    return 0;
}

/** Web service operation '__tds__GetDNS' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__GetDNS(struct soap* soap, struct _tds__GetDNS *tds__GetDNS, struct _tds__GetDNSResponse *tds__GetDNSResponse)
{
    return 0;
}

/** Web service operation '__tds__SetDNS' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__SetDNS(struct soap* soap, struct _tds__SetDNS *tds__SetDNS, struct _tds__SetDNSResponse *tds__SetDNSResponse)
{
    return 0;
}

/** Web service operation '__tds__GetNTP' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__GetNTP(struct soap* soap, struct _tds__GetNTP *tds__GetNTP, struct _tds__GetNTPResponse *tds__GetNTPResponse)
{
    return 0;
}

/** Web service operation '__tds__SetNTP' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__SetNTP(struct soap* soap, struct _tds__SetNTP *tds__SetNTP, struct _tds__SetNTPResponse *tds__SetNTPResponse)
{
    return 0;
}

/** Web service operation '__tds__GetDynamicDNS' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__GetDynamicDNS(struct soap* soap, struct _tds__GetDynamicDNS *tds__GetDynamicDNS, struct _tds__GetDynamicDNSResponse *tds__GetDynamicDNSResponse)
{
    return 0;
}

/** Web service operation '__tds__SetDynamicDNS' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__SetDynamicDNS(struct soap* soap, struct _tds__SetDynamicDNS *tds__SetDynamicDNS, struct _tds__SetDynamicDNSResponse *tds__SetDynamicDNSResponse)
{
    return 0;
}

/** Web service operation '__tds__GetNetworkInterfaces' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__GetNetworkInterfaces(struct soap* soap, struct _tds__GetNetworkInterfaces *tds__GetNetworkInterfaces, struct _tds__GetNetworkInterfacesResponse *tds__GetNetworkInterfacesResponse)
{
    printf("------------------------------__tds__GetNetworkInterfaces-------------------------------------\n");
    char IP[20] = {0};
    getLocalIP(IP, 20);

    tds__GetNetworkInterfacesResponse->__sizeNetworkInterfaces = 1;
    tds__GetNetworkInterfacesResponse->NetworkInterfaces = (struct tt__NetworkInterface *)soap_malloc(soap, sizeof(struct tt__NetworkInterface));
    memset(tds__GetNetworkInterfacesResponse->NetworkInterfaces, 0, sizeof(struct tt__NetworkInterface));

    //<NetworkInterfaces>
    tds__GetNetworkInterfacesResponse->NetworkInterfaces->token = (char *)soap_malloc(soap, sizeof(char) * 32);
    memset(tds__GetNetworkInterfacesResponse->NetworkInterfaces->token, 0, sizeof(char) * 32);
    strcpy(tds__GetNetworkInterfacesResponse->NetworkInterfaces->token, "eth0");
    tds__GetNetworkInterfacesResponse->NetworkInterfaces->Enabled = xsd__boolean__true_;
    //<NetworkInterfaces><Info>
    tds__GetNetworkInterfacesResponse->NetworkInterfaces->Info = (struct tt__NetworkInterfaceInfo *)soap_malloc(soap, sizeof(struct tt__NetworkInterfaceInfo));
    memset(tds__GetNetworkInterfacesResponse->NetworkInterfaces->Info, 0, sizeof(struct tt__NetworkInterfaceInfo));
    tds__GetNetworkInterfacesResponse->NetworkInterfaces->Info->Name = (char *)soap_malloc(soap, sizeof(char) * 32);
    memset(tds__GetNetworkInterfacesResponse->NetworkInterfaces->Info->Name, 0, sizeof(char) * 32);
    strcpy(tds__GetNetworkInterfacesResponse->NetworkInterfaces->Info->Name, "eth0");
    tds__GetNetworkInterfacesResponse->NetworkInterfaces->Info->HwAddress = (char *)soap_malloc(soap, sizeof(char) * 32);
    memset(tds__GetNetworkInterfacesResponse->NetworkInterfaces->Info->HwAddress, 0, sizeof(char) * 32);
    strcpy(tds__GetNetworkInterfacesResponse->NetworkInterfaces->Info->HwAddress, "4E:61:21:AF:73:BD");
    tds__GetNetworkInterfacesResponse->NetworkInterfaces->Info->MTU = (int *)soap_malloc(soap, sizeof(int));
    *(tds__GetNetworkInterfacesResponse->NetworkInterfaces->Info->MTU) = 1500;

    //<NetworkInterfaces><Link>
    tds__GetNetworkInterfacesResponse->NetworkInterfaces->Link = (struct tt__NetworkInterfaceLink *)soap_malloc(soap, sizeof(struct tt__NetworkInterfaceLink));
    memset(tds__GetNetworkInterfacesResponse->NetworkInterfaces->Link, 0, sizeof(struct tt__NetworkInterfaceLink));
    //<NetworkInterfaces><Link><AdminSettings>
    tds__GetNetworkInterfacesResponse->NetworkInterfaces->Link->AdminSettings = (struct tt__NetworkInterfaceConnectionSetting *)soap_malloc(soap, sizeof(struct tt__NetworkInterfaceConnectionSetting));
    memset(tds__GetNetworkInterfacesResponse->NetworkInterfaces->Link->AdminSettings, 0, sizeof(struct tt__NetworkInterfaceConnectionSetting));
    tds__GetNetworkInterfacesResponse->NetworkInterfaces->Link->AdminSettings->AutoNegotiation = xsd__boolean__false_;
    tds__GetNetworkInterfacesResponse->NetworkInterfaces->Link->AdminSettings->Speed = 100;
    tds__GetNetworkInterfacesResponse->NetworkInterfaces->Link->AdminSettings->Duplex = tt__Duplex__Full;

    //<NetworkInterfaces><Link><OperSettings>
    tds__GetNetworkInterfacesResponse->NetworkInterfaces->Link->OperSettings = (struct tt__NetworkInterfaceConnectionSetting *)soap_malloc(soap, sizeof(struct tt__NetworkInterfaceConnectionSetting));
    memset(tds__GetNetworkInterfacesResponse->NetworkInterfaces->Link->OperSettings, 0, sizeof(struct tt__NetworkInterfaceConnectionSetting));
    tds__GetNetworkInterfacesResponse->NetworkInterfaces->Link->OperSettings->AutoNegotiation = xsd__boolean__false_;
    tds__GetNetworkInterfacesResponse->NetworkInterfaces->Link->OperSettings->Speed = 100;
    tds__GetNetworkInterfacesResponse->NetworkInterfaces->Link->OperSettings->Duplex = tt__Duplex__Full;
    tds__GetNetworkInterfacesResponse->NetworkInterfaces->Link->InterfaceType = 6;

    //<NetworkInterfaces><IPv4>
    tds__GetNetworkInterfacesResponse->NetworkInterfaces->IPv4 = (struct tt__IPv4NetworkInterface *)soap_malloc(soap, sizeof(struct tt__IPv4NetworkInterface));
    memset(tds__GetNetworkInterfacesResponse->NetworkInterfaces->IPv4, 0, sizeof(struct tt__IPv4NetworkInterface));
    tds__GetNetworkInterfacesResponse->NetworkInterfaces->IPv4->Enabled = xsd__boolean__true_;
    tds__GetNetworkInterfacesResponse->NetworkInterfaces->IPv4->Config = (struct tt__IPv4Configuration *)soap_malloc(soap, sizeof(struct tt__IPv4Configuration));
    memset(tds__GetNetworkInterfacesResponse->NetworkInterfaces->IPv4->Config, 0, sizeof(struct tt__IPv4Configuration));
    tds__GetNetworkInterfacesResponse->NetworkInterfaces->IPv4->Config->__sizeManual = 1;
    tds__GetNetworkInterfacesResponse->NetworkInterfaces->IPv4->Config->Manual =
            (struct tt__PrefixedIPv4Address *)soap_malloc(soap, sizeof(struct tt__PrefixedIPv4Address) * tds__GetNetworkInterfacesResponse->NetworkInterfaces->IPv4->Config->__sizeManual);
    memset(tds__GetNetworkInterfacesResponse->NetworkInterfaces->IPv4->Config->Manual, 0,
           sizeof(struct tt__PrefixedIPv4Address) * tds__GetNetworkInterfacesResponse->NetworkInterfaces->IPv4->Config->__sizeManual);
    tds__GetNetworkInterfacesResponse->NetworkInterfaces->IPv4->Config->Manual->Address = (char *)soap_malloc(soap, sizeof(char) * 32);
    memset(tds__GetNetworkInterfacesResponse->NetworkInterfaces->IPv4->Config->Manual->Address, '\0', sizeof(char) * 32);
    strcpy(tds__GetNetworkInterfacesResponse->NetworkInterfaces->IPv4->Config->Manual->Address, IP);
    tds__GetNetworkInterfacesResponse->NetworkInterfaces->IPv4->Config->Manual->PrefixLength = 24;
    tds__GetNetworkInterfacesResponse->NetworkInterfaces->IPv4->Config->DHCP = xsd__boolean__false_;

    return SOAP_OK;
}

/** Web service operation '__tds__SetNetworkInterfaces' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__SetNetworkInterfaces(struct soap* soap, struct _tds__SetNetworkInterfaces *tds__SetNetworkInterfaces, struct _tds__SetNetworkInterfacesResponse *tds__SetNetworkInterfacesResponse)
{
    return 0;
}

/** Web service operation '__tds__GetNetworkProtocols' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__GetNetworkProtocols(struct soap* soap, struct _tds__GetNetworkProtocols *tds__GetNetworkProtocols, struct _tds__GetNetworkProtocolsResponse *tds__GetNetworkProtocolsResponse)
{
    return 0;
}

/** Web service operation '__tds__SetNetworkProtocols' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__SetNetworkProtocols(struct soap* soap, struct _tds__SetNetworkProtocols *tds__SetNetworkProtocols, struct _tds__SetNetworkProtocolsResponse *tds__SetNetworkProtocolsResponse)
{
    return 0;
}

/** Web service operation '__tds__GetNetworkDefaultGateway' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__GetNetworkDefaultGateway(struct soap* soap, struct _tds__GetNetworkDefaultGateway *tds__GetNetworkDefaultGateway, struct _tds__GetNetworkDefaultGatewayResponse *tds__GetNetworkDefaultGatewayResponse)
{
    return 0;
}

/** Web service operation '__tds__SetNetworkDefaultGateway' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__SetNetworkDefaultGateway(struct soap* soap, struct _tds__SetNetworkDefaultGateway *tds__SetNetworkDefaultGateway, struct _tds__SetNetworkDefaultGatewayResponse *tds__SetNetworkDefaultGatewayResponse)
{
    return 0;
}

/** Web service operation '__tds__GetZeroConfiguration' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__GetZeroConfiguration(struct soap* soap, struct _tds__GetZeroConfiguration *tds__GetZeroConfiguration, struct _tds__GetZeroConfigurationResponse *tds__GetZeroConfigurationResponse)
{
    return 0;
}

/** Web service operation '__tds__SetZeroConfiguration' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__SetZeroConfiguration(struct soap* soap, struct _tds__SetZeroConfiguration *tds__SetZeroConfiguration, struct _tds__SetZeroConfigurationResponse *tds__SetZeroConfigurationResponse)
{
    return 0;
}

/** Web service operation '__tds__GetIPAddressFilter' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__GetIPAddressFilter(struct soap* soap, struct _tds__GetIPAddressFilter *tds__GetIPAddressFilter, struct _tds__GetIPAddressFilterResponse *tds__GetIPAddressFilterResponse)
{
    return 0;
}

/** Web service operation '__tds__SetIPAddressFilter' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__SetIPAddressFilter(struct soap* soap, struct _tds__SetIPAddressFilter *tds__SetIPAddressFilter, struct _tds__SetIPAddressFilterResponse *tds__SetIPAddressFilterResponse)
{
    return 0;
}

/** Web service operation '__tds__AddIPAddressFilter' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__AddIPAddressFilter(struct soap* soap, struct _tds__AddIPAddressFilter *tds__AddIPAddressFilter, struct _tds__AddIPAddressFilterResponse *tds__AddIPAddressFilterResponse)
{
    return 0;
}

/** Web service operation '__tds__RemoveIPAddressFilter' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__RemoveIPAddressFilter(struct soap* soap, struct _tds__RemoveIPAddressFilter *tds__RemoveIPAddressFilter, struct _tds__RemoveIPAddressFilterResponse *tds__RemoveIPAddressFilterResponse)
{
    return 0;
}

/** Web service operation '__tds__GetAccessPolicy' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__GetAccessPolicy(struct soap* soap, struct _tds__GetAccessPolicy *tds__GetAccessPolicy, struct _tds__GetAccessPolicyResponse *tds__GetAccessPolicyResponse)
{
    return 0;
}


/** Web service operation '__tds__SetAccessPolicy' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__SetAccessPolicy(struct soap* soap, struct _tds__SetAccessPolicy *tds__SetAccessPolicy, struct _tds__SetAccessPolicyResponse *tds__SetAccessPolicyResponse)
{
    return 0;
}


/** Web service operation '__tds__CreateCertificate' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__CreateCertificate(struct soap* soap, struct _tds__CreateCertificate *tds__CreateCertificate, struct _tds__CreateCertificateResponse *tds__CreateCertificateResponse)
{
    return 0;
}


/** Web service operation '__tds__GetCertificates' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__GetCertificates(struct soap* soap, struct _tds__GetCertificates *tds__GetCertificates, struct _tds__GetCertificatesResponse *tds__GetCertificatesResponse)
{
    return 0;
}


/** Web service operation '__tds__GetCertificatesStatus' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__GetCertificatesStatus(struct soap* soap, struct _tds__GetCertificatesStatus *tds__GetCertificatesStatus, struct _tds__GetCertificatesStatusResponse *tds__GetCertificatesStatusResponse)
{
    return 0;
}


/** Web service operation '__tds__SetCertificatesStatus' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__SetCertificatesStatus(struct soap* soap, struct _tds__SetCertificatesStatus *tds__SetCertificatesStatus, struct _tds__SetCertificatesStatusResponse *tds__SetCertificatesStatusResponse)
{
    return 0;
}


/** Web service operation '__tds__DeleteCertificates' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__DeleteCertificates(struct soap* soap, struct _tds__DeleteCertificates *tds__DeleteCertificates, struct _tds__DeleteCertificatesResponse *tds__DeleteCertificatesResponse)
{
    return 0;
}


/** Web service operation '__tds__GetPkcs10Request' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__GetPkcs10Request(struct soap* soap, struct _tds__GetPkcs10Request *tds__GetPkcs10Request, struct _tds__GetPkcs10RequestResponse *tds__GetPkcs10RequestResponse)
{
    return 0;
}


/** Web service operation '__tds__LoadCertificates' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__LoadCertificates(struct soap* soap, struct _tds__LoadCertificates *tds__LoadCertificates, struct _tds__LoadCertificatesResponse *tds__LoadCertificatesResponse)
{
    return 0;
}


/** Web service operation '__tds__GetClientCertificateMode' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__GetClientCertificateMode(struct soap* soap, struct _tds__GetClientCertificateMode *tds__GetClientCertificateMode, struct _tds__GetClientCertificateModeResponse *tds__GetClientCertificateModeResponse)
{
    return 0;
}

/** Web service operation '__tds__SetClientCertificateMode' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__SetClientCertificateMode(struct soap* soap, struct _tds__SetClientCertificateMode *tds__SetClientCertificateMode, struct _tds__SetClientCertificateModeResponse *tds__SetClientCertificateModeResponse)
{
    return 0;
}

/** Web service operation '__tds__GetRelayOutputs' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__GetRelayOutputs(struct soap* soap, struct _tds__GetRelayOutputs *tds__GetRelayOutputs, struct _tds__GetRelayOutputsResponse *tds__GetRelayOutputsResponse)
{
    return 0;
}

/** Web service operation '__tds__SetRelayOutputSettings' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__SetRelayOutputSettings(struct soap* soap, struct _tds__SetRelayOutputSettings *tds__SetRelayOutputSettings, struct _tds__SetRelayOutputSettingsResponse *tds__SetRelayOutputSettingsResponse)
{
    return 0;
}

/** Web service operation '__tds__SetRelayOutputState' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__SetRelayOutputState(struct soap* soap, struct _tds__SetRelayOutputState *tds__SetRelayOutputState, struct _tds__SetRelayOutputStateResponse *tds__SetRelayOutputStateResponse)
{
    return 0;
}

/** Web service operation '__tds__SendAuxiliaryCommand' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__SendAuxiliaryCommand(struct soap* soap, struct _tds__SendAuxiliaryCommand *tds__SendAuxiliaryCommand, struct _tds__SendAuxiliaryCommandResponse *tds__SendAuxiliaryCommandResponse)
{
    return 0;
}

/** Web service operation '__tds__GetCACertificates' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__GetCACertificates(struct soap* soap, struct _tds__GetCACertificates *tds__GetCACertificates, struct _tds__GetCACertificatesResponse *tds__GetCACertificatesResponse)
{
    return 0;
}

/** Web service operation '__tds__LoadCertificateWithPrivateKey' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__LoadCertificateWithPrivateKey(struct soap* soap, struct _tds__LoadCertificateWithPrivateKey *tds__LoadCertificateWithPrivateKey, struct _tds__LoadCertificateWithPrivateKeyResponse *tds__LoadCertificateWithPrivateKeyResponse)
{
    return 0;
}

/** Web service operation '__tds__GetCertificateInformation' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__GetCertificateInformation(struct soap* soap, struct _tds__GetCertificateInformation *tds__GetCertificateInformation, struct _tds__GetCertificateInformationResponse *tds__GetCertificateInformationResponse)
{
    return 0;
}

/** Web service operation '__tds__LoadCACertificates' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__LoadCACertificates(struct soap* soap, struct _tds__LoadCACertificates *tds__LoadCACertificates, struct _tds__LoadCACertificatesResponse *tds__LoadCACertificatesResponse)
{
    return 0;
}

/** Web service operation '__tds__CreateDot1XConfiguration' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__CreateDot1XConfiguration(struct soap* soap, struct _tds__CreateDot1XConfiguration *tds__CreateDot1XConfiguration, struct _tds__CreateDot1XConfigurationResponse *tds__CreateDot1XConfigurationResponse)
{
    return 0;
}

/** Web service operation '__tds__SetDot1XConfiguration' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__SetDot1XConfiguration(struct soap* soap, struct _tds__SetDot1XConfiguration *tds__SetDot1XConfiguration, struct _tds__SetDot1XConfigurationResponse *tds__SetDot1XConfigurationResponse)
{
    return 0;
}

/** Web service operation '__tds__GetDot1XConfiguration' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__GetDot1XConfiguration(struct soap* soap, struct _tds__GetDot1XConfiguration *tds__GetDot1XConfiguration, struct _tds__GetDot1XConfigurationResponse *tds__GetDot1XConfigurationResponse)
{
    return 0;
}

/** Web service operation '__tds__GetDot1XConfigurations' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__GetDot1XConfigurations(struct soap* soap, struct _tds__GetDot1XConfigurations *tds__GetDot1XConfigurations, struct _tds__GetDot1XConfigurationsResponse *tds__GetDot1XConfigurationsResponse)
{
    return 0;
}

/** Web service operation '__tds__DeleteDot1XConfiguration' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__DeleteDot1XConfiguration(struct soap* soap, struct _tds__DeleteDot1XConfiguration *tds__DeleteDot1XConfiguration, struct _tds__DeleteDot1XConfigurationResponse *tds__DeleteDot1XConfigurationResponse)
{
    return 0;
}

/** Web service operation '__tds__GetDot11Capabilities' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__GetDot11Capabilities(struct soap* soap, struct _tds__GetDot11Capabilities *tds__GetDot11Capabilities, struct _tds__GetDot11CapabilitiesResponse *tds__GetDot11CapabilitiesResponse)
{
    return 0;
}

/** Web service operation '__tds__GetDot11Status' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__GetDot11Status(struct soap* soap, struct _tds__GetDot11Status *tds__GetDot11Status, struct _tds__GetDot11StatusResponse *tds__GetDot11StatusResponse)
{
    return 0;
}

/** Web service operation '__tds__ScanAvailableDot11Networks' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__ScanAvailableDot11Networks(struct soap* soap, struct _tds__ScanAvailableDot11Networks *tds__ScanAvailableDot11Networks, struct _tds__ScanAvailableDot11NetworksResponse *tds__ScanAvailableDot11NetworksResponse)
{
    return 0;
}


/** Web service operation '__tds__GetSystemUris' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__GetSystemUris(struct soap* soap, struct _tds__GetSystemUris *tds__GetSystemUris, struct _tds__GetSystemUrisResponse *tds__GetSystemUrisResponse)
{
    return 0;
}

/** Web service operation '__tds__StartFirmwareUpgrade' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__StartFirmwareUpgrade(struct soap* soap, struct _tds__StartFirmwareUpgrade *tds__StartFirmwareUpgrade, struct _tds__StartFirmwareUpgradeResponse *tds__StartFirmwareUpgradeResponse)
{
    return 0;
}

/** Web service operation '__tds__StartSystemRestore' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__StartSystemRestore(struct soap* soap, struct _tds__StartSystemRestore *tds__StartSystemRestore, struct _tds__StartSystemRestoreResponse *tds__StartSystemRestoreResponse)
{
    return 0;
}

/** Web service operation '__tds__GetStorageConfigurations' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__GetStorageConfigurations(struct soap* soap, struct _tds__GetStorageConfigurations *tds__GetStorageConfigurations, struct _tds__GetStorageConfigurationsResponse *tds__GetStorageConfigurationsResponse)
{
    return 0;
}

/** Web service operation '__tds__CreateStorageConfiguration' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__CreateStorageConfiguration(struct soap* soap, struct _tds__CreateStorageConfiguration *tds__CreateStorageConfiguration, struct _tds__CreateStorageConfigurationResponse *tds__CreateStorageConfigurationResponse)
{
    return 0;
}

/** Web service operation '__tds__GetStorageConfiguration' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__GetStorageConfiguration(struct soap* soap, struct _tds__GetStorageConfiguration *tds__GetStorageConfiguration, struct _tds__GetStorageConfigurationResponse *tds__GetStorageConfigurationResponse)
{
    return 0;
}

/** Web service operation '__tds__SetStorageConfiguration' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__SetStorageConfiguration(struct soap* soap, struct _tds__SetStorageConfiguration *tds__SetStorageConfiguration, struct _tds__SetStorageConfigurationResponse *tds__SetStorageConfigurationResponse)
{
    return 0;
}

/** Web service operation '__tds__DeleteStorageConfiguration' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__DeleteStorageConfiguration(struct soap* soap, struct _tds__DeleteStorageConfiguration *tds__DeleteStorageConfiguration, struct _tds__DeleteStorageConfigurationResponse *tds__DeleteStorageConfigurationResponse)
{
    return 0;
}

/** Web service operation '__tds__GetGeoLocation' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__GetGeoLocation(struct soap* soap, struct _tds__GetGeoLocation *tds__GetGeoLocation, struct _tds__GetGeoLocationResponse *tds__GetGeoLocationResponse)
{
    return 0;
}

/** Web service operation '__tds__SetGeoLocation' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__SetGeoLocation(struct soap* soap, struct _tds__SetGeoLocation *tds__SetGeoLocation, struct _tds__SetGeoLocationResponse *tds__SetGeoLocationResponse)
{
    return 0;
}

/** Web service operation '__tds__DeleteGeoLocation' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__DeleteGeoLocation(struct soap* soap, struct _tds__DeleteGeoLocation *tds__DeleteGeoLocation, struct _tds__DeleteGeoLocationResponse *tds__DeleteGeoLocationResponse)
{
    return 0;
}

/** Web service operation '__tds__GetServices_' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__GetServices_(struct soap* soap, struct _tds__GetServices *tds__GetServices, struct _tds__GetServicesResponse *tds__GetServicesResponse)
{
    return 0;
}

/** Web service operation '__tds__GetServiceCapabilities_' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__GetServiceCapabilities_(struct soap* soap, struct _tds__GetServiceCapabilities *tds__GetServiceCapabilities, struct _tds__GetServiceCapabilitiesResponse *tds__GetServiceCapabilitiesResponse)
{
    return 0;
}

/** Web service operation '__tds__GetDeviceInformation_' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__GetDeviceInformation_(struct soap* soap, struct _tds__GetDeviceInformation *tds__GetDeviceInformation, struct _tds__GetDeviceInformationResponse *tds__GetDeviceInformationResponse)
{
    return 0;
}

/** Web service operation '__tds__SetSystemDateAndTime_' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__SetSystemDateAndTime_(struct soap* soap, struct _tds__SetSystemDateAndTime *tds__SetSystemDateAndTime, struct _tds__SetSystemDateAndTimeResponse *tds__SetSystemDateAndTimeResponse)
{
    return 0;
}

/** Web service operation '__tds__GetSystemDateAndTime_' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__GetSystemDateAndTime_(struct soap* soap, struct _tds__GetSystemDateAndTime *tds__GetSystemDateAndTime, struct _tds__GetSystemDateAndTimeResponse *tds__GetSystemDateAndTimeResponse)
{
    return 0;
}

/** Web service operation '__tds__SetSystemFactoryDefault_' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__SetSystemFactoryDefault_(struct soap* soap, struct _tds__SetSystemFactoryDefault *tds__SetSystemFactoryDefault, struct _tds__SetSystemFactoryDefaultResponse *tds__SetSystemFactoryDefaultResponse)
{
    return 0;
}

/** Web service operation '__tds__UpgradeSystemFirmware_' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__UpgradeSystemFirmware_(struct soap* soap, struct _tds__UpgradeSystemFirmware *tds__UpgradeSystemFirmware, struct _tds__UpgradeSystemFirmwareResponse *tds__UpgradeSystemFirmwareResponse)
{
    return 0;
}

/** Web service operation '__tds__SystemReboot_' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__SystemReboot_(struct soap* soap, struct _tds__SystemReboot *tds__SystemReboot, struct _tds__SystemRebootResponse *tds__SystemRebootResponse)
{
    return 0;
}

/** Web service operation '__tds__RestoreSystem_' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__RestoreSystem_(struct soap* soap, struct _tds__RestoreSystem *tds__RestoreSystem, struct _tds__RestoreSystemResponse *tds__RestoreSystemResponse)
{
    return 0;
}

/** Web service operation '__tds__GetSystemBackup_' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__GetSystemBackup_(struct soap* soap, struct _tds__GetSystemBackup *tds__GetSystemBackup, struct _tds__GetSystemBackupResponse *tds__GetSystemBackupResponse)
{
    return 0;
}

/** Web service operation '__tds__GetSystemLog_' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__GetSystemLog_(struct soap* soap, struct _tds__GetSystemLog *tds__GetSystemLog, struct _tds__GetSystemLogResponse *tds__GetSystemLogResponse)
{
    return 0;
}

/** Web service operation '__tds__GetSystemSupportInformation_' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__GetSystemSupportInformation_(struct soap* soap, struct _tds__GetSystemSupportInformation *tds__GetSystemSupportInformation, struct _tds__GetSystemSupportInformationResponse *tds__GetSystemSupportInformationResponse)
{
    return 0;
}

/** Web service operation '__tds__GetScopes_' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__GetScopes_(struct soap* soap, struct _tds__GetScopes *tds__GetScopes, struct _tds__GetScopesResponse *tds__GetScopesResponse)
{
    return 0;
}

/** Web service operation '__tds__SetScopes_' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__SetScopes_(struct soap* soap, struct _tds__SetScopes *tds__SetScopes, struct _tds__SetScopesResponse *tds__SetScopesResponse)
{
    return 0;
}

/** Web service operation '__tds__AddScopes_' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__AddScopes_(struct soap* soap, struct _tds__AddScopes *tds__AddScopes, struct _tds__AddScopesResponse *tds__AddScopesResponse)
{
    return 0;
}

/** Web service operation '__tds__RemoveScopes_' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__RemoveScopes_(struct soap* soap, struct _tds__RemoveScopes *tds__RemoveScopes, struct _tds__RemoveScopesResponse *tds__RemoveScopesResponse)
{
    return 0;
}

/** Web service operation '__tds__GetDiscoveryMode_' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__GetDiscoveryMode_(struct soap* soap, struct _tds__GetDiscoveryMode *tds__GetDiscoveryMode, struct _tds__GetDiscoveryModeResponse *tds__GetDiscoveryModeResponse)
{
    return 0;
}

/** Web service operation '__tds__SetDiscoveryMode_' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__SetDiscoveryMode_(struct soap* soap, struct _tds__SetDiscoveryMode *tds__SetDiscoveryMode, struct _tds__SetDiscoveryModeResponse *tds__SetDiscoveryModeResponse)
{
    return 0;
}

/** Web service operation '__tds__GetRemoteDiscoveryMode_' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__GetRemoteDiscoveryMode_(struct soap* soap, struct _tds__GetRemoteDiscoveryMode *tds__GetRemoteDiscoveryMode, struct _tds__GetRemoteDiscoveryModeResponse *tds__GetRemoteDiscoveryModeResponse)
{
    return 0;
}

/** Web service operation '__tds__SetRemoteDiscoveryMode_' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__SetRemoteDiscoveryMode_(struct soap* soap, struct _tds__SetRemoteDiscoveryMode *tds__SetRemoteDiscoveryMode, struct _tds__SetRemoteDiscoveryModeResponse *tds__SetRemoteDiscoveryModeResponse)
{
    return 0;
}

/** Web service operation '__tds__GetDPAddresses_' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__GetDPAddresses_(struct soap* soap, struct _tds__GetDPAddresses *tds__GetDPAddresses, struct _tds__GetDPAddressesResponse *tds__GetDPAddressesResponse)
{
    return 0;
}

/** Web service operation '__tds__GetEndpointReference_' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__GetEndpointReference_(struct soap* soap, struct _tds__GetEndpointReference *tds__GetEndpointReference, struct _tds__GetEndpointReferenceResponse *tds__GetEndpointReferenceResponse)
{
    return 0;
}

/** Web service operation '__tds__GetRemoteUser_' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__GetRemoteUser_(struct soap* soap, struct _tds__GetRemoteUser *tds__GetRemoteUser, struct _tds__GetRemoteUserResponse *tds__GetRemoteUserResponse)
{
    return 0;
}

/** Web service operation '__tds__SetRemoteUser_' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__SetRemoteUser_(struct soap* soap, struct _tds__SetRemoteUser *tds__SetRemoteUser, struct _tds__SetRemoteUserResponse *tds__SetRemoteUserResponse)
{
    return 0;
}

/** Web service operation '__tds__GetUsers_' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__GetUsers_(struct soap* soap, struct _tds__GetUsers *tds__GetUsers, struct _tds__GetUsersResponse *tds__GetUsersResponse)
{
    return 0;
}

/** Web service operation '__tds__CreateUsers_' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__CreateUsers_(struct soap* soap, struct _tds__CreateUsers *tds__CreateUsers, struct _tds__CreateUsersResponse *tds__CreateUsersResponse)
{
    return 0;
}

/** Web service operation '__tds__DeleteUsers_' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__DeleteUsers_(struct soap* soap, struct _tds__DeleteUsers *tds__DeleteUsers, struct _tds__DeleteUsersResponse *tds__DeleteUsersResponse)
{
    return 0;
}

/** Web service operation '__tds__SetUser_' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__SetUser_(struct soap* soap, struct _tds__SetUser *tds__SetUser, struct _tds__SetUserResponse *tds__SetUserResponse)
{
    return 0;
}

/** Web service operation '__tds__GetWsdlUrl_' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__GetWsdlUrl_(struct soap* soap, struct _tds__GetWsdlUrl *tds__GetWsdlUrl, struct _tds__GetWsdlUrlResponse *tds__GetWsdlUrlResponse)
{
    return 0;
}

/** Web service operation '__tds__GetCapabilities_' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__GetCapabilities_(struct soap* soap, struct _tds__GetCapabilities *tds__GetCapabilities, struct _tds__GetCapabilitiesResponse *tds__GetCapabilitiesResponse)
{
    return 0;
}

/** Web service operation '__tds__SetDPAddresses_' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__SetDPAddresses_(struct soap* soap, struct _tds__SetDPAddresses *tds__SetDPAddresses, struct _tds__SetDPAddressesResponse *tds__SetDPAddressesResponse)
{
    return 0;
}

/** Web service operation '__tds__GetHostname_' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__GetHostname_(struct soap* soap, struct _tds__GetHostname *tds__GetHostname, struct _tds__GetHostnameResponse *tds__GetHostnameResponse)
{
    return 0;
}

/** Web service operation '__tds__SetHostname_' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__SetHostname_(struct soap* soap, struct _tds__SetHostname *tds__SetHostname, struct _tds__SetHostnameResponse *tds__SetHostnameResponse)
{
    return 0;
}

/** Web service operation '__tds__SetHostnameFromDHCP_' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__SetHostnameFromDHCP_(struct soap* soap, struct _tds__SetHostnameFromDHCP *tds__SetHostnameFromDHCP, struct _tds__SetHostnameFromDHCPResponse *tds__SetHostnameFromDHCPResponse)
{
    return 0;
}

/** Web service operation '__tds__GetDNS_' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__GetDNS_(struct soap* soap, struct _tds__GetDNS *tds__GetDNS, struct _tds__GetDNSResponse *tds__GetDNSResponse)
{
    return 0;
}

/** Web service operation '__tds__SetDNS_' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__SetDNS_(struct soap* soap, struct _tds__SetDNS *tds__SetDNS, struct _tds__SetDNSResponse *tds__SetDNSResponse)
{
    return 0;
}

/** Web service operation '__tds__GetNTP_' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__GetNTP_(struct soap* soap, struct _tds__GetNTP *tds__GetNTP, struct _tds__GetNTPResponse *tds__GetNTPResponse)
{
    return 0;
}

/** Web service operation '__tds__SetNTP_' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__SetNTP_(struct soap* soap, struct _tds__SetNTP *tds__SetNTP, struct _tds__SetNTPResponse *tds__SetNTPResponse)
{
    return 0;
}

/** Web service operation '__tds__GetDynamicDNS_' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__GetDynamicDNS_(struct soap* soap, struct _tds__GetDynamicDNS *tds__GetDynamicDNS, struct _tds__GetDynamicDNSResponse *tds__GetDynamicDNSResponse)
{
    return 0;
}

/** Web service operation '__tds__SetDynamicDNS_' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__SetDynamicDNS_(struct soap* soap, struct _tds__SetDynamicDNS *tds__SetDynamicDNS, struct _tds__SetDynamicDNSResponse *tds__SetDynamicDNSResponse)
{
    return 0;
}

/** Web service operation '__tds__GetNetworkInterfaces_' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__GetNetworkInterfaces_(struct soap* soap, struct _tds__GetNetworkInterfaces *tds__GetNetworkInterfaces, struct _tds__GetNetworkInterfacesResponse *tds__GetNetworkInterfacesResponse)
{
    return 0;
}

/** Web service operation '__tds__SetNetworkInterfaces_' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__SetNetworkInterfaces_(struct soap* soap, struct _tds__SetNetworkInterfaces *tds__SetNetworkInterfaces, struct _tds__SetNetworkInterfacesResponse *tds__SetNetworkInterfacesResponse)
{
    return 0;
}

/** Web service operation '__tds__GetNetworkProtocols_' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__GetNetworkProtocols_(struct soap* soap, struct _tds__GetNetworkProtocols *tds__GetNetworkProtocols, struct _tds__GetNetworkProtocolsResponse *tds__GetNetworkProtocolsResponse)
{
    return 0;
}

/** Web service operation '__tds__SetNetworkProtocols_' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__SetNetworkProtocols_(struct soap* soap, struct _tds__SetNetworkProtocols *tds__SetNetworkProtocols, struct _tds__SetNetworkProtocolsResponse *tds__SetNetworkProtocolsResponse)
{
    return 0;
}

/** Web service operation '__tds__GetNetworkDefaultGateway_' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__GetNetworkDefaultGateway_(struct soap* soap, struct _tds__GetNetworkDefaultGateway *tds__GetNetworkDefaultGateway, struct _tds__GetNetworkDefaultGatewayResponse *tds__GetNetworkDefaultGatewayResponse)
{
    return 0;
}

/** Web service operation '__tds__SetNetworkDefaultGateway_' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__SetNetworkDefaultGateway_(struct soap* soap, struct _tds__SetNetworkDefaultGateway *tds__SetNetworkDefaultGateway, struct _tds__SetNetworkDefaultGatewayResponse *tds__SetNetworkDefaultGatewayResponse)
{
    return 0;
}

/** Web service operation '__tds__GetZeroConfiguration_' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__GetZeroConfiguration_(struct soap* soap, struct _tds__GetZeroConfiguration *tds__GetZeroConfiguration, struct _tds__GetZeroConfigurationResponse *tds__GetZeroConfigurationResponse)
{
    return 0;
}

/** Web service operation '__tds__SetZeroConfiguration_' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__SetZeroConfiguration_(struct soap* soap, struct _tds__SetZeroConfiguration *tds__SetZeroConfiguration, struct _tds__SetZeroConfigurationResponse *tds__SetZeroConfigurationResponse)
{
    return 0;
}

/** Web service operation '__tds__GetIPAddressFilter_' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__GetIPAddressFilter_(struct soap* soap, struct _tds__GetIPAddressFilter *tds__GetIPAddressFilter, struct _tds__GetIPAddressFilterResponse *tds__GetIPAddressFilterResponse)
{
    return 0;
}

/** Web service operation '__tds__SetIPAddressFilter_' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__SetIPAddressFilter_(struct soap* soap, struct _tds__SetIPAddressFilter *tds__SetIPAddressFilter, struct _tds__SetIPAddressFilterResponse *tds__SetIPAddressFilterResponse)
{
    return 0;
}

/** Web service operation '__tds__AddIPAddressFilter_' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__AddIPAddressFilter_(struct soap* soap, struct _tds__AddIPAddressFilter *tds__AddIPAddressFilter, struct _tds__AddIPAddressFilterResponse *tds__AddIPAddressFilterResponse)
{
    return 0;
}

/** Web service operation '__tds__RemoveIPAddressFilter_' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__RemoveIPAddressFilter_(struct soap* soap, struct _tds__RemoveIPAddressFilter *tds__RemoveIPAddressFilter, struct _tds__RemoveIPAddressFilterResponse *tds__RemoveIPAddressFilterResponse)
{
    return 0;
}

/** Web service operation '__tds__GetAccessPolicy_' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__GetAccessPolicy_(struct soap* soap, struct _tds__GetAccessPolicy *tds__GetAccessPolicy, struct _tds__GetAccessPolicyResponse *tds__GetAccessPolicyResponse)
{
    return 0;
}

/** Web service operation '__tds__SetAccessPolicy_' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__SetAccessPolicy_(struct soap* soap, struct _tds__SetAccessPolicy *tds__SetAccessPolicy, struct _tds__SetAccessPolicyResponse *tds__SetAccessPolicyResponse)
{
    return 0;
}

/** Web service operation '__tds__CreateCertificate_' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__CreateCertificate_(struct soap* soap, struct _tds__CreateCertificate *tds__CreateCertificate, struct _tds__CreateCertificateResponse *tds__CreateCertificateResponse)
{
    return 0;
}

/** Web service operation '__tds__GetCertificates_' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__GetCertificates_(struct soap* soap, struct _tds__GetCertificates *tds__GetCertificates, struct _tds__GetCertificatesResponse *tds__GetCertificatesResponse)
{
    return 0;
}

/** Web service operation '__tds__GetCertificatesStatus_' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__GetCertificatesStatus_(struct soap* soap, struct _tds__GetCertificatesStatus *tds__GetCertificatesStatus, struct _tds__GetCertificatesStatusResponse *tds__GetCertificatesStatusResponse)
{
    return 0;
}

/** Web service operation '__tds__SetCertificatesStatus_' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__SetCertificatesStatus_(struct soap* soap, struct _tds__SetCertificatesStatus *tds__SetCertificatesStatus, struct _tds__SetCertificatesStatusResponse *tds__SetCertificatesStatusResponse)
{
    return 0;
}

/** Web service operation '__tds__DeleteCertificates_' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__DeleteCertificates_(struct soap* soap, struct _tds__DeleteCertificates *tds__DeleteCertificates, struct _tds__DeleteCertificatesResponse *tds__DeleteCertificatesResponse)
{
    return 0;
}

/** Web service operation '__tds__GetPkcs10Request_' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__GetPkcs10Request_(struct soap* soap, struct _tds__GetPkcs10Request *tds__GetPkcs10Request, struct _tds__GetPkcs10RequestResponse *tds__GetPkcs10RequestResponse)
{
    return 0;
}

/** Web service operation '__tds__LoadCertificates_' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__LoadCertificates_(struct soap* soap, struct _tds__LoadCertificates *tds__LoadCertificates, struct _tds__LoadCertificatesResponse *tds__LoadCertificatesResponse)
{
    return 0;
}

/** Web service operation '__tds__GetClientCertificateMode_' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__GetClientCertificateMode_(struct soap* soap, struct _tds__GetClientCertificateMode *tds__GetClientCertificateMode, struct _tds__GetClientCertificateModeResponse *tds__GetClientCertificateModeResponse)
{
    return 0;
}

/** Web service operation '__tds__SetClientCertificateMode_' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__SetClientCertificateMode_(struct soap* soap, struct _tds__SetClientCertificateMode *tds__SetClientCertificateMode, struct _tds__SetClientCertificateModeResponse *tds__SetClientCertificateModeResponse)
{
    return 0;
}

/** Web service operation '__tds__GetRelayOutputs_' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__GetRelayOutputs_(struct soap* soap, struct _tds__GetRelayOutputs *tds__GetRelayOutputs, struct _tds__GetRelayOutputsResponse *tds__GetRelayOutputsResponse)
{
    return 0;
}

/** Web service operation '__tds__SetRelayOutputSettings_' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__SetRelayOutputSettings_(struct soap* soap, struct _tds__SetRelayOutputSettings *tds__SetRelayOutputSettings, struct _tds__SetRelayOutputSettingsResponse *tds__SetRelayOutputSettingsResponse)
{
    return 0;
}

/** Web service operation '__tds__SetRelayOutputState_' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__SetRelayOutputState_(struct soap* soap, struct _tds__SetRelayOutputState *tds__SetRelayOutputState, struct _tds__SetRelayOutputStateResponse *tds__SetRelayOutputStateResponse)
{
    return 0;
}

/** Web service operation '__tds__SendAuxiliaryCommand_' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__SendAuxiliaryCommand_(struct soap* soap, struct _tds__SendAuxiliaryCommand *tds__SendAuxiliaryCommand, struct _tds__SendAuxiliaryCommandResponse *tds__SendAuxiliaryCommandResponse)
{
    return 0;
}

/** Web service operation '__tds__GetCACertificates_' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__GetCACertificates_(struct soap* soap, struct _tds__GetCACertificates *tds__GetCACertificates, struct _tds__GetCACertificatesResponse *tds__GetCACertificatesResponse)
{
    return 0;
}

/** Web service operation '__tds__LoadCertificateWithPrivateKey_' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__LoadCertificateWithPrivateKey_(struct soap* soap, struct _tds__LoadCertificateWithPrivateKey *tds__LoadCertificateWithPrivateKey, struct _tds__LoadCertificateWithPrivateKeyResponse *tds__LoadCertificateWithPrivateKeyResponse)
{
    return 0;
}

/** Web service operation '__tds__GetCertificateInformation_' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__GetCertificateInformation_(struct soap* soap, struct _tds__GetCertificateInformation *tds__GetCertificateInformation, struct _tds__GetCertificateInformationResponse *tds__GetCertificateInformationResponse)
{
    return 0;
}

/** Web service operation '__tds__LoadCACertificates_' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__LoadCACertificates_(struct soap* soap, struct _tds__LoadCACertificates *tds__LoadCACertificates, struct _tds__LoadCACertificatesResponse *tds__LoadCACertificatesResponse)
{
    return 0;
}

/** Web service operation '__tds__CreateDot1XConfiguration_' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__CreateDot1XConfiguration_(struct soap* soap, struct _tds__CreateDot1XConfiguration *tds__CreateDot1XConfiguration, struct _tds__CreateDot1XConfigurationResponse *tds__CreateDot1XConfigurationResponse)
{
    return 0;
}

/** Web service operation '__tds__SetDot1XConfiguration_' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__SetDot1XConfiguration_(struct soap* soap, struct _tds__SetDot1XConfiguration *tds__SetDot1XConfiguration, struct _tds__SetDot1XConfigurationResponse *tds__SetDot1XConfigurationResponse)
{
    return 0;
}

/** Web service operation '__tds__GetDot1XConfiguration_' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__GetDot1XConfiguration_(struct soap* soap, struct _tds__GetDot1XConfiguration *tds__GetDot1XConfiguration, struct _tds__GetDot1XConfigurationResponse *tds__GetDot1XConfigurationResponse)
{
    return 0;
}

/** Web service operation '__tds__GetDot1XConfigurations_' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__GetDot1XConfigurations_(struct soap* soap, struct _tds__GetDot1XConfigurations *tds__GetDot1XConfigurations, struct _tds__GetDot1XConfigurationsResponse *tds__GetDot1XConfigurationsResponse)
{
    return 0;
}

/** Web service operation '__tds__DeleteDot1XConfiguration_' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__DeleteDot1XConfiguration_(struct soap* soap, struct _tds__DeleteDot1XConfiguration *tds__DeleteDot1XConfiguration, struct _tds__DeleteDot1XConfigurationResponse *tds__DeleteDot1XConfigurationResponse)
{
    return 0;
}

/** Web service operation '__tds__GetDot11Capabilities_' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__GetDot11Capabilities_(struct soap* soap, struct _tds__GetDot11Capabilities *tds__GetDot11Capabilities, struct _tds__GetDot11CapabilitiesResponse *tds__GetDot11CapabilitiesResponse)
{
    return 0;
}

/** Web service operation '__tds__GetDot11Status_' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__GetDot11Status_(struct soap* soap, struct _tds__GetDot11Status *tds__GetDot11Status, struct _tds__GetDot11StatusResponse *tds__GetDot11StatusResponse)
{
    return 0;
}

/** Web service operation '__tds__ScanAvailableDot11Networks_' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__ScanAvailableDot11Networks_(struct soap* soap, struct _tds__ScanAvailableDot11Networks *tds__ScanAvailableDot11Networks, struct _tds__ScanAvailableDot11NetworksResponse *tds__ScanAvailableDot11NetworksResponse)
{
    return 0;
}

/** Web service operation '__tds__GetSystemUris_' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__GetSystemUris_(struct soap* soap, struct _tds__GetSystemUris *tds__GetSystemUris, struct _tds__GetSystemUrisResponse *tds__GetSystemUrisResponse)
{
    return 0;
}

/** Web service operation '__tds__StartFirmwareUpgrade_' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__StartFirmwareUpgrade_(struct soap* soap, struct _tds__StartFirmwareUpgrade *tds__StartFirmwareUpgrade, struct _tds__StartFirmwareUpgradeResponse *tds__StartFirmwareUpgradeResponse)
{
    return 0;
}

/** Web service operation '__tds__StartSystemRestore_' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__StartSystemRestore_(struct soap* soap, struct _tds__StartSystemRestore *tds__StartSystemRestore, struct _tds__StartSystemRestoreResponse *tds__StartSystemRestoreResponse)
{
    return 0;
}

/** Web service operation '__tds__GetStorageConfigurations_' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__GetStorageConfigurations_(struct soap* soap, struct _tds__GetStorageConfigurations *tds__GetStorageConfigurations, struct _tds__GetStorageConfigurationsResponse *tds__GetStorageConfigurationsResponse)
{
    return 0;
}

/** Web service operation '__tds__CreateStorageConfiguration_' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__CreateStorageConfiguration_(struct soap* soap, struct _tds__CreateStorageConfiguration *tds__CreateStorageConfiguration, struct _tds__CreateStorageConfigurationResponse *tds__CreateStorageConfigurationResponse)
{
    return 0;
}

/** Web service operation '__tds__GetStorageConfiguration_' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__GetStorageConfiguration_(struct soap* soap, struct _tds__GetStorageConfiguration *tds__GetStorageConfiguration, struct _tds__GetStorageConfigurationResponse *tds__GetStorageConfigurationResponse)
{
    return 0;
}

/** Web service operation '__tds__SetStorageConfiguration_' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__SetStorageConfiguration_(struct soap* soap, struct _tds__SetStorageConfiguration *tds__SetStorageConfiguration, struct _tds__SetStorageConfigurationResponse *tds__SetStorageConfigurationResponse)
{
    return 0;
}

/** Web service operation '__tds__DeleteStorageConfiguration_' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__DeleteStorageConfiguration_(struct soap* soap, struct _tds__DeleteStorageConfiguration *tds__DeleteStorageConfiguration, struct _tds__DeleteStorageConfigurationResponse *tds__DeleteStorageConfigurationResponse)
{
    return 0;
}

/** Web service operation '__tds__GetGeoLocation_' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__GetGeoLocation_(struct soap* soap, struct _tds__GetGeoLocation *tds__GetGeoLocation, struct _tds__GetGeoLocationResponse *tds__GetGeoLocationResponse)
{
    return 0;
}

/** Web service operation '__tds__SetGeoLocation_' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__SetGeoLocation_(struct soap* soap, struct _tds__SetGeoLocation *tds__SetGeoLocation, struct _tds__SetGeoLocationResponse *tds__SetGeoLocationResponse)
{
    return 0;
}

/** Web service operation '__tds__DeleteGeoLocation_' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tds__DeleteGeoLocation_(struct soap* soap, struct _tds__DeleteGeoLocation *tds__DeleteGeoLocation, struct _tds__DeleteGeoLocationResponse *tds__DeleteGeoLocationResponse)
{
    return 0;
}

/** Web service operation '__tev__Renew' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tev__Renew(struct soap* soap, struct _wsnt__Renew *wsnt__Renew, struct _wsnt__RenewResponse *wsnt__RenewResponse)
{
    return 0;
}

/** Web service operation '__tev__Unsubscribe_' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tev__Unsubscribe_(struct soap* soap, struct _wsnt__Unsubscribe *wsnt__Unsubscribe, struct _wsnt__UnsubscribeResponse *wsnt__UnsubscribeResponse)
{
    return 0;
}

/** Web service operation '__tev__Subscribe' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tev__Subscribe(struct soap* soap, struct _wsnt__Subscribe *wsnt__Subscribe, struct _wsnt__SubscribeResponse *wsnt__SubscribeResponse)
{
    return 0;
}

/** Web service operation '__tev__GetCurrentMessage' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tev__GetCurrentMessage(struct soap* soap, struct _wsnt__GetCurrentMessage *wsnt__GetCurrentMessage, struct _wsnt__GetCurrentMessageResponse *wsnt__GetCurrentMessageResponse)
{
    return 0;
}

/** Web service operation '__tev__Notify' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tev__Notify(struct soap* soap, struct _wsnt__Notify *wsnt__Notify)
{
    return 0;
}

/** Web service operation '__tev__GetMessages' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tev__GetMessages(struct soap* soap, struct _wsnt__GetMessages *wsnt__GetMessages, struct _wsnt__GetMessagesResponse *wsnt__GetMessagesResponse)
{
    return 0;
}

/** Web service operation '__tev__DestroyPullPoint' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tev__DestroyPullPoint(struct soap* soap, struct _wsnt__DestroyPullPoint *wsnt__DestroyPullPoint, struct _wsnt__DestroyPullPointResponse *wsnt__DestroyPullPointResponse)
{
    return 0;
}

/** Web service operation '__tev__Notify_' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tev__Notify_(struct soap* soap, struct _wsnt__Notify *wsnt__Notify)
{
    return 0;
}

/** Web service operation '__tev__CreatePullPoint' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tev__CreatePullPoint(struct soap* soap, struct _wsnt__CreatePullPoint *wsnt__CreatePullPoint, struct _wsnt__CreatePullPointResponse *wsnt__CreatePullPointResponse)
{
    return 0;
}

/** Web service operation '__tev__Renew_' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tev__Renew_(struct soap* soap, struct _wsnt__Renew *wsnt__Renew, struct _wsnt__RenewResponse *wsnt__RenewResponse)
{
    return 0;
}

/** Web service operation '__tev__Unsubscribe__' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tev__Unsubscribe__(struct soap* soap, struct _wsnt__Unsubscribe *wsnt__Unsubscribe, struct _wsnt__UnsubscribeResponse *wsnt__UnsubscribeResponse)
{
    return 0;
}

/** Web service operation '__tev__PauseSubscription' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tev__PauseSubscription(struct soap* soap, struct _wsnt__PauseSubscription *wsnt__PauseSubscription, struct _wsnt__PauseSubscriptionResponse *wsnt__PauseSubscriptionResponse)
{
    return 0;
}

/** Web service operation '__tev__ResumeSubscription' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tev__ResumeSubscription(struct soap* soap, struct _wsnt__ResumeSubscription *wsnt__ResumeSubscription, struct _wsnt__ResumeSubscriptionResponse *wsnt__ResumeSubscriptionResponse)
{
    return 0;
}

/** Web service operation '__tmd__GetServiceCapabilities' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tmd__GetServiceCapabilities(struct soap* soap, struct _tmd__GetServiceCapabilities *tmd__GetServiceCapabilities, struct _tmd__GetServiceCapabilitiesResponse *tmd__GetServiceCapabilitiesResponse)
{
    return 0;
}

/** Web service operation '__tmd__GetRelayOutputOptions' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tmd__GetRelayOutputOptions(struct soap* soap, struct _tmd__GetRelayOutputOptions *tmd__GetRelayOutputOptions, struct _tmd__GetRelayOutputOptionsResponse *tmd__GetRelayOutputOptionsResponse)
{
    return 0;
}

/** Web service operation '__tmd__GetAudioSources' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tmd__GetAudioSources(struct soap* soap, struct tmd__Get *tmd__GetAudioSources, struct tmd__GetResponse *tmd__GetAudioSourcesResponse)
{
    return 0;
}

/** Web service operation '__tmd__GetAudioOutputs' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tmd__GetAudioOutputs(struct soap* soap, struct tmd__Get *tmd__GetAudioOutputs, struct tmd__GetResponse *tmd__GetAudioOutputsResponse)
{
    return 0;
}

/** Web service operation '__tmd__GetVideoSources' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tmd__GetVideoSources(struct soap* soap, struct tmd__Get *tmd__GetVideoSources, struct tmd__GetResponse *tmd__GetVideoSourcesResponse)
{
    return 0;
}

/** Web service operation '__tmd__GetVideoOutputs' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tmd__GetVideoOutputs(struct soap* soap, struct _tmd__GetVideoOutputs *tmd__GetVideoOutputs, struct _tmd__GetVideoOutputsResponse *tmd__GetVideoOutputsResponse)
{
    return 0;
}

/** Web service operation '__tmd__GetVideoSourceConfiguration' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tmd__GetVideoSourceConfiguration(struct soap* soap, struct _tmd__GetVideoSourceConfiguration *tmd__GetVideoSourceConfiguration, struct _tmd__GetVideoSourceConfigurationResponse *tmd__GetVideoSourceConfigurationResponse)
{
    return 0;
}

/** Web service operation '__tmd__GetVideoOutputConfiguration' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tmd__GetVideoOutputConfiguration(struct soap* soap, struct _tmd__GetVideoOutputConfiguration *tmd__GetVideoOutputConfiguration, struct _tmd__GetVideoOutputConfigurationResponse *tmd__GetVideoOutputConfigurationResponse)
{
    return 0;
}

/** Web service operation '__tmd__GetAudioSourceConfiguration' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tmd__GetAudioSourceConfiguration(struct soap* soap, struct _tmd__GetAudioSourceConfiguration *tmd__GetAudioSourceConfiguration, struct _tmd__GetAudioSourceConfigurationResponse *tmd__GetAudioSourceConfigurationResponse)
{
    return 0;
}

/** Web service operation '__tmd__GetAudioOutputConfiguration' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tmd__GetAudioOutputConfiguration(struct soap* soap, struct _tmd__GetAudioOutputConfiguration *tmd__GetAudioOutputConfiguration, struct _tmd__GetAudioOutputConfigurationResponse *tmd__GetAudioOutputConfigurationResponse)
{
    return 0;
}

/** Web service operation '__tmd__SetVideoSourceConfiguration' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tmd__SetVideoSourceConfiguration(struct soap* soap, struct _tmd__SetVideoSourceConfiguration *tmd__SetVideoSourceConfiguration, struct _tmd__SetVideoSourceConfigurationResponse *tmd__SetVideoSourceConfigurationResponse)
{
    return 0;
}

/** Web service operation '__tmd__SetVideoOutputConfiguration' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tmd__SetVideoOutputConfiguration(struct soap* soap, struct _tmd__SetVideoOutputConfiguration *tmd__SetVideoOutputConfiguration, struct _tmd__SetVideoOutputConfigurationResponse *tmd__SetVideoOutputConfigurationResponse)
{
    return 0;
}

/** Web service operation '__tmd__SetAudioSourceConfiguration' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tmd__SetAudioSourceConfiguration(struct soap* soap, struct _tmd__SetAudioSourceConfiguration *tmd__SetAudioSourceConfiguration, struct _tmd__SetAudioSourceConfigurationResponse *tmd__SetAudioSourceConfigurationResponse)
{
    return 0;
}

/** Web service operation '__tmd__SetAudioOutputConfiguration' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tmd__SetAudioOutputConfiguration(struct soap* soap, struct _tmd__SetAudioOutputConfiguration *tmd__SetAudioOutputConfiguration, struct _tmd__SetAudioOutputConfigurationResponse *tmd__SetAudioOutputConfigurationResponse)
{
    return 0;
}

/** Web service operation '__tmd__GetVideoSourceConfigurationOptions' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tmd__GetVideoSourceConfigurationOptions(struct soap* soap, struct _tmd__GetVideoSourceConfigurationOptions *tmd__GetVideoSourceConfigurationOptions, struct _tmd__GetVideoSourceConfigurationOptionsResponse *tmd__GetVideoSourceConfigurationOptionsResponse)
{
    return 0;
}

/** Web service operation '__tmd__GetVideoOutputConfigurationOptions' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tmd__GetVideoOutputConfigurationOptions(struct soap* soap, struct _tmd__GetVideoOutputConfigurationOptions *tmd__GetVideoOutputConfigurationOptions, struct _tmd__GetVideoOutputConfigurationOptionsResponse *tmd__GetVideoOutputConfigurationOptionsResponse)
{
    return 0;
}

/** Web service operation '__tmd__GetAudioSourceConfigurationOptions' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tmd__GetAudioSourceConfigurationOptions(struct soap* soap, struct _tmd__GetAudioSourceConfigurationOptions *tmd__GetAudioSourceConfigurationOptions, struct _tmd__GetAudioSourceConfigurationOptionsResponse *tmd__GetAudioSourceConfigurationOptionsResponse)
{
    return 0;
}

/** Web service operation '__tmd__GetAudioOutputConfigurationOptions' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tmd__GetAudioOutputConfigurationOptions(struct soap* soap, struct _tmd__GetAudioOutputConfigurationOptions *tmd__GetAudioOutputConfigurationOptions, struct _tmd__GetAudioOutputConfigurationOptionsResponse *tmd__GetAudioOutputConfigurationOptionsResponse)
{
    return 0;
}

/** Web service operation '__tmd__GetRelayOutputs' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tmd__GetRelayOutputs(struct soap* soap, struct _tds__GetRelayOutputs *tds__GetRelayOutputs, struct _tds__GetRelayOutputsResponse *tds__GetRelayOutputsResponse)
{
    return 0;
}

/** Web service operation '__tmd__SetRelayOutputSettings' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tmd__SetRelayOutputSettings(struct soap* soap, struct _tmd__SetRelayOutputSettings *tmd__SetRelayOutputSettings, struct _tmd__SetRelayOutputSettingsResponse *tmd__SetRelayOutputSettingsResponse)
{
    return 0;
}

/** Web service operation '__tmd__SetRelayOutputState' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tmd__SetRelayOutputState(struct soap* soap, struct _tds__SetRelayOutputState *tds__SetRelayOutputState, struct _tds__SetRelayOutputStateResponse *tds__SetRelayOutputStateResponse)
{
    return 0;
}

/** Web service operation '__tmd__GetDigitalInputs' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tmd__GetDigitalInputs(struct soap* soap, struct _tmd__GetDigitalInputs *tmd__GetDigitalInputs, struct _tmd__GetDigitalInputsResponse *tmd__GetDigitalInputsResponse)
{
    return 0;
}

/** Web service operation '__tmd__GetDigitalInputConfigurationOptions' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tmd__GetDigitalInputConfigurationOptions(struct soap* soap, struct _tmd__GetDigitalInputConfigurationOptions *tmd__GetDigitalInputConfigurationOptions, struct _tmd__GetDigitalInputConfigurationOptionsResponse *tmd__GetDigitalInputConfigurationOptionsResponse)
{
    return 0;
}

/** Web service operation '__tmd__SetDigitalInputConfigurations' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tmd__SetDigitalInputConfigurations(struct soap* soap, struct _tmd__SetDigitalInputConfigurations *tmd__SetDigitalInputConfigurations, struct _tmd__SetDigitalInputConfigurationsResponse *tmd__SetDigitalInputConfigurationsResponse)
{
    return 0;
}

/** Web service operation '__tmd__GetSerialPorts' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tmd__GetSerialPorts(struct soap* soap, struct _tmd__GetSerialPorts *tmd__GetSerialPorts, struct _tmd__GetSerialPortsResponse *tmd__GetSerialPortsResponse)
{
    return 0;
}

/** Web service operation '__tmd__GetSerialPortConfiguration' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tmd__GetSerialPortConfiguration(struct soap* soap, struct _tmd__GetSerialPortConfiguration *tmd__GetSerialPortConfiguration, struct _tmd__GetSerialPortConfigurationResponse *tmd__GetSerialPortConfigurationResponse)
{
    return 0;
}

/** Web service operation '__tmd__SetSerialPortConfiguration' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tmd__SetSerialPortConfiguration(struct soap* soap, struct _tmd__SetSerialPortConfiguration *tmd__SetSerialPortConfiguration, struct _tmd__SetSerialPortConfigurationResponse *tmd__SetSerialPortConfigurationResponse)
{
    return 0;
}

/** Web service operation '__tmd__GetSerialPortConfigurationOptions' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tmd__GetSerialPortConfigurationOptions(struct soap* soap, struct _tmd__GetSerialPortConfigurationOptions *tmd__GetSerialPortConfigurationOptions, struct _tmd__GetSerialPortConfigurationOptionsResponse *tmd__GetSerialPortConfigurationOptionsResponse)
{
    return 0;
}

/** Web service operation '__tmd__SendReceiveSerialCommand' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __tmd__SendReceiveSerialCommand(struct soap* soap, struct _tmd__SendReceiveSerialCommand *tmd__SendReceiveSerialCommand, struct _tmd__SendReceiveSerialCommandResponse *tmd__SendReceiveSerialCommandResponse)
{
    return 0;
}

/** Web service operation '__trt__GetServiceCapabilities' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __trt__GetServiceCapabilities(struct soap* soap, struct _trt__GetServiceCapabilities *trt__GetServiceCapabilities, struct _trt__GetServiceCapabilitiesResponse *trt__GetServiceCapabilitiesResponse)
{
    return 0;
}

/** Web service operation '__trt__GetVideoSources' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __trt__GetVideoSources(struct soap* soap, struct _trt__GetVideoSources *trt__GetVideoSources, struct _trt__GetVideoSourcesResponse *trt__GetVideoSourcesResponse)
{
    printf("-----------------------------__trt__GetVideoSources------------------------------------");

    /*int s32Ret = soap_wsse_verify_Password(soap, "123456");
    if (SOAP_OK != s32Ret)
    {
        return s32Ret;
    }
    printf("soap_wsse_verify_Password is success!\n");*/

    int size = 1;
    trt__GetVideoSourcesResponse->__sizeVideoSources = size;
    trt__GetVideoSourcesResponse->VideoSources = (struct tt__VideoSource *)soap_malloc(soap, sizeof(struct tt__VideoSource) * size);
    memset(trt__GetVideoSourcesResponse->VideoSources, '\0', sizeof(struct tt__VideoSource) * trt__GetVideoSourcesResponse->__sizeVideoSources);

    trt__GetVideoSourcesResponse->VideoSources->token = (char *)soap_malloc(soap, sizeof(char) * 32);
    memset(trt__GetVideoSourcesResponse->VideoSources->token, '\0', sizeof(char) * 32);
    strcpy(trt__GetVideoSourcesResponse->VideoSources->token, "000");

    trt__GetVideoSourcesResponse->VideoSources->Resolution = (struct tt__VideoResolution *)soap_malloc(soap, sizeof(struct tt__VideoResolution));
    memset(trt__GetVideoSourcesResponse->VideoSources->Resolution, '\0', sizeof(struct tt__VideoResolution));
    trt__GetVideoSourcesResponse->VideoSources->Resolution->Width = ONVIF_FRAME_WIDTH;
    trt__GetVideoSourcesResponse->VideoSources->Resolution->Height = ONVIF_FRAME_HEIGHT;
    trt__GetVideoSourcesResponse->VideoSources->Framerate = 25;

    return SOAP_OK;
}

/** Web service operation '__trt__GetAudioSources' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __trt__GetAudioSources(struct soap* soap, struct _trt__GetAudioSources *trt__GetAudioSources, struct _trt__GetAudioSourcesResponse *trt__GetAudioSourcesResponse)
{
    return 0;
}

/** Web service operation '__trt__GetAudioOutputs' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __trt__GetAudioOutputs(struct soap* soap, struct _trt__GetAudioOutputs *trt__GetAudioOutputs, struct _trt__GetAudioOutputsResponse *trt__GetAudioOutputsResponse)
{
    return 0;
}

/** Web service operation '__trt__CreateProfile' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __trt__CreateProfile(struct soap* soap, struct _trt__CreateProfile *trt__CreateProfile, struct _trt__CreateProfileResponse *trt__CreateProfileResponse)
{
    return 0;
}

/** Web service operation '__trt__GetProfile' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __trt__GetProfile(struct soap* soap, struct _trt__GetProfile *trt__GetProfile, struct _trt__GetProfileResponse *trt__GetProfileResponse)
{
    trt__GetProfileResponse->Profile = (struct tt__Profile *)soap_malloc(soap, sizeof(struct tt__Profile));
    memset(trt__GetProfileResponse->Profile, '\0', sizeof(struct tt__Profile));

    //<profiles><name>和<profiles><token>
    trt__GetProfileResponse->Profile->Name = (char *)soap_malloc(soap, sizeof(char) * 32);
    memset(trt__GetProfileResponse->Profile->Name, '\0', sizeof(char) * 32);
    strcpy(trt__GetProfileResponse->Profile->Name, "MediaProfile_Name");
    trt__GetProfileResponse->Profile->token = (char *)soap_malloc(soap, sizeof(char) * 32);
    memset(trt__GetProfileResponse->Profile->token, '\0', sizeof(char) * 32);
    strcpy(trt__GetProfileResponse->Profile->token, "MediaProfile000");
    trt__GetProfileResponse->Profile->fixed = (enum xsd__boolean *)soap_malloc(soap, sizeof(enum xsd__boolean));
    *(trt__GetProfileResponse->Profile->fixed) = xsd__boolean__true_;

    //<VideoSourceConfiguration><name>和<VideoSourceConfiguration><token>
    trt__GetProfileResponse->Profile->VideoSourceConfiguration = (struct tt__VideoSourceConfiguration *)soap_malloc(soap,sizeof(struct tt__VideoSourceConfiguration));
    memset(trt__GetProfileResponse->Profile->VideoSourceConfiguration, 0, sizeof(struct tt__VideoSourceConfiguration));
    trt__GetProfileResponse->Profile->VideoSourceConfiguration->Name = (char *)soap_malloc(soap,sizeof(char) * 32);
    memset(trt__GetProfileResponse->Profile->VideoSourceConfiguration->Name, '\0', sizeof(char) * 32);
    strcpy(trt__GetProfileResponse->Profile->VideoSourceConfiguration->Name, "SourceConfig_Name");
    trt__GetProfileResponse->Profile->VideoSourceConfiguration->token = (char *)soap_malloc(soap,sizeof(char) * 32);
    memset(trt__GetProfileResponse->Profile->VideoSourceConfiguration->token, '\0', sizeof(char) * 32);
    strcpy(trt__GetProfileResponse->Profile->VideoSourceConfiguration->token, "000");
    trt__GetProfileResponse->Profile->VideoSourceConfiguration->SourceToken = (char *)soap_malloc(soap,sizeof(char) * 32);
    memset(trt__GetProfileResponse->Profile->VideoSourceConfiguration->SourceToken, '\0', sizeof(char) * 32);
    strcpy(trt__GetProfileResponse->Profile->VideoSourceConfiguration->SourceToken, "000");
    trt__GetProfileResponse->Profile->VideoSourceConfiguration->UseCount = 1;
    //<VideoSourceConfiguration><Bounds>
    trt__GetProfileResponse->Profile->VideoSourceConfiguration->Bounds = (struct tt__IntRectangle *)soap_malloc(soap, sizeof(struct tt__IntRectangle));
    memset(trt__GetProfileResponse->Profile->VideoSourceConfiguration->Bounds, 0, sizeof(struct tt__IntRectangle));
    trt__GetProfileResponse->Profile->VideoSourceConfiguration->Bounds->x = 0;
    trt__GetProfileResponse->Profile->VideoSourceConfiguration->Bounds->y = 0;
    trt__GetProfileResponse->Profile->VideoSourceConfiguration->Bounds->width = ONVIF_FRAME_WIDTH;
    trt__GetProfileResponse->Profile->VideoSourceConfiguration->Bounds->height = ONVIF_FRAME_HEIGHT;

    //<VideoEncoderConfiguration>
    trt__GetProfileResponse->Profile->VideoEncoderConfiguration = (struct tt__VideoEncoderConfiguration *)soap_malloc(soap, sizeof(struct tt__VideoEncoderConfiguration)) ;
    memset(trt__GetProfileResponse->Profile->VideoEncoderConfiguration, '\0', sizeof(struct tt__VideoEncoderConfiguration));
    trt__GetProfileResponse->Profile->VideoEncoderConfiguration->Name = (char *)soap_malloc(soap, sizeof(char)*32);
    memset(trt__GetProfileResponse->Profile->VideoEncoderConfiguration->Name, '\0', sizeof(char)*32);
    strcpy(trt__GetProfileResponse->Profile->VideoEncoderConfiguration->Name, "EncoderConfig_Name");
    trt__GetProfileResponse->Profile->VideoEncoderConfiguration->token = (char *)soap_malloc(soap, sizeof(char)*32);
    memset(trt__GetProfileResponse->Profile->VideoEncoderConfiguration->token, '\0', sizeof(char)*32);
    strcpy(trt__GetProfileResponse->Profile->VideoEncoderConfiguration->token, "000");
    trt__GetProfileResponse->Profile->VideoEncoderConfiguration->UseCount = 1;
    trt__GetProfileResponse->Profile->VideoEncoderConfiguration->Encoding = tt__VideoEncoding__H264;
    //<VideoEncoderConfiguration><Resolution>、<RateControl>
    trt__GetProfileResponse->Profile->VideoEncoderConfiguration->Resolution = (struct tt__VideoResolution *)soap_malloc(soap, sizeof(struct tt__VideoResolution));
    memset(trt__GetProfileResponse->Profile->VideoEncoderConfiguration->Resolution, '\0', sizeof(struct tt__VideoResolution));
    trt__GetProfileResponse->Profile->VideoEncoderConfiguration->Resolution->Width = ONVIF_FRAME_WIDTH;
    trt__GetProfileResponse->Profile->VideoEncoderConfiguration->Resolution->Height = ONVIF_FRAME_HEIGHT;
    trt__GetProfileResponse->Profile->VideoEncoderConfiguration->Quality = 6;
    //<VideoEncoderConfiguration><RateControl>
    trt__GetProfileResponse->Profile->VideoEncoderConfiguration->RateControl = (struct tt__VideoRateControl *)soap_malloc(soap, sizeof(struct tt__VideoRateControl));
    memset(trt__GetProfileResponse->Profile->VideoEncoderConfiguration->RateControl, '\0', sizeof(struct tt__VideoRateControl));
    trt__GetProfileResponse->Profile->VideoEncoderConfiguration->RateControl->FrameRateLimit = 25;
    trt__GetProfileResponse->Profile->VideoEncoderConfiguration->RateControl->EncodingInterval = 1;
    trt__GetProfileResponse->Profile->VideoEncoderConfiguration->RateControl->BitrateLimit = 4096;
    //<VideoEncoderConfiguration><H264>
    trt__GetProfileResponse->Profile->VideoEncoderConfiguration->H264 = (struct tt__H264Configuration *)soap_malloc(soap, sizeof(struct tt__H264Configuration));
    memset(trt__GetProfileResponse->Profile->VideoEncoderConfiguration->H264, '\0', sizeof(struct tt__H264Configuration));
    trt__GetProfileResponse->Profile->VideoEncoderConfiguration->H264->GovLength = 60;
    trt__GetProfileResponse->Profile->VideoEncoderConfiguration->H264->H264Profile = tt__H264Profile__High;


    return 0;
}

/** Web service operation '__trt__GetProfiles' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __trt__GetProfiles(struct soap* soap, struct _trt__GetProfiles *trt__GetProfiles, struct _trt__GetProfilesResponse *trt__GetProfilesResponse)
{
    printf("-------------------------------__trt__GetProfiles---------------------------------\n");

    trt__GetProfilesResponse->__sizeProfiles = 1;
    trt__GetProfilesResponse->Profiles = (struct tt__Profile *)soap_malloc(soap, sizeof(struct tt__Profile) * trt__GetProfilesResponse->__sizeProfiles);
    memset(trt__GetProfilesResponse->Profiles, '\0', sizeof(struct tt__Profile) * trt__GetProfilesResponse->__sizeProfiles);

    //<profiles><name>和<profiles><token>
    int i = 0;
    trt__GetProfilesResponse->Profiles[i].Name = (char *)soap_malloc(soap, sizeof(char) * 32);
    memset(trt__GetProfilesResponse->Profiles[i].Name, '\0', sizeof(char) * 32);
    strcpy(trt__GetProfilesResponse->Profiles[i].Name, "MediaProfile_Name");
    trt__GetProfilesResponse->Profiles[i].token = (char *)soap_malloc(soap, sizeof(char) * 32);
    memset(trt__GetProfilesResponse->Profiles[i].token, '\0', sizeof(char) * 32);
    strcpy(trt__GetProfilesResponse->Profiles[i].token, "MediaProfile000");
    
    //<VideoSourceConfiguration><name>和<VideoSourceConfiguration><token>
    trt__GetProfilesResponse->Profiles[i].VideoSourceConfiguration = (struct tt__VideoSourceConfiguration *)soap_malloc(soap,sizeof(struct tt__VideoSourceConfiguration));
    memset(trt__GetProfilesResponse->Profiles[i].VideoSourceConfiguration, 0, sizeof(struct tt__VideoSourceConfiguration));
    trt__GetProfilesResponse->Profiles[i].VideoSourceConfiguration->Name = (char *)soap_malloc(soap,sizeof(char) * 32);
    memset(trt__GetProfilesResponse->Profiles[i].VideoSourceConfiguration->Name, '\0', sizeof(char) * 32);
    strcpy(trt__GetProfilesResponse->Profiles[i].VideoSourceConfiguration->Name, "SourceConfig_Name");
    trt__GetProfilesResponse->Profiles[i].VideoSourceConfiguration->token = (char *)soap_malloc(soap,sizeof(char) * 32);
    memset(trt__GetProfilesResponse->Profiles[i].VideoSourceConfiguration->token, '\0', sizeof(char) * 32);
    strcpy(trt__GetProfilesResponse->Profiles[i].VideoSourceConfiguration->token, "000");
    trt__GetProfilesResponse->Profiles[i].VideoSourceConfiguration->SourceToken = (char *)soap_malloc(soap,sizeof(char) * 32);
    memset(trt__GetProfilesResponse->Profiles[i].VideoSourceConfiguration->SourceToken, '\0', sizeof(char) * 32);
    strcpy(trt__GetProfilesResponse->Profiles[i].VideoSourceConfiguration->SourceToken, "000");
    trt__GetProfilesResponse->Profiles[i].VideoSourceConfiguration->UseCount = 1;
    //<VideoSourceConfiguration><Bounds>
    trt__GetProfilesResponse->Profiles[i].VideoSourceConfiguration->Bounds = (struct tt__IntRectangle *)soap_malloc(soap, sizeof(struct tt__IntRectangle));
    memset(trt__GetProfilesResponse->Profiles[i].VideoSourceConfiguration->Bounds, 0, sizeof(struct tt__IntRectangle));
    trt__GetProfilesResponse->Profiles[i].VideoSourceConfiguration->Bounds->x = 0;
    trt__GetProfilesResponse->Profiles[i].VideoSourceConfiguration->Bounds->y = 0;
    trt__GetProfilesResponse->Profiles[i].VideoSourceConfiguration->Bounds->width = ONVIF_FRAME_WIDTH;
    trt__GetProfilesResponse->Profiles[i].VideoSourceConfiguration->Bounds->height = ONVIF_FRAME_HEIGHT;

    //<VideoEncoderConfiguration>
    trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration = (struct tt__VideoEncoderConfiguration *)soap_malloc(soap, sizeof(struct tt__VideoEncoderConfiguration)) ;
    memset(trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration, '\0', sizeof(struct tt__VideoEncoderConfiguration));
    trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->Name = (char *)soap_malloc(soap, sizeof(char)*32);
    memset(trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->Name, '\0', sizeof(char)*32);
    strcpy(trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->Name, "EncoderConfig_Name");
    trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->token = (char *)soap_malloc(soap, sizeof(char)*32);
    memset(trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->token, '\0', sizeof(char)*32);
    strcpy(trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->token, "000");
    trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->UseCount = 1;
    trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->Encoding = tt__VideoEncoding__H264;
    //<VideoEncoderConfiguration><Resolution>、<RateControl>
    trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->Resolution = (struct tt__VideoResolution *)soap_malloc(soap, sizeof(struct tt__VideoResolution));
    memset(trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->Resolution, '\0', sizeof(struct tt__VideoResolution));
    trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->Resolution->Width = ONVIF_FRAME_WIDTH;
    trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->Resolution->Height = ONVIF_FRAME_HEIGHT;
    trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->Quality = 1;
    //<VideoEncoderConfiguration><RateControl>
    trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->RateControl = (struct tt__VideoRateControl *)soap_malloc(soap, sizeof(struct tt__VideoRateControl));
    memset(trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->RateControl, '\0', sizeof(struct tt__VideoRateControl));
    trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->RateControl->FrameRateLimit = 25;
    trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->RateControl->EncodingInterval = 1;
    trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->RateControl->BitrateLimit = 1024;
    //<VideoEncoderConfiguration><H264>
    trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->H264 = (struct tt__H264Configuration *)soap_malloc(soap, sizeof(struct tt__H264Configuration));
    memset(trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->H264, '\0', sizeof(struct tt__H264Configuration));
    trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->H264->GovLength = 60;
    trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->H264->H264Profile = tt__H264Profile__High;

    trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->Multicast = (struct tt__MulticastConfiguration *)soap_malloc(soap, sizeof(struct tt__MulticastConfiguration));
    memset(trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->Multicast, '\0', sizeof(struct tt__MulticastConfiguration));

    trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->Multicast->Address = (struct tt__IPAddress *)soap_malloc(soap, sizeof(struct tt__IPAddress));
    memset(trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->Multicast->Address, 0, sizeof(struct tt__IPAddress));
    trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->Multicast->Address->Type = tt__IPType__IPv4;
    trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->Multicast->Address->IPv4Address = (char *)soap_malloc(soap, sizeof(char) * 32);
    memset(trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->Multicast->Address->IPv4Address, 0, sizeof(char) * 32);
    strcpy(trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->Multicast->Address->IPv4Address, "224.1.0.0");

    trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->Multicast->Port = 40000;
    trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->Multicast->TTL = 64;
    trt__GetProfilesResponse->Profiles[i].VideoEncoderConfiguration->Multicast->AutoStart = xsd__boolean__true_;

    return 0;
}

/** Web service operation '__trt__AddVideoEncoderConfiguration' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __trt__AddVideoEncoderConfiguration(struct soap* soap, struct _trt__AddVideoEncoderConfiguration *trt__AddVideoEncoderConfiguration, struct _trt__AddVideoEncoderConfigurationResponse *trt__AddVideoEncoderConfigurationResponse)
{
    return 0;
}

/** Web service operation '__trt__AddVideoSourceConfiguration' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __trt__AddVideoSourceConfiguration(struct soap* soap, struct _trt__AddVideoSourceConfiguration *trt__AddVideoSourceConfiguration, struct _trt__AddVideoSourceConfigurationResponse *trt__AddVideoSourceConfigurationResponse)
{
    return 0;
}

/** Web service operation '__trt__AddAudioEncoderConfiguration' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __trt__AddAudioEncoderConfiguration(struct soap* soap, struct _trt__AddAudioEncoderConfiguration *trt__AddAudioEncoderConfiguration, struct _trt__AddAudioEncoderConfigurationResponse *trt__AddAudioEncoderConfigurationResponse)
{
    return 0;
}

/** Web service operation '__trt__AddAudioSourceConfiguration' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __trt__AddAudioSourceConfiguration(struct soap* soap, struct _trt__AddAudioSourceConfiguration *trt__AddAudioSourceConfiguration, struct _trt__AddAudioSourceConfigurationResponse *trt__AddAudioSourceConfigurationResponse)
{
    return 0;
}

/** Web service operation '__trt__AddPTZConfiguration' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __trt__AddPTZConfiguration(struct soap* soap, struct _trt__AddPTZConfiguration *trt__AddPTZConfiguration, struct _trt__AddPTZConfigurationResponse *trt__AddPTZConfigurationResponse)
{
    return 0;
}

/** Web service operation '__trt__AddVideoAnalyticsConfiguration' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __trt__AddVideoAnalyticsConfiguration(struct soap* soap, struct _trt__AddVideoAnalyticsConfiguration *trt__AddVideoAnalyticsConfiguration, struct _trt__AddVideoAnalyticsConfigurationResponse *trt__AddVideoAnalyticsConfigurationResponse)
{
    return 0;
}

/** Web service operation '__trt__AddMetadataConfiguration' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __trt__AddMetadataConfiguration(struct soap* soap, struct _trt__AddMetadataConfiguration *trt__AddMetadataConfiguration, struct _trt__AddMetadataConfigurationResponse *trt__AddMetadataConfigurationResponse)
{
    return 0;
}

/** Web service operation '__trt__AddAudioOutputConfiguration' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __trt__AddAudioOutputConfiguration(struct soap* soap, struct _trt__AddAudioOutputConfiguration *trt__AddAudioOutputConfiguration, struct _trt__AddAudioOutputConfigurationResponse *trt__AddAudioOutputConfigurationResponse)
{
    return 0;
}

/** Web service operation '__trt__AddAudioDecoderConfiguration' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __trt__AddAudioDecoderConfiguration(struct soap* soap, struct _trt__AddAudioDecoderConfiguration *trt__AddAudioDecoderConfiguration, struct _trt__AddAudioDecoderConfigurationResponse *trt__AddAudioDecoderConfigurationResponse)
{
    return 0;
}

/** Web service operation '__trt__RemoveVideoEncoderConfiguration' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __trt__RemoveVideoEncoderConfiguration(struct soap* soap, struct _trt__RemoveVideoEncoderConfiguration *trt__RemoveVideoEncoderConfiguration, struct _trt__RemoveVideoEncoderConfigurationResponse *trt__RemoveVideoEncoderConfigurationResponse)
{
    return 0;
}

/** Web service operation '__trt__RemoveVideoSourceConfiguration' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __trt__RemoveVideoSourceConfiguration(struct soap* soap, struct _trt__RemoveVideoSourceConfiguration *trt__RemoveVideoSourceConfiguration, struct _trt__RemoveVideoSourceConfigurationResponse *trt__RemoveVideoSourceConfigurationResponse)
{
    return 0;
}

/** Web service operation '__trt__RemoveAudioEncoderConfiguration' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __trt__RemoveAudioEncoderConfiguration(struct soap* soap, struct _trt__RemoveAudioEncoderConfiguration *trt__RemoveAudioEncoderConfiguration, struct _trt__RemoveAudioEncoderConfigurationResponse *trt__RemoveAudioEncoderConfigurationResponse)
{
    return 0;
}

/** Web service operation '__trt__RemoveAudioSourceConfiguration' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __trt__RemoveAudioSourceConfiguration(struct soap* soap, struct _trt__RemoveAudioSourceConfiguration *trt__RemoveAudioSourceConfiguration, struct _trt__RemoveAudioSourceConfigurationResponse *trt__RemoveAudioSourceConfigurationResponse)
{
    return 0;
}

/** Web service operation '__trt__RemovePTZConfiguration' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __trt__RemovePTZConfiguration(struct soap* soap, struct _trt__RemovePTZConfiguration *trt__RemovePTZConfiguration, struct _trt__RemovePTZConfigurationResponse *trt__RemovePTZConfigurationResponse)
{
    return 0;
}

/** Web service operation '__trt__RemoveVideoAnalyticsConfiguration' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __trt__RemoveVideoAnalyticsConfiguration(struct soap* soap, struct _trt__RemoveVideoAnalyticsConfiguration *trt__RemoveVideoAnalyticsConfiguration, struct _trt__RemoveVideoAnalyticsConfigurationResponse *trt__RemoveVideoAnalyticsConfigurationResponse)
{
    return 0;
}

/** Web service operation '__trt__RemoveMetadataConfiguration' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __trt__RemoveMetadataConfiguration(struct soap* soap, struct _trt__RemoveMetadataConfiguration *trt__RemoveMetadataConfiguration, struct _trt__RemoveMetadataConfigurationResponse *trt__RemoveMetadataConfigurationResponse)
{
    return 0;
}

/** Web service operation '__trt__RemoveAudioOutputConfiguration' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __trt__RemoveAudioOutputConfiguration(struct soap* soap, struct _trt__RemoveAudioOutputConfiguration *trt__RemoveAudioOutputConfiguration, struct _trt__RemoveAudioOutputConfigurationResponse *trt__RemoveAudioOutputConfigurationResponse)
{
    return 0;
}

/** Web service operation '__trt__RemoveAudioDecoderConfiguration' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __trt__RemoveAudioDecoderConfiguration(struct soap* soap, struct _trt__RemoveAudioDecoderConfiguration *trt__RemoveAudioDecoderConfiguration, struct _trt__RemoveAudioDecoderConfigurationResponse *trt__RemoveAudioDecoderConfigurationResponse)
{
    return 0;
}

/** Web service operation '__trt__DeleteProfile' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __trt__DeleteProfile(struct soap* soap, struct _trt__DeleteProfile *trt__DeleteProfile, struct _trt__DeleteProfileResponse *trt__DeleteProfileResponse)
{
    return 0;
}

/** Web service operation '__trt__GetVideoSourceConfigurations' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __trt__GetVideoSourceConfigurations(struct soap* soap, struct _trt__GetVideoSourceConfigurations *trt__GetVideoSourceConfigurations, struct _trt__GetVideoSourceConfigurationsResponse *trt__GetVideoSourceConfigurationsResponse)
{
    printf("----------------------------__trt__GetVideoSourceConfigurations-----------------------------------\n");
    trt__GetVideoSourceConfigurationsResponse->__sizeConfigurations = 1;
    trt__GetVideoSourceConfigurationsResponse->Configurations =
            (struct tt__VideoSourceConfiguration *)soap_malloc(soap, sizeof(struct tt__VideoSourceConfiguration) * trt__GetVideoSourceConfigurationsResponse->__sizeConfigurations);
    memset(trt__GetVideoSourceConfigurationsResponse->Configurations, '\0', sizeof(struct tt__VideoSourceConfiguration) * trt__GetVideoSourceConfigurationsResponse->__sizeConfigurations);

    trt__GetVideoSourceConfigurationsResponse->Configurations[0].UseCount = 1;
    trt__GetVideoSourceConfigurationsResponse->Configurations[0].Name = (char*)soap_malloc(soap, sizeof(char) * 32);
    memset(trt__GetVideoSourceConfigurationsResponse->Configurations[0].Name, '\0', sizeof(char) * 32);
    strcpy(trt__GetVideoSourceConfigurationsResponse->Configurations[0].Name, "SourceConfig_Name");

    trt__GetVideoSourceConfigurationsResponse->Configurations[0].token = (char*)soap_malloc(soap, sizeof(char) * 32);
    memset(trt__GetVideoSourceConfigurationsResponse->Configurations[0].token, '\0', sizeof(char) * 32);
    strcpy(trt__GetVideoSourceConfigurationsResponse->Configurations[0].token, "000");

    trt__GetVideoSourceConfigurationsResponse->Configurations[0].SourceToken = (char*)soap_malloc(soap, sizeof(char) * 32);
    memset(trt__GetVideoSourceConfigurationsResponse->Configurations[0].SourceToken, '\0', sizeof(char) * 32);
    strcpy(trt__GetVideoSourceConfigurationsResponse->Configurations[0].SourceToken, "000");

    trt__GetVideoSourceConfigurationsResponse->Configurations[0].Bounds = (struct tt__IntRectangle *)soap_malloc(soap, sizeof(struct tt__IntRectangle));
    memset(trt__GetVideoSourceConfigurationsResponse->Configurations[0].Bounds, 0, sizeof(struct tt__IntRectangle));
    trt__GetVideoSourceConfigurationsResponse->Configurations[0].Bounds->x      = 0;
    trt__GetVideoSourceConfigurationsResponse->Configurations[0].Bounds->y      = 0;
    trt__GetVideoSourceConfigurationsResponse->Configurations[0].Bounds->width  = ONVIF_FRAME_WIDTH;
    trt__GetVideoSourceConfigurationsResponse->Configurations[0].Bounds->height = ONVIF_FRAME_HEIGHT;

    return 0;
}

/** Web service operation '__trt__GetVideoEncoderConfigurations' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __trt__GetVideoEncoderConfigurations(struct soap* soap, struct _trt__GetVideoEncoderConfigurations *trt__GetVideoEncoderConfigurations, struct _trt__GetVideoEncoderConfigurationsResponse *trt__GetVideoEncoderConfigurationsResponse)
{
    printf("----------------------------__trt__GetVideoEncoderConfigurations-----------------------------------\n");
    int size = 1;
    trt__GetVideoEncoderConfigurationsResponse->__sizeConfigurations = size;
    trt__GetVideoEncoderConfigurationsResponse->Configurations =
            (struct tt__VideoEncoderConfiguration *)soap_malloc(soap, sizeof(struct tt__VideoEncoderConfiguration) * size);
    memset(trt__GetVideoEncoderConfigurationsResponse->Configurations, '\0', sizeof(struct tt__VideoEncoderConfiguration) * size);

    //<VideoEncoderConfigurations>
    int i = 0;
    trt__GetVideoEncoderConfigurationsResponse->Configurations[i].Name = (char *)soap_malloc(soap, sizeof(char) * 32);
    memset(trt__GetVideoEncoderConfigurationsResponse->Configurations[i].Name, '\0', sizeof(char) * 32);
    trt__GetVideoEncoderConfigurationsResponse->Configurations[i].token = (char *)soap_malloc(soap, sizeof(char) * 32);
    memset(trt__GetVideoEncoderConfigurationsResponse->Configurations[i].token, '\0', sizeof(char) * 32);
        strcpy(trt__GetVideoEncoderConfigurationsResponse->Configurations[i].Name, "EncoderConfig_Name");
    strcpy(trt__GetVideoEncoderConfigurationsResponse->Configurations[i].token, "000");
    trt__GetVideoEncoderConfigurationsResponse->Configurations[i].UseCount = 1;
    trt__GetVideoEncoderConfigurationsResponse->Configurations[i].Quality = 1;
    trt__GetVideoEncoderConfigurationsResponse->Configurations[i].Encoding = tt__VideoEncoding__H264;   // JPEG = 0 , MPEG = 1, H264 = 2;
    //<Configurations><Resolution>
    trt__GetVideoEncoderConfigurationsResponse->Configurations[i].Resolution = (struct tt__VideoResolution *)soap_malloc(soap,sizeof(struct tt__VideoResolution));
    memset(trt__GetVideoEncoderConfigurationsResponse->Configurations[i].Resolution, 0 , sizeof(struct tt__VideoResolution));
    trt__GetVideoEncoderConfigurationsResponse->Configurations[i].Resolution->Width  = ONVIF_FRAME_WIDTH;
    trt__GetVideoEncoderConfigurationsResponse->Configurations[i].Resolution->Height = ONVIF_FRAME_HEIGHT;
    //<Configurations><RateControl>
    trt__GetVideoEncoderConfigurationsResponse->Configurations[i].RateControl = (struct tt__VideoRateControl *)soap_malloc(soap, sizeof(struct tt__VideoRateControl));
    memset(trt__GetVideoEncoderConfigurationsResponse->Configurations[i].RateControl, 0, sizeof(struct tt__VideoRateControl));
    trt__GetVideoEncoderConfigurationsResponse->Configurations[i].RateControl->FrameRateLimit   = 25;
    trt__GetVideoEncoderConfigurationsResponse->Configurations[i].RateControl->EncodingInterval = 1;
    trt__GetVideoEncoderConfigurationsResponse->Configurations[i].RateControl->BitrateLimit     = 1024;
    //<Configurations><H264>
    trt__GetVideoEncoderConfigurationsResponse->Configurations[i].H264 = (struct tt__H264Configuration *)soap_malloc(soap, sizeof(struct tt__H264Configuration));
    memset(trt__GetVideoEncoderConfigurationsResponse->Configurations[i].H264, 0, sizeof(struct tt__H264Configuration));
    trt__GetVideoEncoderConfigurationsResponse->Configurations[i].H264->GovLength  = 60;
    trt__GetVideoEncoderConfigurationsResponse->Configurations[i].H264->H264Profile = tt__H264Profile__High;

    //<Configuration><Multicast>
    trt__GetVideoEncoderConfigurationsResponse->Configurations[i].Multicast = (struct tt__MulticastConfiguration *)soap_malloc(soap, sizeof(struct tt__MulticastConfiguration));
    memset(trt__GetVideoEncoderConfigurationsResponse->Configurations[i].Multicast, 0, sizeof(struct tt__MulticastConfiguration));
    trt__GetVideoEncoderConfigurationsResponse->Configurations[i].Multicast->Address = (struct tt__IPAddress *)soap_malloc(soap, sizeof(struct tt__IPAddress));
    memset(trt__GetVideoEncoderConfigurationsResponse->Configurations[i].Multicast->Address, 0, sizeof(struct tt__IPAddress));
    trt__GetVideoEncoderConfigurationsResponse->Configurations[i].Multicast->Address->Type = tt__IPType__IPv4;
    trt__GetVideoEncoderConfigurationsResponse->Configurations[i].Multicast->Address->IPv4Address = (char *)soap_malloc(soap, sizeof(char) * 32);
    memset(trt__GetVideoEncoderConfigurationsResponse->Configurations[i].Multicast->Address->IPv4Address, '\0', sizeof(char) * 32);
    strcpy(trt__GetVideoEncoderConfigurationsResponse->Configurations[i].Multicast->Address->IPv4Address, "224.1.0.0");
    trt__GetVideoEncoderConfigurationsResponse->Configurations[i].Multicast->Port = 5000;
    trt__GetVideoEncoderConfigurationsResponse->Configurations[i].Multicast->TTL = 64;
    trt__GetVideoEncoderConfigurationsResponse->Configurations[i].Multicast->AutoStart = xsd__boolean__true_;
    //<Configuration><SessionTimeout>
    trt__GetVideoEncoderConfigurationsResponse->Configurations[i].SessionTimeout = ONVIF_TIME_OUT;

    return 0;
}

/** Web service operation '__trt__GetAudioSourceConfigurations' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __trt__GetAudioSourceConfigurations(struct soap* soap, struct _trt__GetAudioSourceConfigurations *trt__GetAudioSourceConfigurations, struct _trt__GetAudioSourceConfigurationsResponse *trt__GetAudioSourceConfigurationsResponse)
{
    return 0;
}

/** Web service operation '__trt__GetAudioEncoderConfigurations' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __trt__GetAudioEncoderConfigurations(struct soap* soap, struct _trt__GetAudioEncoderConfigurations *trt__GetAudioEncoderConfigurations, struct _trt__GetAudioEncoderConfigurationsResponse *trt__GetAudioEncoderConfigurationsResponse)
{
    return 0;
}

/** Web service operation '__trt__GetVideoAnalyticsConfigurations' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __trt__GetVideoAnalyticsConfigurations(struct soap* soap, struct _trt__GetVideoAnalyticsConfigurations *trt__GetVideoAnalyticsConfigurations, struct _trt__GetVideoAnalyticsConfigurationsResponse *trt__GetVideoAnalyticsConfigurationsResponse)
{
    return 0;
}

/** Web service operation '__trt__GetMetadataConfigurations' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __trt__GetMetadataConfigurations(struct soap* soap, struct _trt__GetMetadataConfigurations *trt__GetMetadataConfigurations, struct _trt__GetMetadataConfigurationsResponse *trt__GetMetadataConfigurationsResponse)
{
    return 0;
}

/** Web service operation '__trt__GetAudioOutputConfigurations' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __trt__GetAudioOutputConfigurations(struct soap* soap, struct _trt__GetAudioOutputConfigurations *trt__GetAudioOutputConfigurations, struct _trt__GetAudioOutputConfigurationsResponse *trt__GetAudioOutputConfigurationsResponse)
{
    return 0;
}

/** Web service operation '__trt__GetAudioDecoderConfigurations' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __trt__GetAudioDecoderConfigurations(struct soap* soap, struct _trt__GetAudioDecoderConfigurations *trt__GetAudioDecoderConfigurations, struct _trt__GetAudioDecoderConfigurationsResponse *trt__GetAudioDecoderConfigurationsResponse)
{
    return 0;
}

/** Web service operation '__trt__GetVideoSourceConfiguration' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __trt__GetVideoSourceConfiguration(struct soap* soap, struct _trt__GetVideoSourceConfiguration *trt__GetVideoSourceConfiguration, struct _trt__GetVideoSourceConfigurationResponse *trt__GetVideoSourceConfigurationResponse)
{
    printf("-----------------------------__trt__GetVideoSourceConfiguration-------------------------------------\n");
    trt__GetVideoSourceConfigurationResponse->Configuration = (struct tt__VideoSourceConfiguration *)soap_malloc(soap, sizeof(struct tt__VideoSourceConfiguration));
    memset(trt__GetVideoSourceConfigurationResponse->Configuration, '\0', sizeof(struct tt__VideoSourceConfiguration));

    trt__GetVideoSourceConfigurationResponse->Configuration->UseCount = 1;
    trt__GetVideoSourceConfigurationResponse->Configuration->Name = (char*)soap_malloc(soap, sizeof(char) * 32);
    memset(trt__GetVideoSourceConfigurationResponse->Configuration->Name, '\0', sizeof(char) * 32);
    trt__GetVideoSourceConfigurationResponse->Configuration->token = (char*)soap_malloc(soap, sizeof(char) * 32);
    memset(trt__GetVideoSourceConfigurationResponse->Configuration->token, '\0', sizeof(char) * 32);
        strcpy(trt__GetVideoSourceConfigurationResponse->Configuration->Name, "SourceConfig_Name");
    strcpy(trt__GetVideoSourceConfigurationResponse->Configuration->token, "000");

    trt__GetVideoSourceConfigurationResponse->Configuration->SourceToken = (char*)soap_malloc(soap, sizeof(char) * 32);
    memset(trt__GetVideoSourceConfigurationResponse->Configuration->SourceToken, '\0', sizeof(char) * 32);
    strcpy(trt__GetVideoSourceConfigurationResponse->Configuration->SourceToken, "000");

    trt__GetVideoSourceConfigurationResponse->Configuration->Bounds = (struct tt__IntRectangle *)soap_malloc(soap, sizeof(struct tt__IntRectangle));
    memset(trt__GetVideoSourceConfigurationResponse->Configuration->Bounds, 0, sizeof(struct tt__IntRectangle));
    trt__GetVideoSourceConfigurationResponse->Configuration->Bounds->x      = 0;
    trt__GetVideoSourceConfigurationResponse->Configuration->Bounds->y      = 0;
    trt__GetVideoSourceConfigurationResponse->Configuration->Bounds->width  = ONVIF_FRAME_WIDTH;
    trt__GetVideoSourceConfigurationResponse->Configuration->Bounds->height = ONVIF_FRAME_HEIGHT;

    return 0;
}

/** Web service operation '__trt__GetVideoEncoderConfiguration' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __trt__GetVideoEncoderConfiguration(struct soap* soap, struct _trt__GetVideoEncoderConfiguration *trt__GetVideoEncoderConfiguration, struct _trt__GetVideoEncoderConfigurationResponse *trt__GetVideoEncoderConfigurationResponse)
{
    printf("-----------------------------__trt__GetVideoEncoderConfiguration-------------------------------------\n");
    trt__GetVideoEncoderConfigurationResponse->Configuration = (struct tt__VideoEncoderConfiguration *)soap_malloc(soap, sizeof(struct tt__VideoEncoderConfiguration));
    memset(trt__GetVideoEncoderConfigurationResponse->Configuration, '\0', sizeof(struct tt__VideoEncoderConfiguration));

    //<VideoEncoderConfiguration>
    trt__GetVideoEncoderConfigurationResponse->Configuration->Name = (char *)soap_malloc(soap, sizeof(char) * 32);
    memset(trt__GetVideoEncoderConfigurationResponse->Configuration->Name, '\0', sizeof(char)* 32);
    strcpy(trt__GetVideoEncoderConfigurationResponse->Configuration->Name, "EncoderConfig_Name");
    trt__GetVideoEncoderConfigurationResponse->Configuration->token = (char *)soap_malloc(soap, sizeof(char) * 32);
    memset(trt__GetVideoEncoderConfigurationResponse->Configuration->token, '\0', sizeof(char) * 32);
    strcpy(trt__GetVideoEncoderConfigurationResponse->Configuration->token, "000");
    trt__GetVideoEncoderConfigurationResponse->Configuration->UseCount = 1;
    trt__GetVideoEncoderConfigurationResponse->Configuration->Quality = 1;
    //根据前端设备时间支持的编码格式选择对应的值，因为我测试的是设备只支持H264 ，所以选了2
    trt__GetVideoEncoderConfigurationResponse->Configuration->Encoding = tt__VideoEncoding__H264;   // JPEG = 0 , MPEG = 1, H264 = 2;
    //<Configuration><Resolution>
    trt__GetVideoEncoderConfigurationResponse->Configuration->Resolution = (struct tt__VideoResolution *)soap_malloc(soap,sizeof(struct tt__VideoResolution));
    memset(trt__GetVideoEncoderConfigurationResponse->Configuration->Resolution, 0 , sizeof(struct tt__VideoResolution));
    trt__GetVideoEncoderConfigurationResponse->Configuration->Resolution->Width  = ONVIF_FRAME_WIDTH;
    trt__GetVideoEncoderConfigurationResponse->Configuration->Resolution->Height = ONVIF_FRAME_HEIGHT;
    //<Configuration><RateControl>
    trt__GetVideoEncoderConfigurationResponse->Configuration->RateControl = (struct tt__VideoRateControl *)soap_malloc(soap, sizeof(struct tt__VideoRateControl));
    memset(trt__GetVideoEncoderConfigurationResponse->Configuration->RateControl, 0, sizeof(struct tt__VideoRateControl));
    trt__GetVideoEncoderConfigurationResponse->Configuration->RateControl->FrameRateLimit   = 25;
    trt__GetVideoEncoderConfigurationResponse->Configuration->RateControl->EncodingInterval = 1;
    trt__GetVideoEncoderConfigurationResponse->Configuration->RateControl->BitrateLimit     = 1024;
    //<Configuration><H264>
    trt__GetVideoEncoderConfigurationResponse->Configuration->H264 = (struct tt__H264Configuration *)soap_malloc(soap, sizeof(struct tt__H264Configuration));
    memset(trt__GetVideoEncoderConfigurationResponse->Configuration->H264, 0, sizeof(struct tt__H264Configuration));
    trt__GetVideoEncoderConfigurationResponse->Configuration->H264->GovLength  = 60;
    trt__GetVideoEncoderConfigurationResponse->Configuration->H264->H264Profile = tt__H264Profile__High;

    //<Configuration><Multicast>
    trt__GetVideoEncoderConfigurationResponse->Configuration->Multicast = (struct tt__MulticastConfiguration *)soap_malloc(soap, sizeof(struct tt__MulticastConfiguration));
    memset(trt__GetVideoEncoderConfigurationResponse->Configuration->Multicast, 0, sizeof(struct tt__MulticastConfiguration));
    trt__GetVideoEncoderConfigurationResponse->Configuration->Multicast->Address = (struct tt__IPAddress *)soap_malloc(soap, sizeof(struct tt__IPAddress));
    memset(trt__GetVideoEncoderConfigurationResponse->Configuration->Multicast->Address, 0, sizeof(struct tt__IPAddress));
    trt__GetVideoEncoderConfigurationResponse->Configuration->Multicast->Address->Type = tt__IPType__IPv4;
    trt__GetVideoEncoderConfigurationResponse->Configuration->Multicast->Address->IPv4Address = (char *)soap_malloc(soap, sizeof(char) * 32);
    memset(trt__GetVideoEncoderConfigurationResponse->Configuration->Multicast->Address->IPv4Address, '\0', sizeof(char) * 32);
    strcpy(trt__GetVideoEncoderConfigurationResponse->Configuration->Multicast->Address->IPv4Address, "224.1.0.0");
    trt__GetVideoEncoderConfigurationResponse->Configuration->Multicast->Port = 5000;
    trt__GetVideoEncoderConfigurationResponse->Configuration->Multicast->TTL = 64;
    trt__GetVideoEncoderConfigurationResponse->Configuration->Multicast->AutoStart = xsd__boolean__true_;
    //<Configuration><SessionTimeout>
    trt__GetVideoEncoderConfigurationResponse->Configuration->SessionTimeout = ONVIF_TIME_OUT;

    return SOAP_OK;
}

/** Web service operation '__trt__GetAudioSourceConfiguration' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __trt__GetAudioSourceConfiguration(struct soap* soap, struct _trt__GetAudioSourceConfiguration *trt__GetAudioSourceConfiguration, struct _trt__GetAudioSourceConfigurationResponse *trt__GetAudioSourceConfigurationResponse)
{
    return 0;
}

/** Web service operation '__trt__GetAudioEncoderConfiguration' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __trt__GetAudioEncoderConfiguration(struct soap* soap, struct _trt__GetAudioEncoderConfiguration *trt__GetAudioEncoderConfiguration, struct _trt__GetAudioEncoderConfigurationResponse *trt__GetAudioEncoderConfigurationResponse)
{
    return 0;
}

/** Web service operation '__trt__GetVideoAnalyticsConfiguration' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __trt__GetVideoAnalyticsConfiguration(struct soap* soap, struct _trt__GetVideoAnalyticsConfiguration *trt__GetVideoAnalyticsConfiguration, struct _trt__GetVideoAnalyticsConfigurationResponse *trt__GetVideoAnalyticsConfigurationResponse)
{
    return 0;
}

/** Web service operation '__trt__GetMetadataConfiguration' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __trt__GetMetadataConfiguration(struct soap* soap, struct _trt__GetMetadataConfiguration *trt__GetMetadataConfiguration, struct _trt__GetMetadataConfigurationResponse *trt__GetMetadataConfigurationResponse)
{
    return 0;
}

/** Web service operation '__trt__GetAudioOutputConfiguration' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __trt__GetAudioOutputConfiguration(struct soap* soap, struct _trt__GetAudioOutputConfiguration *trt__GetAudioOutputConfiguration, struct _trt__GetAudioOutputConfigurationResponse *trt__GetAudioOutputConfigurationResponse)
{
    return 0;
}

/** Web service operation '__trt__GetAudioDecoderConfiguration' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __trt__GetAudioDecoderConfiguration(struct soap* soap, struct _trt__GetAudioDecoderConfiguration *trt__GetAudioDecoderConfiguration, struct _trt__GetAudioDecoderConfigurationResponse *trt__GetAudioDecoderConfigurationResponse)
{
    return 0;
}

/** Web service operation '__trt__GetCompatibleVideoEncoderConfigurations' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __trt__GetCompatibleVideoEncoderConfigurations(struct soap* soap, struct _trt__GetCompatibleVideoEncoderConfigurations *trt__GetCompatibleVideoEncoderConfigurations, struct _trt__GetCompatibleVideoEncoderConfigurationsResponse *trt__GetCompatibleVideoEncoderConfigurationsResponse)
{
    return 0;
}

/** Web service operation '__trt__GetCompatibleVideoSourceConfigurations' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __trt__GetCompatibleVideoSourceConfigurations(struct soap* soap, struct _trt__GetCompatibleVideoSourceConfigurations *trt__GetCompatibleVideoSourceConfigurations, struct _trt__GetCompatibleVideoSourceConfigurationsResponse *trt__GetCompatibleVideoSourceConfigurationsResponse)
{
    return 0;
}

/** Web service operation '__trt__GetCompatibleAudioEncoderConfigurations' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __trt__GetCompatibleAudioEncoderConfigurations(struct soap* soap, struct _trt__GetCompatibleAudioEncoderConfigurations *trt__GetCompatibleAudioEncoderConfigurations, struct _trt__GetCompatibleAudioEncoderConfigurationsResponse *trt__GetCompatibleAudioEncoderConfigurationsResponse)
{
    return 0;
}

/** Web service operation '__trt__GetCompatibleAudioSourceConfigurations' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __trt__GetCompatibleAudioSourceConfigurations(struct soap* soap, struct _trt__GetCompatibleAudioSourceConfigurations *trt__GetCompatibleAudioSourceConfigurations, struct _trt__GetCompatibleAudioSourceConfigurationsResponse *trt__GetCompatibleAudioSourceConfigurationsResponse)
{
    return 0;
}

/** Web service operation '__trt__GetCompatibleVideoAnalyticsConfigurations' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __trt__GetCompatibleVideoAnalyticsConfigurations(struct soap* soap, struct _trt__GetCompatibleVideoAnalyticsConfigurations *trt__GetCompatibleVideoAnalyticsConfigurations, struct _trt__GetCompatibleVideoAnalyticsConfigurationsResponse *trt__GetCompatibleVideoAnalyticsConfigurationsResponse)
{
    return 0;
}

/** Web service operation '__trt__GetCompatibleMetadataConfigurations' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __trt__GetCompatibleMetadataConfigurations(struct soap* soap, struct _trt__GetCompatibleMetadataConfigurations *trt__GetCompatibleMetadataConfigurations, struct _trt__GetCompatibleMetadataConfigurationsResponse *trt__GetCompatibleMetadataConfigurationsResponse)
{
    return 0;
}

/** Web service operation '__trt__GetCompatibleAudioOutputConfigurations' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __trt__GetCompatibleAudioOutputConfigurations(struct soap* soap, struct _trt__GetCompatibleAudioOutputConfigurations *trt__GetCompatibleAudioOutputConfigurations, struct _trt__GetCompatibleAudioOutputConfigurationsResponse *trt__GetCompatibleAudioOutputConfigurationsResponse)
{
    return 0;
}

/** Web service operation '__trt__GetCompatibleAudioDecoderConfigurations' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __trt__GetCompatibleAudioDecoderConfigurations(struct soap* soap, struct _trt__GetCompatibleAudioDecoderConfigurations *trt__GetCompatibleAudioDecoderConfigurations, struct _trt__GetCompatibleAudioDecoderConfigurationsResponse *trt__GetCompatibleAudioDecoderConfigurationsResponse)
{
    return 0;
}

/** Web service operation '__trt__SetVideoSourceConfiguration' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __trt__SetVideoSourceConfiguration(struct soap* soap, struct _trt__SetVideoSourceConfiguration *trt__SetVideoSourceConfiguration, struct _trt__SetVideoSourceConfigurationResponse *trt__SetVideoSourceConfigurationResponse)
{
    return 0;
}

/** Web service operation '__trt__SetVideoEncoderConfiguration' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __trt__SetVideoEncoderConfiguration(struct soap* soap, struct _trt__SetVideoEncoderConfiguration *trt__SetVideoEncoderConfiguration, struct _trt__SetVideoEncoderConfigurationResponse *trt__SetVideoEncoderConfigurationResponse)
{
   /* if (NULL != trt__SetVideoEncoderConfiguration->Configuration &&
        tt__VideoEncoding__H264 == trt__SetVideoEncoderConfiguration->Configuration->Encoding) {

        if (NULL != trt__SetVideoEncoderConfiguration->Configuration->Resolution) {
            if (0 == strcmp(trt__SetVideoEncoderConfiguration->Configuration->token, "000")) {
                stVideoCfg.u32MainWidth = trt__SetVideoEncoderConfiguration->Configuration->Resolution->Width;
                stVideoCfg.u32MainHeight = trt__SetVideoEncoderConfiguration->Configuration->Resolution->Height;
            }
            else {
                stVideoCfg.u32SubWidth = trt__SetVideoEncoderConfiguration->Configuration->Resolution->Width;
                stVideoCfg.u32SubHeight = trt__SetVideoEncoderConfiguration->Configuration->Resolution->Height;
            }
        }

    }*/

    return 0;
}

/** Web service operation '__trt__SetAudioSourceConfiguration' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __trt__SetAudioSourceConfiguration(struct soap* soap, struct _trt__SetAudioSourceConfiguration *trt__SetAudioSourceConfiguration, struct _trt__SetAudioSourceConfigurationResponse *trt__SetAudioSourceConfigurationResponse)
{
    return 0;
}

/** Web service operation '__trt__SetAudioEncoderConfiguration' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __trt__SetAudioEncoderConfiguration(struct soap* soap, struct _trt__SetAudioEncoderConfiguration *trt__SetAudioEncoderConfiguration, struct _trt__SetAudioEncoderConfigurationResponse *trt__SetAudioEncoderConfigurationResponse)
{
    return 0;
}

/** Web service operation '__trt__SetVideoAnalyticsConfiguration' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __trt__SetVideoAnalyticsConfiguration(struct soap* soap, struct _trt__SetVideoAnalyticsConfiguration *trt__SetVideoAnalyticsConfiguration, struct _trt__SetVideoAnalyticsConfigurationResponse *trt__SetVideoAnalyticsConfigurationResponse)
{
    return 0;
}

/** Web service operation '__trt__SetMetadataConfiguration' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __trt__SetMetadataConfiguration(struct soap* soap, struct _trt__SetMetadataConfiguration *trt__SetMetadataConfiguration, struct _trt__SetMetadataConfigurationResponse *trt__SetMetadataConfigurationResponse)
{
    return 0;
}

/** Web service operation '__trt__SetAudioOutputConfiguration' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __trt__SetAudioOutputConfiguration(struct soap* soap, struct _trt__SetAudioOutputConfiguration *trt__SetAudioOutputConfiguration, struct _trt__SetAudioOutputConfigurationResponse *trt__SetAudioOutputConfigurationResponse)
{
    return 0;
}

/** Web service operation '__trt__SetAudioDecoderConfiguration' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __trt__SetAudioDecoderConfiguration(struct soap* soap, struct _trt__SetAudioDecoderConfiguration *trt__SetAudioDecoderConfiguration, struct _trt__SetAudioDecoderConfigurationResponse *trt__SetAudioDecoderConfigurationResponse)
{
    return 0;
}

/** Web service operation '__trt__GetVideoSourceConfigurationOptions' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __trt__GetVideoSourceConfigurationOptions(struct soap* soap, struct _trt__GetVideoSourceConfigurationOptions *trt__GetVideoSourceConfigurationOptions, struct _trt__GetVideoSourceConfigurationOptionsResponse *trt__GetVideoSourceConfigurationOptionsResponse)
{
    return 0;
}

/** Web service operation '__trt__GetVideoEncoderConfigurationOptions' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __trt__GetVideoEncoderConfigurationOptions(struct soap* soap, struct _trt__GetVideoEncoderConfigurationOptions *trt__GetVideoEncoderConfigurationOptions, struct _trt__GetVideoEncoderConfigurationOptionsResponse *trt__GetVideoEncoderConfigurationOptionsResponse)
{
    return 0;
}

/** Web service operation '__trt__GetAudioSourceConfigurationOptions' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __trt__GetAudioSourceConfigurationOptions(struct soap* soap, struct _trt__GetAudioSourceConfigurationOptions *trt__GetAudioSourceConfigurationOptions, struct _trt__GetAudioSourceConfigurationOptionsResponse *trt__GetAudioSourceConfigurationOptionsResponse)
{
    return 0;
}

/** Web service operation '__trt__GetAudioEncoderConfigurationOptions' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __trt__GetAudioEncoderConfigurationOptions(struct soap* soap, struct _trt__GetAudioEncoderConfigurationOptions *trt__GetAudioEncoderConfigurationOptions, struct _trt__GetAudioEncoderConfigurationOptionsResponse *trt__GetAudioEncoderConfigurationOptionsResponse)
{
    return 0;
}

/** Web service operation '__trt__GetMetadataConfigurationOptions' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __trt__GetMetadataConfigurationOptions(struct soap* soap, struct _trt__GetMetadataConfigurationOptions *trt__GetMetadataConfigurationOptions, struct _trt__GetMetadataConfigurationOptionsResponse *trt__GetMetadataConfigurationOptionsResponse)
{
    return 0;
}

/** Web service operation '__trt__GetAudioOutputConfigurationOptions' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __trt__GetAudioOutputConfigurationOptions(struct soap* soap, struct _trt__GetAudioOutputConfigurationOptions *trt__GetAudioOutputConfigurationOptions, struct _trt__GetAudioOutputConfigurationOptionsResponse *trt__GetAudioOutputConfigurationOptionsResponse)
{
    return 0;
}

/** Web service operation '__trt__GetAudioDecoderConfigurationOptions' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __trt__GetAudioDecoderConfigurationOptions(struct soap* soap, struct _trt__GetAudioDecoderConfigurationOptions *trt__GetAudioDecoderConfigurationOptions, struct _trt__GetAudioDecoderConfigurationOptionsResponse *trt__GetAudioDecoderConfigurationOptionsResponse)
{
    return 0;
}

/** Web service operation '__trt__GetGuaranteedNumberOfVideoEncoderInstances' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __trt__GetGuaranteedNumberOfVideoEncoderInstances(struct soap* soap, struct _trt__GetGuaranteedNumberOfVideoEncoderInstances *trt__GetGuaranteedNumberOfVideoEncoderInstances, struct _trt__GetGuaranteedNumberOfVideoEncoderInstancesResponse *trt__GetGuaranteedNumberOfVideoEncoderInstancesResponse)
{
    return 0;
}

/** Web service operation '__trt__GetStreamUri' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __trt__GetStreamUri(struct soap* soap, struct _trt__GetStreamUri *trt__GetStreamUri, struct _trt__GetStreamUriResponse *trt__GetStreamUriResponse)
{
    printf("--------------------------__trt__GetStreamUri---------------------------\n");

    trt__GetStreamUriResponse->MediaUri = (struct tt__MediaUri *)soap_malloc(soap, sizeof(struct tt__MediaUri));
    memset(trt__GetStreamUriResponse->MediaUri, 0, sizeof(struct tt__MediaUri));

    trt__GetStreamUriResponse->MediaUri->Uri = (char *)soap_malloc(soap, sizeof(char) * 100);
    memset(trt__GetStreamUriResponse->MediaUri->Uri, '\0', sizeof(char) * 100);
    //根据各个设备的rtsp协议的uri不同填写对应的值

    char IP[20] = {0};
    getLocalIP(IP, 20);
    sprintf(trt__GetStreamUriResponse->MediaUri->Uri, "rtsp://%s/live", IP);
    trt__GetStreamUriResponse->MediaUri->InvalidAfterConnect = xsd__boolean__true_;
    trt__GetStreamUriResponse->MediaUri->InvalidAfterReboot  = xsd__boolean__true_;
    //超时时间
    trt__GetStreamUriResponse->MediaUri->Timeout = 0;

    return 0;
}

/** Web service operation '__trt__StartMulticastStreaming' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __trt__StartMulticastStreaming(struct soap* soap, struct _trt__StartMulticastStreaming *trt__StartMulticastStreaming, struct _trt__StartMulticastStreamingResponse *trt__StartMulticastStreamingResponse)
{
    return 0;
}

/** Web service operation '__trt__StopMulticastStreaming' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __trt__StopMulticastStreaming(struct soap* soap, struct _trt__StopMulticastStreaming *trt__StopMulticastStreaming, struct _trt__StopMulticastStreamingResponse *trt__StopMulticastStreamingResponse)
{
    return 0;
}

/** Web service operation '__trt__SetSynchronizationPoint' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __trt__SetSynchronizationPoint(struct soap* soap, struct _trt__SetSynchronizationPoint *trt__SetSynchronizationPoint, struct _trt__SetSynchronizationPointResponse *trt__SetSynchronizationPointResponse)
{
    return 0;
}

/** Web service operation '__trt__GetSnapshotUri' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __trt__GetSnapshotUri(struct soap* soap, struct _trt__GetSnapshotUri *trt__GetSnapshotUri, struct _trt__GetSnapshotUriResponse *trt__GetSnapshotUriResponse)
{
    printf("-------------------__trt__GetSnapshotUri------------------------\n");

    trt__GetSnapshotUriResponse->MediaUri = (struct tt__MediaUri *)soap_malloc(soap, sizeof(struct tt__MediaUri));
    memset(trt__GetSnapshotUriResponse->MediaUri, 0, sizeof(struct tt__MediaUri));

    char IP[20] = {0};
    getLocalIP(IP, 20);
    trt__GetSnapshotUriResponse->MediaUri->Uri = (char *)soap_malloc(soap, sizeof(char) * 100);
    memset(trt__GetSnapshotUriResponse->MediaUri->Uri, 0, sizeof(char) * 100);
    sprintf(trt__GetSnapshotUriResponse->MediaUri->Uri, "http://%s:%d/snap0.jpeg", IP, 80);
    trt__GetSnapshotUriResponse->MediaUri->InvalidAfterConnect = xsd__boolean__false_;
    trt__GetSnapshotUriResponse->MediaUri->InvalidAfterReboot = xsd__boolean__false_;
    trt__GetSnapshotUriResponse->MediaUri->Timeout = ONVIF_TIME_OUT;

    return 0;
}

/** Web service operation '__trt__GetVideoSourceModes' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __trt__GetVideoSourceModes(struct soap* soap, struct _trt__GetVideoSourceModes *trt__GetVideoSourceModes, struct _trt__GetVideoSourceModesResponse *trt__GetVideoSourceModesResponse)
{
    return 0;
}

/** Web service operation '__trt__SetVideoSourceMode' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __trt__SetVideoSourceMode(struct soap* soap, struct _trt__SetVideoSourceMode *trt__SetVideoSourceMode, struct _trt__SetVideoSourceModeResponse *trt__SetVideoSourceModeResponse)
{
    return 0;
}

/** Web service operation '__trt__GetOSDs' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __trt__GetOSDs(struct soap* soap, struct _trt__GetOSDs *trt__GetOSDs, struct _trt__GetOSDsResponse *trt__GetOSDsResponse)
{
    return 0;
}

/** Web service operation '__trt__GetOSD' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __trt__GetOSD(struct soap* soap, struct _trt__GetOSD *trt__GetOSD, struct _trt__GetOSDResponse *trt__GetOSDResponse)
{
    return 0;
}

/** Web service operation '__trt__GetOSDOptions' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __trt__GetOSDOptions(struct soap* soap, struct _trt__GetOSDOptions *trt__GetOSDOptions, struct _trt__GetOSDOptionsResponse *trt__GetOSDOptionsResponse)
{
    return 0;
}

/** Web service operation '__trt__SetOSD' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __trt__SetOSD(struct soap* soap, struct _trt__SetOSD *trt__SetOSD, struct _trt__SetOSDResponse *trt__SetOSDResponse)
{
    return 0;
}

/** Web service operation '__trt__CreateOSD' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __trt__CreateOSD(struct soap* soap, struct _trt__CreateOSD *trt__CreateOSD, struct _trt__CreateOSDResponse *trt__CreateOSDResponse)
{
    return 0;
}

/** Web service operation '__trt__DeleteOSD' (returns SOAP_OK or error code) */
SOAP_FMAC5 int SOAP_FMAC6 __trt__DeleteOSD(struct soap* soap, struct _trt__DeleteOSD *trt__DeleteOSD, struct _trt__DeleteOSDResponse *trt__DeleteOSDResponse)
{
    return 0;
}


