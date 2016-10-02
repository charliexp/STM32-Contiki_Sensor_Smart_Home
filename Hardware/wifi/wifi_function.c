#include "wifi_function.h"
#include "wifi_config.h"
#include "bsp_gpio.h"
#include "bsp_usart1.h"
#include "bsp_usart2.h"
#include <string.h> 
#include <stdio.h>  
#include <stdbool.h>



/*
 * ��������ESP8266_Rst
 * ����  ������WF-ESP8266ģ��
 * ����  ����
 * ����  : ��
 * ����  ����ESP8266_AT_Test����
 */
void ESP8266_Rst(void)
{

//    ESP8266_Cmd("AT+RST", "OK", "ready", 2500);   	

    ESP8266_RST_LOW_LEVEL();
    Delay_ms(100); 
    ESP8266_RST_HIGH_LEVEL();
    Delay_ms(2000); 
}


/*
 * ��������ESP8266_AT_Test
 * ����  ����WF-ESP8266ģ�����AT��������
 * ����  ����
 * ����  : ��
 * ����  �����ⲿ����
 */
void ESP8266_AT_Test(void)
{
	ESP8266_RST_HIGH_LEVEL();
	Delay_ms(2000); 
	while (!ESP8266_Cmd ("AT", "OK", NULL, 250))
    {
        ESP8266_Rst();
    }
}


/*
 * ��������ESP8266_Cmd
 * ����  ����WF-ESP8266ģ�鷢��ATָ��
 * ����  ��cmd�������͵�ָ��
 *         reply1��reply2���ڴ�����Ӧ��ΪNULL��������Ӧ������Ϊ���߼���ϵ
 *         waittime���ȴ���Ӧ��ʱ��
 * ����  : 1��ָ��ͳɹ�
 *         0��ָ���ʧ��
 * ����  �����ⲿ����
 */
bool ESP8266_Cmd (char * cmd, char * reply1, char * reply2, u32 waittime)
{    
	strEsp8266_Fram_Record .InfBit .FramLength = 0;               //���¿�ʼ�����µ����ݰ�

	ESP8266_Usart("%s\r\n", cmd);

	if((reply1 == 0) &&(reply2 == 0))                      //����Ҫ��������
		return true;
	
	Delay_ms(waittime);                 //��ʱ
	
	strEsp8266_Fram_Record .Data_RX_BUF [ strEsp8266_Fram_Record .InfBit .FramLength ]  = '\0';

  
	if((reply1 != 0) &&(reply2 != 0))
		return((bool) strstr(strEsp8266_Fram_Record .Data_RX_BUF, reply1) ||(bool) strstr(strEsp8266_Fram_Record .Data_RX_BUF, reply2)); 
 	
	else if(reply1 != 0)
		return((bool) strstr(strEsp8266_Fram_Record .Data_RX_BUF, reply1));
	
	else
		return((bool) strstr(strEsp8266_Fram_Record .Data_RX_BUF, reply2));
	

}


/*
 * ��������ESP8266_Net_Mode_Choose
 * ����  ��ѡ��WF-ESP8266ģ��Ĺ���ģʽ
 * ����  ��enumMode������ģʽ
 * ����  : 1��ѡ��ɹ�
 *         0��ѡ��ʧ��
 * ����  �����ⲿ����
 */
bool ESP8266_Net_Mode_Choose(ENUM_Net_ModeTypeDef enumMode)
{
    switch(enumMode)
    {
        case STA:
            return ESP8266_Cmd("AT+CWMODE=1", "OK", "no change", 250); 

        case AP:
            return ESP8266_Cmd("AT+CWMODE=2", "OK", "no change", 250); 

        case STA_AP:
            return ESP8266_Cmd("AT+CWMODE=3", "OK", "no change", 250); 

        default:
            return false;
    }
	
}


/*
 * ��������ESP8266_JoinAP
 * ����  ��WF-ESP8266ģ�������ⲿWiFi
 * ����  ��pSSID��WiFi�����ַ���
 *       ��pPassWord��WiFi�����ַ���
 * ����  : 1�����ӳɹ�
 *         0������ʧ��
 * ����  �����ⲿ����
 */
