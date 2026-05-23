#pragma once

class Graphic;
class App;

class Widget
{
public:
    Widget(Graphic *owner, App *application) : MyOwner(owner), MyApp(application) {};
    virtual void OnRender() {};

protected:
    Graphic *GetUI() { return MyOwner; }
    App *GetApp() { return MyApp; }

private:
    Graphic *MyOwner;
    App *MyApp;
};