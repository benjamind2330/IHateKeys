#include "uuid.h"
#include "optional.h"

#include <SPI.h>
#include <MFRC522.h>

#include <vector>
#include <functional>

struct CardData
{
  enum class Type
  {
    CARD,
    FOB,
    STICKER,
    EMULATED,
    UNKNOWN
  };

  Uuid id;
  String owner;
  Type type;
};

bool operator==(const CardData &lhs, const CardData &rhs)
{
  return lhs.id == rhs.id;
}

bool operator!=(const CardData &lhs, const CardData &rhs)
{
  return !(lhs == rhs);
}

bool operator<(const CardData &lhs, const CardData &rhs)
{
  return lhs.id < rhs.id;
}

bool operator>(const CardData &lhs, const CardData &rhs)
{
  return lhs.id > rhs.id;
}

bool operator<=(const CardData &lhs, const CardData &rhs)
{
  return !(lhs > rhs);
}

bool operator>=(const CardData &lhs, const CardData &rhs)
{
  return !(lhs < rhs);
}

CardData make_card(const String &id, const String &owner, CardData::Type type)
{
  return CardData{Uuid::make_uuid(id), owner, type};
}

String toString(CardData::Type type)
{
  switch (type)
  {
  case CardData::Type::CARD:
    return "CARD";
  case CardData::Type::FOB:
    return "FOB";
  case CardData::Type::STICKER:
    return "STICKER";
  case CardData::Type::EMULATED:
    return "EMULATED";
  case CardData::Type::UNKNOWN:
    return "UNKNOWN";
  }
}

String toString(const CardData &card)
{
  return "{\n  Owner:\t" + card.owner + "\n  type:\t\t" + toString(card.type) + "\n  id:\t\t" + toString(card.id) + "\n}";
}

class CardRegistry
{
public:
  void addCard(const CardData &card);

  bool isMember(Uuid id) const noexcept;
  std_bp::Optional<CardData> card(Uuid id) const noexcept;

private:
  std::vector<CardData> store_{};
};

void CardRegistry::addCard(const CardData &card)
{
  if (isMember(card.id))
  {
    return;
  }

  store_.push_back(card);

  // We sort every time because we will do this once when we make the CardRegistry
  // then never again.
  std::sort(std::begin(store_), std::end(store_), [](const CardData &lhs, const CardData &rhs) {
    return lhs.id < rhs.id;
  });
}

bool CardRegistry::isMember(Uuid id) const noexcept
{
  return std::binary_search(std::begin(store_), std::end(store_),
                            CardData{id, "", CardData::Type::UNKNOWN});
}

std_bp::Optional<CardData> CardRegistry::card(Uuid id) const noexcept
{
  auto card = std::lower_bound(std::begin(store_), std::end(store_), CardData{id, "", CardData::Type::UNKNOWN});

  if (card != std::end(store_))
  {
    return {*card};
  }
  return {};
}

template <hardware::Pin::Id SS_PIN, hardware::Pin::Id RST_PIN>
class CardAccessManager
{

public:
  using DispatchAction = std::function<void(CardData)>;

  CardAccessManager(CardRegistry &&cardRegistry,
                    DispatchAction onRegisteredCard,
                    DispatchAction onUnregisteredCard) : cardRegistry_(std::move(cardRegistry)),
                                                         onRegisteredCard_{onRegisteredCard},
                                                         onUnregisteredCard_{onUnregisteredCard},
                                                         mfrc522_(SS_PIN, 255)

  {
    SPI.begin();
    delay(STARTUP_DELAY_TIME);
    mfrc522_.PCD_Init();
    delay(STARTUP_DELAY_TIME);
  }

  void run()
  {

    if (!mfrc522_.PICC_IsNewCardPresent())
    {
      return;
    }

    if (!mfrc522_.PICC_ReadCardSerial())
    {
      return;
    }

    auto tag = Uuid::make_uuid(std::begin(mfrc522_.uid.uidByte), std::begin(mfrc522_.uid.uidByte) + mfrc522_.uid.size);
    auto card = cardRegistry_.card(tag);

    if (card)
    {
      onRegisteredCard_(*card);
    }
    else
    {
      auto invalidCard = CardData{tag, "Unknown", CardData::Type::UNKNOWN};
      onUnregisteredCard_(invalidCard);
    }
  }

private:
  struct NoAction
  {
    void operator()(CardData){};
  };

  static constexpr unsigned STARTUP_DELAY_TIME = 100; //ms
  MFRC522 mfrc522_;
  CardRegistry cardRegistry_{};
  DispatchAction onRegisteredCard_{NoAction{}};
  DispatchAction onUnregisteredCard_{NoAction{}};
};