bool ESP8266_JoinAP(char * pSSID, char * pPassWord)
{
	char cCmd [120];
	sprintf(cCmd, "AT+CWJAP=\"%s\",\"%s\"", pSSID, pPassWord);
	return ESP8266_Cmd(cCmd, "OK", NULL, 1300);
}

/*
 * ��������ESP8266_STAJoinTCP
 * ����  ��WF-ESP8266ģ������WiFi������tcp server
 * ����  ��ip��tcp server��ַ
 *       ��port��tcp server �˿�
 * ����  : 1�����ӳɹ�
 *         0������ʧ��
 * ����  �����ⲿ����
 */
bool ESP8266_STAJoinTCP(char * ip, char * port)
{
	char cCmd [120];

	sprintf(cCmd, "AT+CIPSTART=\"TCP\",\"%s\",%s", ip, port);
	
	return ESP8266_Cmd(cCmd, "OK", "ALREADY CONNECTED", 1000);
	
}

/*
 * ��������ESP8266_BuildAP
 * ����  ��WF-ESP8266ģ�鴴��WiFi�ȵ�
 * ����  ��pSSID��WiFi�����ַ���
 *       ��pPassWord��WiFi�����ַ���
 *       ��enunPsdMode��WiFi���ܷ�ʽ�����ַ���
 * ����  : 1�������ɹ�
 *         0������ʧ��
 * ����  �����ⲿ����
 */
bool ESP8266_BuildAP(char * pSSID, char * pPassWord, char * enunPsdMode)
{
	char cCmd [120];

	sprintf(cCmd, "AT+CWSAP=\"%s\",\"%s\",1,%s", pSSID, pPassWord, enunPsdMode);
	
	return ESP8266_Cmd(cCmd, "OK", 0, 1000);
	
}


/*
 * ��������ESP8266_Enable_MultipleId
 * ����  ��WF-ESP8266ģ������������
 * ����  ��enumEnUnvarnishTx�������Ƿ������
 * ����  : 1�����óɹ�
 *         0������ʧ��
 * ����  �����ⲿ����
 */
bool ESP8266_Enable_MultipleId(FunctionalState enumEnUnvarnishTx)
{
	char cStr [20];
	
	sprintf(cStr, "AT+CIPMUX=%d",(enumEnUnvarnishTx ? 1 : 0));
	
	return ESP8266_Cmd(cStr, "OK", 0, 500);
	
}


/*
 * ��������ESP8266_Link_Server
 * ����  ��WF-ESP8266ģ�������ⲿ������
 * ����  ��enumE������Э��
 *       ��ip��������IP�ַ���
 *       ��ComNum���������˿��ַ���
 *       ��id��ģ�����ӷ�������ID
 * ����  : 1�����ӳɹ�
 *         0������ʧ��
 * ����  �����ⲿ����
 */
bool ESP8266_Link_Server(ENUM_NetPro_TypeDef enumE, char * ip, char * ComNum, ENUM_ID_NO_TypeDef id)
{
    char cStr [100] = { 0 }, cCmd [120];

    switch( enumE)
    {
        case enumTCP:
            sprintf(cStr, "\"%s\",\"%s\",%s", "TCP", ip, ComNum);
            break;

        case enumUDP:
            sprintf(cStr, "\"%s\",\"%s\",%s", "UDP", ip, ComNum);
            break;

        default:
            break;
    }

    if(id < 5)
        sprintf(cCmd, "AT+CIPSTART=%d,%s", id, cStr);

    else
        sprintf(cCmd, "AT+CIPSTART=%s", cStr);

    return ESP8266_Cmd(cCmd, "OK", "ALREAY CONNECT", 500);
	
}
//���� tcp�ĺ���
void ESP8266_linkTCP_join(void)
{
	while(!ESP8266_STAJoinTCP(TCP_SERVER_IPADDRESS,TCP_SERVER_PORT));
}

