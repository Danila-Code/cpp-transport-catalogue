#include "json_builder.h"

#include <exception>

using namespace std::literals;

namespace json {

Builder::Builder() : root_{}, unfinished_nodes_{&root_} {
}
// Задание ключа в словаре (после создания словаря)
Builder::KeyContext Builder::Key(std::string key) {
    Node& last_node = GetLastNode();
    if(!last_node.IsMap()) {
        throw std::logic_error("The key is expected after the \"StartDict()\" or \"Value()\" methods"s);
    }
    auto& dict = std::get<Dict>(last_node.GetValue());
    // сохраняем адрес пустого Node из вставленной в словарь пары {key, Node{}}
    unfinished_nodes_.push_back(&dict[std::move(key)]);
    return BaseContext{*this};
}
// Задание значения для массива, пары для ключа в словаре или для всего поля root_
Builder::BaseContext Builder::Value(Node::Value value) {
    AddNewObject(std::move(value), true);
    return BaseContext{*this};
}
// Создаёт словарь для дальнейшего заполнения
Builder::DictContext Builder::StartDict() {
    AddNewObject(Dict(), false);
    return BaseContext{*this};
}
// Создаёт массив для дальнейшего заполнения
Builder::ArrayContext Builder::StartArray() {
    AddNewObject(Array(), false);
    return BaseContext{*this};
}
// Заканчивает задание элементов словаря
Builder::BaseContext Builder::EndDict() {
    if(unfinished_nodes_.empty() || !GetLastNode().IsMap()) {
        throw std::logic_error("The unfinished dictionary is expected before the \"EndDict()\" method"s);
    }
    unfinished_nodes_.pop_back();
    return BaseContext{*this};
}
// Заканчивает задание элементов массива
Builder::BaseContext Builder::EndArray() {
    if(unfinished_nodes_.empty() || !GetLastNode().IsArray()) {
        throw std::logic_error("The unfinished array is expected abefore the \"EndArray()\" method"s);
    }
    unfinished_nodes_.pop_back();
    return BaseContext{*this};
}

Node Builder::Build() {
    if(!unfinished_nodes_.empty()) {
        throw std::logic_error("The json object is not complete"s);
    }
    return root_;
}
// Возвращает последний сохраненный незавершённый узел
Node& Builder::GetLastNode() const {
    if(unfinished_nodes_.empty()) {
        throw std::logic_error("Attempt to modify a completed JSON object"s);
    }
    return *unfinished_nodes_.back();
}
// Проверяет контекст вызова методов добавления / завершения объектов
void Builder::AssertNewObjectContext() const {
    if(!unfinished_nodes_.back()->IsNull()) {
        throw std::logic_error("Incorrect method call context"s);
    }
}
// Добавляет новый объект, is_finished - нужно ли удалять последний незавершенный узел
void Builder::AddNewObject(Node::Value value, bool is_finished) {
    Node& last_node = GetLastNode();
    // Вызов после Value() в массиве
    if(last_node.IsArray()) {
        auto& node = std::get<Array>(last_node.GetValue()).emplace_back(std::move(value));
        // for map and array
        if(!is_finished) {
            unfinished_nodes_.push_back(&node);
        }
    } else {
        AssertNewObjectContext();
        last_node = std::move(value);
        // for value that not array or map
        if(is_finished) {
            unfinished_nodes_.pop_back();
        }
    }
}
}  //namespace json