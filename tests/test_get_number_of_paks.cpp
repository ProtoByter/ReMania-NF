//
// Created by protobit on 10/02/2021.
//

#include <assetLayer.h>

int main() {
    assetLayer layer;
    std::ifstream file("assets/Packs/packlist.dat");
    if (layer.getNumPaks(file) == 10) {
        printf("Test passed");
    }
    else {
        printf("Test failed");
    }
}