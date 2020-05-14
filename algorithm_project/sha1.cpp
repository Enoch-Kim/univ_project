/*
    wikipedia sha1 의사코드 참고
    https://en.wikipedia.org/wiki/SHA-1#SHA-1_pseudocode
*/
#include <iostream>
#include <vector>
#include <string>

// initial variable

// template<typename T>
// inline T rotateReft(T& target, int bits){
//     return (target << bits) | (target >> (32-bits));
// }
using std::string;
using std::cout;

u_int32_t* h = new u_int32_t[5]{
    0x67452301,
    0xEFCDAB89,
    0x98BADCFE,
    0x10325476,
    0xC3D2E1F0
};

// u_int32_t h0 = 0x67452301;
// u_int32_t h1 = 0xEFCDAB89;
// u_int32_t h2 = 0x98BADCFE;
// u_int32_t h4 = 0x10325476;
// u_int32_t h5 = 0xC3D2E1F0;

string getSHA1(char* arg);


int main(int argc, char* argv[]) {
    if(argc < 2){
        cout << "Please Enter arguments" << '\n';
    }
    
    string sha1 = getSHA1(argv[1]);

    cout << sha1 << '\n';

    return 0;
}

u_int32_t rotateLeft(u_int32_t target, int bits){
    return (target<<bits) | (target>>(32 - bits));
}

void padding(string& message){
    u_int64_t messageLength = message.size()*8; // length in bits... 이걸로 1시간 소모...
    
    message += char(0x80);      // 원래는 1만 추가해야하지만 후에 0이 붙을 수밖에 없으므로 8bit의 기준으로 붙여준다.

    int mod512 = message.size() % 64; // 64byte == 512bit
    // 8byte의 length 정보를 담기위해 56byte 단위로 맞춰준다.
    if(mod512 > 56)
        message += string(64 - mod512 + 56, char(0x00));
    else
        message += string(56 - mod512, char(0x00));

    for(int i=7; i>=0; i--){
        char sizeToByte = (messageLength >> 8*i) & 0xff; 
        message += sizeToByte;
    }

}

void doSha1ForEachBlock(string&& block){
    std::vector<u_int32_t> words(80, 0);
    for(int i=0; i<16; i++){
        words[i] = (block[4*i+3] & 0xff) 
                | (block[4*i+2] & 0xff)<<8 
                | (block[4*i+1] & 0xff)<<16 
                | (block[4*i+0] & 0xff)<<24;
    }
    for(int i=16; i<80; i++){
        words[i] = rotateLeft(words[i-16] ^ words[i-14] ^ words[i-8] ^ words[i-3], 1);
    }

    u_int32_t A = h[0];
    u_int32_t B = h[1];
    u_int32_t C = h[2];
    u_int32_t D = h[3];
    u_int32_t E = h[4];

    for(int i=0; i<80; i++){
        u_int32_t F=0, K=0;
        if(i < 20){
            F = (B & C) | (~B & D);
            K = 0x5A827999;
        }else if(i < 40){
            F = B ^ C ^ D;
            K = 0x6ED9EBA1;
        }else if(i < 60){
            F = (B & C) | (B & D) | (C & D);
            K = 0x8F1BBCDC;
        }else{
            F = B ^ C ^ D;
            K = 0xCA62C1D6;
        }

        u_int32_t temp = rotateLeft(A, 5) + F + E + K + words[i];
        E = D;
        D = C;
        C = rotateLeft(B, 30);
        B = A;
        A = temp;
    }

    h[0] += A;
    h[1] += B;
    h[2] += C;
    h[3] += D;
    h[4] += E;
}

string getSHA1(char* arg){
    string message(arg);

    padding(message);

    if(message.size() % 64 != 0) {
        cout << "Something wrong" << '\n';
        exit(1);
    }

    int blockNum = message.size() / 64;
    
    for(int i=blockNum-1; i>=0; --i){
        doSha1ForEachBlock(message.substr(i*64, 64));
    }

    string result = "";

    for(int i=0; i<5; i++){
        for(int j=0; j<8; j++){
            u_int32_t temp = ((h[i] << j*4)>> 28);

            if( temp < 10 ){
                result += '0' + temp;
                // cout << char('0' + temp) << "  ";
            }else{
                result += 'A' + (temp - 10);
                // cout << char('A' + (temp - 10)) << "  ";
            }
        }
        // cout << '\n';
    }

    return result;
}