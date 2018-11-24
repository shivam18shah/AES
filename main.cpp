//
//  main.cpp
//  HelloWorld
//
//  Created by Vasisht Sankaranrayanan on 03/11/18.
//  Copyright © 2018 Vasisht Sankaranrayanan. All rights reserved.
//

#include <iostream>
#include <curses.h>
#include "lookupboxes.h"
#include "keygen.h"
#include <typeinfo>
#include <stdlib.h>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>
#include <stdio.h>
#include <sstream>
#include<algorithm>

using namespace std;

void PrintHex(unsigned char x){
    if(x/16<10) cout<<(char)((x/16)+'0');
    if(x/16>=10) cout<<(char)((x/16 - 10)+'A');
    if(x%16<10) cout<<(char)((x%16)+'0');
    if(x%16>=10) cout<<(char)((x%16-10)+'A');
}

void KeyExpansionCore(unsigned char* in, unsigned char i){
    cout<<"KEY CORE"<<endl;
    //rotate left
    unsigned int* q = (unsigned int*)in;
    *q = (*q >> 8) | ((*q & 0xff) << 24);
    //    unsigned char t = in[0];
    //    in[0] = in[1];
    //    in[1] = in[2];
    //    in[2] = in[3];
    //    in[3] = t;
    
    //s-box four bytes
    cout<<int(in[0])<<": "<<s_box[in[0]]<<endl;
    in[0] = s_box[in[0]];
    in[1] = s_box[in[1]];
    in[2] = s_box[in[2]];
    in[3] = s_box[in[3]];
    
    cout<<in[0]<<endl;
    //RCon
    cout<<"RCON: ";
    cout<<in[0]<<" "<<int(rcon[i])<<endl;
    in[0] ^= rcon[i];
    cout<<"AFTER RCON: "<<in[0]<<endl;
}

void KeyExpansion(unsigned char* inputKey, unsigned char* expandedKeys){
    //the first 16 bytes generated are the original key
    cout<<"Key being copied: ";
    for(int i=0; i<16; i++){
        expandedKeys[i] = inputKey[i];
        cout<<int(inputKey[i])<<" ";
    }
    cout<<endl;
    //variables:
    int bytesGenerated = 16; //we've generated 16 btyes so far, keep track of all the bytes generated
    int rconIteration = 1; //rcon itearation begins at 1
    unsigned char temp[4]; //temp storage for core
    
    while(bytesGenerated<176){ //176 because we go through 10 rounds of generating 16 bytes, and we have 16 bytes at the start
        for(int i=0; i<4; i++){
            temp[i] = expandedKeys[i + bytesGenerated - 4]; //read previously generated 4 bytes
        }
        
        //perform the core once for each 16 byte key which is generated:
        //once every new key, call key expansion core
        if(bytesGenerated%16==0){
            KeyExpansionCore(temp, rconIteration++);
        }
        
        //XOR temp with [bytesGenerated-16] and store in expandedKeys
        for(unsigned char a=0; a<4; a++){
            expandedKeys[bytesGenerated] = expandedKeys[bytesGenerated - 16] ^ temp[a];
            bytesGenerated++;
        }
    }
}

void SubBytes(unsigned char* state){
    for(int i=0; i<16; i++){
        state[i] = s_box[state[i]];
    }
}

void ShiftRows(unsigned char* state){
    unsigned char tmp[16];
    tmp[0] = state[0];
    tmp[1] = state[5];
    tmp[2] = state[10];
    tmp[3] = state[15];

    tmp[4] = state[4];
    tmp[5] = state[9];
    tmp[6] = state[14];
    tmp[7] = state[3];

    tmp[8] = state[8];
    tmp[9] = state[13];
    tmp[10] = state[2];
    tmp[11] = state[7];

    tmp[12] = state[12];
    tmp[13] = state[1];
    tmp[14] = state[6];
    tmp[15] = state[11];

    for(int i=0; i<16; i++){
        state[i] = tmp[i];
    }
}

