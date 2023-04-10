#pragma once
#include "transport_catalogue.h"
#include "map_renderer.h"
#include <optional>

// Класс RequestHandler играет роль Фасада, упрощающего взаимодействие JSON reader-а с другими подсистемами приложения.

class RequestHandler {
public:
    
    RequestHandler(tc::TransportСatalogue& tc, renderer::MapRenderer& renderer) : tc_(tc), renderer_(renderer)  {}
 
    void RenderMap();

private:
    // RequestHandler использует агрегацию объектов "Транспортный Справочник" и "Визуализатор Карты"
    tc::TransportСatalogue& tc_;
    renderer::MapRenderer& renderer_;

 
};