// спасибо за проверку моей работы! Буду с нетерпением ждать обратной связи по тестированию
// Евсеев Григорий Алексеевич, 03.02.2004, автор

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

class ClubTime {        // класс, отслеживающий время в клубе
private: 
    int TotalMin;       // время в минутах, начиная от начала работы клуба

public:
    ClubTime() {        // конструктор по умолчанию
        TotalMin = 0;   // ставим время = 0
    }

    ClubTime(int Hours, int Minutes) {     // конструктор по введенным часам и минутам
        TotalMin = Hours * 60 + Minutes;   // перевод в минуты
    }

    static ClubTime FromString(const std::string& str_time) {      // метод перевода времени из формата ЧЧ:ММ в TotalMin
        if ((str_time.size() != 5) || (str_time[2] != ':')) {      // исключение, если нет двоеточия или размер "часов" неверный
            throw std::invalid_argument("ErrorInTimeInput\n");  
        }

        int Hours = std::stoi(str_time.substr(0, 2));       // часы в строке (через метод string to int)   
        int Minutes = std::stoi(str_time.substr(3, 2));     // минуты в строке

        if ((Hours < 0) || (Hours > 23) || (Minutes < 0) || (Minutes > 59)) {   // исключение, если введенное время неправильное (например, 25:61)
            throw std::invalid_argument("WrongTime!\n");            
        }

        return ClubTime(Hours, Minutes);        // собирает объект CLubTime и возвращает его
    }

    std::string ClubTimeToString() const {      // метод перевода времени в строку
        std::ostringstream oss;                 // переменная выходного потока
        oss << std::setfill('0') << std::setw(2) << (TotalMin / 60) << ":" << std::setw(2) << (TotalMin % 60);
            // заполняем через метод setfill нулями (00000), метод setw(2) устанавливает ширину в 2 символа, ставим часы, ставим :, ставим минуты (аналогично)
        return oss.str();       // возврат итоговой строки
    }

    int GetTotalMin() const { return TotalMin; }        // геттер, возвращает количество минут, прошедших с начала работы клуба

    friend std::ostream& operator<<(std::ostream& os, const ClubTime& _ClubTime) {                // дружественный оператор вывода
        os << _ClubTime.ClubTimeToString();     
        return os;
    }

    // логические операторы сравнения ClubTime
    bool operator<(const ClubTime _ClubTime) const { return (TotalMin < _ClubTime.TotalMin); }
    bool operator<=(const ClubTime _ClubTime) const { return (TotalMin <= _ClubTime.TotalMin); }
    bool operator>(const ClubTime _ClubTime) const { return (TotalMin > _ClubTime.TotalMin); }
    bool operator>=(const ClubTime _ClubTime) const { return (TotalMin >= _ClubTime.TotalMin); }
};

class Tables {                  // класс для столов
private:
    int TableID;                // идентификатор стола
    int Price;                  // цена за час пользования
    bool Occupancy;             // логическая переменная, указываюшая, занят ли стол
    std::string CurrentUser;    // имя клиента, занимающего стол сейчас
    ClubTime UseStart;          // время начала пользования столом
    int TotalUseTime;           // общее время, которое стол был занят
    int TotalIncome;            // общая выручка за стол
public:
    Tables(int _TableID, int _Price) {      // конструктор, собирающий объект стол по идентификатору и цене
        TableID = _TableID;
        Occupancy = false;
        TotalUseTime = 0;
        TotalIncome = 0;
        Price = _Price;
    }

    void StartUsing(const ClubTime& _StartTime, const std::string& _UserName) {     // метод начала пользования столом   
        Occupancy = true;       // занят
        UseStart = _StartTime;      
        CurrentUser = _UserName;    
    }

    void StopUsing(const ClubTime& _EndTime) {         // метод конца пользования столом
        if (!Occupancy) {
            throw std::invalid_argument("TableIsNotOccupied\n");    
        };
        Occupancy = false;  // освобождаем стол
        int UseTime = _EndTime.GetTotalMin() - UseStart.GetTotalMin();      // время пользования
        TotalUseTime += UseTime;        // прибавляем к общему времени пользования

        int UsingHours = (UseTime + 59) / 60;       // подсчет целого числа часов, с округлением в большую сторону
        TotalIncome += UsingHours * Price;          // подсчет выручки

        CurrentUser = "";       // занимающий клиент - опустошаем
    }

    bool IsOccupied() const { return Occupancy; }        // метод показа, "занят ли стол"
    int GetTableID() const { return TableID; }           // геттер, возвращает ID стола
    int GetTotalIncome() const { return TotalIncome; }   // геттер, возвращает общую выручку стола
    int GetUsageTime() const { return TotalUseTime; }    // геттер, возвращает общее время занятости стола
};

