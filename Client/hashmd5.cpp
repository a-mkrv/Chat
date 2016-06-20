#include "hashmd5.h"
#include <QTime>
#include <QDebug>

HashMD5::HashMD5()
{
  QTime time = QTime::currentTime();
  qsrand((uint)time.msec());
}

QString HashMD5::HashSumPass(QString _pass)
{
  QCryptographicHash CalculateMD5(QCryptographicHash::Md5);
  QByteArray in;
  in.append(_pass);
  CalculateMD5.addData(in);

  QByteArray res = CalculateMD5.result().toHex();
  QString str = QString::fromUtf8(res.constData());

  return str;
}

QString HashMD5::SaltGeneration()
{
  QString pass;

  for (int i = 0; i < 8; i++)
    {
      int key = rand() % 3;
      char word_pass = 0;

      switch (key)
        {
        case 0:
          word_pass = qrand() % 9 + 48;
          break;
        case 1:
          word_pass = qrand() % 26 + 65;
          break;
        case 2:
          word_pass = qrand() % 26 + 97;
          break;
        }
      pass.append(word_pass);
    }

  return pass;
}

void HashMD5::Hash(QString message)
{
  const uint32_t S[64] = { 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,
                           5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20,
                           4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,
                           6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21 };

  const uint32_t K[64] = { 0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,
                           0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
                           0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
                           0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
                           0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,
                           0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
                           0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,
                           0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
                           0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
                           0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
                           0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05,
                           0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
                           0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039,
                           0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
                           0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
                           0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391 };

  QVector<BYTE> byteMsg;
  std::string mes = message.toStdString();
  for (size_t i = 0; i < message.length(); i++)
    byteMsg.push_back(mes[i]);


  byteMsg.push_back((unsigned char)0x80);

  while ((byteMsg.size() * 8) % 512 != 448)
    byteMsg.push_back((unsigned char)0x00);


  uint64_t originalLength = (message.length() * 8) % ((uint64_t)pow(2.0, 64));

  for (int i = 0; i < 8; i++)
    byteMsg.push_back(originalLength >> i * 8);

  uint32_t a0 = 0x67452301;
  uint32_t b0 = 0xefcdab89;
  uint32_t c0 = 0x98badcfe;
  uint32_t d0 = 0x10325476;

  for (size_t i = 0; i < byteMsg.size(); i += 64)
    {

      QVector<uint32_t> M;
      for (int j = 0; j < 64; j += 4)
        {
          uint32_t word = byteMsg[i + j] ^
              byteMsg[i + j + 1] << 8 ^
                                    byteMsg[i + j + 2] << 16 ^
                                                          byteMsg[i + j + 3] << 24;
          M.push_back(word);
        }

      uint32_t A = a0;
      uint32_t B = b0;
      uint32_t C = c0;
      uint32_t D = d0;

      for (int j = 0; j < 64; j++)
        {
          uint32_t F, g;
          if (0 <= j && j <= 15)
            {
              F = (B & C) | ((~B) & D);
              g = j;
            }
          else if (16 <= j && j <= 31)
            {
              F = (D & B) | ((~D) & C);
              g = (5 * j + 1) % 16;
            }
          else if (32 <= j && j <= 47)
            {
              F = B ^ C ^ D;
              g = (3 * j + 5) % 16;
            }
          else if (48 <= j && j <= 63)
            {
              F = C ^ (B | (~D));
              g = (7 * j) % 16;
            }
          uint32_t tempD = D;
          D = C;
          C = B;
          B = B + RotateLeft32(A + F + K[j] + M[g], S[j]);
          A = tempD;
        }

      a0 += A;
      b0 += B;
      c0 += C;
      d0 += D;
    }

  uint8_t *p = (uint8_t *)&a0;
  QString resultHash;
  QString valueInHex = QString("%1%2%3%4").arg(p[0] , 0, 16).arg(p[1] , 0, 16).arg(p[2] , 0, 16).arg(p[3] , 0, 16);
  resultHash.append(valueInHex);
  p = (uint8_t *)&b0;
  valueInHex = QString("%1%2%3%4").arg(p[0] , 0, 16).arg(p[1] , 0, 16).arg(p[2] , 0, 16).arg(p[3] , 0, 16);
  resultHash.append(valueInHex);
  p = (uint8_t *)&c0;
  valueInHex = QString("%1%2%3%4").arg(p[0] , 0, 16).arg(p[1] , 0, 16).arg(p[2] , 0, 16).arg(p[3] , 0, 16);
  resultHash.append(valueInHex);
  p = (uint8_t *)&d0;
  valueInHex = QString("%1%2%3%4").arg(p[0] , 0, 16).arg(p[1] , 0, 16).arg(p[2] , 0, 16).arg(p[3] , 0, 16);
  resultHash.append(valueInHex);

  //Лажает =/

  return;
}

uint32_t HashMD5::RotateLeft32(uint32_t val, int amount)
{
  amount %= 32;
  if (amount == 0)
    return val;
  return (val << amount) | (val >> (32 - amount));
}

HashMD5::~HashMD5()
{

}

