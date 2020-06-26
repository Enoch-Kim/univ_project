/*
    wikipedia sha2 의사코드 참고
    https://en.wikipedia.org/wiki/SHA-2#Pseudocode
*/
#include <iostream>
#include <vector>
#include <string>
#include <array>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <iomanip>
#include <fstream>
#include <sstream>
// initial variable

// template<typename T>
// inline T rotateReft(T& target, int bits){
//     return (target << bits) | (target >> (32-bits));
// }
using std::string;
using std::cout;

u_int32_t *h = new u_int32_t[8]{
    0x6a09e667,
    0xbb67ae85,
    0x3c6ef372,
    0xa54ff53a,
    0x510e527f,
    0x9b05688c,
    0x1f83d9ab,
    0x5be0cd19
};

u_int32_t *K = new u_int32_t[64]{
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1,
	0x923f82a4, 0xab1c5ed5, 0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
	0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174, 0xe49b69c1, 0xefbe4786,
	0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
	0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147,
	0x06ca6351, 0x14292967, 0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
	0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85, 0xa2bfe8a1, 0xa81a664b,
	0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
	0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a,
	0x5b9cca4f, 0x682e6ff3, 0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
	0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

// u_int32_t h0 = 0x67452301;
// u_int32_t h1 = 0xEFCDAB89;
// u_int32_t h2 = 0x98BADCFE;
// u_int32_t h4 = 0x10325476;
// u_int32_t h5 = 0xC3D2E1F0;

string getSHA256(char* arg);


int main(int argc, char* argv[]) {
    if(argc < 2){
        cout << "Please Enter arguments" << '\n';
    }
    
    string sha1 = getSHA256(argv[1]);

    cout << sha1 << '\n';

    return 0;
}

u_int32_t rotateLeft(u_int32_t target, int bits){
    return (target << bits) | (target >> (32 - bits));
}

u_int32_t rotateRight(u_int32_t target, int bits){
    return (target >> bits) | (target << (32 - bits));
}

uint32_t Ch(uint32_t x, uint32_t y, uint32_t z)
{
    return (x & y) ^ (~x & z);
}

uint32_t Ma(uint32_t x, uint32_t y, uint32_t z)
{
    return (x & y) ^ (x & z) ^ (y & z);
}



uint32_t Sigma0(uint32_t x)
{
    return rotateRight(x, 2) ^ rotateRight(x, 13) ^ rotateRight(x, 22);
}

uint32_t Sigma1(uint32_t x)
{
    return rotateRight(x, 6) ^ rotateRight(x, 11) ^ rotateRight(x, 25);
}

/*
append the bit '1' to the message e.g. by adding 0x80 if message length is a multiple of 8 bits.
append 0 ≤ k < 512 bits '0', such that the resulting message length in bits
   is congruent to −64 ≡ 448 (mod 512)
append ml, the original message length, as a 64-bit big-endian integer. 
   Thus, the total length is a multiple of 512 bits.

*/

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

// SHA-2 가 뚜렷하게 구별되는 핵심 함수
void doSha256ForEachBlock(string&& block){
    std::vector<u_int32_t> words(64, 0); // 여기선 64개의 워드로 나눈다
    for(int i=0; i<16; i++){
        words[i] = (block[4*i+3] & 0xff) 
                | (block[4*i+2] & 0xff)<<8 
                | (block[4*i+1] & 0xff)<<16 
                | (block[4*i+0] & 0xff)<<24;
    }

    // Extend the first 16 words into the remaining 48 words w[16..63] of the message schedule array
    // 연산자 우선순위 꼭 고려하자. 항상 xor은 따로 묶어줘야한다 + 보다 우선순위 낮다
    for(int i=16; i<64; i++){
        words[i] = words[i-16]
                 + (rotateRight(words[i-15], 7) ^ rotateRight(words[i-15], 18) ^ (words[i-15] >> 3))
                 + words[i-7]
                 + (rotateRight(words[i-2], 17) ^ rotateRight(words[i-2], 19) ^ (words[i-2] >> 10));
    }

    u_int32_t A = h[0];
    u_int32_t B = h[1];
    u_int32_t C = h[2];
    u_int32_t D = h[3];
    u_int32_t E = h[4];
    u_int32_t F = h[5];
    u_int32_t G = h[6];
    u_int32_t H = h[7];

    // Compression function main loop:
    for(int i=0; i<64; i++){
        u_int32_t temp1 = H + Sigma1(E) + Ch(E, F, G) + K[i] + words[i];
        u_int32_t temp2 = Sigma0(A) + Ma(A, B, C);
        
        H = G;
        G = F;
        F = E;
        E = D + temp1;
        D = C;
        C = B;
        B = A;
        A = temp1 + temp2;
    }
    // Add the compressed chunk to the current hash value
    h[0] += A;
    h[1] += B;
    h[2] += C;
    h[3] += D;
    h[4] += E;
    h[5] += F;
    h[6] += G;
    h[7] += H;
}


string getSHA256(char* arg){
    string message(arg);

    padding(message);

    if(message.size() % 64 != 0) {
        cout << "Something wrong" << '\n';
        exit(1);
    }
    
    int blockNum = message.size() / 64;
    
    // for(int i=blockNum-1; i>=0; --i){
    for(int i=0; i<blockNum; ++i){
        doSha256ForEachBlock(message.substr(i*64, 64));
    }
    
    
    string result = "";

    for(int i=0; i<8; i++){
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