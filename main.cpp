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

class ClubTime {        
private: 
    int TotalMin;       

public:
    ClubTime() {
        TotalMin = 0;   
    }

    ClubTime(int Hours, int Minutes) {     
        TotalMin = Hours * 60 + Minutes;
    }

    static ClubTime FromString(const std::string& str_time) {      
        if ((str_time.size() != 5) || (str_time[2] != ':')) {     
            throw std::invalid_argument("ErrorInTimeInput\n");
        }

        int Hours = std::stoi(str_time.substr(0, 2));               
        int Minutes = std::stoi(str_time.substr(3, 2));             

        if ((Hours < 0) || (Hours > 23) || (Minutes < 0) || (Minutes > 59)) {
            throw std::invalid_argument("WrongTime!\n");            
        }

        return ClubTime(Hours, Minutes);
    }

    std::string ClubTimeToString() const {
        std::ostringstream oss;
        oss << std::setfill('0') << std::setw(2) << (TotalMin / 60) << ":" << std::setw(2) << (TotalMin % 60);
        return oss.str();
    }

    int GetTotalMin() const { return TotalMin; }

    friend std::ostream& operator<<(std::ostream& os, const ClubTime& _ClubTime) {                
        os << _ClubTime.ClubTimeToString();
        return os;
    }

    bool operator<(const ClubTime _ClubTime) const { return (TotalMin < _ClubTime.TotalMin); }
    bool operator<=(const ClubTime _ClubTime) const { return (TotalMin <= _ClubTime.TotalMin); }
    bool operator>(const ClubTime _ClubTime) const { return (TotalMin > _ClubTime.TotalMin); }
    bool operator>=(const ClubTime _ClubTime) const { return (TotalMin >= _ClubTime.TotalMin); }
};

class Tables {                  
private:
    int TableID;                
    int Price;                  
    bool Occupancy;             
    std::string CurrentUser;    
    ClubTime UseStart;          
    int TotalUseTime;           
    int TotalIncome;            
public:
    Tables(int _TableID, int _Price) {            
        TableID = _TableID;
        Occupancy = false;
        TotalUseTime = 0;
        TotalIncome = 0;
        Price = _Price;
    }

    void StartUsing(const ClubTime& _StartTime, const std::string& _UserName) {    
        Occupancy = true;           
        UseStart = _StartTime;      
        CurrentUser = _UserName;    
    }

    void StopUsing(const ClubTime& _EndTime) {         
        if (!Occupancy) {
            throw std::invalid_argument("TableIsNotOccupied\n");    
        };
        Occupancy = false;  
        int UseTime = _EndTime.GetTotalMin() - UseStart.GetTotalMin();  
        TotalUseTime += UseTime;        

        int UsingHours = (UseTime + 59) / 60; // Исправлено округление
        TotalIncome += UsingHours * Price;

        CurrentUser = "";
    }

    bool IsOccupied() const { return Occupancy; }        
    int GetTableID() const { return TableID; }           
    int GetTotalIncome() const { return TotalIncome; }   
    int GetUsageTime() const { return TotalUseTime; }    
};

class Event {       
public:
    ClubTime Time;              
    int EventID;                
    std::string ClientName;     
    int TableID;                
    std::string ErrorMessage;   

    Event() = delete;

    Event(const std::string& _Event) {       
        std::istringstream iss(_Event);                 
        std::string Temp;                               

        iss >> Temp >> EventID;     
        Time = ClubTime::FromString(Temp);      

        switch (EventID)            
        {
        case 1: case 3: case 4: case 11:    
            if (!(iss >> ClientName)) {
                throw std::invalid_argument("Incorrect Client Name input for ID = 1\n");
            }
            break;
        case 2: case 12:   
            if (!(iss >> ClientName)) {
                throw std::invalid_argument("Incorrect Client Name input for ID = 2\n");
            }
            if ((!(iss >> TableID)) || (TableID <= 0)) {
                throw std::invalid_argument("Incorrect TableID input for ID = 2\n");
            }
            break;
        case 13: {        
            std::getline(iss >> std::ws, ErrorMessage);
            size_t first = ErrorMessage.find_first_not_of(' ');
            if (first != std::string::npos) {
                ErrorMessage = ErrorMessage.substr(first);
            }
            break;
        }
        default:   
            throw std::invalid_argument("Incorrect Input Event ID: " + std::to_string(EventID));
        }

        std::string extra;  
        if (iss >> extra) {
            throw std::invalid_argument("Too many arguments for ID: " + std::to_string(EventID));
        }
    }

    friend std::ostream& operator<<(std::ostream& os, const Event& _Event) {      
        os << _Event.Time.ClubTimeToString() << " " << _Event.EventID;
        switch(_Event.EventID)
        {
        case 1: case 2: case 3: case 4: case 11: case 12:               
            os << " " << _Event.ClientName;
            if (_Event.EventID == 12) {      
                os << " " << _Event.TableID;
            }
            break;
        case 13:
            os << " " << _Event.ErrorMessage;       
            break;
        default:
            throw std::invalid_argument("Incorrect Output Event ID: " + std::to_string(_Event.EventID));
        }
        return os;
    }
};

