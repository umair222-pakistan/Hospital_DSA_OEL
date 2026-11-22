#include <iostream>
#include <string>
#include <iomanip>
#include <cctype>
#include <cstdlib>
#include <fstream>
using namespace std;

int nextPatientId = 1;

struct Patient {
    int id;
    string name, city, phone;

    // User se details leta hai. ID khud ba khud milti hai.
    Patient() {
        id = nextPatientId++;
        input();
    }

    // Tayyar values se patient banata hai ("Not found" patient ke liye)
    Patient(int i, string n, string c, string p) : id(i), name(n), city(c), phone(p) {}

    // Constructor aur update dono isay use karte hain, is liye checks sirf aik baar likhe hain
    void input() {
        bool ok;

        do {
            cout << "Enter name: ";
            getline(cin >> ws, name);
            ok = true;
            for (char c : name)
                if (!isalpha(c) && c != ' ') ok = false;
            if (!ok) cout << "Name can only have letters and spaces.\n";
        } while (!ok);

        do {
            cout << "Enter city: ";
            getline(cin >> ws, city);
            ok = true;
            for (char c : city)
                if (!isalpha(c) && c != ' ') ok = false;
            if (!ok) cout << "City can only have letters and spaces.\n";
        } while (!ok);

        do {
            cout << "Enter phone (10 digits): ";
            cin >> phone;
            ok = (phone.size() == 10);
            for (char c : phone)
                if (!isdigit(c)) ok = false;
            if (!ok) cout << "Phone must be exactly 10 digits.\n";
        } while (!ok);
    }

    static void header() {
        cout << left << setw(6) << "ID" << setw(24) << "Name"
             << setw(18) << "City" << setw(12) << "Phone" << "\n"
             << string(60, '-') << "\n";
    }

    void display() {
        cout << left << setw(6) << id << setw(24) << name
             << setw(18) << city << setw(12) << phone << "\n";
    }
};

struct PatientNode {
    Patient data;
    PatientNode* next;
    PatientNode(Patient p) : data(p), next(nullptr) {}
};

// Patients ki linked list
struct PatientList {
    PatientNode* head = nullptr;
    PatientNode* tail = nullptr;
    int count = 0;

    ~PatientList() {
        while (head) {
            PatientNode* temp = head;
            head = head->next;
            delete temp;
        }
    }

    // Agar ID pehle se majood ho to false return karta hai
    bool add(Patient p) {
        if (exists(p.id)) return false;
        PatientNode* node = new PatientNode(p);
        if (!head) head = tail = node;
        else { tail->next = node; tail = node; }
        count++;
        return true;
    }

    // Patient majood hai ya nahi, ye check karta hai
    bool exists(int id) {
        for (PatientNode* cur = head; cur; cur = cur->next)
            if (cur->data.id == id) return true;
        return false;
    }

    // Hamesha aisi cheez return karta hai jo display ho sake. ID 0 ka matlab patient nahi mila.
    Patient find(int id) {
        for (PatientNode* cur = head; cur; cur = cur->next)
            if (cur->data.id == id) return cur->data;
        return Patient(0, "Not found", "N/A", "N/A");
    }

    bool update(int id) {
        for (PatientNode* cur = head; cur; cur = cur->next)
            if (cur->data.id == id) {
                cur->data.input();
                return true;
            }
        return false;
    }

    bool remove(int id) {
        PatientNode* prev = nullptr;
        PatientNode* cur = head;
        while (cur && cur->data.id != id) {
            prev = cur;
            cur = cur->next;
        }
        if (!cur) return false;
        if (prev) prev->next = cur->next;
        else head = cur->next;
        if (cur == tail) tail = prev;
        delete cur;
        count--;
        return true;
    }

    void displayAll() {
        if (count == 0) {
            cout << "No patients registered yet.\n";
            return;
        }
        Patient::header();
        for (PatientNode* cur = head; cur; cur = cur->next) cur->data.display();
    }

    bool displayById(int id) {
        Patient p = find(id);
        if (p.id == 0) return false;
        Patient::header();
        p.display();
        return true;
    }

    // Naam ka kuch hissa kaafi hai, capital ya small letters se farq nahi parta
    void searchByName(string key) {
        for (char& c : key) c = tolower(c);
        bool found = false;
        for (PatientNode* cur = head; cur; cur = cur->next) {
            string n = cur->data.name;
            for (char& c : n) c = tolower(c);
            if (n.find(key) != string::npos) {
                if (!found) Patient::header();
                cur->data.display();
                found = true;
            }
        }
        if (!found) cout << "No patient found with that name.\n";
    }

    // Counter aur saare patients file mein save karta hai
    void save() {
        ofstream file("patients.txt");
        file << nextPatientId << "\n";
        for (PatientNode* cur = head; cur; cur = cur->next)
            file << cur->data.id << "|" << cur->data.name << "|"
                 << cur->data.city << "|" << cur->data.phone << "\n";
    }

    // Program shuru hone par counter aur saare patients file se wapas laata hai
    void load() {
        ifstream file("patients.txt");
        if (!file) return;   // file nahi hai to khali shuru hota hai

        string line, id, name, city, phone;
        getline(file, line);
        nextPatientId = atoi(line.c_str());
        if (nextPatientId < 1) nextPatientId = 1;

        while (getline(file, id, '|')) {
            getline(file, name, '|');
            getline(file, city, '|');
            getline(file, phone);
            int pid = atoi(id.c_str());
            if (pid > 0) {
                add(Patient(pid, name, city, phone));
                if (pid >= nextPatientId) nextPatientId = pid + 1;
            }
        }
    }
};