class Event {       // класс событий
public:
    ClubTime Time;      // время события
    int EventID;        // ID события
    std::string ClientName;     // имя клиента в событии
    int TableID;                // ID стола для событий с ID 2 и ID 12
    std::string ErrorMessage;   // сообщение об ошибке

    Event() = delete;   // конструктор по умолчанию, не даю создать пустые события

    Event(const std::string& _Event) {      // конструктор события по строке
        std::istringstream iss(_Event);     // поток, принимающий строку
        std::string Temp;                   // временная строка            

        iss >> Temp >> EventID;     // поток переносит время в временную строку, далее записывает ID события
        Time = ClubTime::FromString(Temp);  // перевод времени события в тип ClubTime из строки Temp

        switch (EventID)            // switch по ID события
        {
        case 1: case 3: case 4: case 11:    // для ID = 1, 3, 4, 11 
            if (!(iss >> ClientName)) {     // проверка, нормально ли введенно имя клиента (есть ли оно)
                throw std::invalid_argument("Incorrect Client Name input for ID = 1\n");    // если нет, то выброс исключения
            }
            break;

        case 2: case 12:        // для ID = 2, 12
            if (!(iss >> ClientName)) {     // проверка, есть ли имя клиента
                throw std::invalid_argument("Incorrect Client Name input for ID = 2\n");    // если нет, то выброс исключения
            }
            if ((!(iss >> TableID)) || (TableID <= 0)) {        // провека на ID стола, что оно есть или ID стола <= 0
                throw std::invalid_argument("Incorrect TableID input for ID = 2\n");        // выброс исключения
            }
            break;

        case 13: {       // для ID = 13
            std::getline(iss >> std::ws, ErrorMessage); // поток принимает строку с сообщением об ошибке
            size_t first = ErrorMessage.find_first_not_of(' '); // задаем беззнаковое целое число, обозначающее размер сообщения об ошибке до первого НЕ пробела
            if (first != std::string::npos) {                   // если размер не равен максимальному размеру
                ErrorMessage = ErrorMessage.substr(first);      // задаем сообщение об ошибке как подстроку с размером first
            }
            break;          // для каждого case в конце задаем break, чтобы cases не выполнялись последовательно
        }
        default:   // для непредусмотренных выше случаев
            throw std::invalid_argument("Incorrect Input Event ID: " + std::to_string(EventID));        // выброс исключения и номер полученного ID
        }

        std::string extra;          // строка с дополнительными символами
        if (iss >> extra) {         // если поток принял доп. символы ...
            throw std::invalid_argument("Too many arguments for ID: " + std::to_string(EventID));       // ..., то выброс исключения
        }
    }

    friend std::ostream& operator<<(std::ostream& os, const Event& _Event) {        // дружественный оператор вывода события 
        os << _Event.Time.ClubTimeToString() << " " << _Event.EventID;
        switch(_Event.EventID)
        {
        case 1: case 2: case 3: case 4: case 11: case 12:   // упрощенное программирование для case 2 и case 12, по сравнению с предыдущим switch             
            os << " " << _Event.ClientName;                 
            if ((_Event.EventID == 12) || (_Event.EventID == 2)) {      // если ID = 2 || 12
                os << " " << _Event.TableID;        // вывод номера стола
            }
            break;
        case 13:        // вывод ошибки для случая ID = 13
            os << " " << _Event.ErrorMessage;       
            break;
        default:        // иные случая
            throw std::invalid_argument("Incorrect Output Event ID: " + std::to_string(_Event.EventID));    // выброс исключения
        }
        return os;      // закрытие потока
    }
};

class ComputerClub {            // класс компьютерного клуба    
private:
    int NumberOfTables;         // число столов в клубе
    ClubTime TimeOpen;          // время открытия клуба
    ClubTime TimeClose;         // время закрытия клуба
    ClubTime CurrentTime;       // текущее время 
    int PricePerHour;           // стоимость одного часа 

    std::vector<Tables> ListTable;         // список столов, задан вектром из элементов Tables
    std::vector<Event> OutputEvents;       // список исходящих событий, задан вектором из элементов Event 
    std::queue<std::string> Waiting;       // очередь посетителей, используется библиотека <queue>
    std::map<std::string, int> Clients;    // таблица клиентов и их нумерация, используется 

    void ClientArrived(const Event& _Event) {   // сценарий "Клиент появился"
        if (_Event.Time < TimeOpen || _Event.Time > TimeClose) {    // выброс ошибки, если клиент появился в нерабочее время
            throw std::runtime_error("NotOpenYet");
        }
        if (Clients.count(_Event.ClientName)) {     // выброс ошибки, если имя клиента есть в таблице, т.е. он уже в заведении
            throw std::runtime_error("YouShallNotPass");
        }
        Clients[_Event.ClientName] = -1;        // ставлю -1, т.к. клиент только пришел
    }

