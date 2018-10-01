#pragma once

#include <memory>


namespace gv {


class GlobeViewer
{
public:
    GlobeViewer();
    ~GlobeViewer();

    bool validSetup() const;
    void render();

    void resize( int w, int h );
    void move( int x, int y );

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};


}