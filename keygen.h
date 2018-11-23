//
//  keygen.hpp
//  HelloWorld
//
//  Created by Vasisht Sankaranrayanan on 20/11/18.
//  Copyright © 2018 Vasisht Sankaranrayanan. All rights reserved.
//

#ifndef keygen_hpp
#define keygen_hpp


#include "keygen.h"
#include <iostream>
#include <cmath>
#include<bitset>
#include <string>
using namespace std;
typedef long double ll;

class bbs{
    
    ll p, q, M, seed, actual;
    
    ll gcd(ll a, ll b){
        if(b == 0) return a;
        return gcd(b, fmod(a,b));
    }
    
public:
    
    bbs(ll p, ll q, ll s){
        this->p = p;
        this->q = q;
        this->seed = s;
        M = p*q;
        actual = s;
    }
    
    ll getrandom(){
        
        ll r = fmod(actual*actual,M);
        actual = r;
        return r;
    }
    
    ll getirandom(int i){
        
        ll g = gcd(p, q);
        ll lcm = p*q/g;
        
        ll exp = 1;
        for(int j = 1; j <= i; ++j) exp = fmod((exp+exp),lcm);
        
        ll x0 = seed*seed;
        ll r = x0;
        
        for(int j = 2; j <= exp; ++j) {
            r = fmod((r*x0),M);
        }
        
        return r;
    }
    
    char ConvertHex(unsigned char x){
        if(x<10) return(char)((x)+'0');
        if(x>=10) return(char)((x - 10)+'A');
        return '0';
    }
    
    void generateKey(unsigned char* key){
            ll p = 65537;
            ll q = 10007;
            ll s = 100140048;
            int strkey[4];
            short y;
            int quot;
            int key_index = 0;
            bbs b(p, q, s);
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
    
};

#endif /* keygen_hpp */
