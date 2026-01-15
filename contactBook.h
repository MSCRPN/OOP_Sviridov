#ifndef CONTACT_BOOK_H
#define CONTACT_BOOK_H

#include "contact.h"
#include "contactBuilder.h"
#include <vector>
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QDebug>

using namespace std;

class ContactBook {
private:
    vector<Contact> contacts;
    QString filename;

public:
    ContactBook(const QString& filename = "contacts.txt");

    void addContact(const Contact& contact);
    bool removeContact(const string& email);
    vector<Contact> findContactsByFilter(
        const string& firstName = "",
        const string& lastName = "",
        const string& patronymic = "",
        const string& email = "",
        const string& phone = "") const;

    vector<Contact> getAllContacts() const;
    bool loadFromFile();
    bool saveToFile() const;
    size_t getCount() const;
    void sortByLastName();
    void addContact(const QString& name);
};

#endif
