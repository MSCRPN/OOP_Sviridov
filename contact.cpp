#include "contact.h"
#include <sstream>

using namespace std;

Contact::Contact(const string& firstName, const string& lastName,
                 const string& email, const string& phone) {
    this->firstName = firstName;
    this->lastName = lastName;
    this->email = email;
    this->phones.push_back(phone);
    this->patronymic = "";
    this->address = "";
    this->birthDate = "";
}


string Contact::getFirstName() const { return firstName; }
string Contact::getLastName() const { return lastName; }
string Contact::getPatronymic() const { return patronymic; }
string Contact::getAddress() const { return address; }
string Contact::getBirthDate() const { return birthDate; }
string Contact::getEmail() const { return email; }
vector<string> Contact::getPhones() const { return phones; }
string Contact::getPrimaryPhone() const {
    return phones.empty() ? "" : phones[0];
}

void Contact::setFirstName(const string& firstName) { this->firstName = firstName; }
void Contact::setLastName(const string& lastName) { this->lastName = lastName; }
void Contact::setPatronymic(const string& patronymic) { this->patronymic = patronymic; }
void Contact::setAddress(const string& address) { this->address = address; }
void Contact::setBirthDate(const string& date) { this->birthDate = date; }
void Contact::setEmail(const string& email) { this->email = email; }
void Contact::setPhones(const vector<string>& phones) { this->phones = phones; }
void Contact::addPhone(const string& phone) { this->phones.push_back(phone); }

string Contact::toString() const {
    stringstream ss;
    ss << "Name: " << firstName << "\n"
       << "Surname: " << lastName << "\n"
       << "Patronymic: " << (patronymic.empty() ? "N/A" : patronymic) << "\n"
       << "Addres: " << (address.empty() ? "N/A" : address) << "\n"
       << "Birth date: " << (birthDate.empty() ? "N/A" : birthDate) << "\n"
       << "Email: " << email << "\n"
       << "Phones: ";

    for (size_t i = 0; i < phones.size(); ++i) {
        if (i > 0) ss << ", ";
        ss << phones[i];
    }
    ss << "\n-------------------";

    return ss.str();
}

string Contact::toFileString() const {
    stringstream ss;
    ss << firstName << ";" << lastName << ";" << patronymic << ";"
       << address << ";" << birthDate << ";" << email << ";";

    for (size_t i = 0; i < phones.size(); ++i) {
        if (i > 0) ss << ",";
        ss << phones[i];
    }

    return ss.str();
}
