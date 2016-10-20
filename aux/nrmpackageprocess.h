#ifndef NRMPACKAGEPROCESS_H
#define NRMPACKAGEPROCESS_H

#include <deque>
#include <vector>
#include <cstdint>
#include <cstring>

class nrmPackageProcess
{
public:
    nrmPackageProcess();
    ~nrmPackageProcess();

    void addRawData(const char *data, const int size);

    #pragma pack(push,1)
    typedef struct
    {
        struct{
            //Информация об устройстве
            uint32_t Dev_Serial; //Серийный номер устройства
            uint32_t Dev_Class; //Класс устройства (НРМ или ПНМ)
            uint32_t nSec2000; //количество секунд с 00:00:00 01.01.2000 г.
            uint16_t ms; //количество миллисекунд в текущей секунде
            //Параметры носимого модуля
            uint8_t fPackageSystem; //Битовые флаги состава системы
            /* 1 бит - наличие внутренней АКБ
            2 бит - наличие капнографа
            3 бит - наличие датчика потока
            4 бит - наличие канала ЭКГ
            5 бит - наличие канала ФПГ
            6 бит - наличие канала температуры
            */
            int8_t T_body; //Температура внутри корпуса в градусах цельсия
            uint8_t U_ExtACB; //Напряжение внешненей АКБ в 0,1В
            uint8_t U_IntACB; //Напряжение внутреней АКБ в 0,1В
            uint16_t Temperature; //Температура батареи, 0.1К
            uint16_t Current; //Ток потребления, мА
            uint8_t StateOfCharge; //Уровень заряда, %
            uint16_t TimeToEmpty; //Время до разряда, минутах
            //Канал капнографа
            int16_t fiCO2; //Концентрация на вдохе СО2 в 0,1%
            int16_t etCO2; //Концентрация на выдохе СО2 в 0,1%
            int16_t fiO2; //Концентрация на вдохе О2 в 0,1%
            int16_t etO2; //Концентрация на выдохе О2 в 0,1%
            uint8_t cBreath; //Частота дыхания, дых/мин. Число 255 - нет результата
            uint8_t Capno_Status; //Статус-сообщение капнографа
            //Канал метаболографа
            int16_t VCO2; // количество углекислоты выделенной испытуемым за минуту (мл/мин)
            int16_t VO2; //количество кислорода, потребленного испытуемым за минуту
            int16_t RQ; //дыхательный коэффициент
            int16_t REE; //потребление энергии испытуемым за сутки
            int16_t Vexp; //объем выдоха
            int16_t MV; //минутный объем дыхания
            //Канал температуры
            int16_t T; // температура, 0.1 °С, диапазон от -5.0 до +51.0 °С, значения >= 100.0 – нет результата
            uint8_t T_Status; //Статус-сообщение канала температуры
            //Канал ЭКГ
            uint16_t HR; //Частота сердечных сокращений
            uint8_t ECG_Status; //Статус-сообщение ECG
            //Канал SpO2
            uint16_t PR; //Частота пульса
            uint16_t Filling; //наполнение, 0,001%, диапазон 0,000..65,535
            uint8_t Saturation; //сатурация, %, нормальный диапазон 0..100, значение 255 – нет результата
            uint8_t SpO2_Status; //Статус-сообщение SpO2
            uint8_t ECG_Amp; //Усиление отведений ЭКГ. По умолчанию равно 1.
            //Канал Давления
            uint16_t Sys_Press; //систолическое давление
            uint16_t Dia_Press; //диастолическое давление
            uint16_t Sys_Init; //систолическое начальное давление
            uint16_t Dia_Init; //диастолическое начальное давление
            //Индекс тяжести состояния
            uint8_t IHS; //ИТС
            uint8_t reserved[31]; //дополняем структуру до 100
        } DATA_ST;
        //Волны медленные
        int16_t cCO2[50]; //50 отсчётов CO2, разрешение 0.01 % (капно)
        int16_t cO2[50]; //50 отсчётов O2, разрешение 0.01 %
        int32_t cFlow[50]; //1 отсчёт потока, 0,1 мл/мин. Знак значения обозначает направление + - выдох, - - вдох.
        int8_t PPG[100]; //Сто отсчётов фото-плетизмограммы. Разрешение по вертикали – автомасштабирование. Диапазон значений 0..250. Числа 251..255 являются служебными.
        //Волны быстрые
        int8_t ECG_I[200]; //200 отсчётов отведений ЭКГ. Разрешение по вертикали при усилении равном 1 128 мкВ/ед. Знаковые 8-битные числа.
        int8_t ECG_II[200];
        int8_t ECG_III[200];
        int8_t aVR[200];
        int8_t aVL[200];
        int8_t aVF[200];
        int8_t V1[200];
        int8_t V2[200];
        int8_t V3[200];
        int8_t V4[200];
        int8_t V5[200];
        int8_t V6[200];
    }RADIO_DATAPAC_STR;
    #pragma pack(pop)

    #pragma pack(push,1)
    typedef struct
    {
        uint8_t start_byte;
        uint8_t length;
        uint32_t dest_addr;
        uint32_t src_addr;
        uint8_t option;
        //data
        uint8_t type;
        uint8_t id;
        uint8_t data[100];
        //end data
        uint8_t crc;
    }RF_DATA_PACK;
    #pragma pack(pop)

    RADIO_DATAPAC_STR *out;

private:
    static const int STRUCT_SIZE = 114;
    static const int OUT_SIZE = 3000;

    enum stage{SEARCH, WRITE};
    stage s;

    std::deque<char> buffer;
    std::vector<char> package;

    char tmpOut[OUT_SIZE];

    void processBuffer();
    void processPackage();

};

#endif // NRMPACKAGEPROCESS_H
