#include "Editor/core/Editor.h"
#include "Editor/UI/Widget/W_HelloWorld.h"
#include "Editor/UI/Widget/W_TEST.h"
#include "Editor/UI/Widget/W_Demo.h"

int main()
{
    Editor editor;

    editor.GetUI()->CreateWidget<W_HelloWorld>();
    editor.GetUI()->CreateWidget<W_TEST>();
    editor.GetUI()->CreateWidget<W_Demo>();

    editor.Run();

    return 0;
}
