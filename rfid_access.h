#include <SPI.h>
#include <MFRC522.h>

struct CardMetaData
{

  enum class
  {
    CARD,
    FOB,
    STICKER,
    EMULATED
  } type;

  String owner;
};

class CardRegistry
{

  bool isMember(Uuid card) const;
};
