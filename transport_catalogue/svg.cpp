#include "svg.h"

namespace svg {

using namespace std::literals;

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
    line_.push_back(point);
    return *this;
}


void Polyline::RenderObject(const RenderContext& context) const {
    auto& out = context.out;
    out << "<polyline points=\""sv ;
    bool a = true;
    for(auto i : line_){
        
        if (a == true) {out << ""; a = false ;} else out << " "; 
        
        out << i.x << ",";
        out << i.y ; 

        
    }    
   
     out << "\""sv  ;
    
    RenderAttrs(out);
     out << " " ;
    out << "/>"sv;
    
}


    
    


// ---------- Text ------------------


    Text& Text::SetPosition([[maybe_unused]]Point pos){
        pos_ = pos;
    return *this;
    }

    // Задаёт смещение относительно опорной точки (атрибуты dx, dy)
    Text& Text::SetOffset([[maybe_unused]]Point offset){
    offset_ = offset;
        return *this;
    }
    
    // Задаёт размеры шрифта (атрибут font-size)
    Text& Text::SetFontSize([[maybe_unused]]uint32_t size){
        size_ = size;
    return *this;
    }
    
    // Задаёт название шрифта (атрибут font-family)
    Text& Text::SetFontFamily([[maybe_unused]]std::string font_family){
        font_family_ = font_family;
    return *this;
    }
    
    // Задаёт толщину шрифта (атрибут font-weight)
    Text& Text::SetFontWeight([[maybe_unused]]std::string font_weight){
        font_weight_ = font_weight;
    return *this;
    }
    
    // Задаёт текстовое содержимое объекта (отображается внутри тега text)
    Text& Text::SetData([[maybe_unused]]std::string data){
        data_ = data;
    return *this;
    }
    // Прочие данные и методы, необходимые для реализации элемента <text>
    
    void Text::RenderObject(const RenderContext& context) const  {
     auto& out = context.out;
        
     out << "<text " ;
        
        
     out << "x=";
         out << "\""<< pos_.x <<"\""sv;   
             out << " ";
        
     out << "y=";
         out << "\""<< pos_.y <<"\""sv;   
             out << " ";
        
     out << "dx=";
         out << "\""<< offset_.x <<"\""sv;   
             out << " ";
        
     out << "dy=";
         out << "\""<< offset_.y <<"\""sv;   
             out << " ";

     out << "font-size=";
         out << "\""<< size_ <<"\""sv;   
             out << " ";
        
           if (font_family_ != "") {    
     out << "font-family=";
         out << "\""<< font_family_ <<"\""sv;   
             out << " ";
           }
        
            if (font_weight_ != "") {    
     out << "font-weight=";
         out << "\""<< font_weight_ <<"\""sv;   

                   }
        
        RenderAttrs(out);
 
        out << ">";
         out << data_;   
             out << "</";
         
          out << "text>";
    }


// ---------- ObjectContainer ------------------    



// ---------- Drawable ------------------    
    
    

// ---------- Document ------------------
    
    // Добавляет в svg-документ объект-наследник svg::Object
    
    void Document::AddPtr(std::unique_ptr<Object>&& obj){
       objects_.emplace_back(move(obj));
    }
    
    // Выводит в ostream svg-представление документа
    void Document::Render(std::ostream& out) const {        
    out << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"sv << std::endl;
    out << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">"sv << std::endl;  
        RenderContext A1(out);
        for (auto& a : objects_) {
            
            a->Render(A1);
        }
    out << "</svg>"sv;
    }


}  // namespace svg