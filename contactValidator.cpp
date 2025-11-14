#include "contactValidator.h"
#include <iostream>
#include <algorithm>
#include <ctime>


string ContactValidator::trim(const string& str) {
    const string whitespace = " \n\r\t\f\v";
    size_t start = str.find_first_not_of(whitespace);
    if (start == string::npos) return "";
    size_t end = str.find_last_not_of(whitespace);
    return str.substr(start, end - start + 1);
}

const regex ContactValidator::LATIN_NAME_REGEX(R"(^[A-Za-z][A-Za-z\s\-]*$)");
const regex ContactValidator::PHONE_REGEX(R"(^(\+7|8)[\s\-]?\(?\d{3}\)?[\s\-]?\d{3}[\s\-]?\d{2}[\s\-]?\d{2}$)");
const regex ContactValidator::EMAIL_REGEX(R"(^[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\.[A-Za-z]{2,}$)");
const regex ContactValidator::DATE_REGEX(R"(^\d{4}-(0[1-9]|1[0-2])-(0[1-9]|[12][0-9]|3[01])$)");

bool ContactValidator::validateName(const string& name) {
    string trimmed = trim(name);
    
    if (trimmed.empty() || trimmed.front() == '-' || trimmed.back() == '-') 
        return false;
    
    bool hasCyrillic = false;
    for (unsigned char c : trimmed) {
        if ((c >= 128 && c <= 175) || (c >= 224 && c <= 241) || c == 184 || c == 168) {
            hasCyrillic = true;
            break;
        }
    }
    
    if (hasCyrillic) {
        for (unsigned char c : trimmed) {
            if ((c >= 128 && c <= 175) || (c >= 224 && c <= 241) || c == 184 || c == 168) {
                continue;
            }
            else if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')) {
                return false;
            }
            else if (isdigit(c) || c == ' ' || c == '-') {
                continue;
            }
            else {
                return false;
            }
        }
        return true;
    } else {
        return regex_match(trimmed, LATIN_NAME_REGEX);
    }
}

bool ContactValidator::validateEmail(const string& email) {
    string trimmed = trim(email);
    if (trimmed.find(' ') != string::npos) return false;
    return regex_match(trimmed, EMAIL_REGEX);
}

bool ContactValidator::validatePhone(const string& phone) {
    string trimmed = trim(phone);
    return regex_match(trimmed, PHONE_REGEX);
}

bool ContactValidator::validateDate(const string& date) {
    if (date.empty()) return true;
    
    if (!regex_match(date, DATE_REGEX)) return false;
    
    int year = stoi(date.substr(0, 4));
    int month = stoi(date.substr(5, 2)); 
    int day = stoi(date.substr(8, 2));
    
    if (month < 1 || month > 12) return false;
    
    int daysInMonth[] = {31,28,31,30,31,30,31,31,30,31,30,31};
    bool leap = (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0));
    if (leap) daysInMonth[1] = 29;
    
    if (day < 1 || day > daysInMonth[month-1]) return false;
    
    time_t t = time(nullptr);
    tm* now = localtime(&t);
    int currentYear = now->tm_year + 1900;
    int currentMonth = now->tm_mon + 1;
    int currentDay = now->tm_mday;
    
    if (year > currentYear) return false;
    if (year == currentYear && month > currentMonth) return false;
    if (year == currentYear && month == currentMonth && day > currentDay) return false;
    
    return true;
}

ValidationError ContactValidator::validateContactDetailed(
    const string& firstName, 
    const string& lastName,
    const string& patronymic,
    const string& address, 
    const string& birthDate,
    const string& email,
    const vector<string>& phones) {
    
    if (!validateName(firstName)) return ERROR_FIRST_NAME;
    if (!validateName(lastName)) return ERROR_LAST_NAME;
    if (!validateEmail(email)) return ERROR_EMAIL;
    if (phones.empty()) return ERROR_NO_PHONES;
    
    for (const auto& phone : phones) {
        if (!validatePhone(phone)) return ERROR_PHONE;
    }
    
    if (!patronymic.empty() && !validateName(patronymic)) return ERROR_PATRONYMIC;
    if (!birthDate.empty() && !validateDate(birthDate)) return ERROR_BIRTH_DATE;
    
    return NO_ERROR;
}