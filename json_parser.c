//
// Created by user on 08.06.2021.
//

#include "json_parser.h"
#include <string.h>
#include <stdlib.h>

#ifdef JSON_PARSER_USE_SIMPLIFY_TABLE

#include "json_simplify.h"

extern const uint8_t json_decode[2][256];
#else
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
const uint8_t json_decode[2][256] =
        {
                {
                        //снаружи кавычек
                        TEND, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, //0x0_
                        SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, //0x1_
                        SKIP, SKIP, J_QT, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, J_CM, J_DT, J_DT, SKIP, //0x2_
                        J_DT, J_DT, J_DT, J_DT, J_DT, J_DT, J_DT, J_DT, J_DT, J_DT, J_CL, SKIP, SKIP, SKIP, SKIP, SKIP, //0x3_
                        SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, //0x4_
                        SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, J_OO, SKIP, J_OC, SKIP, SKIP, //0x5_
                        SKIP, J_DT, SKIP, SKIP, SKIP, J_DT, J_DT, SKIP, SKIP, SKIP, SKIP, SKIP, J_DT, SKIP, J_DT, SKIP, //0x6_
                        SKIP, SKIP, J_DT, J_DT, J_DT, J_DT, SKIP, SKIP, SKIP, SKIP, SKIP, J_OO, SKIP, J_OC, SKIP, SKIP, //0x7_
                        // 0     1     2     3     4     5     6     7     8     9     a     b     c     d     e     f
                        // unicode
                        SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, //0x8_
                        SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, //0x9_
                        SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP,
                        SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP,
                        SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP,
                        SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP,
                        SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP,
                        SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP

                },
                {
                        //внутри кавычек
                        TEND, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, //0x0_
                        SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, SKIP, //0x1_
                        J_DT, J_DT, J_QT, J_DT, J_DT, J_DT, J_DT, J_DT, J_DT, J_DT, J_DT, J_DT, J_DT, J_DT, J_DT, J_DT, //0x2_
                        J_DT, J_DT, J_DT, J_DT, J_DT, J_DT, J_DT, J_DT, J_DT, J_DT, J_DT, J_DT, J_DT, J_DT, J_DT, J_DT, //0x3_
                        J_DT, J_DT, J_DT, J_DT, J_DT, J_DT, J_DT, J_DT, J_DT, J_DT, J_DT, J_DT, J_DT, J_DT, J_DT, J_DT, //0x4_
                        J_DT, J_DT, J_DT, J_DT, J_DT, J_DT, J_DT, J_DT, J_DT, J_DT, J_DT, J_DT, J_SL, J_DT, J_DT, J_DT, //0x5_
                        J_DT, J_DT, J_DT, J_DT, J_DT, J_DT, J_DT, J_DT, J_DT, J_DT, J_DT, J_DT, J_DT, J_DT, J_DT, J_DT, //0x6_
                        J_DT, J_DT, J_DT, J_DT, J_DT, J_DT, J_DT, J_DT, J_DT, J_DT, J_DT, J_DT, J_DT, J_DT, J_DT, J_DT, //0x7_
                        // 0     1     2     3     4     5     6     7     8     9     a     b     c     d     e     f
                        // unicode
                        J_UD, J_UD, J_UD, J_UD, J_UD, J_UD, J_UD, J_UD, J_UD, J_UD, J_UD, J_UD, J_UD, J_UD, J_UD, J_UD, //0x8_
                        J_UD, J_UD, J_UD, J_UD, J_UD, J_UD, J_UD, J_UD, J_UD, J_UD, J_UD, J_UD, J_UD, J_UD, J_UD, J_UD, //0x9_
                        J_UD, J_UD, J_UD, J_UD, J_UD, J_UD, J_UD, J_UD, J_UD, J_UD, J_UD, J_UD, J_UD, J_UD, J_UD, J_UD, //0xa_
                        J_UD, J_UD, J_UD, J_UD, J_UD, J_UD, J_UD, J_UD, J_UD, J_UD, J_UD, J_UD, J_UD, J_UD, J_UD, J_UD, //0xb_
                        J_U2, J_U2, J_U2, J_U2, J_U2, J_U2, J_U2, J_U2, J_U2, J_U2, J_U2, J_U2, J_U2, J_U2, J_U2, J_U2, //0xc_
                        J_U2, J_U2, J_U2, J_U2, J_U2, J_U2, J_U2, J_U2, J_U2, J_U2, J_U2, J_U2, J_U2, J_U2, J_U2, J_U2, //0xd_
                        J_U3, J_U3, J_U3, J_U3, J_U3, J_U3, J_U3, J_U3, J_U3, J_U3, J_U3, J_U3, J_U3, J_U3, J_U3, J_U3, //0xe_
                        J_U4, J_U4, J_U4, J_U4, J_U4, J_U4, J_U4, J_U4, J_UE, J_UE, J_UE, J_UE, J_UE, J_UE, J_UE, J_UE  //0xf_
                }
        };
