#pragma once

class IDraw
{
public:
    virtual ~IDraw() = default;
    virtual void Draw() = 0;
};
