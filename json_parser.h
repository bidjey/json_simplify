//
// Created by user on 08.06.2021.
//

#ifndef TEST_JSON_JSON_PARSER_H
#define TEST_JSON_JSON_PARSER_H

#include <stdint.h>
#include <stdbool.h>

#define JSON_PARSER_USE_SIMPLIFY_TABLE

// fixme: возможно придти от ссылок внутри буффера на прямые???
typedef struct {
    uint32_t key_start; // позиция в общем буфере, только текст ключа
    uint32_t key_length;
    uint32_t value_start;
    uint32_t value_length;
    uint8_t type;
} Json_Token_T;


enum json_object_type {
    JSON_BAD,
    JSON_STRING,
    JSON_OBJECT,
    JSON_BOOL,
    JSON_NUMBER,
    JSON_ARRAY,
    JSON_NULL,
    JSON_TYPE_LEN
};

enum json_parser_state {
    JSON_PARSER_NO_INIT = 0,
    JSON_PARSER_IN_PROGRESS = 1,
    JSON_PARSER_COMPLETE= 2,
    JSON_PARSER_END_LEN = 3,
    JSON_PARSER_NULL_TER = 99,
    JSON_PARSER_ERROR = 100,
    JSON_PARSER_ERROR_TOO_MANY_KEYS = 101,
    JSON_PARSER_ERROR_JSON_START = 102,
    JSON_PARSER_ERROR_JSON_DATA = 103,
    JSON_PARSER_ERROR_UNKNOWN = 199 // JSONS_ERROR_UNKNOWN
};

typedef struct {
    uint32_t status;

    bool open_quote;
    uint32_t level;
    uint32_t key_length;
    uint32_t keys_max_size;
    uint8_t root_type;
    Json_Token_T *keys;
    uint8_t *data;
} Json_Parser_T;


//
int json_parse_init(Json_Parser_T *parser, Json_Token_T *keys, uint32_t keys_max_size);

//
void json_parse_buffer(uint8_t *in_data, uint32_t length, Json_Parser_T *parser);

//
int json_parser_get_id_by_name(Json_Parser_T *parser, uint8_t *name, uint8_t name_length);

char *json_parser_get_string_by_name(Json_Parser_T *parser, char *name, int *out_len, int max_len);
uint32_t json_parser_get_long_by_name(Json_Parser_T *parser, char *name);

// возвращает следующий элемент из массива json,
// указатель должен стоять на начало элемента
char *json_parser_get_next_array_element(const uint8_t *data_start, uint32_t data_length, int *out_len);

// определяет тип элемента enum json_object_type
uint8_t json_parser_detect_type(uint8_t *check_value);

// возвращает длину массива json
int json_parser_get_elements_len(const uint8_t *data_start, uint32_t data_length);

// берет элемент из массива json по номеру, перебирает с начала, поэтому медленный!!!
char *json_parser_get_array_element(const uint8_t *data_start, uint32_t data_length, uint32_t number, int *out_len);

#endif //TEST_JSON_JSON_PARSER_H