void MixColumns(unsigned char* state){
    unsigned char tmp[16];
    
    tmp[0] = (unsigned char)(mul2[state[0]] ^ mul3[state[1]] ^ state[2] ^ state[3]);
    tmp[1] = (unsigned char)(state[0] ^ mul2[state[1]] ^ mul3[state[2]] ^ state[3]);
    tmp[2] = (unsigned char)(state[0] ^ state[1] ^ mul2[state[2]] ^ mul3[state[3]]);
    tmp[3] = (unsigned char)(mul3[state[0]] ^ state[1] ^ state[2] ^ mul2[state[3]]);
    
    tmp[4] = (unsigned char)(mul2[state[4]] ^ mul3[state[5]] ^ state[6] ^ state[7]);
    tmp[5] = (unsigned char)(state[4] ^ mul2[state[5]] ^ mul3[state[6]] ^ state[7]);
    tmp[6] = (unsigned char)(state[4] ^ state[5] ^ mul2[state[6]] ^ mul3[state[7]]);
    tmp[7] = (unsigned char)(mul3[state[4]] ^ state[5] ^ state[6] ^ mul2[state[7]]);
    
    tmp[8] = (unsigned char)(mul2[state[8]] ^ mul3[state[9]] ^ state[10] ^ state[11]);
    tmp[9] = (unsigned char)(state[8] ^ mul2[state[9]] ^ mul3[state[10]] ^ state[11]);
    tmp[10] = (unsigned char)(state[8] ^ state[9] ^ mul2[state[10]] ^ mul3[state[11]]);
    tmp[11] = (unsigned char)(mul3[state[8]] ^ state[9] ^ state[10] ^ mul2[state[11]]);
    
    tmp[12] = (unsigned char)(mul2[state[12]] ^ mul3[state[13]] ^ state[14] ^ state[15]);
    tmp[13] = (unsigned char)(state[12] ^ mul2[state[13]] ^ mul3[state[14]] ^ state[15]);
    tmp[14] = (unsigned char)(state[12] ^ state[13] ^ mul2[state[14]] ^ mul3[state[15]]);
    tmp[15] = (unsigned char)(mul3[state[12]] ^ state[13] ^ state[14] ^ mul2[state[15]]);
    
    for (int i = 0; i < 16; i++)
        state[i] = tmp[i];
}

void AddRoundKey(unsigned char* state, unsigned char* roundKey){
    for(int i=0; i<16; i++){
        state[i] ^= roundKey[i];
    }
}

void inv_shift_rows(unsigned char* state) {
    unsigned char tmp[16];
    
    // First row don't shift (idx = idx)
    tmp[0] = state[0];
    tmp[4] = state[4];
    tmp[8] = state[8];
    tmp[12] = state[12];
    
    // Second row shift right once (idx = (idx - 4) % 16)
    tmp[1] = state[13];
    tmp[5] = state[1];
    tmp[9] = state[5];
    tmp[13] = state[9];
    
    // Third row shift right twice (idx = (idx +/- 8) % 16)
    tmp[2] = state[10];
    tmp[6] = state[14];
    tmp[10] = state[2];
    tmp[14] = state[6];
    
    // Fourth row shift right three times (idx = (idx + 4) % 16)
    tmp[3] = state[7];
    tmp[7] = state[11];
    tmp[11] = state[15];
    tmp[15] = state[3];
    
    for (int i = 0; i < 16; i++)
        state[i] = tmp[i];
}

void inv_sub_bytes(unsigned char* state) {
    // Substitute each state value with another byte in the Rijndael S-Box
    for (int i = 0; i < 16; i++)
        state[i] = inv_s_box[state[i]];
}

