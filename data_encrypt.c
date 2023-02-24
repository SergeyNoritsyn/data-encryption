#include "data_encrypt.h"
int min_int(int a, int b, int c);

int bitwise_xor(int value){
    return value ^ KEY;
}

char *xor_encrypt(char c){
    int bitwise = bitwise_xor((int)(c));
    int index = 6;
    char *encrypt = (char *)malloc(sizeof(char) * 8);
    while (bitwise > 0 || index >= 0){
        int bit = bitwise % 2;
        if (bit == 1){
            *(encrypt + index) = '1';
        }
        else{
            *(encrypt + index) = '0';
        }
        index--;
        bitwise /= 2;
    }
    *(encrypt + 7) = '\0';
    return encrypt;
}

char xor_decrypt(char *s){
    int encr_char = 0;
    int power = 1;
    for (int i = 6; i >= 0; i--){
        encr_char += (*(s + i) - 48) * power;
        power *= 2;
    }

    int bitwise = bitwise_xor(encr_char);

    return (char)(bitwise);
}

char *gen_code(char *msg){
    char *sc_code = (char *)malloc(sizeof(char) * 257);

    int num_chars = strlen(msg);
    char encr_msg[num_chars + 1][7];
    char *copy;
    for (int i = 0; i < num_chars; i++){
        copy = xor_encrypt((char)msg[i]);
        strcpy(encr_msg[i], copy);
        free(copy);
        copy = NULL;
    }
    copy = xor_encrypt('\0');
    strcpy(encr_msg[num_chars], copy);
    free(copy);
    copy = NULL;

    int word_index = 0, char_index = 0, code_index = 0;
    for (int i = 0; i < 16; i++){
        for (int j = 0; j < 16; j++){
            if (i == 15 && j == 15){
                *(sc_code + code_index) = '1';
            }
            else if (i >= 11 && j <= 4)
            {
                if (i == 11 || i == 15 || j == 0 || j == 4 || (i == 13 && j == 2)){
                    *(sc_code + code_index) = '1';
                }
                else{
                    *(sc_code + code_index) = '0';
                }
            }
            else if (i <= 4 && j <= 4){
                if (i == 0 || i == 4 || j == 0 || j == 4 || (i == 2 && j == 2)){
                    *(sc_code + code_index) = '1';
                }
                else{
                    *(sc_code + code_index) = '0';
                }
            }
            else if (i <= 4 && j >= 11){
                if (i == 0 || i == 4 || j == 11 || j == 15 || (i == 2 && j == 13)){
                    *(sc_code + code_index) = '1';
                }
                else{
                    *(sc_code + code_index) = '0';
                }
            }
            else if (word_index <= num_chars){
                *(sc_code + code_index) = encr_msg[word_index][char_index];
                if (char_index == 6){
                    word_index++;
                    char_index = 0;
                }
                else{
                    char_index++;
                }
            }
            else{
                *(sc_code + code_index) = '0';
            }
            code_index++;
        }
    }
    *(sc_code + 256) = '\0';
    return sc_code;
}

char *read_code(char *code){
    char decr[26]; // -> (256 - 25*3 - 1) // 7 = 25
    int code_index = 0, char_index = 0, decr_index = 0;
    char *bitwise = (char*)malloc(sizeof(char) * 8);
    for (int i = 0; i < 16; i++){
        for (int j = 0; j < 16; j++){
            if ((i >= 11 && j <= 4) || (i <= 4 && j <= 4) || (i <= 4 && j >= 11) || (i == 15 && j == 15)){
                code_index++;
                continue;
            }
            else {
                *(bitwise + char_index) = *(code + code_index);
                char_index++;
                if (char_index == 7){
                    char decr_char = xor_decrypt(bitwise);
                    decr[decr_index] = decr_char;
                    char_index = 0;
                    decr_index++;
                    free(bitwise);
                    bitwise = NULL;
                    if (decr_char == '\0')
                    {
                        char *decr_msg = (char*)malloc(sizeof(char) * decr_index);
                        strcpy(decr_msg, decr);
                        return decr_msg;
                    }
                    bitwise = (char*)malloc(sizeof(char) * 8);
                }
            }
            code_index++;
        }
    }
}

char *compress(char *code){
    char *hex_code = (char*)malloc(sizeof(char) * 65);
    for (int i = 0; i < 64; i++){
        int bin_sum = 0, power = 8;
        for (int j = i * 4; j < (i + 1) * 4; j++){
            bin_sum += power * (*(code + j) - 48);
            power /= 2;
        }
        if (bin_sum < 10){
            *(hex_code + i) = (char)(bin_sum + 48);
        }
        else{
            *(hex_code + i) = (char)((bin_sum - 10) + 65);
        }
    }
    *(hex_code + 64) = '\0';
    return hex_code;
}

char *decompress(char *code){
    char *bin_code = (char*)malloc(sizeof(char) * 257);
    for (int i = 0; i < 64; i++){
        int bin_sum = 0;
        if (*(code + i) < 65){
            bin_sum = *(code + i) - 48;
        }
        else{
            bin_sum = (*(code + i) - 65) + 10;
        }
        for (int j = i * 4 + 3; j >= i * 4; j--){
            int bit = bin_sum % 2;
            if (bit == 1){
                *(bin_code + j) = '1';
            }
            else{
                *(bin_code + j) = '0';
            }
            bin_sum /= 2;
        }
    }
    *(bin_code + 256) = '\0';
    return bin_code;
}

int min_int(int a, int b, int c){
    if (a >= b){
        if (b >= c){
            return c;
        }
        else{
            return b;
        }
    }
    else
    {
        if (a >= c){
            return c;
        }
        else{
            return a;
        }
    }
}

int calc_ld(char *sandy, char *cima){
    if (strlen(sandy) == 0){
        return strlen(cima);
    }
    else if (strlen(cima) == 0){
        return strlen(sandy);
    }

    char new_sandy[strlen(sandy)];
    strncpy(new_sandy, &sandy[1], strlen(sandy) - 1);
    new_sandy[strlen(sandy) - 1] = '\0';
    
    char new_cima[strlen(cima)];
    strncpy(new_cima, &cima[1], strlen(cima) - 1); 
    new_cima[strlen(cima) - 1] = '\0';  

    if (sandy[0] == cima[0]){
        return calc_ld(new_sandy, new_cima);
    }
    else{
        return 1 + min_int(calc_ld(sandy, new_cima), calc_ld(new_sandy, cima), calc_ld(new_sandy, new_cima));
    }
}
