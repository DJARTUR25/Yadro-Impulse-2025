#include <iostream>
#include <cmath>
#include <string>
#include <iomanip>
#include <vector>
#include <sstream>
#include <queue>
#include <map>
#include <fstream>
#include <algorithm>

class ClubTime {        // класс для обработки времени
private: 
    int TotalMin;       // общее количество минут, начиная с начала дня

public:
    ClubTime() {
        TotalMin = 0;   // конструктор по умолчанию, задается с нулевым количеством минут
    }

    ClubTime (int Hours, int Minutes) {     // конструктор, создает объект по количеству часов и минут (формат XX, XX)
        TotalMin = Hours * 60 + Minutes;
    }

    static ClubTime FromString (const std::string& str_time) {      // функция для перевода времени из строки в TotalMin
        if ((str_time.size() != 5) || (str_time[2] != ':')) {     // вывод исключения при неправильном вводе времени или без разделителя :
            throw std::invalid_argument("ErrorInTimeInput\n");
        }

        int Hours = std::stoi(str_time.substr(0, 2));               // часы = (подстрока в целое число)
        int Minutes = std::stoi(str_time.substr(3, 2));             // минуты = (подстрока в целое число)

        if ((Hours < 0) || (Hours > 23) || (Minutes < 0) || (Minutes > 59)) {
            throw std::invalid_argument("WrongTime!\n");            // вывод исключения при неверном указании времени
        }

        ClubTime result = ClubTime(Hours, Minutes);

        return result;
    }

    std::string ClubTimeToString() const {
        std::ostringstream oss;
        oss << std::setfill('0') << std::setw(2) << (TotalMin / 60) << ":" << std::setw(2) << (TotalMin % 60);

        return oss.str();
    }

    int GetTotalMin () const { return TotalMin; }

    friend std::ostream& operator << (std::ostream& os, const ClubTime& _ClubTime) {                // дружеский оператор вывода объекта ClubTime
        os << _ClubTime.ClubTimeToString();
        return os;
    }

    // логические операторы сравнения
    bool operator < (const ClubTime _ClubTime) const { return (TotalMin < _ClubTime.TotalMin); }
    bool operator <= (const ClubTime _ClubTime) const { return (TotalMin <= _ClubTime.TotalMin); }
    bool operator > (const ClubTime _ClubTime) const { return (TotalMin > _ClubTime.TotalMin); }
    bool operator >= (const ClubTime _ClubTime) const { return (TotalMin >= _ClubTime.TotalMin); }
};

class Tables {                  // класс, отвечающий за занятость столов и их доход за рабочий день
private:
    int TableID;                // ID стола
    int Price;                  // цена одного часа
    bool Occupancy;             // занят или нет
    std::string CurrentUser;    // клиент за столом
    ClubTime UseStart;          // время начала сессии
    int TotalUseTime;           // общее время занятости клиентом
    int TotalIncome;            // общий доход стола за рабочий день
public:
    Tables(int _TableID, int _Price) {            // конструктор по умолчанию
        TableID = _TableID;
        Occupancy = false;
        TotalUseTime = 0;
        TotalIncome = 0;
        Price = _Price;
    }

    void StartUsing (const ClubTime& _StartTime, const std::string& _UserName) {    // начало пользования столом
        Occupancy = true;           // указание, что стол занят
        UseStart = _StartTime;      // запись начала пользования столом
        CurrentUser = _UserName;    // запись имени клиента, занявшего стол
    }

    void StopUsing (const ClubTime& _EndTime) {         // конец пользования
        if (!Occupancy) {
            throw std::invalid_argument("TableIsNotOccupied\n");    // исключение, если стол не занят
        };
        Occupancy = false;  // указание, что стол свободен
        int UseTime = _EndTime.GetTotalMin() - UseStart.GetTotalMin();  //подсчет времени пользования в минутах
        TotalUseTime += UseTime;        // прибавляем время к общему времени пользования столом

        if ((UseTime % 60) == 0) {      // подсчет выручки, если стол был занят целое число часов
            int UsingHours = UseTime / 60;
            TotalIncome += UsingHours * Price;
        }
        else {                          // подсчет выручки, если стол был занят нецелое число часов => округление в большую сторону
            int UsingHours = UseTime / 60 + 1;
            TotalIncome += UsingHours * Price;
        }

        CurrentUser = "";
        TotalUseTime = 0;
    }

    bool IsOccupied () const { return Occupancy; }        // функция проверки на занятость стола
    int GetTableID () const { return TableID; }           // геттер АйДи стола
    int GetTotalIncome () const { return TotalIncome; }   // геттер выручки стола за прошедшее время
    int GetUsageTime () const { return TotalUseTime; }    // геттер времени пользования ПК
};

