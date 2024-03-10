#include <pyramidscript.h>
#include <tokenizer.h>
#include <stdio.h>
#include <stdlib.h>

void test_tokenizer() {

}


static char* load_file(const char* path) {
    FILE* f = fopen(path, "rb");
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);  /* same as rewind(f); */

    char* string = malloc(fsize + 1);
    fread(string, fsize, 1, f);
    fclose(f);

    string[fsize] = 0;
    return string;
}

int main(int argc, char* argv[]) {
    struct PyramidScriptByteCode* ssss = PyramidScript.Compile(load_file("e:\\Project\\Develop_2023\\Sima-Engine\\scripts\\test.foe"));
    struct PyramidScriptState* state = 0;

    PyramidScript.Run(state, "e:\\Project\\Develop_2023\\Sima-Engine\\scripts\\test11.foe");

    return 0;
}