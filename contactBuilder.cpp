#include "contactBuilder.h"

ContactBuilder& ContactBuilder::setFirstName(const string& first) {
    this->firstName = first;
    return *this;
}

ContactBuilder& ContactBuilder::setLastName(const string& last) {
    this->lastName = last;
    return *this;
}

ContactBuilder& ContactBuilder::setPatronymic(const string& patronymic) {
    this->patronymic = patronymic;
    return *this;
}

ContactBuilder& ContactBuilder::setAddress(const string& address) {
    this->address = address;
    return *this;
}

ContactBuilder& ContactBuilder::setBirthDate(const string& date) {
    this->birthDate = date;
    return *this;
}

ContactBuilder& ContactBuilder::setEmail(const string& mail) {
    this->email = mail;
    return *this;
}

ContactBuilder& ContactBuilder::setPhones(const vector<string>& phones) {
    this->phones = phones;
    return *this;
}

ContactBuilder& ContactBuilder::addPhone(const string& phone) {
    this->phones.push_back(phone);
    return *this;
}

Contact ContactBuilder::build() {
    ValidationError error = ContactValidator::validateContactDetailed(
        firstName, lastName, patronymic, address, birthDate, email, phones);
    
    if (error != NO_ERROR) {
        string errorMsg;
        switch (error) {
            case ERROR_FIRST_NAME: errorMsg = "Invalid first name"; break;
            case ERROR_LAST_NAME: errorMsg = "Invalid last name"; break;
            case ERROR_PATRONYMIC: errorMsg = "Invalid patronymic"; break;
            case ERROR_EMAIL: errorMsg = "Invalid email"; break;
            case ERROR_PHONE: errorMsg = "Invalid phone"; break;
            case ERROR_BIRTH_DATE: errorMsg = "Invalid birth date"; break;
            case ERROR_NO_PHONES: errorMsg = "At least one phone required"; break;
            default: errorMsg = "Unknown validation error";
        }
        throw invalid_argument(errorMsg);
    }
    
    Contact contact(firstName, lastName, email, phones[0]);
    
    if (!patronymic.empty()) contact.setPatronymic(patronymic);
    if (!address.empty()) contact.setAddress(address);
    if (!birthDate.empty()) contact.setBirthDate(birthDate);
    
    for (size_t i = 1; i < phones.size(); ++i) {
        contact.addPhone(phones[i]);
    }
    
    return contact;
}