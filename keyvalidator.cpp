#include "keyvalidator.h"

KeyValidator::KeyValidator()
{

}

bool KeyValidator::validate(QString key)
{
    bool valid = true;

    for (int i = 0; i < key.length() && valid; i++) {
        if (i != key.lastIndexOf(key.at(i))) {
            valid = false;
        }
    }
    return valid;
}
