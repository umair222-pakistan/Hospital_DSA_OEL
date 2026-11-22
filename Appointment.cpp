#include "Patient.cpp"

// Mahino ke naamon ki sirf aik list
string months[12] = {"January", "February", "March", "April", "May", "June",
                     "July", "August", "September", "October", "November", "December"};

int nextAppointmentId = 1;

struct Appointment {
    int id;
    Patient patient;
    int day, month, year;
    string status;   // Scheduled, Completed ya Cancelled

    // User se patient aur date leta hai.
    // Agar appointment na bane to ID 0 rehti hai aur status "Not made" hota hai.
    Appointment(PatientList& patients)
        : id(0), patient(0, "N/A", "N/A", "N/A"), day(1), month(1), year(2000), status("Not made") {

        if (patients.count == 0) {
            cout << "No patients registered. Register a patient first.\n";
            return;
        }

        int pid;
        cout << "Enter patient ID (0 to cancel): ";
        while (!(cin >> pid) || (pid != 0 && !patients.exists(pid))) {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "Patient not found. Enter a valid patient ID (0 to cancel): ";
        }
        if (pid == 0) return;
        patient = patients.find(pid);

        cout << "Enter year (2000-2100): ";
        while (!(cin >> year) || year < 2000 || year > 2100) {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "Invalid year. Enter a year from 2000 to 2100: ";
        }

        cout << "Enter month (1-12): ";
        while (!(cin >> month) || month < 1 || month > 12) {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "Invalid month. Enter a number from 1 to 12: ";
        }

        // Is mahine ka akhri din, leap year samet
        int maxDay = 31;
        if (month == 4 || month == 6 || month == 9 || month == 11) maxDay = 30;
        else if (month == 2) maxDay = ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0) ? 29 : 28;

        cout << "Enter day (1-" << maxDay << "): ";
        while (!(cin >> day) || day < 1 || day > maxDay) {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "Invalid day. Enter a day from 1 to " << maxDay << ": ";
        }

        id = nextAppointmentId++;
        status = "Scheduled";
    }

    // Tayyar values se appointment banata hai (file se load karne ke liye)
    Appointment(int i, Patient p, int d, int m, int y, string s)
        : id(i), patient(p), day(d), month(m), year(y), status(s) {}

    static void header() {
        cout << left << setw(6) << "ID" << setw(10) << "Patient"
             << setw(22) << "Patient Name" << setw(18) << "Date"
             << setw(12) << "Status" << "\n"
             << string(68, '-') << "\n";
    }

    void display() {
        string date = (day < 10 ? "0" : "") + to_string(day) + " " + months[month - 1] + " " + to_string(year);
        cout << left << setw(6) << id << setw(10) << patient.id
             << setw(22) << patient.name << setw(18) << date
             << setw(12) << status << "\n";
    }
};

struct AppointmentNode {
    Appointment data;
    AppointmentNode* next;
    AppointmentNode(Appointment a) : data(a), next(nullptr) {}
};

// Linked list queue. Nayi appointments rear par judti hain, is liye arrival ka
// order barqarar rehta hai. Completed aur cancelled appointments bhi queue mein
// rehti hain taake unhein dikhaya ja sake. Display methods sirf front se chalte
// hain aur order kabhi nahi badalte.
struct AppointmentQueue {
    AppointmentNode* front = nullptr;
    AppointmentNode* rear = nullptr;
    int count = 0;

    ~AppointmentQueue() {
        while (front) {
            AppointmentNode* temp = front;
            front = front->next;
            delete temp;
        }
    }

    // Appointment na bani ho (ID 0) to false return karta hai
    bool enqueue(Appointment a) {
        if (a.id == 0) return false;
        AppointmentNode* node = new AppointmentNode(a);
        if (!front) front = rear = node;
        else { rear->next = node; rear = node; }
        count++;
        return true;
    }

    bool hasScheduledFor(int patientId) {
        for (AppointmentNode* cur = front; cur; cur = cur->next)
            if (cur->data.patient.id == patientId && cur->data.status == "Scheduled") return true;
        return false;
    }

    // Patient update hone ke baad appointments mein uski details bhi update karta hai
    void refreshPatient(Patient p) {
        for (AppointmentNode* cur = front; cur; cur = cur->next)
            if (cur->data.patient.id == p.id) cur->data.patient = p;
    }

    // Sab se pehle wali scheduled appointment ko complete karta hai
    void processNext() {
        if (count == 0) {
            cout << "The appointment queue is empty.\n";
            return;
        }
        for (AppointmentNode* cur = front; cur; cur = cur->next)
            if (cur->data.status == "Scheduled") {
                cur->data.status = "Completed";
                cout << "Appointment " << cur->data.id << " marked as completed.\n";
                return;
            }
        cout << "No scheduled appointments are waiting.\n";
    }

