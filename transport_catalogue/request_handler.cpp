#include "request_handler.h"


void RequestHandler::RenderMap() {
    renderer_.Render(tc_.GetRoutesNames(), std::cout);
}
