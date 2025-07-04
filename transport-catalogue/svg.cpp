#include "svg.h"
#include <string_view>

namespace svg {

using namespace std::literals;

namespace detail {

void StringToHtmlEncode(std::ostream& out, std::string_view text) {
    for(auto& c : text) {
        switch(c) {
            case '"':
            out << "&quot;"sv;
            break;
            case '\'':
            out << "&apos;"sv;
            break;
            case '<':
            out << "&lt;"sv;
            break;
            case '>':
            out << "&gt"sv;
            break;
            case '&':
            out << "&amp;"sv;
            break;
            default:
            out.put(c);
            break;
        }
    }
}
}  // namespace detail

std::ostream& operator<<(std::ostream& out, StrokeLineCap stroke_line_cap) {
    switch(stroke_line_cap) {
        case StrokeLineCap::BUTT:
        out << "butt"sv;
        break;
        case StrokeLineCap::ROUND:
        out << "round"sv;
        break;
        case StrokeLineCap::SQUARE:
        out << "square"sv;
        break;
    }
    return out;
}

std::ostream& operator<<(std::ostream& out, StrokeLineJoin stroke_line_join) {
    switch(stroke_line_join) {
        case StrokeLineJoin::ARCS:
        out << "arcs"sv;
        break;
        case StrokeLineJoin::BEVEL:
        out << "bevel"sv;
        break;
        case StrokeLineJoin::MITER:
        out << "miter"sv;
        break;
        case StrokeLineJoin::MITER_CLIP:
        out << "miter-clip"sv;
        break;
        case StrokeLineJoin::ROUND:
        out << "round"sv;
        break;
    }
    return out;
}

void ColorPrint::operator()(std::monostate) const {
    out << NoneColor;
}

void ColorPrint::operator()(const std::string& color) const {
    out << color;
}

void ColorPrint::operator()(const Rgb& color) const {
    out << "rgb("sv << static_cast<int>(color.red) << ","sv
                    << static_cast<int>(color.green) << ","sv
                    << static_cast<int>(color.blue) << ")"sv;
}

void ColorPrint::operator()(const Rgba& color) const {
    out << "rgba("sv << static_cast<int>(color.red) << ","sv
                     << static_cast<int>(color.green) << ","sv
                     << static_cast<int>(color.blue) << ","sv
                     << color.opacity << ")"sv;
}

std::ostream& operator<<(std::ostream& out, const Color& color) {
    std::visit(ColorPrint(out), color);
    return out;
}

void Object::Render(const RenderContext& context) const {
    context.RenderIndent();

    // Делегируем вывод тега своим подклассам
    RenderObject(context);

    context.out << std::endl;
}

// ---------- Circle ------------------

Circle& Circle::SetCenter(Point center)  {
    center_ = center;
    return *this;
}

Circle& Circle::SetRadius(double radius)  {
    radius_ = radius;
    return *this;
}

void Circle::RenderObject(const RenderContext& context) const {
    auto& out = context.out;
    out << "<circle cx=\""sv << center_.x << "\" cy=\""sv << center_.y << "\" "sv;
    out << "r=\""sv << radius_ << "\" "sv;
    RenderAttrs(out);
    out << "/>"sv;
}

// ---------- Polyline ------------------
Polyline& Polyline::AddPoint(Point point) {
    points_.push_back(std::move(point));
    return *this;
}

void Polyline::RenderObject(const RenderContext& context) const {
    auto& out = context.out;
    bool first = true;
    
    out << "<polyline points=\""sv;

    for(const Point& point : points_) {
        if(first) {
            first = false;
        } else {
            out << ' ';
        }
        out << point.x << ',' << point.y;
        first = false;
    }
    
    out << "\" "sv;
    RenderAttrs(out);
    out << "/>"sv;
}

// ------------ Text -------------------
Text& Text::SetPosition(Point pos) {
    pos_ = pos;
    return *this;
}

Text& Text::SetOffset(Point offset) {
    offset_ = offset;
    return *this;
}

Text& Text::SetFontSize(uint32_t font_size) {
    font_size_ = font_size;
    return *this;
}

Text& Text::SetFontFamily(std::string font_family) {
    font_family_ = std::move(font_family);
    return *this;
}

Text& Text::SetFontWeight(std::string font_weight) {
    font_weight_ = std::move(font_weight);
    return *this;
}

Text& Text::SetData(std::string text) {
    text_ = std::move(text);
    return *this;
}

void Text::RenderObject(const RenderContext& context) const {
    auto& out = context.out;

    out << "<text "sv;
    RenderAttrs(out);

    using detail::RenderAttr;
    RenderAttr(out, " x"sv, pos_.x);
    RenderAttr(out, " y"sv, pos_.y);
    RenderAttr(out, " dx"sv, offset_.x);
    RenderAttr(out, " dy"sv, offset_.y);
    RenderAttr(out, " font-size"sv, font_size_);
    if(!font_family_.empty()) {
        RenderAttr(out, " font-family"sv, font_family_);
    }
    if(!font_weight_.empty()) {
        RenderAttr(out, " font-weight"sv, font_weight_);
    }
    out << '>';
    detail::StringToHtmlEncode(out, text_);
    out << "</text>";
}

// ---------- Document ------------------
void Document::AddPtr(std::unique_ptr<Object>&& object) {
    objects_.push_back(std::move(object));
}

void Document::Render(std::ostream& out) const {
    out << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"sv << std::endl;
    out << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">"sv << std::endl;

    RenderContext context{out, 2, 2};
    for(const auto& ob : objects_) {
        ob->Render(context);
    }

    out << "</svg>"sv;
}

}  // namespace svg