class ComputerClub {        
private:
    int NumberOfTables;     
    ClubTime TimeOpen;          
    ClubTime TimeClose;         
    ClubTime CurrentTime;
    int PricePerHour;       

    std::vector<Tables> ListTable;         
    std::vector<Event> OutputEvents;       
    std::queue<std::string> Waiting;        
    std::map<std::string, int> Clients;    

    void ClientArrived(const Event& _Event) {
        if (_Event.Time < TimeOpen || _Event.Time > TimeClose) { // Исправлено условие
            throw std::runtime_error("NotOpenYet");
        }
        if (Clients.count(_Event.ClientName)) {
            throw std::runtime_error("YouShallNotPass");
        }
        Clients[_Event.ClientName] = -1;        
    }

    void ClientPlaced(const Event& _Event) {
        if (!Clients.count(_Event.ClientName)) 
            throw std::runtime_error("ClientUnknown");
        
        int TableIndex = _Event.TableID - 1;

        if (TableIndex < 0 || TableIndex >= NumberOfTables)
            throw std::invalid_argument("Invalid Table ID");
        if (ListTable[TableIndex].IsOccupied()) 
            throw std::runtime_error("PlaceIsBusy"); // Исправлена опечатка

        if (Clients[_Event.ClientName] != -1) {
            ListTable[Clients[_Event.ClientName]].StopUsing(_Event.Time);
        }

        ListTable[TableIndex].StartUsing(_Event.Time, _Event.ClientName);
        Clients[_Event.ClientName] = TableIndex;
    }

    void ClientWaiting(const Event& _Event) {
        if (!Clients.count(_Event.ClientName)) 
            throw std::runtime_error("ClientUnknown");
        
        bool IsTableFree = false;
        for (const auto& table : ListTable) {
            if (!table.IsOccupied()) {
                IsTableFree = true;
                break;
            }
        }

        if (IsTableFree)
            throw std::runtime_error("ICanWaitNoLonger!");
        
        if (Waiting.size() >= static_cast<size_t>(NumberOfTables)) { // Исправлено условие
            OutputEvents.emplace_back(Event(CurrentTime.ClubTimeToString() + " 11 " + _Event.ClientName));
            Clients.erase(_Event.ClientName);
        }
        else {
            Waiting.push(_Event.ClientName);
        }
    }

    void ClientLeft(const Event& _Event) {
        if (!Clients.count(_Event.ClientName))
            throw std::runtime_error("ClientUnknown");
        
        if (Clients[_Event.ClientName] != -1) {
            int TableIndex = Clients[_Event.ClientName];
            ListTable[TableIndex].StopUsing(_Event.Time);

            if (!Waiting.empty()) {
                std::string NextClient = Waiting.front();
                Waiting.pop();
                ListTable[TableIndex].StartUsing(_Event.Time, NextClient);
                Clients[NextClient] = TableIndex;
                OutputEvents.emplace_back(Event(CurrentTime.ClubTimeToString() + " 12 " + NextClient + " " + std::to_string(TableIndex + 1)));
            }
        }
        Clients.erase(_Event.ClientName); // Удаление после обработки
    }
 
public:
    ComputerClub(int _NumTables, const ClubTime& _Open, const ClubTime& _Close, int _Price) {
        NumberOfTables = _NumTables;        
        TimeOpen = _Open;
        TimeClose = _Close;
        PricePerHour = _Price;

        for (int i = 1; i <= _NumTables; i++) {  
            ListTable.emplace_back(i, PricePerHour);
        }
    }

    void CurrentEvent(const Event& _Event) {
        CurrentTime = _Event.Time;
        OutputEvents.push_back(_Event);

        try {
            switch (_Event.EventID) {
                case 1: ClientArrived(_Event); break;
                case 2: ClientPlaced(_Event); break;
                case 3: ClientWaiting(_Event); break;
                case 4: ClientLeft(_Event); break;
            }
        }
        catch (const std::exception& Exc) {
            OutputEvents.emplace_back(Event(CurrentTime.ClubTimeToString() + " 13 " + Exc.what()));
        }
    }

    void EndDay() {
        std::vector<std::string> StayingClients;
        for (const auto& [Client, _] : Clients) StayingClients.push_back(Client);
        std::sort(StayingClients.begin(), StayingClients.end());

        for (const auto& Client : StayingClients) {
            OutputEvents.emplace_back(Event(TimeClose.ClubTimeToString() + " 11 " + Client));
            if (Clients[Client] != -1) {
                ListTable[Clients[Client]].StopUsing(TimeClose);
            }
        }
    }

    void PrintResults() const {
        std::cout << TimeOpen << "\n";
        for (const auto& _Event : OutputEvents) std::cout << _Event << "\n";
        std::cout << TimeClose << "\n";

        for (const auto& Table : ListTable) {
            int totalMinutes = Table.GetUsageTime();
            ClubTime UsageTime(totalMinutes / 60, totalMinutes % 60);
            std::cout << Table.GetTableID() << " " << Table.GetTotalIncome() << " " << UsageTime << "\n";
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