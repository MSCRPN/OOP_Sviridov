#include "contactBook.h"
#include <QStringList>
#include <algorithm>
#include <cctype>
#include <iostream>
#include <QDebug>
#include <QFileInfo>

using namespace std;

ContactBook::ContactBook(const QString& filename) : filename(filename) {}

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
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << " Не удалось открыть файл:" << filename;
        return false;
    }

    QTextStream in(&file);
    in.setCodec("UTF-8");

    contacts.clear();
    int lineNum = 0;

    while (!in.atEnd()) {
        lineNum++;
        QString line = in.readLine().trimmed();
        if (line.isEmpty()) continue;

        QStringList parts = line.split(';');
        if (parts.size() >= 7) {
            try {
                QString firstName = parts[0].trimmed();
                QString lastName = parts[1].trimmed();
                QString patronymic = parts[2].trimmed();
                QString address = parts[3].trimmed();
                QString birthDate = parts[4].trimmed();
                QString emailStr = parts[5].trimmed();
                QString phonesStr = parts[6].trimmed();

                QStringList phonesList = phonesStr.split(',');
                vector<string> phones;
                for (const QString& p : phonesList) {
                    if (!p.trimmed().isEmpty()) {
                        phones.push_back(p.trimmed().toStdString());
                    }
                }

                if (phones.empty()) {
                    qDebug() << "⏭️  Строка" << lineNum << "- нет телефонов";
                    continue;
                }

                ContactBuilder builder;
                Contact contact = builder
                    .setFirstName(firstName.toStdString())
                    .setLastName(lastName.toStdString())
                    .setPatronymic(patronymic.toStdString())
                    .setAddress(address.toStdString())
                    .setBirthDate(birthDate.toStdString())
                    .setEmail(emailStr.toStdString())
                    .setPhones(phones)
                    .build();

                bool duplicate = false;
                for (const auto& existing : contacts) {
                    if (existing.getEmail() == contact.getEmail()) {
                        qDebug() << "🔄 Дубликат email в строке" << lineNum;
                        duplicate = true;
                        break;
                    }
                }

                if (!duplicate) {
                    contacts.push_back(contact);
                    qDebug() << "Загружен контакт:" << QString::fromStdString(contact.getFirstName());
                }

            } catch (const exception& e) {
                qDebug() << " Ошибка парсинга строки" << lineNum << ":" << e.what();
            }
        } else {
            qDebug() << " Неверный формат строки" << lineNum << ":" << line.left(50);
        }
    }

    file.close();
    qDebug() << "Загружено контактов:" << contacts.size() << "из" << QFileInfo(filename).fileName();
    return !contacts.empty();
}

bool ContactBook::saveToFile() const {
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        qDebug() << " Не удалось сохранить:" << filename;
        return false;
    }

    QTextStream out(&file);
    out.setCodec("UTF-8");

    for (const auto& contact : contacts) {
        out << QString::fromStdString(contact.toFileString()) << "\n";
    }

    file.close();
    qDebug() << "💾 Сохранено:" << contacts.size() << "контактов в" << QFileInfo(filename).fileName();
    return true;
}

size_t ContactBook::getCount() const {
    return contacts.size();
}

void ContactBook::sortByLastName() {
    sort(contacts.begin(), contacts.end(), [](const Contact& a, const Contact& b) {
        return a.getLastName() < b.getLastName();
    });
}

void ContactBook::addContact(const QString& name) {
    Contact dummy;
    contacts.push_back(dummy);
    qDebug() << "Добавлен пустой контакт, всего:" << contacts.size();
}