/*
 * ��������ESP8266_StartOrShutServer
 * ����  ��WF-ESP8266ģ�鿪����رշ�����ģʽ
 * ����  ��enumMode������/�ر�
 *       ��pPortNum���������˿ں��ַ���
 *       ��pTimeOver����������ʱʱ���ַ�������λ����
 * ����  : 1�������ɹ�
 *         0������ʧ��
 * ����  �����ⲿ����
 */
bool ESP8266_StartOrShutServer(FunctionalState enumMode, char * pPortNum, char * pTimeOver)
{
	char cCmd1 [120], cCmd2 [120];

	if(enumMode)
	{
		sprintf(cCmd1, "AT+CIPSERVER=%d,%s", 1, pPortNum);
		
		sprintf(cCmd2, "AT+CIPSTO=%s", pTimeOver);

		return(ESP8266_Cmd(cCmd1, "OK", 0, 500) && ESP8266_Cmd(cCmd2, "OK", 0, 500));
	}
	
	else
	{
		sprintf(cCmd1, "AT+CIPSERVER=%d,%s", 0, pPortNum);

		return ESP8266_Cmd(cCmd1, "OK", 0, 500);
	}
	
}


/*
 * ��������ESP8266_TransparentTransmission
 * ����  ������WF-ESP8266ģ�����͸������
 * ����  ����
 * ����  : 1�����óɹ�
 *         0������ʧ��
 * ����  �����ⲿ����
 */
bool ESP8266_TransparentTransmission(void)
{
	return (ESP8266_Cmd("AT+CIPMODE=1", "OK", "Link is builded", 250) && ESP8266_Cmd("AT+CIPSEND", "\r\n", ">", 250));
}


/*
 * ��������ESP8266_SendString
 * ����  ��WF-ESP8266ģ�鷢���ַ���
 * ����  ��enumEnUnvarnishTx�������Ƿ���ʹ����͸��ģʽ
 *       ��pStr��Ҫ���͵��ַ���
 *       ��ulStrLength��Ҫ���͵��ַ������ֽ���
 *       ��ucId���ĸ�ID���͵��ַ���
 * ����  : 1�����ͳɹ�
 *         0������ʧ��
 * ����  �����ⲿ����

 //	ESP8266_SendString(DISABLE, cStrInput, ul,(ENUM_ID_NO_TypeDef) uc);
 */
bool ESP8266_SendString(FunctionalState enumEnUnvarnishTx, char * pStr, u32 ulStrLength, ENUM_ID_NO_TypeDef ucId)
{
	char cStr [20];
	bool bRet = false;
		
	if(enumEnUnvarnishTx)
		ESP8266_Usart("%s", pStr);

	
	else
	{
		if(ucId < 5)
			sprintf(cStr, "AT+CIPSEND=%d,%d", ucId, ulStrLength + 2);

		else
			sprintf(cStr, "AT+CIPSEND=%d", ulStrLength + 2);
		
		ESP8266_Cmd(cStr, "> ", 0, 1000);

		bRet = ESP8266_Cmd(pStr, "SEND OK", 0, 1000);
  }
	
	return bRet;

}
void ESP8266_WIFIAP_join(void)
{
    uint8_t tryCount=0;
	while(!ESP8266_JoinAP(WIFI_SSID_NAME,WIFI_PASSWORD))printf("%d\r\n",tryCount++);;	
}


/*
 * ��������ESP8266_ReceiveString
 * ����  ��WF-ESP8266ģ������ַ���
 * ����  ��enumEnUnvarnishTx�������Ƿ���ʹ����͸��ģʽ
 * ����  : ���յ����ַ����׵�ַ
 * ����  �����ⲿ����
 */
char * ESP8266_ReceiveString(FunctionalState enumEnUnvarnishTx)
{
	char * pRecStr = 0;
	
	strEsp8266_Fram_Record .InfBit .FramLength = 0;
	strEsp8266_Fram_Record .InfBit .FramFinishFlag = 0;
	while(! strEsp8266_Fram_Record .InfBit .FramFinishFlag);
	strEsp8266_Fram_Record .Data_RX_BUF [ strEsp8266_Fram_Record .InfBit .FramLength ] = '\0';
	
	if(enumEnUnvarnishTx)
	{
		if(strstr(strEsp8266_Fram_Record .Data_RX_BUF, ">"))
			pRecStr = strEsp8266_Fram_Record .Data_RX_BUF;

	}
	
	else 
	{
		if(strstr(strEsp8266_Fram_Record .Data_RX_BUF, "+IPD"))
			pRecStr = strEsp8266_Fram_Record .Data_RX_BUF;

	}

	return pRecStr;
}

