//
// Created by bg on 19.07.2020.
//

#ifndef __JSON_SIMPLIFY_H
#define __JSON_SIMPLIFY_H
#include <stdbool.h>
#include <stdint.h>

#define JSON_SIMPLE_ESCAPE_PYTHON //символы \n \r \t \f \b будут обработаны как в python
//#define JSON_SIMPLE_ENABLE_COMPRESS //
// #define JSON_UNICODE_4BYTE_PROCESS // для полной обработки 4 байтных символов unicode

#ifdef JSON_SIMPLE_ENABLE_COMPRESS
#define _put_byte(b) in_json->put_byte(b)
#else
#define _put_byte(b)  in_json->out_data[in_json->summary_length++] = b
#endif

enum JSONS_PROCESSING{
    SKIP, // пропуск символа
    TEND, // конец обработки \0
    J_DT, // 0-9 . -  false true null в таблице [0],  данные для сохранения
    J_CM, // запятая
    J_CL, // двоеточие
    J_QT, // "
    J_OO, // { и [
    J_OC, // } и ]

    J_U2, // utf8 2 байта
    J_U3, // utf8 3 байта
    J_U4, // utf8 4 байта
    J_SL, // /  backslash, escape, поддерживаются  \b \t \n \f \r \\ \" \/  \uXXXX

    J_UD, // данные unicode, должны идти после символа J_U2, J_U3 или J_U4
    J_UE, // ошибочный символ json
};


enum JSONS_PARSE_ERROR {
    JSONS_OK = 0,
    JSONS_INVALID_ESCAPE =  50,
    JSONS_UTF8_WRONG_SYMBOL = 51,
    JSONS_ERROR_UNKNOWN = 199
};

enum JSON_PARSE_STATUS{
    JSONS_BODY,
    JSONS_ESCAPE,
    JSONS_ESCAPE_U,
    JSONS_UTF8_2b,
    JSONS_UTF8_3b,
    JSONS_UTF8_4b
};

// прототип функции упаковки ссылка на данные, длинна
// при ее вызове будет уставновлен out_position = 0
typedef void (*Put_byte_Func_T)(uint8_t);
typedef bool (*Pack_Func_T)(uint8_t * data, uint32_t length) ;
typedef char* (*UnPack_Func_T)(uint8_t * data, uint32_t start, uint32_t length) ;

typedef struct{
    uint8_t * out_data; //ссылка на выход
    uint32_t summary_length;
    uint8_t status;

    bool in_progress;
    uint32_t error_code;

    uint8_t tail[4];
    uint32_t tail_position;
    //
    Pack_Func_T pack_func;
    Put_byte_Func_T put_byte;
    //
    bool open_quote; // открыта ли скобка "
}Json_Simplify_T;

void json_simplify_init(void);
void json_simplify_start(Json_Simplify_T *in_json, uint8_t *out);
void json_simplify_add_utf8(Json_Simplify_T *in_json, const uint8_t *in_data_new, uint32_t length);

#endif //__JSON_SIMPLIFY_H