void inv_mix_columns(unsigned char* state) {
    unsigned char tmp[16];
    
    // Column 1
    tmp[0] = (unsigned char) (mul14[state[0]] ^ mul11[state[1]] ^ mul13[state[2]] ^ mul9[state[3]]);
    tmp[1] = (unsigned char) (mul9[state[0]] ^ mul14[state[1]] ^ mul11[state[2]] ^ mul13[state[3]]);
    tmp[2] = (unsigned char) (mul13[state[0]] ^ mul9[state[1]] ^ mul14[state[2]] ^ mul11[state[3]]);
    tmp[3] = (unsigned char) (mul11[state[0]] ^ mul13[state[1]] ^ mul9[state[2]] ^ mul14[state[3]]);
    
    // Column 2
    tmp[4] = (unsigned char) (mul14[state[4]] ^ mul11[state[5]] ^ mul13[state[6]] ^ mul9[state[7]]);
    tmp[5] = (unsigned char) (mul9[state[4]] ^ mul14[state[5]] ^ mul11[state[6]] ^ mul13[state[7]]);
    tmp[6] = (unsigned char) (mul13[state[4]] ^ mul9[state[5]] ^ mul14[state[6]] ^ mul11[state[7]]);
    tmp[7] = (unsigned char) (mul11[state[4]] ^ mul13[state[5]] ^ mul9[state[6]] ^ mul14[state[7]]);
    
    // Column 3
    tmp[8] = (unsigned char) (mul14[state[8]] ^ mul11[state[9]] ^ mul13[state[10]] ^ mul9[state[11]]);
    tmp[9] = (unsigned char) (mul9[state[8]] ^ mul14[state[9]] ^ mul11[state[10]] ^ mul13[state[11]]);
    tmp[10] = (unsigned char) (mul13[state[8]] ^ mul9[state[9]] ^ mul14[state[10]] ^ mul11[state[11]]);
    tmp[11] = (unsigned char) (mul11[state[8]] ^ mul13[state[9]] ^ mul9[state[10]] ^ mul14[state[11]]);
    
    // Column 4
    tmp[12] = (unsigned char) (mul14[state[12]] ^ mul11[state[13]] ^ mul13[state[14]] ^ mul9[state[15]]);
    tmp[13] = (unsigned char) (mul9[state[12]] ^ mul14[state[13]] ^ mul11[state[14]] ^ mul13[state[15]]);
    tmp[14] = (unsigned char) (mul13[state[12]] ^ mul9[state[13]] ^ mul14[state[14]] ^ mul11[state[15]]);
    tmp[15] = (unsigned char) (mul11[state[12]] ^ mul13[state[13]] ^ mul9[state[14]] ^ mul14[state[15]]);
    
    for (int i = 0; i < 16; i++)
        state[i] = tmp[i];
}

void AES_Encrypt(unsigned char* message, unsigned char* key){
    
    unsigned char state[16];
    for(int i=0; i<16; i++){
        state[i] = message[i];
        cout<<message[i];
    }
    cout<<endl;
    int numberOfRounds = 9; //apart from final round
    
    //Expand the keys:
    unsigned char expandedKey[176];
    KeyExpansion(key, expandedKey);
    AddRoundKey(state, key); //whitening addRoundKey
    for(int i=0; i<numberOfRounds; i++){
        SubBytes(state);
        ShiftRows(state);
        MixColumns(state);
        AddRoundKey(state, expandedKey + (16*(i+1)));
    }
    
    //Final Round
    SubBytes(state);
    ShiftRows(state);
    AddRoundKey(state, expandedKey + 160);
    
    for(int i=0; i<16; i++){
        message[i] = state[i];
    }
}


void AES_Decrypt(unsigned char* message, unsigned char* key){
    unsigned char state[16];
    
    // Take only the first 16 characters of the message
    for (int i = 0; i < 16; i++)
        state[i] = message[i];
    
    const int round_cnt = 9;
    unsigned char expandedKey[176];
    KeyExpansion(key, expandedKey);
    AddRoundKey(state, expandedKey + 160);
    
    for (int i = round_cnt; i > 0; i--) {
        inv_shift_rows(state);
        inv_sub_bytes(state);
        AddRoundKey(state, expandedKey + (16 * i));
        inv_mix_columns(state);
    }
    inv_shift_rows(state);
    inv_sub_bytes(state);
    AddRoundKey(state, expandedKey);
    
    for(int i=0; i<16; i++){
        message[i] = state[i];
    }
}

