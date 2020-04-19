#include "uuid.h"
#include "optional.h"

#include <SPI.h>
#include <MFRC522.h>

#include <vector>

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
