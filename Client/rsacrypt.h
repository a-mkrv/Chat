#ifndef RSACRYPT_H
#define RSACRYPT_H

#include <QString>
#include <QStringList>
#include <QTime>

class RSACrypt
{
private:
    int p_simple;
    int q_simple;
    int module;
    int d;
    int f;
    int e;

    bool SimpleNumber(int &n, int t);
    int modExp(long long,  long long, long long );
    int NOD(int p, int q);

public:
    RSACrypt();
    QString encodeText(QString, int, int);
    QString decodeText(QString, int, int);
    void generationKeys();
    int getE();
    int getModule();
    int getD(); //Временно, для теста.
};

#endif // RSACRYPT_H