class Event {       // класс, обрабатывающий входящие и исходящие события
public:
    ClubTime Time;              // время события
    int EventID;                // АйДи события
    std::string ClientName;     // имя клиента
    int TableID;                // АйДи стола для ID = 2
    std::string ErrorMessage;   // сообщение об ошибке (для ID 13)

    Event () = delete;

    Event(const std::string& _Event) {       // обработка ввода события
        std::istringstream iss(_Event);                 // используется istingstream
        std::string Temp;                               // временная строка для получения времени

        iss >> Temp >> EventID;     // запись из ввода во временную строку, затем ввод ID события

        Time.FromString(Temp);      // запись времени

        switch (EventID)            // обработка исключений разных ID
        {
        case 1: case 3: case 4: case 11:    // Клиент пришел, Клиент ожидает, Клиент ушел, Клиент ушел(вывод)
            if (!(iss >> ClientName)) {
                throw std::invalid_argument("Incorrect Client Name input for ID = 1\n");
            }
            break;
        case 2: case 12:   // Клиент сел за стол, Клиент сел за стол(вывод)
            if (!(iss >> ClientName)) {
                throw std::invalid_argument("Incorrect Client Name input for ID = 2\n");
            }
            if ((!(iss >> TableID)) || (TableID <= 0)) {
                throw std::invalid_argument("Incorrect TableID input for ID = 2\n");
            }
            break;
        case 13:        // Ошибка
            std::getline(iss >> std::ws, ErrorMessage);
            break;
        default:   // иные вводы ID, не предусмотрены ТЗ.
            throw std::invalid_argument("Incorrect Input Event ID: " + std::to_string(EventID));
            break;
        }

        std::string extra;  // обработка лишних аргументов при их наличии
        if (iss >> extra) {
            throw std::invalid_argument("Too many arguments for ID: " + std::to_string(EventID));
        }

    }

    friend std::ostream& operator << (std::ostream& os, const Event& _Event) {      // обработка вывода события
        os << _Event.Time.ClubTimeToString() << " " << _Event.EventID;
        switch(_Event.EventID)
        {
        case 1: case 2: case 3: case 4: case 11: case 12:               // вывод имени клиента для 11 и 12 ID
            os << " " << _Event.ClientName;
            if (_Event.EventID == 12) {      // для ID 12 вывод номера стола
                os << " " << _Event.TableID;
            }
            break;
        case 13:
            os << " " << _Event.ErrorMessage;       // вывод сообщения об ошибке для ID 13
            break;

        default:
            throw std::invalid_argument("Incorrect Output Event ID: " + std::to_string(_Event.EventID));
            break;
        }

        return os;
    }
};

class ComputerClub {        // класс с общими характеристиками ПК клуба
private:
    int NumberOfTables;     // число столов в клубе
    ClubTime TimeOpen;          // время открытия
    ClubTime TimeClose;         // время закрытия
    int PricePerHour;       // цена одного ПК за час

    std::vector <Tables> ListTable;         // список столов
    std::vector <Event> OutputEvents;       // список выходных событий
    std::queue<std::string> Waiting;        // очередь ожидания
    std::map<std::string, int> Clients;     // словарь клиентов

    void ClientArrived(const Event& _Event) {
        if ( (_Event.Time < TimeOpen) || (_Event.Time >= TimeClose) ) {
            throw std::runtime_error("NotOpenYet");
        }
        if (Clients.count(_Event.ClientName) == 1) {
            throw std::runtime_error ("YouShallNotPass");
        }
        Clients[_Event.ClientName] = -1;        // спец значение, что клиент только появился
    }

    void ClientPlaced(const Event& _Event) {
        if (!(Clients.count(_Event.ClientName))) 
            throw std::runtime_error("ClientUnknown");
        
        int TableIndex = _Event.TableID - 1;

        if (TableIndex < 0 || TableIndex >= NumberOfTables)
            throw std::invalid_argument ("Invalid Table ID");
        if (ListTable[TableIndex].IsOccupied()) 
            throw std::runtime_error("PlaceIsBusy");

        if (Clients[_Event.ClientName] != -1) {
            ListTable[Clients[_Event.ClientName]].StopUsing(_Event.Time);
        }

        ListTable[TableIndex].StartUsing(_Event.Time, _Event.ClientName);
        Clients[_Event.ClientName] = TableIndex;
    }

    void ClientWaiting(const Event& _Event) {
        if (!(Clients.count(_Event.ClientName))) 
            throw std::runtime_error("ClientUnknown");
        
        bool IsTableFree = false;
        
        for (size_t i = 0; i < ListTable.size(); i++) {
            if (ListTable[i].IsOccupied() == false) {
                IsTableFree = true;
                break;
            }
        }

        if (IsTableFree == true)
            throw std::runtime_error("ICanWaitNoLonger!");
        
        if (Waiting.size() >= static_cast<size_t>(NumberOfTables)) {
            std::string Msg = _Event.Time.ClubTimeToString() + " 11 " + _Event.ClientName;
            OutputEvents.emplace_back(Event(Msg));
            Clients.erase(_Event.ClientName);
        }
        else {
            Waiting.push(_Event.ClientName);
        }
    }

