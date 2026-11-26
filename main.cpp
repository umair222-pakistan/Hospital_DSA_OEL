// Sirf yehi file compile hoti hai. Ye Appointment.cpp include karti hai, jo Patient.cpp include karti hai.
#include "Appointment.cpp"

int main() {
    PatientList patients;
    AppointmentQueue appointments;
    int choice = -1;

    // Pehle se saved patients aur appointments wapas load hoti hain
    patients.load();
    appointments.load();

    while (choice != 0) {
        cout << "\n===== Naveed Medical Complex =====\n"
             << "  1. Register patient\n"
             << "  2. View all patients\n"
             << "  3. View patient by ID\n"
             << "  4. Search patient by name\n"
             << "  5. Update patient\n"
             << "  6. Delete patient\n"
             << "  7. Make appointment\n"
             << "  8. View all appointments\n"
             << "  9. View appointment by ID\n"
             << " 10. View scheduled appointments\n"
             << " 11. View completed appointments\n"
             << " 12. View cancelled appointments\n"
             << " 13. View appointments of a month and year\n"
             << " 14. View appointments sorted by date\n"
             << " 15. Process next appointment\n"
             << " 16. Mark appointment as completed\n"
             << " 17. Cancel appointment\n"
             << "  0. Exit\n"
             << "Enter your choice: ";
        while (!(cin >> choice) || choice < 0 || choice > 17) {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "Invalid choice. Enter a number from 0 to 17: ";
        }
        cout << "\n";

        // In choices ke liye ID chahiye
        int id = 0;
        if (choice == 3 || choice == 5 || choice == 6 || choice == 9 || choice == 16 || choice == 17) {
            cout << "Enter ID: ";
            while (!(cin >> id) || id < 1) {
                cin.clear();
                cin.ignore(1000, '\n');
                cout << "Invalid ID. Enter a positive number: ";
            }
        }

        if (choice == 1) {
            Patient p;
            if (patients.input(p)) {      // false = pehle se registered, message dikh gaya
                patients.add(p);
                cout << "Patient registered. Patient ID: " << p.id << "\n";
            }
        }
        else if (choice == 2) patients.displayAll();
        else if (choice == 3) {
            if (!patients.displayById(id)) cout << "Patient ID " << id << " does not exist.\n";
        }
        else if (choice == 4) {
            string text;
            cout << "Enter name to search: ";
            getline(cin >> ws, text);
            patients.searchByName(text);
        }
        else if (choice == 5) {
            if (patients.update(id)) {
                appointments.refreshPatient(patients.find(id));
                cout << "Patient updated.\n";
            }
            else cout << "Patient ID " << id << " does not exist.\n";
        }
        else if (choice == 6) {
            if (!patients.exists(id)) cout << "Patient ID " << id << " does not exist.\n";
            else if (appointments.hasScheduledFor(id)) cout << "Cannot delete. This patient has scheduled appointments.\n";
            else {
                patients.remove(id);
                cout << "Patient deleted.\n";
            }
        }
        else if (choice == 7) {
            Appointment a(patients);
            if (appointments.enqueue(a)) cout << "Appointment made. Appointment ID: " << a.id << "\n";
            else cout << "Appointment not made.\n";
        }
        else if (choice == 8) appointments.displayAll();
        else if (choice == 9) {
            if (!appointments.displayById(id)) cout << "Appointment ID " << id << " does not exist.\n";
        }
        else if (choice == 10) appointments.displayByStatus("Scheduled");
        else if (choice == 11) appointments.displayByStatus("Completed");
        else if (choice == 12) appointments.displayByStatus("Cancelled");
        else if (choice == 13) {
            string month;
            int year;
            cout << "Enter month name (e.g. March): ";
            cin >> month;
            cout << "Enter year (2000-2100): ";
            while (!(cin >> year) || year < 2000 || year > 2100) {
                cin.clear();
                cin.ignore(1000, '\n');
                cout << "Invalid year. Enter a year from 2000 to 2100: ";
            }
            appointments.displayByMonth(month, year);
        }
        else if (choice == 14) appointments.displaySortedByDate();
        else if (choice == 15) appointments.processNext();
        else if (choice == 16) appointments.changeStatus(id, "Completed");
        else if (choice == 17) appointments.changeStatus(id, "Cancelled");

        // Koi bhi tabdeeli hui to dono files foran save ho jati hain
        if (choice == 1 || choice == 5 || choice == 6 || choice == 7 ||
            choice == 15 || choice == 16 || choice == 17) {
            patients.save();
            appointments.save();
        }
    }

    cout << "Goodbye.\n";
}