#pragma once

#include "json.h"

#include <vector>

namespace json {

class Builder {
private:
    // Вспомогательные классы для возарата необходимых методов при построении JSON объекта
    class BaseContext;
    class DictContext;
    class ArrayContext;
    class KeyContext;

public:
    Builder();

    KeyContext Key(std::string key);
    BaseContext Value(Node::Value value);
    DictContext StartDict();
    ArrayContext StartArray();
    BaseContext EndDict();
    BaseContext EndArray();
    Node Build();

private:
    // Проверяет контекст вызова методов добавления / завершения объектов
    void AssertNewObjectContext() const;
    // Добавляет новый объект, is_finished - нужно ли удалять последний незавершенный узел
    void AddNewObject(Node::Value value, bool is_finished);
    // Возвращает последний сохраненный незавершённый узел
    Node& GetLastNode() const;

    Node root_;
    std::vector<Node*> unfinished_nodes_;

    // Вспомогательные классы для возарата необходимых методов при построении JSON объекта
    class BaseContext {
    public:
        BaseContext(Builder& builder) : builder_{builder} {}

        KeyContext Key(std::string key) {
            return builder_.Key(std::move(key));
        }
        BaseContext Value(Node::Value value) {
            return builder_.Value(std::move(value));
        }
        DictContext StartDict() {
            return builder_.StartDict();
        }
        ArrayContext StartArray() {
            return builder_.StartArray();
        }
        BaseContext EndDict() {
            return builder_.EndDict();
        }
        BaseContext EndArray() {
            return builder_.EndArray();
        }
        Node Build() {
            return builder_.Build();
        }

    private:
        Builder& builder_;
    };

    class DictContext : public BaseContext {
    public:
        DictContext(BaseContext base) : BaseContext{base} {}

        BaseContext Value(Node::Value) = delete;
        DictContext StartDict() = delete;
        ArrayContext StartArray() = delete;
        BaseContext EndArray() = delete;
        Node Build() = delete;
    };

    class ArrayContext : public BaseContext {
    public:
        ArrayContext(BaseContext base) : BaseContext{base} {}

        ArrayContext Value(Node::Value value) {
            return BaseContext::Value(std::move(value));
        }
        KeyContext Key(std::string) = delete;
        BaseContext EndDict() = delete;
        Node Build() = delete;        
    };

    class KeyContext : public BaseContext {
    public:
        KeyContext(BaseContext base) : BaseContext{base} {}
        
        DictContext Value(Node::Value value) {
            return BaseContext::Value(std::move(value));
        }
        KeyContext Key(std::string key) = delete;
        BaseContext EndDict() = delete;
        BaseContext EndArray() = delete;
        Node Build() = delete;
    };
};

}  //namespace json