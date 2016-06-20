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
  int ModExp(long long,  long long, long long );
  int NOD(int p, int q);

public:
  RSACrypt();
  QString EncodeText(QString, int, int);
  QString DecodeText(QString, int, int);
  void GenerationKeys();
  int GetE();
  int GetModule();
  int GetD(); //Временно, для теста.
};

#endif // RSACRYPT_H
