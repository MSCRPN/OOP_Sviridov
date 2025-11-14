#include "consoleUI.h"
#include <iostream>
#include <limits>

using namespace std;

ConsoleUI::ConsoleUI(ContactBook& book) : contactBook(book) {}

void ConsoleUI::run() {
    contactBook.loadFromFile();
    cout << "Total contacts: " << contactBook.getCount() << endl;
    
    while (true) {
        showMainMenu();
        int choice = getIntInput("Select option: ");
        
        switch (choice) {
            case 1: showAllContacts(); break;
            case 2: addContact(); break;
            case 3: searchContacts(); break;
            case 4: removeContact(); break;
            case 5: 
                cout << "Goodbye!" << endl;
                return;
            default:
                cout << "Invalid option!" << endl;
        }
    }
}

void ConsoleUI::showMainMenu() {
    cout << "\n=== CONTACT BOOK ===" << endl;
    cout << "1. Show all contacts" << endl;
    cout << "2. Add contact" << endl;
    cout << "3. Search contacts" << endl;
    cout << "4. Remove contact" << endl;
    cout << "5. Exit" << endl;
}

void ConsoleUI::showAllContacts() {
    cout << "\n=== ALL CONTACTS ===" << endl;
    auto contacts = contactBook.getAllContacts();
    contactBook.sortByLastName();
    
    if (contacts.empty()) {
        cout << "No contacts found." << endl;
        return;
    }
    
    for (size_t i = 0; i < contacts.size(); ++i) {
        cout << i + 1 << ". " << contacts[i].toString() << endl;
    }
}

void ConsoleUI::addContact() {
    cout << "\n=== ADD CONTACT ===" << endl;
    
    string firstName = getInput("Enter first name: ");
    string lastName = getInput("Enter last name: ");
    string patronymic = getInput("Enter patronymic (optional): ");
    string address = getInput("Enter address (optional): ");
    string birthDate = getInput("Enter birth date YYYY-MM-DD (optional): ");
    string email = getInput("Enter email: ");
    string phonesInput = getInput("Enter phones (comma separated): ");
    
    vector<string> phones;
    stringstream ss(phonesInput);
    string phone;
    while (getline(ss, phone, ',')) {
        string trimmedPhone = phone;
        size_t start = trimmedPhone.find_first_not_of(" \t");
        size_t end = trimmedPhone.find_last_not_of(" \t");
        if (start != string::npos && end != string::npos) {
            trimmedPhone = trimmedPhone.substr(start, end - start + 1);
        }
        if (!trimmedPhone.empty()) {
            phones.push_back(trimmedPhone);
        }
    }
    
    try {
        Contact contact = ContactBuilder()
            .setFirstName(firstName)
            .setLastName(lastName)
            .setPatronymic(patronymic)
            .setAddress(address)
            .setBirthDate(birthDate)
            .setEmail(email)
            .setPhones(phones)
            .build();
        
        contactBook.addContact(contact);
        cout << "Contact added successfully!" << endl;
        
    } catch (const exception& e) {
        cout << "Error creating contact: " << e.what() << endl;
        cout << "Contact not added. Returning to main menu." << endl;
    }
}

void ConsoleUI::searchContacts() {
    cout << "\n=== SEARCH CONTACTS ===" << endl;
    cout << "Enter search criteria (leave empty to skip field):" << endl;
    
    string firstName = getInput("First name: ");
    string lastName = getInput("Last name: ");
    string patronymic = getInput("Patronymic: ");
    string email = getInput("Email: ");
    string phone = getInput("Phone: ");
    
    auto results = contactBook.findContactsByFilter(firstName, lastName, patronymic, email, phone);
    
    cout << "\nFound " << results.size() << " contacts:" << endl;
    
    if (results.empty()) {
        cout << "No contacts found matching your criteria." << endl;
        return;
    }
    
    for (size_t i = 0; i < results.size(); ++i) {
        cout << i + 1 << ". " << results[i].toString() << endl;
    }
}

void ConsoleUI::removeContact() {
    cout << "\n=== REMOVE CONTACT ===" << endl;
    string email = getInput("Enter email of contact to remove: ");
    
    if (contactBook.removeContact(email)) {
        cout << "Contact removed successfully!" << endl;
    } else {
        cout << "Contact not found!" << endl;
    }
}

string ConsoleUI::getInput(const string& prompt) {
    string input;
    cout << prompt;
    getline(cin, input);
    return input;
}

int ConsoleUI::getIntInput(const string& prompt) {
    int value;
    while (true) {
        cout << prompt;
        cin >> value;
        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input! Please enter a number." << endl;
        } else {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return value;
        }
    }
}