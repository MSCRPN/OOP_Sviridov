#ifndef CONTACT_VALIDATOR_H
#define CONTACT_VALIDATOR_H

#include <string>
#include <regex>
#include <vector>

using namespace std;

enum ValidationError {
    NO_ERROR = 0,
    ERROR_FIRST_NAME,
    ERROR_LAST_NAME, 
    ERROR_PATRONYMIC,
    ERROR_EMAIL,
    ERROR_PHONE,
    ERROR_BIRTH_DATE,
    ERROR_NO_PHONES
};

class ContactValidator {
private:
    static string trim(const string& s);
    static const regex LATIN_NAME_REGEX;
    static const regex PHONE_REGEX; 
    static const regex EMAIL_REGEX;
    static const regex DATE_REGEX;

public:
    static bool validateName(const string& name);
    static bool validateEmail(const string& email);
    static bool validatePhone(const string& phone);
    static bool validateDate(const string& date);

    static string normalizeEmail(std::string s);

    static ValidationError validateContactDetailed(
        const string& firstName, 
        const string& lastName,
        const string& patronymic,
        const string& address, 
        const string& birthDate,
        const string& email,
        const vector<string>& phones);
};

#endif
