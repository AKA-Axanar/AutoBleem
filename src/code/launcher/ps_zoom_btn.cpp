//
// Created by screemer on 2/11/19.
//

#include "ps_zoom_btn.h"

void PsZoomBtn::update(long time) {
    if (!visible) return;
    long timeSinceLastFrame = time - lastTime;

    if (up) {
        zoom += (zoomStep * timeSinceLastFrame);
        w = ow * zoom;
        h = oh * zoom;
    } else {
        zoom -= (zoomStep * timeSinceLastFrame);
        w = ow * zoom;
        h = oh * zoom;
    }

    if (zoom >= maxZoom) {
        up = false;
    }
    if (zoom <= 1.0f) {
        up = true;
    }


    int sizeChangeX = w - ow;
    int sizeChangeY = h - oh;

    x = ox - (sizeChangeX / 2);
    y = oy - (sizeChangeY / 2);

    lastTime = time;
}