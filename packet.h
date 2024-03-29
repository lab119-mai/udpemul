#include <stdint.h>

#define MAX_QUANTITY_ADC3_PB   5
#define MAX_QUANTITY_DIN3_PB   4
#define MAX_QUANTITY_RSM_BOX   2
#define MAX_QUANTITY_VMOD_DIN3 4

typedef struct
{
    unsigned short Value;
    unsigned short Dostovernost;
} RSM_Result;

typedef struct RESULT
{
    short value_of_adc[MAX_QUANTITY_ADC3_PB][8];
    unsigned char BipOrUNBip[MAX_QUANTITY_ADC3_PB][8]; /* информация об режиме измерений каналов adc3 (полярный, биполярный) */
    unsigned short Spare1[4];                          /* для выравнивания */
    /*unsigned char  din[4][4]; /* [channel: 0-port A, 1-port B, 2-port H, 3-empty][pb] */
    unsigned char din3[MAX_QUANTITY_VMOD_DIN3][4][4]; /* [channel: 0-port A, 1-port B, 2-port H, 3-empty][pb] */
    unsigned short vadi_value[16];                    /* на самом деле каналов 9 */
    RSM_Result RsmResult[MAX_QUANTITY_RSM_BOX][8];    /* rsm driver */
} measure_result;

typedef struct
{
    uint32_t s_addr;   /* адрес получателя (кому отправляется пакет. что-) */ // in_addr_t = 4 bytes
    char SysCode[4]; /* описание источника пакета */
    char Version[4]; /* версия пакета */
    int32_t PktSize;     /* общий размер пакета */
    int32_t DataOffset;  /* смещение в пакете до данных MeasurementFrame */
    int32_t DataSize;    /* размер полезных данных  */
    int32_t count;      /* порядковый номер пакета */
    int Time;
    int Date;
    int Tick;
    int RelativeTime; /* некоторе относительное время в тиках */
    /* char  Spare[4]; */
    uint16_t marker; /* abcd */
    uint16_t Crc;    /* контрольная сумма данных от начала структуры до поля Crc */
    char MeasurementFrame[sizeof(measure_result)];
    short MeasurementCrc; /* контрольная сумма MeasurementFrame */
    /* разделение контрольных сумм специально сделано разделено. т.к. MeasurementFrame не меняется для всех получателей */
} __attribute__((packed)) TSurMeasurementPkt ;
