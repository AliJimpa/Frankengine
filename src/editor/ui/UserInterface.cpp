
#include "UserInterface.h"
#include "W_HelloWorld.h"
#include "W_TEST.h"
#include "W_Demo.h"

UserInterface::UserInterface(App *owner) : Graphic(owner)
{
    CreateWidget<W_HelloWorld>();
    CreateWidget<W_TEST>();
    CreateWidget<W_Demo>();
}