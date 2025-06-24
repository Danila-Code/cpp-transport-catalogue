#pragma once

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <cstddef>
#include <variant>

namespace json {

class Node;

using Dict = std::map<std::string, Node>;
using Array = std::vector<Node>;

// Эта ошибка должна выбрасываться при ошибках парсинга JSON
class ParsingError : public std::runtime_error {
public:
    using runtime_error::runtime_error;
};

class Node final 
    : private std::variant<std::nullptr_t, int, double, bool, std::string, Array, Dict> {
public:
    using Value = std::variant<std::nullptr_t, int, double, bool, std::string, Array, Dict>;

    using variant::variant;

    // Методы сообщают, хранится ли внутри значение некоторого типа:
    bool IsNull() const {
        return std::holds_alternative<std::nullptr_t>(*this);
    }
    bool IsInt() const {
        return std::holds_alternative<int>(*this);
    }
    bool IsDouble() const {
        return std::holds_alternative<double>(*this) || std::holds_alternative<int>(*this);
    }
    bool IsPureDouble() const {
        return std::holds_alternative<double>(*this);
    }
    bool IsBool() const {
        return std::holds_alternative<bool>(*this);
    }
    bool IsString() const {
        return std::holds_alternative<std::string>(*this);
    }
    bool IsArray() const {
        return std::holds_alternative<Array>(*this);
    }
    bool IsMap() const {
        return std::holds_alternative<Dict>(*this);
    }

    // Методы возвращают хранящиеся внутри Node значение заданного типа
    int AsInt() const {
        if(!IsInt()) {
            throw std::logic_error("The node type is not int");
        }
        return std::get<int>(*this);
    }
    double AsDouble() const {
        if(!IsDouble()) {
            throw std::logic_error("The node type is not double");
        } 
        return IsPureDouble() ? std::get<double>(*this) : static_cast<double>(std::get<int>(*this));
    }
    bool AsBool() const {
        if(!IsBool()) {
            throw std::logic_error("The node type is not bool");
        } 
        return std::get<bool>(*this);
    }
    const std::string& AsString() const {
        if(!IsString()) {
            throw std::logic_error("The node type is not string");
        } 
        return std::get<std::string>(*this);
    }
    const Array& AsArray() const {
        if(!IsArray()) {
            throw std::logic_error("The node type is not Array");
        } 
        return std::get<Array>(*this);
    }
    const Dict& AsMap() const {
        if(!IsMap()) {
            throw std::logic_error("The node type is not map");
        } 
        return std::get<Dict>(*this);
    }
    
    // Методы сравнения двух экземпляров Node
    bool operator==(const Node& other) const noexcept {
        return this->GetValue() == other.GetValue();
    }
    bool operator!=(const Node& other) const noexcept {
        return !(*this == other);
    }

    const Value& GetValue() const {
        return *this;
    }
};

class Document {
public:
    explicit Document(Node root) : root_(move(root)) {
    }

    const Node& GetRoot() const {
        return root_;
    }

    bool operator==(const Document& other) const {
        return root_ == other.root_;
    }

    bool operator!=(const Document& other) const {
        return !(*this == other);
    }
private:
    Node root_;
};

// Загрузка json-документа из потока
Document Load(std::istream& input);

// Вывод json-документа в поток
void Print(const Document& doc, std::ostream& output);

}  // namespace json