/*
 * ��������ESP8266_STA_TCP_Client
 * ����  ��WF-ESP8266ģ�����STA TCP Clien����
 * ����  ����
 * ����  : ��
 * ����  �����ⲿ����
 */
void ESP8266_STA_TCP_Client(void)
{
	IS_WIFI_LOG_DEBUG && PC_Usart("\r\nESP8266 WiFi Module Start.\r\n");     //��ӡ����������ʾ��Ϣ
    IS_WIFI_LOG_DEBUG && PC_Usart("Try to enter command mode.\r\n");
    do
    {
        ESP8266_Usart("+++");
        Delay_ms (50) ;
    }while(!ESP8266_Cmd ("AT", "OK", NULL, 100));//����AT����ģʽ
    IS_WIFI_LOG_DEBUG && PC_Usart("AT command is OK.\r\n");
    
    if(ESP8266_Cmd ("AT+CIFSR","+CIFSR:STAIP,\"0.0.0.0\"",NULL,250))
    {
        PC_Usart("Not connected wifi\r\n");
        while(!ESP8266_Net_Mode_Choose(STA));
        IS_WIFI_LOG_DEBUG && PC_Usart("Try to connect wifi with SSID:%s.\r\n",WIFI_SSID_NAME);
        ESP8266_WIFIAP_join();
        IS_WIFI_LOG_DEBUG && PC_Usart("Linked Wifi.\r\n");
        IS_WIFI_LOG_DEBUG && PC_Usart("Config module into TCP Client\r\n");
        ESP8266_Enable_MultipleId(DISABLE);
         
    }
    IS_WIFI_LOG_DEBUG && PC_Usart("Try to connect TCP Server\r\n");
    ESP8266_linkTCP_join();//����  tcp �������ӵĺ���
    IS_WIFI_LOG_DEBUG && PC_Usart("Connected TCP Server\r\n");
    //�����������  ����͸��ģʽ
    IS_WIFI_LOG_DEBUG && PC_Usart("Try to enter Transparent Transmission\r\n");
    while(!ESP8266_TransparentTransmission());
    IS_WIFI_LOG_DEBUG && PC_Usart("Changed into Transparent Transmission\r\n");
}


/*
 * ��������ESP8266_AP_TCP_Server
 * ����  ��WF-ESP8266ģ�����AP TCP Server����
 * ����  ����
 * ����  : ��
 * ����  �����ⲿ����
 */