    // newStatus "Completed" ya "Cancelled" hota hai
    void changeStatus(int id, string newStatus) {
        for (AppointmentNode* cur = front; cur; cur = cur->next)
            if (cur->data.id == id) {
                if (cur->data.status != "Scheduled")
                    cout << "Appointment " << id << " is already " << cur->data.status << ".\n";
                else {
                    cur->data.status = newStatus;
                    cout << "Appointment " << id << " is now " << newStatus << ".\n";
                }
                return;
            }
        cout << "Appointment ID " << id << " does not exist.\n";
    }

    void displayAll() {
        if (count == 0) {
            cout << "The appointment queue is empty.\n";
            return;
        }
        Appointment::header();
        for (AppointmentNode* cur = front; cur; cur = cur->next) cur->data.display();
    }

    bool displayById(int id) {
        for (AppointmentNode* cur = front; cur; cur = cur->next)
            if (cur->data.id == id) {
                Appointment::header();
                cur->data.display();
                return true;
            }
        return false;
    }

    void displayByStatus(string s) {
        bool found = false;
        for (AppointmentNode* cur = front; cur; cur = cur->next)
            if (cur->data.status == s) {
                if (!found) Appointment::header();
                cur->data.display();
                found = true;
            }
        if (!found) cout << "No " << s << " appointments found.\n";
    }

    // Mahina naam se diya jata hai jaise "March" (capital ya small dono chalte hain)
    void displayByMonth(string monthText, int year) {
        for (char& c : monthText) c = tolower(c);
        monthText[0] = toupper(monthText[0]);

        int m = 0;
        for (int i = 0; i < 12; i++)
            if (months[i] == monthText) m = i + 1;
        if (m == 0) {
            cout << "Invalid month name.\n";
            return;
        }

        bool found = false;
        for (AppointmentNode* cur = front; cur; cur = cur->next)
            if (cur->data.month == m && cur->data.year == year) {
                if (!found) Appointment::header();
                cur->data.display();
                found = true;
            }
        if (!found) cout << "No appointments found in " << months[m - 1] << " " << year << ".\n";
    }

    // Pointers par bubble sort, is liye queue ka order nahi badalta
    void displaySortedByDate() {
        if (count == 0) {
            cout << "The appointment queue is empty.\n";
            return;
        }
        Appointment** arr = new Appointment*[count];
        int i = 0;
        for (AppointmentNode* cur = front; cur; cur = cur->next) arr[i++] = &cur->data;

        for (int a = 0; a < count - 1; a++)
            for (int b = 0; b < count - 1 - a; b++) {
                int x = arr[b]->year * 10000 + arr[b]->month * 100 + arr[b]->day;
                int y = arr[b + 1]->year * 10000 + arr[b + 1]->month * 100 + arr[b + 1]->day;
                if (x > y) {
                    Appointment* temp = arr[b];
                    arr[b] = arr[b + 1];
                    arr[b + 1] = temp;
                }
            }

        Appointment::header();
        for (int k = 0; k < count; k++) arr[k]->display();
        delete[] arr;
    }

    // Counter aur saari appointments file mein save karta hai (queue ke order mein)
    void save() {
        ofstream file("appointments.txt");
        file << nextAppointmentId << "\n";
        for (AppointmentNode* cur = front; cur; cur = cur->next) {
            Appointment a = cur->data;
            file << a.id << "|" << a.patient.id << "|" << a.patient.name << "|"
                 << a.patient.city << "|" << a.patient.phone << "|"
                 << a.day << "|" << a.month << "|" << a.year << "|" << a.status << "\n";
        }
    }

    // Program shuru hone par counter aur saari appointments file se wapas laata hai
    void load() {
        ifstream file("appointments.txt");
        if (!file) return;   // file nahi hai to khali shuru hota hai

        string line, id, pid, name, city, phone, day, month, year, status;
        getline(file, line);
        nextAppointmentId = atoi(line.c_str());
        if (nextAppointmentId < 1) nextAppointmentId = 1;

        while (getline(file, id, '|')) {
            getline(file, pid, '|');
            getline(file, name, '|');
            getline(file, city, '|');
            getline(file, phone, '|');
            getline(file, day, '|');
            getline(file, month, '|');
            getline(file, year, '|');
            getline(file, status);
            int aid = atoi(id.c_str());
            int m = atoi(month.c_str());
            if (aid > 0 && m >= 1 && m <= 12) {
                Patient p(atoi(pid.c_str()), name, city, phone);
                enqueue(Appointment(aid, p, atoi(day.c_str()), m, atoi(year.c_str()), status));
                if (aid >= nextAppointmentId) nextAppointmentId = aid + 1;
            }
        }
    }
};
