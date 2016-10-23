#include "keyvalidator.h"
#include "QDebug"
KeyValidator::KeyValidator()
{

}

/**
 * Returns true if key is valid
 * */
bool KeyValidator::validate(QString key)
{
    bool valid = true;

    if (key.length() != 26) {
        valid = false;
    }

    for (int i = 0; i < key.length() && valid; i++) {
        if (i != key.lastIndexOf(key.at(i))) {
            valid = false;
        }
    }
    return valid;
}
