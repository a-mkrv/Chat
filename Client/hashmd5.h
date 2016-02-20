#ifndef HASHMD5_H
#define HASHMD5_H

#include <QCryptographicHash>
#include <QString>
#include <QVector>
#include <qmath.h>
#include <stdint.h>
#include <QTextCodec>

typedef unsigned char BYTE;
typedef unsigned int  uint32_t;

class HashMD5
{
private:
    QString hashPas;
    QString hashFile;
    uint32_t rotateLeft32(uint32_t val, int amount);

public:
    HashMD5();
    QString hashSumPass(QString);

    void hash(QString message);

    ~HashMD5();
};

#endif // HASHMD5_H
