#include "simplesubstitutioner.h"

SimpleSubstitutioner::SimpleSubstitutioner(QString key){
  char letterA = 'a';
  for(int i = 0; i < key.length(); i++) {
    this->keyMapping.insert((char)(letterA + i), key.at(i).toLatin1());
  }
}

QString SimpleSubstitutioner::substitute(QString src){
  QString crypted;

  for(int i = 0; i < src.length(); i++) {
    crypted.append(this->keyMapping[src.at(i).toLatin1()]);
  }

  return crypted;
}