    void ClientPlaced(const Event& _Event) {        // сценарий "Клиент занял стол"
        if (!Clients.count(_Event.ClientName))      // если имя клиента нет в таблице
            throw std::runtime_error("ClientUnknown");  // значит, клиент не существует
        
        int TableIndex = _Event.TableID - 1;        // идентификатор стола, за который садится клиент

        if (TableIndex < 0 || TableIndex >= NumberOfTables)     // если номер стола не в списке, то выброс ошибки
            throw std::invalid_argument("Invalid Table ID");
        if (ListTable[TableIndex].IsOccupied())                 // если стол занят
            throw std::runtime_error("PlaceIsBusy");            // выброс ошибки

        if (Clients[_Event.ClientName] != -1) {                 // если клиент сидит за другим столом 
            ListTable[Clients[_Event.ClientName]].StopUsing(_Event.Time);       // заканчиваем пользование другим столом
        }

        ListTable[TableIndex].StartUsing(_Event.Time, _Event.ClientName);       // начинаем пользование новым столом из _Event
        Clients[_Event.ClientName] = TableIndex;                                // запоминаем, что клиент сел за этот стол
    }

    void ClientWaiting(const Event& _Event) {           // сценарий "Клиент ожидает"
        if (!Clients.count(_Event.ClientName))          // если имя клиента не значится в списке, то он неизвестен
            throw std::runtime_error("ClientUnknown");
        
        bool IsTableFree = false;       // создание логической переменной о занятости стола (можно было использовать IsOccupied())
        for (const auto& table : ListTable) {       // цикл по всей таблице клиентов, ищем свободный стол
            if (!table.IsOccupied()) {              // если стол не занят
                IsTableFree = true;                 // значит стол свободен, выход из цикла
                break;
            }
        }

        if (IsTableFree)        // если есть свободный стол
            throw std::runtime_error("ICanWaitNoLonger!");  // выброс ошибки по ТЗ
        
        if (Waiting.size() >= static_cast<size_t>(NumberOfTables)) {            // если размер очереди больше числа столов, то 
            OutputEvents.emplace_back(Event(CurrentTime.ClubTimeToString() + " 11 " + _Event.ClientName));      // выброс ошибки с соотв. ID
            Clients.erase(_Event.ClientName);           // удаление из очереди этого клиента
        }
        else {      // в ином случае, в очередь добавляется клиент
            Waiting.push(_Event.ClientName);
        }
    }

    void ClientLeft(const Event& _Event) {      // сценарий "Клиент ушел"
        if (!Clients.count(_Event.ClientName))  // если имя клиента неизвестно
            throw std::runtime_error("ClientUnknown");
        
        if (Clients[_Event.ClientName] != -1) {     // если клиент за столом
            int TableIndex = Clients[_Event.ClientName];    // запоминаем ID занятого стола
            ListTable[TableIndex].StopUsing(_Event.Time);   // клиент закончил пользоваться столом

            if (!Waiting.empty()) {             // если очередь не пустая
                std::string NextClient = Waiting.front();   // запоминаем имя следующего клиента (в начале очереди)
                Waiting.pop();              // вытаскиываем из очереди следующего клиента
                ListTable[TableIndex].StartUsing(_Event.Time, NextClient);  // начинаем пользование столом (следующий клиент)
                Clients[NextClient] = TableIndex;   // записываем ID стола этого клиента
                OutputEvents.emplace_back(Event(CurrentTime.ClubTimeToString() + " 12 " + NextClient + " " + std::to_string(TableIndex + 1)));  // выпускаем событие с ID 12
            }
        }
        Clients.erase(_Event.ClientName); // Удаление после обработки
    }
 
public:
    ComputerClub(int _NumTables, const ClubTime& _Open, const ClubTime& _Close, int _Price) {       // конструктор по умолчанию
        NumberOfTables = _NumTables;        // число столов
        TimeOpen = _Open;       // время открытия
        TimeClose = _Close;     // время закрытия
        PricePerHour = _Price;  // цена за час

        for (int i = 1; i <= _NumTables; i++) {     
            ListTable.emplace_back(i, PricePerHour);    // заполняем список столов (нумерация + цена)
        }
    }

    void CurrentEvent(const Event& _Event) {        // запись события, происходящего сейчас
        CurrentTime = _Event.Time;      // запись времени сейчас
        OutputEvents.push_back(_Event); // в конец выходящих событий записываем _Event

        try {       // блок try, ловим исключения (если будут)
            switch (_Event.EventID) {       // рассматриваю несколько входящих сценариев
                case 1: ClientArrived(_Event); break;
                case 2: ClientPlaced(_Event); break;
                case 3: ClientWaiting(_Event); break;
                case 4: ClientLeft(_Event); break;
            }
        }   
        catch (const std::exception& Exc) {     // если EventUD != 1, 2, 3 || 4, то записываю ошибку с ID 13
            OutputEvents.emplace_back(Event(CurrentTime.ClubTimeToString() + " 13 " + Exc.what()));
        }
    }

