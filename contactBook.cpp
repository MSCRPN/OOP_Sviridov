#include "contactBook.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iostream>
#include <cctype>

using namespace std;

ContactBook::ContactBook(const string& filename) : filename(filename) {}

void ContactBook::addContact(const Contact& contact) {
    for (const auto& existingContact : contacts) {
        if (existingContact.getEmail() == contact.getEmail()) {
            throw invalid_argument("Contact with email " + contact.getEmail() + " already exists");
        }
    }
    contacts.push_back(contact);
    saveToFile();
}

bool ContactBook::removeContact(const string& email) {
    auto it = remove_if(contacts.begin(), contacts.end(),
        [&email](const Contact& contact) {
            return contact.getEmail() == email;
        });
    
    if (it != contacts.end()) {
        contacts.erase(it, contacts.end());
        saveToFile();
        return true;
    }
    return false;
}

vector<Contact> ContactBook::findContactsByFilter(
    const string& firstName,
    const string& lastName, 
    const string& patronymic,
    const string& email,
    const string& phone) const {
    
    vector<Contact> results;
    
    for (const auto& contact : contacts) {
        bool match = true;
        
        if (!firstName.empty()) {
            string firstLower = contact.getFirstName();
            string searchLower = firstName;
            transform(firstLower.begin(), firstLower.end(), firstLower.begin(), ::tolower);
            transform(searchLower.begin(), searchLower.end(), searchLower.begin(), ::tolower);
            if (firstLower.find(searchLower) == string::npos) match = false;
        }
        
        if (!lastName.empty() && match) {
            string lastLower = contact.getLastName();
            string searchLower = lastName;
            transform(lastLower.begin(), lastLower.end(), lastLower.begin(), ::tolower);
            transform(searchLower.begin(), searchLower.end(), searchLower.begin(), ::tolower);
            if (lastLower.find(searchLower) == string::npos) match = false;
        }
        
        if (!patronymic.empty() && match) {
            string patronymicLower = contact.getPatronymic();
            string searchLower = patronymic;
            transform(patronymicLower.begin(), patronymicLower.end(), patronymicLower.begin(), ::tolower);
            transform(searchLower.begin(), searchLower.end(), searchLower.begin(), ::tolower);
            if (patronymicLower.find(searchLower) == string::npos) match = false;
        }
        
        if (!email.empty() && match) {
            string emailLower = contact.getEmail();
            string searchLower = email;
            transform(emailLower.begin(), emailLower.end(), emailLower.begin(), ::tolower);
            transform(searchLower.begin(), searchLower.end(), searchLower.begin(), ::tolower);
            if (emailLower.find(searchLower) == string::npos) match = false;
        }
        
        if (!phone.empty() && match) {
            bool phoneMatch = false;
            for (const auto& contactPhone : contact.getPhones()) {
                if (contactPhone.find(phone) != string::npos) {
                    phoneMatch = true;
                    break;
                }
            }
            if (!phoneMatch) match = false;
        }
        
        if (match) {
            results.push_back(contact);
        }
    }
    return results;
}

vector<Contact> ContactBook::getAllContacts() const {
    return contacts;
}

bool ContactBook::loadFromFile() {
    ifstream file(filename);
    if (!file.is_open()) return false;
    
    contacts.clear();
    string line;
    int lineNum = 0;
    
    while (getline(file, line)) {
        lineNum++;
        if (line.empty()) continue;
        
        stringstream ss(line);
        string firstName, lastName, patronymic, address, birthDate, email, phonesStr;
        
        if (getline(ss, firstName, ';') &&
            getline(ss, lastName, ';') &&
            getline(ss, patronymic, ';') &&
            getline(ss, address, ';') &&
            getline(ss, birthDate, ';') &&
            getline(ss, email, ';') &&
            getline(ss, phonesStr)) {
            
            try {
                vector<string> phones;
                stringstream phonesStream(phonesStr);
                string phone;
                while (getline(phonesStream, phone, ',')) {
                    phones.push_back(phone);
                }
                
                if (phones.empty()) {
                    cout << "Skipping contact without phones at line " << lineNum << endl;
                    continue;
                }
                
                ContactBuilder builder;
                Contact contact = builder.setFirstName(firstName)
                    .setLastName(lastName)
                    .setPatronymic(patronymic)
                    .setAddress(address)
                    .setBirthDate(birthDate)
                    .setEmail(email)
                    .setPhones(phones)
                    .build();
                
                bool duplicate = false;
                for (const auto& existing : contacts) {
                    if (existing.getEmail() == contact.getEmail()) {
                        cout << "Skipping duplicate email at line " << lineNum << endl;
                        duplicate = true;
                        break;
                    }
                }
                
                if (!duplicate) {
                    contacts.push_back(contact);
                }
                
            } catch (const exception& e) {
                cout << "Skipping invalid data at line " << lineNum << ": " << e.what() << endl;
            }
        } else {
            cout << "Skipping invalid format at line " << lineNum << endl;
        }
    }
    
    file.close();
    return true;
}

bool ContactBook::saveToFile() const {
    ofstream file(filename);
    if (!file.is_open()) {
        cout << "ERROR: Cannot open file " << filename << " for writing!" << endl;
        return false;
    }
    
    for (const auto& contact : contacts) {
        file << contact.toFileString() << endl;
    }
    
    file.close();
    return true;
}

size_t ContactBook::getCount() const {
    return contacts.size();
}

void ContactBook::sortByLastName() {
    sort(contacts.begin(), contacts.end(),
        [](const Contact& a, const Contact& b) {
            return a.getLastName() < b.getLastName();
        });
}