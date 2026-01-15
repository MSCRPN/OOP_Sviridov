#include "contactBook.h"
#include <QStringList>
#include <algorithm>
#include <cctype>
#include <iostream>
#include <QDebug>
#include <QFileInfo>

using namespace std;
static QStringList parsePgTextArray(QString pgArray)
{
    pgArray = pgArray.trimmed();
    if (pgArray.startsWith("{") && pgArray.endsWith("}")) {
        pgArray = pgArray.mid(1, pgArray.size() - 2); // убрали { }
    }

    if (pgArray.isEmpty())
        return {};

    QStringList items;
    QString current;
    bool inQuotes = false;
    bool escape = false;

    for (int i = 0; i < pgArray.size(); ++i) {
        QChar ch = pgArray[i];

        if (escape) {                // обработка \" и \\ внутри кавычек
            current += ch;
            escape = false;
            continue;
        }

        if (inQuotes && ch == '\\') { // backslash escape внутри "..."
            escape = true;
            continue;
        }

        if (ch == '"') {
            inQuotes = !inQuotes;
            continue;
        }

        if (!inQuotes && ch == ',') {
            items << current.trimmed();
            current.clear();
            continue;
        }

        current += ch;
    }

    if (!current.isEmpty())
        items << current.trimmed();

    // Postgres может писать NULL без кавычек как NULL-элемент (это реально NULL, не строка)
    // Если хочешь — выкидываем такие элементы:
    for (int i = items.size() - 1; i >= 0; --i) {
        if (items[i].compare("NULL", Qt::CaseInsensitive) == 0)
            items.removeAt(i);
    }

    return items;
}




ContactBook::ContactBook(const QString& filename) : filename(filename) {}

void ContactBook::addContact(const Contact& contact) {
    for (const auto& existingContact : contacts) {
        if (existingContact.getEmail() == contact.getEmail()) {
            throw invalid_argument("Contact with email " + contact.getEmail() + " already exists");
        }
    }
    contacts.push_back(contact);
    if (currentMode == FILE_MODE) saveToFile();
        else saveToDb();
   }


bool ContactBook::reload() {
    if (currentMode == FILE_MODE) return loadFromFile();
    return loadFromDb();
}