    void EndDay() {     // конец дня
        std::vector<std::string> StayingClients;        // строка для клиентов, оставшихся в клубе в конце дня
        for (const auto& [Client, _] : Clients) StayingClients.push_back(Client);   // записываем всеъ клиентов в конец вектора
        std::sort(StayingClients.begin(), StayingClients.end());        // сортировка всех клиентов

        for (const auto& Client : StayingClients) {     
            OutputEvents.emplace_back(Event(TimeClose.ClubTimeToString() + " 11 " + Client));       // выписываю событие с ID 11 
            if (Clients[Client] != -1) {
                ListTable[Clients[Client]].StopUsing(TimeClose);        // для каждого оставшегося клиента заканчиваю пользование столом
            }
        }
    }

    void PrintResults() const {     // метод вывода результатов
        std::cout << TimeOpen << "\n";      // вывод начала рабочего дня
        for (const auto& _Event : OutputEvents) std::cout << _Event << "\n";        // вывод произошедших событий
        std::cout << TimeClose << "\n";     // вывод конца рабочего дня

        for (const auto& Table : ListTable) {               // для всех столов из списка
            int totalMinutes = Table.GetUsageTime();        // время, которое стол использовался в минутах
            ClubTime UsageTime(totalMinutes / 60, totalMinutes % 60);           // время пользования в формате ЧЧ:ММ
            std::cout << Table.GetTableID() << " " << Table.GetTotalIncome() << " " << UsageTime << "\n";   // вывод ID стола, выручки и времени пользования 
        }
    }
};

int main(int argc, char* argv[]) {      // функция main, принимает количество аргументов и вектор из аргументов
    if (argc != 2) {        // если количество не 2
        std::cerr << "Used: " << argv[0] << " <input_file>\n";      // вывод ошибки и конец программы 
        return 1;
    }

    std::ifstream file(argv[1]);        // запись файла в поток
    if (!file) {        // если файл не открывается, то вывод ошибки и конец работы
        std::cerr << "Error In Opening File\n";
        return 1;
    }

    std::vector<std::string> Strings;       // вектор строк (временная)
    std::string String;                     // строка (временная)

    while (std::getline(file, String)) {        // пока из файла получаем строку
        if (!String.empty())                    // если строка не пустая
            Strings.emplace_back(String);       // запись строки в вектор строк
    }

    try {
        if (Strings.size() < 3) {       // если число строк в файле меньше 3
            throw std::invalid_argument("Invalid Input");   // некорректный ввод
        }

        int NumberOfTables = std::stoi(Strings[0]);     // запись числа столов (в файле в первой строке)

        if (NumberOfTables <= 0) {      // если столов <= 0
            throw std::invalid_argument("Invalid Table's count");       // выброс исключения
        }

        std::istringstream Time(Strings[1]);        // поток для записи времени из файла
        std::string OpenTimeString;                 // время открытия из файла 
        std::string CloseTimeString;                // время закрытия из файла
        if (!(Time >> OpenTimeString >> CloseTimeString))       // если время не получено из файла
            throw std::invalid_argument("Invalid time of work");        // выброс исключения
        
        ClubTime OpenTime = ClubTime::FromString(OpenTimeString);       // перевод времени открытия в ClubTime
        ClubTime CloseTime = ClubTime::FromString(CloseTimeString);     // перевод времени закрытия в ClubTime
        if (CloseTime <= OpenTime)          // если время закрытия раньше, чем время открытия
            throw std::invalid_argument("Closes before opens");     // выброс исключения
        
        int Price = std::stoi(Strings[2]);      // цена, получена из строки, переведена в int
        if (Price <= 0)         // если цена отрицательна или равна 0
            throw std::invalid_argument("Invalid price");       // выброс исключения
        
        ComputerClub CLUB(NumberOfTables, OpenTime, CloseTime, Price);      // создаем объект ComputerClub

        for (size_t i = 3; i < Strings.size(); i++) {           // запись произошедших события в клубе из вектора строк
            Event Event(Strings[i]);
            CLUB.CurrentEvent(Event);
        }

        CLUB.EndDay();              // сценарий "Конец дня"
        CLUB.PrintResults();        // вывод результатов
    }
    catch (const std::exception& Exc) {     // ловим исключения и выводим их, если они есть, конец программы в таком случае
        std::cout << Exc.what() << std::endl;
        return 1;
    }

    return 0;           // возврат 0, если программа отработала корректно
}