void ESP8266_AP_TCP_Server(void)
{
	char cStrInput [100] = { 0 }, * pStrDelimiter [3], * pBuf, * pStr;
	u8 uc = 0;
 	 u32 ul = 0;


	ESP8266_AT_Test ();
	
	ESP8266_Net_Mode_Choose(AP);


	PC_Usart("\r\n������Ҫ������WiFi�����ơ����ܷ�ʽ����Կ�����ܷ�ʽ�ı��Ϊ��\
              \r\n0 = OPEN_PWD\
              \r\n1 = WEP\
              \r\n2 = WPA_PSK\
	            \r\n3 = WPA2_PSK\
              \r\n4 = WPA_WPA2_PSK\
							\r\n�����ʽΪ�������ַ�+Ӣ�Ķ���+���ܷ�ʽ���+Ӣ�Ķ���+��Կ�ַ�+�ո񣬵������\r\n");

	scanf("%s", cStrInput);

	PC_Usart("\r\n�Ե�Ƭ�� ����\r\n");

	pBuf = cStrInput;
	uc = 0;
	while(( pStr = strtok(pBuf, ",")) != NULL)
	{
		pStrDelimiter [ uc ++ ] = pStr;
		pBuf = NULL;
	} 
	
	ESP8266_BuildAP(pStrDelimiter [0], pStrDelimiter [2], pStrDelimiter [1]);
	ESP8266_Cmd("AT+RST", "OK", "ready", 2500); //*
		

	ESP8266_Enable_MultipleId(ENABLE);
		
	
	PC_Usart("\r\n�����������Ҫ�����Ķ˿ںźͳ�ʱʱ�䣨0~28800����λ���룩�������ʽΪ���˿ں��ַ�+Ӣ�Ķ���+��ʱʱ���ַ�+�ո񣬵������\r\n");

	scanf("%s", cStrInput);

	PC_Usart("\r\n�Ե�Ƭ�� ����\r\n");

	pBuf = cStrInput;
	uc = 0;
	while(( pStr = strtok(pBuf, ",")) != NULL)
	{
		pStrDelimiter [ uc ++ ] = pStr;
		pBuf = NULL;
	} 

	ESP8266_StartOrShutServer(ENABLE, pStrDelimiter [0], pStrDelimiter [1]);
	
	
	do
	{
		PC_Usart("\r\n����ѯ��ģ��IP����\r\n");
	  ESP8266_Cmd("AT+CIFSR", "OK", "Link", 500);
		
		PC_Usart("\r\n�����ֻ����ӸղŴ�����WiFi������ֻ����һ���ֻ�����ΪID0��Ȼ�����ֻ��������������TCP Client���Ӹղſ����ķ�������AP IP������\r\n");
		Delay_ms(20000) ;
	}	while(! ESP8266_Cmd("AT+CIPSTATUS", "+CIPSTATUS:0", 0, 500));
	

	PC_Usart("\r\n������Ҫ��˿��ֻ���ID0�����͵��ַ����������ʽΪ���ַ����������ո�+�ո񣬵������\r\n");

	scanf("%s", cStrInput);

	ul = strlen(cStrInput);
	
	ESP8266_SendString(DISABLE, cStrInput, ul, Multiple_ID_0);

	
	PC_Usart("\r\n�����ֻ�����������ַ����ַ���\r\n");
	while (1)
	{
	  pStr = ESP8266_ReceiveString(DISABLE);
		PC_Usart("%s", pStr);
	}

}


/*
 * ��������ESP8266_StaTcpClient_ApTcpServer
 * ����  ��WF-ESP8266ģ�����STA(TCP Client)+AP(TCP Server)����
 * ����  ����
 * ����  : ��
 * ����  �����ⲿ����
 */
