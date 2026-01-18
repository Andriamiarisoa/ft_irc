# Design Pattern : Strategy

## 📚 Source
Basé sur [Refactoring.Guru - Strategy](https://refactoring.guru/design-patterns/strategy)

---

## 🎯 Objectif

Le **Strategy** est un pattern comportemental qui définit une famille d'algorithmes, encapsule chacun d'eux et les rend interchangeables.

---

## 🤔 Problème

**Dans IRC**, la validation des nicknames peut varier :
- Validation stricte (RFC)
- Validation permissive
- Validation custom

Sans Strategy :
```cpp
void validateNickname(std::string nick, ValidationMode mode) {
    if (mode == STRICT) {
        // validation stricte
    } else if (mode == PERMISSIVE) {
        // validation permissive
    }
}
```

---

## ✅ Solution

```cpp
// Interface Strategy
class NicknameValidator {
public:
    virtual bool isValid(const std::string& nick) = 0;
};

// Stratégies concrètes
class StrictValidator : public NicknameValidator {
public:
    bool isValid(const std::string& nick) override {
        // RFC 1459 strict
        return nick.length() <= 9 && isalpha(nick[0]);
    }
};

class PermissiveValidator : public NicknameValidator {
public:
    bool isValid(const std::string& nick) override {
        return !nick.empty() && nick.length() <= 30;
    }
};

// Contexte
class NickCommand {
private:
    NicknameValidator* validator;
public:
    void setValidator(NicknameValidator* v) { validator = v; }
    
    void execute() {
        if (validator->isValid(newNick)) {
            // ...
        }
    }
};
```

---

## ✅ Avantages

- **Algorithmes interchangeables** à runtime
- **Évite les conditionnelles** complexes
- **Facile à tester** chaque stratégie

---

## ⏱️ Temps d'apprentissage : ~1h