void generateKey(unsigned char* key, bbs &b){
    int strkey[4];
    short y;
    int quot;
    int key_index = 0;
    for(int i = 0; i < 128; ++i) {
        std::bitset<1> y(b.getrandom());
        strkey[i%4]=int(y.to_ulong());
        if(i%8 == 7){
            int binNum = 0;
            for(int j=3; j>=0; j--){
                binNum += strkey[j%3] * pow(10, j);
            }
            int decimalNumber = 0, i = 0, remainder;
            while (binNum!=0)
            {
                remainder = binNum%10;
                binNum /= 10;
                decimalNumber += remainder*pow(2,i);
                ++i;
            }
            key[key_index] = decimalNumber;
            key_index++;
        }
    }
}

std::string exec(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::shared_ptr<FILE> pipe(popen(cmd, "r"), pclose);
    if (!pipe) throw std::runtime_error("popen() failed!");
    while (!feof(pipe.get())) {
        if (fgets(buffer.data(), 128, pipe.get()) != nullptr)
            result += buffer.data();
    }
    return result;
}

int main(){
//        unsigned char key[16] =
//        {
//            1, 2, 3, 4,
//            5, 6, 7, 8,
//            9, 10, 11, 12,
//            13, 14, 15, 16
//        };
    unsigned char key[16];
    unsigned char iv[16];
    for(int i=0; i<16; i++){
        string key_str = exec("od -vAn -N1 -tu < /dev/urandom");
        string iv_str = exec("od -vAn -N1 -tu < /dev/urandom");
        key_str.erase(remove(key_str.begin(), key_str.end(), ' '), key_str.end()); //removing white spaces from the key string
        iv_str.erase(remove(iv_str.begin(), iv_str.end(), ' '), iv_str.end());//removing white spaces from the iv string
        key_str.erase(remove(key_str.begin(), key_str.end(), '\n'), key_str.end()); //removing new line characters from the key string
        iv_str.erase(remove(iv_str.begin(), iv_str.end(), '\n'), iv_str.end()); //removing new line characters from the iv string
        //convert string to integer
        stringstream ss_key(key_str);
        stringstream ss_iv(iv_str);
        int key_int = 0;
        int iv_int = 0;
        ss_key >> key_int;
        ss_iv >> iv_int;
        key[i] = key_int;
        iv[i] = iv_int;
    }
    
    unsigned char message[] = "This is a message we will encrypt with DES!";
    int originalLen = strlen((const char*)message);
    int lenOfPaddedMessage = originalLen;
    if(lenOfPaddedMessage%16!=0){
        lenOfPaddedMessage = (lenOfPaddedMessage/16+1) * 16;
    }
    unsigned char* paddedMessage = new unsigned char[lenOfPaddedMessage];
    unsigned char* c = new unsigned char[lenOfPaddedMessage];
    
    for(int i=0; i<lenOfPaddedMessage; i++){
        if(i>=originalLen){paddedMessage[i] = 0;}
        else {paddedMessage[i] = message[i];}
    }
    
    for(int i=0; i<16; i++){
        c[i] = iv[i];
    }
    unsigned char current_msg[16];
    //CBC starts here
    for(int i=0; i<lenOfPaddedMessage; i+=16){
        for(int j=0; j<16; j++){
            current_msg[j] = current_msg[i+j] ^ c[i+j];
        }
        AES_Encrypt(current_msg, key);
        for(int k=0; k<16; k++){
            c[i+k+16] = current_msg[k];
        }
//        AES_Encrypt(paddedMessage+i, key);
    }
    
    cout<<"\nEncrypted Message: "<<endl;
    for(int i=0; i<lenOfPaddedMessage; i++){
        PrintHex(c[i]);
        cout<<" ";
    }
    
   
    for(int i=0; i<lenOfPaddedMessage-1; i+=16){
        for(int j=0; j<16; j++){
            AES_Decrypt(c+i+j+16, key);
        }
        for(int j=0; j<16; j++){
            c[i+j] = c[i+j] ^ c[i+j+16];
        }
//        AES_Decrypt(paddedMessage+i , key);
    }
    
    cout<<"\nDecrypted Message: "<<endl;
    for(int i=0; i<lenOfPaddedMessage; i++){
        cout<<(paddedMessage[i]);
    }
    cout<<endl;
    delete[] paddedMessage;
    return 0;
}

