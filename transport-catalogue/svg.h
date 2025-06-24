#pragma once

#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <optional>
#include <cstdint>
#include <variant>
#include <vector>

namespace svg {

namespace detail {

template <typename T>
inline void RenderValue(std::ostream& out, const T& value) {
    out << value;
}

void StringToHtmlEncode(std::ostream& out, std::string_view text);

template <>
inline void RenderValue<std::string>(std::ostream& out, const std::string& s) {
    StringToHtmlEncode(out, s);
}

template <typename AttrType>
inline void RenderAttr(std::ostream& out, std::string_view name, const AttrType& value) {
    using namespace std::literals;
    out << name << "=\""sv;
    RenderValue(out, value);
    out.put('"');
}

template <typename AttrType>
inline void RenderOptionalAttr(std::ostream& out, std::string_view name, 
                               const std::optional<AttrType>& value) {
    if (value) {
        RenderAttr(out, name, *value);
    }
}

}  // namespace detail

inline const std::string NoneColor{"none"};

struct Rgb {
    Rgb() = default;
    Rgb(uint8_t r, uint8_t g, uint8_t b) : red{r}, green{g}, blue{b} {
    }

    uint8_t red{};
    uint8_t green{};
    uint8_t blue{};
};

struct Rgba : public Rgb {
    Rgba() : Rgb() {}

    Rgba(uint8_t r, uint8_t g, uint8_t b, double op) : Rgb(r, g, b), opacity{op} {
    }

    double opacity = 1.0;
};

using Color = std::variant<std::monostate, std::string, Rgb, Rgba>;

struct ColorPrint {
    std::ostream& out;

    void operator()(std::monostate) const;
    void operator()(const std::string& color) const;
    void operator()(const Rgb& color) const;
    void operator()(const Rgba& color) const;
};

enum class StrokeLineCap {
    BUTT,
    ROUND,
    SQUARE,
};

enum class StrokeLineJoin {
    ARCS,
    BEVEL,
    MITER,
    MITER_CLIP,
    ROUND,
};

std::ostream& operator<<(std::ostream& out, StrokeLineCap stroke_line_cap);
std::ostream& operator<<(std::ostream& out, StrokeLineJoin stroke_line_join);
std::ostream& operator<<(std::ostream& out, const Color& color);

struct Point {
    Point() = default;
    Point(double x, double y)
        : x(x)
        , y(y) {
    }
    double x = 0;
    double y = 0;
};

/*
 * Вспомогательная структура, хранящая контекст для вывода SVG-документа с отступами.
 * Хранит ссылку на поток вывода, текущее значение и шаг отступа при выводе элемента
 */
struct RenderContext {
    RenderContext(std::ostream& out)
        : out(out) {
    }

    RenderContext(std::ostream& out, int indent_step, int indent = 0)
        : out(out)
        , indent_step(indent_step)
        , indent(indent) {
    }

    RenderContext Indented() const {
        return {out, indent_step, indent + indent_step};
    }

    void RenderIndent() const {
        for (int i = 0; i < indent; ++i) {
            out.put(' ');
        }
    }

    std::ostream& out;
    int indent_step = 0;
    int indent = 0;
};

/*
 * Абстрактный базовый класс Object служит для унифицированного хранения
 * конкретных тегов SVG-документа
 * Реализует паттерн "Шаблонный метод" для вывода содержимого тега
 */
class Object {
public:
    void Render(const RenderContext& context) const;

    virtual ~Object() = default;

private:
    virtual void RenderObject(const RenderContext& context) const = 0;
};

template <typename Owner>
class PathProps {
public:
    Owner& SetFillColor(Color color) {
        fill_color_ = std::move(color);
        return AsOwner();
    }

    Owner& SetStrokeColor(Color color) {
        stroke_color_ = std::move(color);
        return AsOwner();
    }

    Owner& SetStrokeWidth(double width) {
        stroke_width_ = width;
        return AsOwner();
    }

