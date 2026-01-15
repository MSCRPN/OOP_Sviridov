#ifndef CONTACT_H
#define CONTACT_H

#include <string>
#include <vector>

using namespace std;

class Contact {
private:
    string firstName;
    string lastName;
    string patronymic;
    string address;
    string birthDate;
    string email;
    vector<string> phones;

public:
    Contact() = default;
    Contact(const string& first, const string& last, 
            const string& mail, const string& phone);
    
    string getFirstName() const;
    string getLastName() const;
    string getPatronymic() const;
    string getAddress() const;
    string getBirthDate() const;
    string getEmail() const;
    vector<string> getPhones() const;
    string getPrimaryPhone() const;
    
    void setFirstName(const string& first);
    void setLastName(const string& last);
    void setPatronymic(const string& patronymic);
    void setAddress(const string& address);
    void setBirthDate(const string& date);
    void setEmail(const string& mail);
    void setPhones(const vector<string>& phones);
    void addPhone(const string& phone);
    
    string toString() const;
    string toFileString() const;
};

#endif