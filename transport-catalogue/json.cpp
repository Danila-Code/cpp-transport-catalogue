#include "json.h"
#include <algorithm>

using namespace std::literals;

namespace json {

namespace {

// считывает из потока подряд идущие буквы и возвращает
void GetWord(std::istream& input, std::string& word) {
    //std::string word;
    while(std::isalpha(input.peek())) {
        word.push_back(static_cast<char>(input.get()));
    }
    //return word;
}

bool IsEscapeSeq(char c) {
    static const char escape_seq[] = "\r\n\t\"\\";
    static const size_t size = 5;
    return std::find(escape_seq, escape_seq + size, c) != escape_seq + size;
}

bool IsFromEscapeSeq(char c) {
    static const std::string escape_seq = "rnt\""s;
    return escape_seq.find(c) != escape_seq.npos;
}

char GetEscapeSeq(char c) {
    char escape_seq{};
    switch(c) {
        case 'n':
        escape_seq = '\n';
        break;
        case 'r':
        escape_seq = '\r';
        break;
        case 't':
        escape_seq = '\t';
        break;
        case '"':
        escape_seq = '\"';
        break;
    }
    return escape_seq;
}

std::string GetDoubleEscapeSeq(char c) {
    std::string escape_seq{};
    switch(c) {
        case '\n':
        escape_seq = "\\n"s;
        break;
        case '\r':
        escape_seq = "\\r"s;
        break;
        case '\t':
        escape_seq = "\\t"s;
        break;
        case '\"':
        escape_seq = "\\\""s;
        break;
        case '\\':
        escape_seq = "\\\\"s;
        break;
    }
    return escape_seq;
}

// Функции чтения из потока

// читает null из потока
Node LoadNull(std::istream& input) {
    std::string word;
    GetWord(input, word);

    if(word != "null"sv) {
        throw ParsingError("Failed to parse '"s + word + "' as null"s);
    }
    return Node{};
}

// читает bool из потока
Node LoadBool(std::istream& input) {
    std::string word;
    GetWord(input, word);
    
    if(word == "true"sv) {
        return Node{true};
    } else if(word == "false"sv) {
        return Node{false};
    } else {
        throw ParsingError("Failed to parse '"s + word + "' as bool"s);
    }
}

// читает int и double из потока
Node LoadNumber(std::istream& input) {
    std::string parsed_num;

    // Считывает в parsed_num очередной символ из input
    auto read_char = [&parsed_num, &input] {
        parsed_num += static_cast<char>(input.get());
        if (!input) {
            throw ParsingError("Failed to read number from stream"s);
        }
    };

    // Считывает одну или более цифр в parsed_num из input
    auto read_digits = [&input, read_char] {
        if (!std::isdigit(input.peek())) {
            throw ParsingError("A digit is expected"s);
        }
        while (std::isdigit(input.peek())) {
            read_char();
        }
    };

    if (input.peek() == '-') {
        read_char();
    }
    // Парсим целую часть числа
    if (input.peek() == '0') {
        read_char();
        // После 0 в JSON не могут идти другие цифры
    } else {
        read_digits();
    }

    bool is_int = true;
    // Парсим дробную часть числа
    if (input.peek() == '.') {
        read_char();
        read_digits();
        is_int = false;
    }

    // Парсим экспоненциальную часть числа
    if (int ch = input.peek(); ch == 'e' || ch == 'E') {
        read_char();
        if (ch = input.peek(); ch == '+' || ch == '-') {
            read_char();
        }
        read_digits();
        is_int = false;
    }

    try {
        if (is_int) {
            // Сначала пробуем преобразовать строку в int
            try {
                return Node{std::stoi(parsed_num)};
            } catch (...) {
                // В случае неудачи, например, при переполнении,
                // код ниже попробует преобразовать строку в double
            }
        }
        return Node{std::stod(parsed_num)};
    } catch (...) {
        throw ParsingError("Failed to convert "s + parsed_num + " to number"s);
    }
}

// читает string из потока
Node LoadString(std::istream& input) {
    std::string res{};
    bool is_screened = false;
    input >> std::noskipws;

    char c;
    for(; input >> c && (c != '\"' || is_screened == true);) {
        if(c == '\\') {
            if(is_screened == true) {
                res.push_back(c);
                is_screened = false;
                continue;
            }
            is_screened = true;
            continue;
        } 
        if(is_screened) {
            if(IsFromEscapeSeq(c)) {
                res.push_back(GetEscapeSeq(c));
            } else {
                throw ParsingError("Unrecognized escape sequence \\"s + c);
            }
            is_screened = false;
            continue;
        }
        if(c == '\n' || c == '\r') {
                throw ParsingError("Unexpected end of line"s);
        }
        res.push_back(c);
    }
    if (!input) {
        throw ParsingError("String parsing error"s);
    }
    input >> std::skipws;
    return Node{move(res)};
}

Node LoadNode(std::istream& input);

// читает Array из потока
Node LoadArray(std::istream& input) {
    Array result;
    for (char c; input >> c && c != ']';) {
        if (c != ',') {
            input.putback(c);
        }
        result.push_back(LoadNode(input));
    }
    if (!input) {
        throw ParsingError("Array parsing error"s);
    }
    return Node(move(result));
}

// читает Dict из потока
Node LoadDict(std::istream& input) {
    Dict result;
    for (char c; input >> c && c != '}';) {
        if(c == '"') {
            std::string key = LoadString(input).AsString();
            if(input >> c && c == ':') {
                if(result.find(key) != result.end()) {
                    throw ParsingError("Duplicate of key '"s + key + "' was found"s);
                }
                result.emplace(move(key), LoadNode(input));
            } else {
                throw ParsingError("'"s + c + "' was found instead of ':'"s);
            }
        } else if(c != ',') {
            throw ParsingError("'"s + c + "' was found instead of ','"s);
        }
    }

    if(!input) {
        throw ParsingError("Dictionary parsing error"s);
    }
    return Node(move(result));
}

// читает Node из потока и вызывает соответствующие загрузчики
Node LoadNode(std::istream& input) {  
    char c;
    if(!(input >> c)) {
        throw ParsingError("Unexpected EOF"s);
    };
    switch(c) {
        case '[':
            return LoadArray(input);
        case '{':
            return LoadDict(input);
        case '"':
            return LoadString(input);
        case 'n':
            input.putback(c);
            return LoadNull(input);
        case 't':
            [[fallthrough]];
        case 'f':
            input.putback(c);
            return LoadBool(input);
        default:
            input.putback(c);
            return LoadNumber(input);
    }
}

// Функции вывода данных в поток в формате JSON
// Структура для формирования отступа
struct PrintContext {
    std::ostream& out;
    int indent_step = 4;
    int indent = 0;