#endif


int json_parse_init(Json_Parser_T *parser, Json_Token_T *keys, uint32_t keys_max_size) {
    parser->open_quote = false;
    parser->level = 0;
    parser->key_length = 0;
    parser->keys_max_size = keys_max_size - 1;
    parser->keys = keys;
    parser->root_type = JSON_BAD;
    parser->status = JSON_PARSER_IN_PROGRESS;
    // for (int i=0;i<keys_max_size,)
}

uint8_t json_parser_detect_type(uint8_t *check_value) {
    switch (check_value[0]) {
        case '{':
            return JSON_OBJECT;
        case '[':
            return JSON_ARRAY;
        case '\"':
            return JSON_STRING;
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        case '-':
            // fixme: полная проверка?
            return JSON_NUMBER;
        case 'n':
            return JSON_NULL; //if (strncmp("null", (char *) check_value, 4) == 0)
        case 't':
        case 'f':
            return JSON_BOOL;
    }
    return JSON_BAD;
}

// fixme: для сжатия нужно добавить многое, сейчас частями не умеет
void json_parse_buffer(uint8_t *in_data, uint32_t length, Json_Parser_T *parser) {

    uint32_t in_data_pos = 0;
    uint32_t current_key = 0;
    uint32_t start_data = 0;

    switch (in_data[0]) {
        case '{':
            parser->root_type = JSON_OBJECT;
            break;
        case '[':
            parser->root_type = JSON_ARRAY;
            break;
        default:
            parser->root_type = JSON_BAD;
            parser->status = JSON_PARSER_ERROR_JSON_START;
            return;
    }

    parser->data = in_data;
    Json_Token_T *keys = parser->keys;

    while (parser->status == JSON_PARSER_IN_PROGRESS) {
        switch (json_decode[parser->open_quote][in_data[in_data_pos]]) {
            case TEND:
                // конец нультерминированной строки
                parser->status = JSON_PARSER_NULL_TER;
                break;
            case J_QT: // "
                parser->open_quote = !parser->open_quote;
                break;
            case J_SL: // обработка \ (escape) просто пропускаем
                in_data_pos++;
                break;
            case J_OO:
                parser->level++;
                break;
            case J_OC:
                parser->level--;
                if (parser->level == 0) parser->status = JSON_PARSER_COMPLETE;
                break;
            case J_CM: // запятая
                if (parser->level == 1) {
                    keys[current_key++].key_start = start_data + 1;
                    start_data = in_data_pos;
                    if (current_key >= (parser->keys_max_size)) {
                        parser->status = JSON_PARSER_ERROR_TOO_MANY_KEYS;
                        return;
                    }
                }
                break;
            case J_CL: //двоеточие
                if (parser->level == 1) {
                    keys[current_key].key_length = in_data_pos - start_data - 1;
                    keys[current_key].value_start = in_data_pos + 1;
                }
                break;
        }

        in_data_pos++;
        if (in_data_pos >= length) break;
    }

    // обработка хвоста
    keys[current_key++].key_start = start_data + 1;
    keys[current_key].key_start = in_data_pos;
    parser->key_length = current_key;

    // коррекция длины
    if (parser->root_type == JSON_OBJECT) {
        for (int i = 0; i < parser->key_length; i++) {
            keys[i].value_length = keys[i + 1].key_start - keys[i].value_start - 1;
            keys[i].key_start++;
            keys[i].key_length -= 2;
        }
    } else {
        // parser->root_type == JSON_ARRAY
        for (int i = 0; i < parser->key_length; i++) {
            keys[i].value_start = keys[i].key_start;
            keys[i].value_length = keys[i + 1].key_start - keys[i].value_start - 1;
        }
    }

    for (int i = 0; i < parser->key_length; i++) {
        keys[i].type = json_parser_detect_type(in_data + keys[i].value_start);
        if (keys[i].type == JSON_BAD) {
            parser->status = JSON_PARSER_ERROR_JSON_DATA;
            return;
        }
        if (keys[i].type == JSON_STRING) {
            keys[i].value_start++;
            keys[i].value_length -= 2;
        }
    }
    // check result
    if (parser->level!=0) parser->status = JSON_PARSER_ERROR_JSON_DATA;
    if (parser->open_quote) parser->status = JSON_PARSER_ERROR_JSON_DATA;
}

int json_parser_get_id_by_name(Json_Parser_T *parser, uint8_t *name, uint8_t name_length) {
    for (int i = 0; i < parser->key_length; i++) {
        if (parser->keys[i].key_length == name_length) {
            if (strncmp((char *) parser->data + parser->keys[i].key_start,
                        (char *) name, name_length) == 0) {
                return i;
            }
        }
    }
    return -1;
}