    Owner& SetStrokeLineCap(StrokeLineCap stroke_line_cap) {
        stroke_line_cap_ = stroke_line_cap;
        return AsOwner();
    }

    Owner& SetStrokeLineJoin(StrokeLineJoin stroke_line_join) {
        stroke_line_join_ = stroke_line_join;
        return AsOwner();
    }

protected:
    ~PathProps() = default;

    void RenderAttrs(std::ostream& out) const {
        using detail::RenderOptionalAttr;
        using namespace std::literals;

        RenderOptionalAttr(out, "fill"sv, fill_color_);
        RenderOptionalAttr(out, " stroke"sv, stroke_color_);
        RenderOptionalAttr(out, " stroke-width"sv, stroke_width_);
        RenderOptionalAttr(out, " stroke-linecap"sv, stroke_line_cap_);
        RenderOptionalAttr(out, " stroke-linejoin"sv, stroke_line_join_);
    }

private:
    Owner& AsOwner() {
        return static_cast<Owner&>(*this);
    }

    std::optional<Color> fill_color_;
    std::optional<Color> stroke_color_;
    std::optional<double> stroke_width_;
    std::optional<StrokeLineCap> stroke_line_cap_;
    std::optional<StrokeLineJoin> stroke_line_join_;
};

/*
 * Класс Circle моделирует элемент <circle> для отображения круга
 * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/circle
 */
class Circle final : public Object, public PathProps<Circle> {
public:
    Circle& SetCenter(Point center);
    Circle& SetRadius(double radius);

private:
    void RenderObject(const RenderContext& context) const override;

    Point center_;
    double radius_ = 1.0;
};

/*
 * Класс Polyline моделирует элемент <polyline> для отображения ломаных линий
 * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/polyline
 */
class Polyline final : public Object, public PathProps<Polyline> {
public:
    // Добавляет очередную вершину к ломаной линии
    Polyline& AddPoint(Point point);

    /*
     * Прочие методы и данные, необходимые для реализации элемента <polyline>
     */
private:
    void RenderObject(const RenderContext& context) const override;

    std::vector<Point> points_;
};

/*
 * Класс Text моделирует элемент <text> для отображения текста
 * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/text
 */
class Text : public Object, public PathProps<Text> {
public:
    // Задаёт координаты опорной точки (атрибуты x и y)
    Text& SetPosition(Point pos);

    // Задаёт смещение относительно опорной точки (атрибуты dx, dy)
    Text& SetOffset(Point offset);

    // Задаёт размеры шрифта (атрибут font-size)
    Text& SetFontSize(uint32_t size);

    // Задаёт название шрифта (атрибут font-family)
    Text& SetFontFamily(std::string font_family);

    // Задаёт толщину шрифта (атрибут font-weight)
    Text& SetFontWeight(std::string font_weight);

    // Задаёт текстовое содержимое объекта (отображается внутри тега text)
    Text& SetData(std::string data);

    // Прочие данные и методы, необходимые для реализации элемента <text>
private:
    void RenderObject(const RenderContext& context) const override;

    Point pos_{};
    Point offset_{};
    uint32_t font_size_ = 1;
    std::string font_family_{};
    std::string font_weight_{};
    std::string text_{};
};

// Interfaces
class ObjectContainer {
public:
    template <typename Obj>
    void Add(Obj object) {
        AddPtr(std::make_unique<Obj>(std::move(object)));
    };

    virtual void AddPtr(std::unique_ptr<Object>&& object) = 0; 
    virtual ~ObjectContainer() = default;
};

class Drawable {
public:
    virtual void Draw(ObjectContainer& container) const = 0;
    virtual ~Drawable() = default;
};

class Document : public ObjectContainer {
public:
    // Добавляет в svg-документ объект-наследник svg::Object
    void AddPtr(std::unique_ptr<Object>&& obj);

    // Выводит в ostream svg-представление документа
    void Render(std::ostream& out) const;

private:
    std::vector<std::unique_ptr<Object>> objects_;
};

}  // namespace svg