    void PrintIndent() const {
        for (int i = 0; i < indent; ++i) {
            out.put(' ');
        }
    }

    PrintContext Indented() const {
        return {out, indent_step, indent_step + indent};
    }
};

void PrintNode(const Node& node, const PrintContext& context);

template<typename Value>
void PrintValue(const Value& value, const PrintContext& context) {
    context.out << value;
}

// Выводит "null" 
template<>
void PrintValue<std::nullptr_t>(const std::nullptr_t&, const PrintContext& context) {
    context.out << "null"sv;
}

// Выводит string
template<>
void PrintValue<std::string>(const std::string& str, const PrintContext& context) {
    context.out << "\"";
    for(const char& c : str) {
        if(IsEscapeSeq(c)) {
            context.out << GetDoubleEscapeSeq(c);
        } else {
            context.out << c;
        }
    }
    context.out << "\"";
}

// Выводит тип bool
template<>
void PrintValue<bool>(const bool& value, const PrintContext& context) {
    context.out << std::boolalpha << value;
}

// Выводит Array
template<>
void PrintValue<Array>(const Array& array, const PrintContext& context) {
    std::ostream& out = context.out;
    bool first = true;
    out << "[\n"sv;
    auto inner_context = context.Indented();
    for(const auto& node : array) {
        if(first) {
            first = false;
        } else {
            out << ",\n"sv;
        }
        inner_context.PrintIndent();
        PrintNode(node, inner_context);
    }
    out.put('\n');
    context.PrintIndent();
    out.put(']');
}

// Выводит Dict
template<>
void PrintValue<Dict>(const Dict& dict, const PrintContext& context) {
    std::ostream& out = context.out;
    bool first = true;
    out << "{\n"sv;
    auto inner_context = context.Indented();
    for(auto& [key, node] : dict) {
        if(first) {
            first = false;
        } else {
            out << ",\n"sv;
        }
        context.PrintIndent();
        PrintValue(key, context);
        out << ": "sv;
        PrintNode(node, inner_context);
    }
    out.put('\n');
    context.PrintIndent();
    out.put('}');
}

// Выводит в поток Node
void PrintNode(const Node& node, const PrintContext& context) {
    visit(
        [&context](const auto& value) { PrintValue(value, context);},
        node.GetValue());
}
}  // namespace

// Загрузка json-документа из потока
Document Load(std::istream& input) {
    return Document{LoadNode(input)};
}

// Вывод json-документа в поток
void Print(const Document& doc, std::ostream& output) {
    PrintNode(doc.GetRoot(), PrintContext{output});
}

}  // namespace json