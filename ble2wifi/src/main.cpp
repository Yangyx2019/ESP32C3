#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <WiFi.h>
#include "common.h"

/*
   1. 创建一个 BLE Server
   2. 创建一个 BLE Service
   3. 创建一个 BLE Characteristic
   4. 创建一个 BLE Descriptor
   5. 开始服务
   6. 开始广播
*/

BLEServer *pServer = NULL;            //BLEServer指针 pServer
BLECharacteristic *pTxCharacteristic; //BLECharacteristic指针 pTxCharacteristic
bool deviceConnected = false;         //本次连接状态
bool oldDeviceConnected = false;      //上次连接状态
uint8_t txValue = 0;
u8 txTXBuffer[10];

void communicateBLE(std::string rxData);
void BLE2WiFi(std::string rxData);


// https://www.uuidgenerator.net/ 这个网站可以生成UUID

#define SERVICE_UUID "6E400001-B5A3-F393-E0A9-E50E24DCCA9E" // UART service UUID
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

//创建MyServerCallbacks类，其继承自BLEServerCallbacks
class MyServerCallbacks : public BLEServerCallbacks
{
    void onConnect(BLEServer *pServer)
    {
        deviceConnected = true;
    };

    void onDisconnect(BLEServer *pServer)
    {
        deviceConnected = false;
    }
};

//创建MyCallbacks类，其继承自BLECharacteristicCallbacks
class MyCallbacks : public BLECharacteristicCallbacks
{
    void onWrite(BLECharacteristic *pCharacteristic)
    {
        std::string rxValue = pCharacteristic->getValue(); //接收信息

        if (rxValue.length() > 0)
        { //向串口输出收到的值
            Serial.print("Rx: ");
            for (int i = 0; i < rxValue.length(); i++)
                Serial.print(rxValue[i]);

            Serial.println();
            communicateBLE(rxValue); //	下面开始处理信息
        }
    }
};

void setup()
{
    Serial.begin(115200);

    // 初始化BLE
    BLEDevice::init("ESP32_BLE2WiFi");

    // 创建BLE服务
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks()); //设置回调函数
    BLEService *pService = pServer->createService(SERVICE_UUID);

    // 创建BLE的特性
    pTxCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID_TX,
        BLECharacteristic::PROPERTY_NOTIFY);
    pTxCharacteristic->addDescriptor(new BLE2902()); //添加描述符
    BLECharacteristic *pRxCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID_RX,
        BLECharacteristic::PROPERTY_WRITE);

    pRxCharacteristic->setCallbacks(new MyCallbacks()); //设置回调函数
    pService->start();                                  // 开始服务
    pServer->getAdvertising()->start();                 // 开始广播

    Serial.println("Waiting a client connection to notify...");
}

void loop()
{
    // deviceConnected	已连接
    if (deviceConnected)
    {
        pTxCharacteristic->setValue(&txValue, 1); // 设置要发送的值为1
        pTxCharacteristic->notify();              // 广播
        txValue++;                                // 指针地址自加1
        delay(2000);                              // 如果有太多包要发送，蓝牙会堵塞
    }

    // disconnecting  断连
    if (!deviceConnected && oldDeviceConnected)
    {
        delay(500);                  // 留时间给蓝牙缓冲
        pServer->startAdvertising(); // 重新广播
        Serial.println("Began to Broadcast");
        oldDeviceConnected = deviceConnected;
    }

    // connecting  正在连接
    if (deviceConnected && !oldDeviceConnected)
    {
        // do stuff here on connecting
        oldDeviceConnected = deviceConnected;
    }
}

void communicateBLE(std::string rxData)
{
    u8_t Length = rxData.length();

    if ('W' == rxData[0] && '*' == rxData[Length - 1])
    {
        Serial.printf("Ready for Wi-Fi\r\n");
        BLE2WiFi(rxData);
    }
}

void BLE2WiFi(std::string WiFi_Data)
{
    u8_t Length = WiFi_Data.length();
    u8_t Interval = 0, ConnectCnt = 0;

    // 首先找到间隔'#'，确定SSID和PSWD的长度
    for (u8_t i = 0; i < Length; i++)
    {
        if ('#' == WiFi_Data[i])
        {
            Interval = i;
        }
    }

    // 首先获取SSID
    std::string Temp = "";
    for (u8_t i = 0; i < Interval - 1; i++)
    {
        Temp += WiFi_Data[i + 1];
    }
    char *SSID = new char[Interval - 1];
    strcpy(SSID, Temp.c_str());
    Serial.printf("SSID:%s\r\n", SSID);

    // 然后才是PSWD
    Temp = "";
    for (u8_t i = 0; i < Length - Interval - 2; i++)
    {
        Temp += WiFi_Data[i + 1 + Interval];
    }
    char *PSWD = new char[Length - Interval - 2];
    strcpy(PSWD, Temp.c_str());
    Serial.printf("PSWD:%s\r\n", PSWD);

    // 准备连接Wi-Fi
    Serial.printf("Connectingto WiFi.");
    WiFi.begin(SSID, PSWD);
    ConnectCnt = 0;
    while (WL_CONNECTED != WiFi.status())
    {
       
        delay(1000);
        Serial.printf(".");
        if ((++ConnectCnt > 10) || (WL_CONNECTED == WiFi.status()))
        {
            break;
        }
    }

    // 判断是否连接成功
    if (WL_CONNECTED == WiFi.status())
    {
        Serial.printf("Connected to the WiFi network\r\n");
        Serial.printf("IP address: ");
        Serial.println(WiFi.localIP());

    }
    else
    {
        Serial.print("\r\nFile to connected WiFi\r\n");
    }
}