bool ContactBook::removeContact(const string& email)
{
    // 1) Удаляем из БД или из файла (зависит от режима)
    bool storageOk = true;

    if (currentMode == DB_MODE) {
        storageOk = removeFromDb(email);
        if (!storageOk) return false;
    }

    // 2) Удаляем из памяти (чтобы таблица обновилась сразу)
    auto it = remove_if(contacts.begin(), contacts.end(),
                        [&](const Contact& c){ return c.getEmail() == email; });

    if (it != contacts.end()) {
        contacts.erase(it, contacts.end());
    }

    // 3) Если FILE_MODE — сохраняем файл
    if (currentMode == FILE_MODE) {
        saveToFile();
    }

    return true;
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

const vector<Contact>& ContactBook::getAllContacts() const { return contacts; }

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
void ContactBook::setStorageMode(StorageMode mode) {
    currentMode = mode;
}
StorageMode ContactBook::getMode() const {
    return currentMode;
}


void ContactBook::addContact(const QString& name) {
    Contact dummy;
    contacts.push_back(dummy);
    qDebug() << "Добавлен пустой контакт, всего:" << contacts.size();
}
void ContactBook::switchToDb(const QString& host, int port, const QString& dbname,
                            const QString& user, const QString& pass) {
    dbManager = new DatabaseManager();
    if (!dbManager->connect(host, port, dbname, user, pass)) {
        qDebug() << "DB connect failed!";
        return;
    }
    setStorageMode(DB_MODE);
    loadFromDb();
    qDebug() << "Switched to DB";
}

bool ContactBook::loadFromDb()
{
    if (!dbManager || !dbManager->isConnected())
        return false;

    contacts.clear();

    QSqlQuery query(dbManager->getDatabase());
    if(!query.exec("SELECT id, first_name, last_name, patronymic, address, birth_date, email, phones FROM contacts ORDER BY last_name"))  {
        qDebug() << "DB select error:" << query.lastError().text();
        return false;
    }

    while (query.next()) {
        try {
            QString firstName  = query.value("first_name").toString();
            QString lastName   = query.value("last_name").toString();
            QString patronymic = query.value("patronymic").toString();
            QString address    = query.value("address").toString();
            QString birthDate  = query.value("birth_date").toString();
            QString email      = query.value("email").toString();

            QString phonesRaw = query.value("phones").toString();
            QStringList phonesList = parsePgTextArray(phonesRaw);
            std::vector<std::string> phones;
            for (const QString& p : phonesList) {
                QString t = p.trimmed();
                if (!t.isEmpty()) phones.push_back(t.toStdString());
            }

            ContactBuilder builder;
            Contact c = builder
                .setFirstName(firstName.toStdString())
                .setLastName(lastName.toStdString())
                .setPatronymic(patronymic.toStdString())
                .setAddress(address.toStdString())
                .setBirthDate(birthDate.toStdString())
                .setEmail(email.toStdString())
                .setPhones(phones)
                .build();

            contacts.push_back(c);
        }
        catch (const std::exception& e) {
            int badId = query.value("id").toInt();
            QString badEmail = query.value("email").toString();

            qDebug() << "Skip invalid DB contact. id=" << badId
                     << "email=" << badEmail
                     << "reason=" << e.what();
            continue;
        }
    }


    return true;
}

static QString toPgTextArrayLiteral(const std::vector<std::string> &phones)
{
    QStringList escaped;
    for (const auto &p : phones) {
        QString s = QString::fromStdString(p);
        s.replace("\\", "\\\\");
        s.replace("\"", "\\\"");
        escaped << ("\"" + s + "\"");
    }
    return "{" + escaped.join(",") + "}";
}

bool ContactBook::removeFromDb(const std::string& email)
{
    if (!dbManager || !dbManager->isConnected()) return false;

    QSqlQuery q(dbManager->getDatabase());
    q.prepare("DELETE FROM contacts WHERE email = ?");
    q.addBindValue(QString::fromStdString(email));

    if (!q.exec()) {
        qDebug() << "DELETE error:" << q.lastError().text();
        return false;
    }


    qDebug() << "Rows affected:" << q.numRowsAffected();

    return true;
}


bool ContactBook::saveToDb()
{
    if (!dbManager || !dbManager->isConnected()) return false;

    for (const auto& contact : contacts) {
        QString phonesLiteral = toPgTextArrayLiteral(contact.getPhones());

        QSqlQuery query(dbManager->getDatabase());
        query.prepare(
            "INSERT INTO contacts (first_name, last_name, patronymic, address, birth_date, email, phones) "
            "VALUES (?, ?, ?, ?, ?, ?, ?::text[]) "
            "ON CONFLICT (email) DO UPDATE SET "
            "first_name=EXCLUDED.first_name, "
            "last_name=EXCLUDED.last_name, "
            "patronymic=EXCLUDED.patronymic, "
            "address=EXCLUDED.address, "
            "birth_date=EXCLUDED.birth_date, "
            "phones=EXCLUDED.phones"
        );

        query.addBindValue(QString::fromStdString(contact.getFirstName()));
        query.addBindValue(QString::fromStdString(contact.getLastName()));
        query.addBindValue(QString::fromStdString(contact.getPatronymic()));
        query.addBindValue(QString::fromStdString(contact.getAddress()));
        query.addBindValue(QString::fromStdString(contact.getBirthDate()));
        query.addBindValue(QString::fromStdString(contact.getEmail()));
        query.addBindValue(phonesLiteral);

        if (!query.exec()) {
            qDebug() << "INSERT/UPDATE error:" << query.lastError().text();
            qDebug() << "phonesLiteral:" << phonesLiteral;
            return false;
        }
    }
    return true;
}


bool ContactBook::updateInDb(const std::string& oldEmail, const Contact& updated)
{
    if (!dbManager || !dbManager->isConnected()) return false;

    QString phonesLiteral = toPgTextArrayLiteral(updated.getPhones());

    QSqlQuery q(dbManager->getDatabase());
    q.prepare(
        "UPDATE contacts SET first_name=?, last_name=?, patronymic=?, address=?, birth_date=?, email=?, phones=?::text[] "
        "WHERE email=?"
    );

    q.addBindValue(QString::fromStdString(updated.getFirstName()));
    q.addBindValue(QString::fromStdString(updated.getLastName()));
    q.addBindValue(QString::fromStdString(updated.getPatronymic()));
    q.addBindValue(QString::fromStdString(updated.getAddress()));
    q.addBindValue(QString::fromStdString(updated.getBirthDate()));
    q.addBindValue(QString::fromStdString(updated.getEmail()));
    q.addBindValue(phonesLiteral);
    q.addBindValue(QString::fromStdString(oldEmail));

    if (!q.exec()) {
        qDebug() << "UPDATE error:" << q.lastError().text();
        qDebug() << "phonesLiteral:" << phonesLiteral;
        return false;
    }
    return true;
}

