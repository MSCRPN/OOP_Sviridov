#ifndef CONTACT_BOOK_H
#define CONTACT_BOOK_H

#include "contact.h"
#include "contactBuilder.h"
#include <vector>
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include "databasemanager.h"

using namespace std;
enum StorageMode { FILE_MODE, DB_MODE };
inline constexpr int FILE_MODE_VAL = 0;
inline constexpr int DB_MODE_VAL = 1;

class ContactBook {
private:
    vector<Contact> contacts;
    QString filename;
    DatabaseManager* dbManager = nullptr;
    StorageMode currentMode = FILE_MODE;

public:
    ContactBook(const QString& filename = "contacts.txt");

    void switchToDb(const QString& host, int port, const QString& dbname, const QString& user, const QString& pass);
    bool loadFromDb();
    bool saveToDb();
    void addContact(const Contact& contact);
    bool removeContact(const string& email);
    bool reload();
    bool removeFromDb(const std::string& email);
    bool updateInDb(const std::string& oldEmail, const Contact& updated);
    vector<Contact> findContactsByFilter(
        const string& firstName = "",
        const string& lastName = "",
        const string& patronymic = "",
        const string& email = "",
        const string& phone = "") const;
    void setStorageMode(StorageMode mode);
    StorageMode getMode() const;

    const vector<Contact>& getAllContacts() const;
    bool loadFromFile();
    bool saveToFile() const;
    size_t getCount() const;
    void sortByLastName();
    void addContact(const QString& name);
};
#endif