void ESP8266_StaTcpClient_ApTcpServer(void)
{
	char cStrInput [100] = { 0 }, * pStrDelimiter [3], * pBuf, * pStr;
	u8 uc = 0;
  u32 ul = 0;


	ESP8266_AT_Test ();
	
	ESP8266_Net_Mode_Choose(STA_AP);


	PC_Usart("\r\n������Ҫ������WiFi�����ơ����ܷ�ʽ����Կ�����ܷ�ʽ�ı��Ϊ��\
						\r\n0 = OPEN_PWD\
						\r\n1  =WEP\
						\r\n2 = WPA_PSK\
						\r\n3 = WPA2_PSK\
						\r\n4 = WPA_WPA2_PSK\
						\r\n�����ʽΪ�������ַ�+Ӣ�Ķ���+���ܷ�ʽ���+Ӣ�Ķ���+��Կ�ַ�+�ո񣬵������\r\n");

	scanf("%s", cStrInput);

	PC_Usart("\r\n�Ե�Ƭ�� ����\r\n");

	pBuf = cStrInput;
	uc = 0;
	while(( pStr = strtok(pBuf, ",")) != NULL)
	{
		pStrDelimiter [ uc ++ ] = pStr;
		pBuf = NULL;
	} 
	
	ESP8266_BuildAP(pStrDelimiter [0], pStrDelimiter [2], pStrDelimiter [1]);
	ESP8266_Cmd("AT+RST", "OK", "ready", 2500); //*
	

	ESP8266_Cmd("AT+CWLAP", "OK", 0, 5000);
		
  do
	{
		PC_Usart("\r\n������Ҫ���ӵ�WiFi���ƺ���Կ�������ʽΪ�������ַ�+Ӣ�Ķ���+��Կ�ַ�+�ո񣬵������\r\n");

		scanf("%s", cStrInput);

		PC_Usart("\r\n�Ե�Ƭ�� ����\r\n");

		pBuf = cStrInput;
		uc = 0;
		while(( pStr = strtok(pBuf, ",")) != NULL)
		{
			pStrDelimiter [ uc ++ ] = pStr;
			pBuf = NULL;
		} 
		
  } while(! ESP8266_JoinAP(pStrDelimiter [0], pStrDelimiter [1]));

	
	ESP8266_Enable_MultipleId(ENABLE);
		
	
	PC_Usart("\r\n�����������Ҫ�����Ķ˿ںźͳ�ʱʱ�䣨0~28800����λ���룩�������ʽΪ���˿ں��ַ�+Ӣ�Ķ���+��ʱʱ���ַ�+�ո񣬵������\r\n");

	scanf("%s", cStrInput);

	PC_Usart("\r\n�Ե�Ƭ�� ����\r\n");

	pBuf = cStrInput;
	uc = 0;
	while(( pStr = strtok(pBuf, ",")) != NULL)
	{
		pStrDelimiter [ uc ++ ] = pStr;
		pBuf = NULL;
	} 

	ESP8266_StartOrShutServer(ENABLE, pStrDelimiter [0], pStrDelimiter [1]);
	
	
	do 
	{
		PC_Usart("\r\n���ڵ����Ͻ��������������TCP Server�������磬��������Ե�IP�Ͷ˿ںţ������ʽΪ������IP+Ӣ�Ķ���+�˿ں�+�ո񣬵������\r\n");

		scanf("%s", cStrInput);

		PC_Usart("\r\n�Ե�Ƭ�� ����\r\n");

		pBuf = cStrInput;
		uc = 0;
		while(( pStr = strtok(pBuf, ",")) != NULL)
		{
			pStrDelimiter [ uc ++ ] = pStr;
			pBuf = NULL;
		} 
		
  } while(!(ESP8266_Link_Server(enumTCP, pStrDelimiter [0], pStrDelimiter [1], Multiple_ID_0) &&
	              ESP8266_Link_Server(enumTCP, pStrDelimiter [0], pStrDelimiter [1], Multiple_ID_1) &&
	              ESP8266_Link_Server(enumTCP, pStrDelimiter [0], pStrDelimiter [1], Multiple_ID_2)));
		
	
	do
	{
		PC_Usart("\r\n����ѯ��ģ��IP��ǰһ��ΪAP IP����һ��ΪSTA IP����\r\n");
	  ESP8266_Cmd("AT+CIFSR", "OK", "Link", 500);
		
		PC_Usart("\r\n�����ֻ����ӸղŴ�����WiFi������ֻ����һ���ֻ�����ΪID3��Ȼ�����ֻ��������������TCP Client���Ӹղſ����ķ�������AP IP������\r\n");
		Delay_ms(20000) ;
	}	while(! ESP8266_Cmd("AT+CIPSTATUS", "+CIPSTATUS:3", 0, 500));
	

	for(uc = 0; uc < 3; uc ++)
	{
		PC_Usart("\r\n������˿�ID%dҪ���͵��ַ����������ʽΪ���ַ����������ո�+�ո񣬵������\r\n", uc);

		scanf("%s", cStrInput);

		ul = strlen(cStrInput);
		
		ESP8266_SendString(DISABLE, cStrInput, ul,(ENUM_ID_NO_TypeDef) uc);
		
	}
	
	
	PC_Usart("\r\n������Ҫ��˿��ֻ���ID3�����͵��ַ����������ʽΪ���ַ����������ո�+�ո񣬵������\r\n");

	scanf("%s", cStrInput);

	ul = strlen(cStrInput);
	
	ESP8266_SendString(DISABLE, cStrInput, ul, Multiple_ID_3);

	
	PC_Usart("\r\n���ڵ�������������ֻ��ֻ�����������ַ����ַ�������\r\n");
	while (1)
	{
	  pStr = ESP8266_ReceiveString(DISABLE);
		PC_Usart("%s", pStr);
	}
	
}