    void ClientLeft(const Event& _Event) {
        if (!(Clients.count(_Event.ClientName)))
            throw std::runtime_error("ClientUnknown");
        
        if (Clients[_Event.ClientName] != -1) {
            int TableIndex = Clients[_Event.ClientName];
            ListTable[TableIndex].StopUsing(_Event.Time);

            if (!Waiting.empty()) {
                std::string NextClient = Waiting.front();
                Waiting.pop();
                ListTable[TableIndex].StartUsing(_Event.Time, NextClient);
                Clients[NextClient] = TableIndex;
                std::string Msg = _Event.Time.ClubTimeToString() + " 12 " + NextClient + "  " + std::to_string(TableIndex + 1);
                OutputEvents.emplace_back(Event(Msg));
            }
        }
    }
 
public:
    // конструктор ПК клуба
    ComputerClub (int _NumTables, const ClubTime& _Open, const ClubTime& _Close, int _Price) {
        NumberOfTables = _NumTables;        
        TimeOpen = _Open;
        TimeClose = _Close;
        PricePerHour = _Price;

        for (int i = 1; i < _NumTables + 1; i++) {  // цикл с заполнением списка столов и их цен
            ListTable.emplace_back(i, PricePerHour);
        }
    }

    void CurrentEvent (const Event& _Event) {
        OutputEvents.emplace_back(_Event);

        try {
            switch (_Event.EventID) {
                case 1: ClientArrived(_Event); break;
                case 2: ClientPlaced(_Event); break;
                case 3: ClientWaiting(_Event); break;
                case 4: ClientLeft(_Event); break;
            }
        }
        catch (const std::exception& Exc) {
            std::string ErrorStr = _Event.Time.ClubTimeToString() + "13" + Exc.what();
            OutputEvents.emplace_back(Event(ErrorStr));
        }
    }

    void EndDay() {
        std::vector<std::string> StayingClients;
        for (const auto& [Client, _] : Clients) StayingClients.push_back(Client);
        std::sort(StayingClients.begin(), StayingClients.end());

        for (const auto& Client : StayingClients) {
            std::string Msg = TimeClose.ClubTimeToString() + " 11 " + Client;
            OutputEvents.emplace_back(Event(Msg));

            if (Clients[Client] != -1) {
                ListTable[Clients[Client]].StopUsing(TimeClose);
            }
        }
    }

    void PrintResults() const {
        std::cout << TimeOpen << "\n";
        for (const auto& _Event : OutputEvents) std::cout << _Event << std::endl;
        std::cout << TimeClose << std::endl;

        for (const auto& Table : ListTable) {
            ClubTime UsageTime(0, Table.GetUsageTime());
            std::cout << Table.GetTableID() << " " << Table.GetTotalIncome() << " " << UsageTime << std::endl;
        }
    }
};



int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Used: " << argv[0] << " <input_file>\n";
        return 1;
    }

    std::ifstream file(argv[1]);
    if (!file) {
        std::cerr << "Error In Opening File\n";
        return 1;
    }

    std::vector<std::string> Strings;
    std::string String;

    while (std::getline(file, String)) {
        if (!String.empty())
            Strings.emplace_back(String);
    }

    try {
        if (Strings.size() < 3) {
            throw std::invalid_argument("Invalid Input");
        }

        int NumberOfTables = std::stoi(Strings[0]);

        if (NumberOfTables <= 0) {
            throw std::invalid_argument("Invalid Table's count");
        }

        std::istringstream Time(Strings[1]);
        std::string OpenTimeString;
        std::string CloseTimeString;
        if (!(Time >> OpenTimeString >> CloseTimeString))
            throw std::invalid_argument("Invalid time of work");
        
        ClubTime OpenTime = ClubTime::FromString(OpenTimeString);
        ClubTime CloseTime = ClubTime::FromString(CloseTimeString);
        if (CloseTime <= OpenTime) 
            throw std::invalid_argument("Closes before opens");
        
        int Price = std::stoi(Strings[2]);
        if (Price <= 0)
            throw std::invalid_argument("Invalid price");
        
        ComputerClub CLUB(NumberOfTables, OpenTime, CloseTime, Price);

        for (size_t i = 3; i < Strings.size(); i++) {
            Event Event(Strings[i]);
            CLUB.CurrentEvent(Event);
        }

        CLUB.EndDay();
        CLUB.PrintResults();
    }
    catch (const std::exception& Exc) {
        std::cout << Exc.what() << std::endl;
        return 1;
    }

    return 0;
}