#pragma once

#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <deque>

#include <optional>


namespace svg {


    
//------------------------------------------------for graphics--------------------------------------------//
using Color = std::string;

inline const Color NoneColor{"none"};
  
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
    
 //---------------------------------------------------------------------------------------------------------//

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


    

 //Абстрактный базовый класс Object служит для унифицированного хранения конкретных тегов SVG-документа
 //Реализует паттерн "Шаблонный метод" для вывода содержимого тега
 
class Object {

public:
    void Render(const RenderContext& context) const;

    virtual ~Object() = default;

private:
    std::deque<std::string> tags_;
    virtual void RenderObject(const RenderContext& context) const = 0;
};

    
 inline std::ostream& operator << ([[maybe_unused]]std::ostream& os, [[maybe_unused]]const svg::StrokeLineCap& obj)
{
    switch(obj){
        case StrokeLineCap::BUTT: os << "butt" ; break;    
         case StrokeLineCap::ROUND: os << "round" ;     break;     
              case StrokeLineCap::SQUARE: os << "square" ; break;    
    } 

   return os;
}
    
 inline std::ostream& operator << ([[maybe_unused]]std::ostream& os, [[maybe_unused]]const svg::StrokeLineJoin& obj)
{
    
    switch(obj){
        case StrokeLineJoin::ARCS: os << "arcs" ; break;    
         case StrokeLineJoin::BEVEL: os << "bevel" ;     break;     
              case StrokeLineJoin::MITER: os << "miter" ; break;    
         case StrokeLineJoin::MITER_CLIP: os << "miter-clip" ;     break;     
              case StrokeLineJoin::ROUND: os << "round" ; break; 
    } 
   return os;
}   



    
    
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
        stroke_width_ = std::move(width);
        return AsOwner();
    }
    Owner& SetStrokeLineCap(StrokeLineCap line_cap) {
        line_cap_ = std::move(line_cap);
        return AsOwner();
    }
    Owner& SetStrokeLineJoin(StrokeLineJoin line_join) {
        line_join_ = std::move(line_join);
        return AsOwner();
    }    
    
protected:
    ~PathProps() = default;

    void RenderAttrs(std::ostream& out) const {
        using namespace std::literals;

        if (fill_color_) {
            out << " fill=\""sv << *fill_color_ << "\""sv;
        }
        if (stroke_color_) {
            out << " stroke=\""sv << *stroke_color_ << "\""sv;
        }
        if (stroke_width_) {
            out << " stroke-width=\""sv << *stroke_width_ << "\""sv;
        }
        if (line_cap_) {
            out << " stroke-linecap=\""sv << *line_cap_ << "\""sv;
        }
        if (line_join_) {
            out << " stroke-linejoin=\""sv << *line_join_ << "\""sv;
        }
        
    }

private:
    Owner& AsOwner() {
        // static_cast безопасно преобразует *this к Owner&,
        // если класс Owner — наследник PathProps
        return static_cast<Owner&>(*this);
    }

    std::optional<Color> fill_color_;
    std::optional<Color> stroke_color_;
    std::optional<double> stroke_width_;
    std::optional<StrokeLineCap> line_cap_;
    std::optional<StrokeLineJoin> line_join_;
    
  };      
    

    

//Класс Circle моделирует элемент <circle> для отображения круга
 
class Circle final : public Object, public PathProps<Circle> {
public:
    Circle& SetCenter(Point center);
    Circle& SetRadius(double radius);

private:
    void RenderObject(const RenderContext& context) const override;

    Point center_;
    double radius_ = 1.0;
};


    
    

//Класс Polyline моделирует элемент <polyline> для отображения ломаных линий

class Polyline final : public Object, public PathProps<Polyline>   {
public:
    // Добавляет очередную вершину к ломаной линии
    Polyline& AddPoint(Point point);

    private:
    
    void RenderObject(const RenderContext& context) const override;
    
    std::vector<Point> line_;
    
};

    
 
    

//Класс Text моделирует элемент <text> для отображения текста

class Text final : public Object,  public PathProps<Text>   {
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
    
    Point pos_ = {0.0, 0.0};
    Point offset_ = {0.0, 0.0};
    uint32_t size_ = 1;
    std::string font_family_ = "";
    std::string font_weight_ = "";
    std::string data_  = "" ;
    
    
};

class ObjectContainer {
public:
    virtual ~ObjectContainer(){};

    virtual void AddPtr(std::unique_ptr<Object>&& obj) = 0;
    
    template <typename Obj>
    void Add(Obj obj) {
        AddPtr(std::make_unique<Obj>(std::move(obj)));
    } 


};

// Интерфейс Drawable задаёт объекты, которые можно нарисовать с помощью Graphics
class Drawable {
public:
    virtual ~Drawable(){};

    virtual void Draw(ObjectContainer& object_container) const = 0;
};     
    
    
class Document : public ObjectContainer {

public:

    // Добавляет в svg-документ объект-наследник svg::Object
    void AddPtr(std::unique_ptr<Object>&& obj) override;
    
    // Выводит в ostream svg-представление документа
    void Render(std::ostream& out) const;

    
    // Прочие методы и данные, необходимые для реализации класса Document
    protected:
    std::vector<std::unique_ptr<Object>> objects_; 
    
};


    
    
}  // namespace svg


