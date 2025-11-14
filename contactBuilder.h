#ifndef CONTACT_BUILDER_H
#define CONTACT_BUILDER_H

#include "contact.h"
#include "contactValidator.h"
#include <stdexcept>
#include <vector>

using namespace std;

class ContactBuilder {
private:
    string firstName;
    string lastName;
    string patronymic;
    string address;
    string birthDate;
    string email;
    vector<string> phones;

public:
    ContactBuilder& setFirstName(const string& first);
    ContactBuilder& setLastName(const string& last);
    ContactBuilder& setPatronymic(const string& patronymic);
    ContactBuilder& setAddress(const string& address);
    ContactBuilder& setBirthDate(const string& date);
    ContactBuilder& setEmail(const string& mail);
    ContactBuilder& setPhones(const vector<string>& phones);
    ContactBuilder& addPhone(const string& phone);
    
    Contact build();
};

#endif