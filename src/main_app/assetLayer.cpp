//
// Created by Bob on 08/02/2021.
//

#include "assetLayer.h"

void assetLayer::printIndex(const char *file) {
    std::ifstream pakFile(file);
    std::ifstream encryptionKeyFile("assets/Packs/packlist.dat");

    std::string magic = getMagic(pakFile);
    logger::log(logger::INFO, ("The magic in the file is: "+magic).c_str(), "Assets");

    if (magic=="NadeoPak") {
        logger::log(logger::INFO, "Magic is correct","Assets");
    }
    else {
        logger::log(logger::FATAL, "File magic is incorrect, please check if the file is not corrupted", "Assets");
    }

    uint8_t encryptionVer = getPackListVer(pakFile);
    logger::log(logger::INFO, ("The encryption key file version is: "+std::to_string(encryptionVer)).c_str(),"Assets");

    // Adapted code from https://wiki.xaseco.org/wiki/Packlist.dat

    uint8_t numPacks = getNumPaks(pakFile);

    uint32_t salt = getSalt(pakFile);

    std::unordered_map<std::string,std::string> packs;

    for (int i = i; i < numPacks+1; i++) {
        std::string name = getName(i,salt,pakFile);
        std::string key = getKey(i,salt,name.c_str(),pakFile);
        packs[std::string(name)] = std::string(key);
    }
    logger::log(logger::INFO,"Finished processing packlist.dat","Assets");
    for (auto pair : packs) {
            logger::log(logger::VERBOSE,(pair.first+"\t:\t"+pair.second).c_str(),"Assets");
    }
}

uint32_t assetLayer::getSalt(std::ifstream &file) {
    file.seekg(6);
    readFileWithDecl(file, 6, 4, ret, uint32_t)
    return ret;
}

uint8_t assetLayer::getPackListVer(std::ifstream &file) {
    file.seekg(0);
    readFileWithDecl(file, 0, 1, ret, uint8_t)
    return ret;
}

uint8_t assetLayer::getNumPaks(std::ifstream &file) {
    readFileWithDecl(file, 1, 1, ret, uint8_t)
    return ret;
}

std::string assetLayer::getName(uint8_t index, uint32_t salt, std::ifstream &file) {
    long counter = getIndexStart(index,file);

    file.seekg(counter);
    uint8_t nameLength = 0;
    file.read((char*)&nameLength,1);

    file.seekg(counter+1);
    char encryptedName[32];
    file.read(encryptedName,nameLength);
    encryptedName[nameLength] = '\0';

    std::string name;
    std::string nameKeyString = "6611992868945B0B59536FC3226F3FD0"+std::to_string(salt);
    uint8_t nameKey[16];
    mbedtls_md5((unsigned char *) nameKeyString.c_str(), nameKeyString.size(),
                (unsigned char *) nameKey);

    for (int j = 0; j < nameLength; j++)
        name += (char)(encryptedName[j] ^ nameKey[j % 16]);

    return name;
}

std::string assetLayer::getKey(uint8_t index, uint32_t salt, const char *name, std::ifstream &file) {
    int start = getIndexStart(index,file);
    uint8_t nameLength = 0;
    file.seekg(start);
    file.read((char*)&nameLength,1);
    file.seekg(start+nameLength);
    uint8_t encryptedKeyString[32];
    file.read((char*)&encryptedKeyString,32);
    std::string KeyStringInput = name+std::to_string(salt)+"B97C1205648A66E04F86A1B5D5AF9862";
    uint8_t keyString[32];
    mbedtls_md5((unsigned char *) KeyStringInput.c_str(), KeyStringInput.size(),
                (unsigned char *) keyString);

    for (int j = 0; j < 0x20; j++) {
        keyString[j] = encryptedKeyString[j] ^ keyString[j % 16];
    }

    std::string key;
    key.resize(16,'\0');
    mbedtls_md5((unsigned char *) (std::string((char*)keyString)+"NadeoPak").c_str(), 40,
                (unsigned char *) key.data());

    return key;
}

long assetLayer::getIndexStart(uint8_t index, std::ifstream& file) {
    int counter = 11;
    uint8_t nameLength = 0;
    file.seekg(counter);
    file.read((char*)&nameLength,sizeof(uint8_t));
    for (int i = 0; i < index-1; i++) {
        file.seekg(counter);
        file.read((char*)&nameLength,sizeof(uint8_t));
        counter += nameLength + 34;
    }
    return counter;
}

std::string assetLayer::getMagic(std::ifstream &file) {
    std::string magic;
    magic.resize(8,'\0');
    file.read(magic.data(),8);
    return magic;
}

void assetLayer::CalcIVXor(int _ivXor, char *pInput, int count) {
    for (int i = 0; i < count; i++)
    {
        uint lopart = _ivXor & 0xFFFFFFFF;
        uint hipart = _ivXor >> 32;
        lopart = (pInput[i] | 0xAA) ^ ((lopart << 13) | (hipart >> 19));
        hipart = (_ivXor << 13) >> 32;
        _ivXor = (hipart << 32) | lopart;
    }
}
