#include <iostream>
#include <string>
#include <iomanip>
#include <cctype>
#include <cstdlib>
#include <fstream>
using namespace std;

int nextPatientId = 1;

struct Patient {
    int id = 0;                          // 0 ka matlab abhi ID nahi mili
    string name = "N/A", city = "N/A", phone = "N/A";

    void display() {
        cout << "ID: " << id << " | Name: " << name
             << " | City: " << city << " | Phone: " << phone << "\n";
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

    // Naye patient (id 0) ko ID yahin milti hai. ID pehle se majood ho to false return karta hai
    bool add(Patient& p) {
        if (p.id == 0) p.id = nextPatientId++;
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
        Patient notFound;                // id 0 ka matlab patient nahi mila
        notFound.name = "Not found";
        return notFound;
    }

    // Register aur update dono yehi aik input method use karte hain.
    // p.id == 0 ka matlab naya patient hai.
    // true  = details tayyar hain.
    // false = patient pehle se registered tha (message yahin dikha diya jata hai).
    bool input(Patient& p) {
        bool ok;

        do {
            cout << "Enter name: ";
            getline(cin >> ws, p.name);
            ok = true;
            for (char c : p.name)
                if (!isalpha(c) && c != ' ') ok = false;
            if (!ok) cout << "Name can only have letters and spaces.\n";
        } while (!ok);

        do {
            cout << "Enter city: ";
            getline(cin >> ws, p.city);
            ok = true;
            for (char c : p.city)
                if (!isalpha(c) && c != ' ') ok = false;
            if (!ok) cout << "City can only have letters and spaces.\n";
        } while (!ok);

        do {
            cout << "Enter phone (10 digits): ";
            cin >> p.phone;
            ok = (p.phone.size() == 10);
            for (char c : p.phone)
                if (!isdigit(c)) ok = false;
            if (!ok) {
                cout << "Phone must be exactly 10 digits.\n";
                continue;
            }

            // Ye phone kisi aur patient ke paas hai? (apna record skip hota hai)
            for (PatientNode* cur = head; cur; cur = cur->next) {
                if (cur->data.id != p.id && cur->data.phone == p.phone) {
                    string oldName = cur->data.name, newName = p.name;
                    string oldCity = cur->data.city, newCity = p.city;
                    for (char& c : oldName) c = tolower(c);
                    for (char& c : newName) c = tolower(c);
                    for (char& c : oldCity) c = tolower(c);
                    for (char& c : newCity) c = tolower(c);

                    // Naya patient, same phone, same naam aur same city: pehle se registered hai
                    if (p.id == 0 && oldName == newName && oldCity == newCity) {
                        cout << "You have already been registered. Your ID is " << cur->data.id << ".\n";
                        return false;
                    }
                    cout << "This phone number already exists. Please input another phone.\n";
                    ok = false;
                    break;
                }
            }
        } while (!ok);

        return true;
    }

    bool update(int id) {
        for (PatientNode* cur = head; cur; cur = cur->next)
            if (cur->data.id == id) {
                input(cur->data);
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
        for (PatientNode* cur = head; cur; cur = cur->next) cur->data.display();
    }

    bool displayById(int id) {
        Patient p = find(id);
        if (p.id == 0) return false;
        p.display();
        return true;
    }

    // Poora naam bilkul same hona chahiye, capital ya small letters se farq nahi parta
    void searchByName(string key) {
        for (char& c : key) c = tolower(c);
        bool found = false;
        for (PatientNode* cur = head; cur; cur = cur->next) {
            string n = cur->data.name;
            for (char& c : n) c = tolower(c);
            if (n == key) {
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
                Patient p;
                p.id = pid;
                p.name = name;
                p.city = city;
                p.phone = phone;
                add(p);
                if (pid >= nextPatientId) nextPatientId = pid + 1;
            }
        }
    }
};