char *json_parser_get_string_by_name(Json_Parser_T *parser, char *name, int *out_len, int max_len) {
    uint8_t name_length = strlen(name);
    for (int i = 0; i < parser->key_length; i++) {
        if (parser->keys[i].key_length == name_length) {
            uint8_t *pointer = parser->data + parser->keys[i].key_start;
            if (strncmp((char *) pointer, name, name_length) == 0) {
                *out_len = (int) parser->keys[i].value_length;
                if (max_len > 0) {
                    if (parser->keys[i].value_length > max_len) *out_len = max_len;
                }
                return (char *) parser->data + parser->keys[i].value_start;
            }
        }
    }
    *out_len = -1;
    return NULL;
}

uint32_t json_parser_get_long_by_name(Json_Parser_T *parser, char *name) {
    int temp_len;
    char *temp = json_parser_get_string_by_name(parser, name, &temp_len, 0);
    if (temp_len > 0) {
        return atol(temp);
    } else {
        return 0;
    }
}

// возвращает длину массива json
int json_parser_get_elements_len(const uint8_t *data_start, uint32_t data_length) {
    bool open_quote = false;
    uint32_t status = JSONS_OK;
    uint32_t level = 0;
    int el_count = 0;

    for (int i = 0; i < data_length; i++) {
        switch (json_decode[open_quote][data_start[i]]) {
            case TEND:
                // конец нультерминированной строки
                status = JSON_PARSER_NULL_TER;
                break;
            case J_QT: // "
                open_quote = !open_quote;
                break;
            case J_SL: // обработка \ (escape) просто пропускаем
                i++;
                break;
            case J_OO:
                level++;
                break;
            case J_OC:
                level--;
                if (level == 0) status = JSON_PARSER_COMPLETE;
                break;
            case J_CM: // запятая
                if (level == 1) el_count++;
                break;
        }
    }
    if (status != JSON_PARSER_COMPLETE) el_count = -1;

    return ++el_count;
}

// возвращает следующий элемент из массива json,
// указатель должен стоять на начало элемента
/** пример - печать всех элементов массива
    char *pointer = json_array_data + 1; // указатель на начало первого
    uint32_t data_len = json_array_length - 1; // длина, соответсвенно, -1
    int out_len;
    do {
        pointer = json_parser_get_next_array_element((uint8_t *) pointer, data_len, &out_len);
        if (pointer != NULL){
            printf("%.*s\n", out_len, pointer);
            pointer += out_len + 1;
            data_len -= out_len + 1;
        }
    } while (pointer != NULL);
**/
char *json_parser_get_next_array_element(const uint8_t *data_start, uint32_t data_length, int *out_len) {
    bool open_quote = false;
    uint32_t level = 0;
    for (int i = 0; i < data_length; i++) {
        switch (json_decode[open_quote][data_start[i]]) {
            case TEND:
                // конец нультерминированной строки
                return NULL;
            case J_QT: // "
                open_quote = !open_quote;
                break;
            case J_SL: // обработка \ (escape) просто пропускаем
                i++;
                break;
            case J_OO:
                level++;
                break;
            case J_OC:
                level--;
                if (level > 0) {
                    *out_len = i;
                    return (char *) data_start;
                }
                break;
            case J_CM: // запятая
                if (level == 0) {
                    *out_len = i;
                    return (char *) data_start;
                }
                break;
        }
    }

    return NULL;
}


// берет элемент из массива json по номеру, перебирает с начала
char *json_parser_get_array_element(const uint8_t *data_start, uint32_t data_length, uint32_t number, int *out_len) {
    bool open_quote = false;
    uint32_t status = JSONS_OK;
    uint32_t level = 0;
    int el_count = 0;
    int start_pos = 1;

    for (int i = 0; i < data_length; i++) {
        switch (json_decode[open_quote][data_start[i]]) {
            case TEND:
                // конец нультерминированной строки
                status = JSON_PARSER_NULL_TER;
                break;
            case J_QT: // "
                open_quote = !open_quote;
                break;
            case J_SL: // обработка \ (escape) просто пропускаем
                i++;
                break;
            case J_OO:
                level++;
                break;
            case J_OC:
                level--;
                if (level == 0) status = JSON_PARSER_COMPLETE;
                break;
            case J_CM: // запятая
                if (level == 1) {
                    el_count++;
                    if ((el_count - 1) == number) {
                        *out_len = i - start_pos;
                        return (char *) data_start + start_pos;
                    }
                    start_pos = i + 1;
                }
                break;
        }
    }

    if (status == JSON_PARSER_COMPLETE) {
        if (el_count == number) {
            *out_len = (int) data_length - start_pos - 1;
            return (char *) data_start + start_pos;
        }
    };
    return NULL;
}

