//
// Created by bg on 26.05.2021.
//

#include "json_simplify.h"
#include "unicode.h"
#include <stddef.h>
#include <string.h>

// value = false / null / true / object / array / number / string

static uint8_t *default_put_data;
static uint32_t *default_put_counter;

void default_put_byte(uint8_t byte) {
    default_put_data[(*default_put_counter)++]=byte;
}

// попробовал переносить в ccmram +1% в обработке
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



//, Pack_Func_T pack Json_Token_T *keys,
void json_simplify_start(Json_Simplify_T *in_json, uint8_t *out) {
    in_json->out_data = out;
    in_json->summary_length = 0;
    in_json->status = JSONS_BODY;
    in_json->in_progress = true;
    in_json->error_code = JSONS_OK;
    in_json->open_quote = false;
    in_json->tail[0] = 0; // обнуляем хвост
    in_json->tail_position = 0;
    // pack, сейчас не используется
    in_json->pack_func = NULL;
    // default settings
    default_put_counter = &in_json->summary_length;
    default_put_data = in_json->out_data;
    in_json->put_byte = default_put_byte;
}

void json_simplify_process_char(Json_Simplify_T *in_json, const uint8_t in_char) {
    uint8_t switch_key = json_decode[in_json->open_quote][in_char];
    switch (switch_key) {
        case SKIP:
            break;
        case TEND:
            // конец нультерминированной строки
            in_json->in_progress = false;
            _put_byte(0);
            return;

        case J_SL: // обработка \ (escape)
            in_json->status = JSONS_ESCAPE;
            break;

        case J_OO:
        case J_OC:
        case J_CM: // запятая
        case J_CL: //двоеточие
        case J_DT:
            _put_byte(in_char);
            break;

            // json обработка, объекты и массивы
        case J_QT:
            in_json->open_quote = !in_json->open_quote;
            _put_byte(in_char);
            break;
            // обработка unicode
        case J_U2:
            in_json->status = JSONS_UTF8_2b;
            in_json->tail[0] = in_char;
            break;
        case J_U3:
            in_json->status = JSONS_UTF8_3b;
            in_json->tail_position = 0;
            in_json->tail[0] = in_char;
            break;
        case J_U4:
            in_json->status = JSONS_UTF8_4b;
            in_json->tail_position = 0;
            in_json->tail[0] = in_char;
            break;
            // ошибочные симврлы utf8
        case J_UD:
        case J_UE:
            in_json->error_code = JSONS_UTF8_WRONG_SYMBOL;
            in_json->in_progress = false;
            return;
        default:
            // не должны сюда попадать
            in_json->error_code = JSONS_ERROR_UNKNOWN;
            in_json->in_progress = false;
            return;
    }
}

void json_simplify_process_escape(Json_Simplify_T *in_json, const uint8_t in_char) {
    switch (in_char) {
#ifdef JSON_SIMPLE_ESCAPE_PYTHON
        // оставлено для теста, совместимо с python json
        case 'b':
        case 'f':
        case 'n':
        case 'r':
        case 't':
#endif
            // это останется в любом случае
        case '\"':
        case '\\':
            _put_byte('\\');
            _put_byte(in_char);
            in_json->status = JSONS_BODY;
            break;
            // заменяем на /
        case '/':
            _put_byte('/');
            in_json->status = JSONS_BODY;
            break;
#ifndef JSON_SIMPLE_ESCAPE_PYTHON
            // пропускам символы форматирования
        case 'b':
        case 'f':
        case 'n':
        case 'r':
        case 't':
            in_json->status = JSONS_BODY;
            break;
#endif
            // unicode:
        case 'u':
            in_json->status = JSONS_ESCAPE_U;
            in_json->tail_position = 0;
            break;
        default:
            in_json->error_code = JSONS_INVALID_ESCAPE;
            in_json->in_progress = false;
            return;
    }
}

void json_simplify_add_utf8(Json_Simplify_T *in_json, const uint8_t *in_data_new, uint32_t length) {

    uint32_t unicode_char;
    uint8_t cp866_char;
    uint32_t i = 0;

    while (in_json->in_progress) {
        switch (in_json->status) {
            case JSONS_BODY:
                json_simplify_process_char(in_json, in_data_new[i]);
                break;
            case JSONS_UTF8_2b:
                unicode_char = ((in_json->tail[0] & 31u) << 6u) + (in_data_new[i] & 63);
                in_json->status = JSONS_BODY;
                cp866_char = uni_widechar_to_cp866(unicode_char);
                if (cp866_char) _put_byte(cp866_char);
                break;
            case JSONS_UTF8_3b:
                if ((in_json->tail_position) < 1) {
                    in_json->tail[++in_json->tail_position] = in_data_new[i];
                } else {
                    unicode_char =
                            ((in_json->tail[0] & 15u) << 12u) +
                            ((in_json->tail[1] & 63) << 6u) +
                            (in_data_new[i] & 63);
                    cp866_char = uni_widechar_to_cp866(unicode_char);
                    if (cp866_char) _put_byte(cp866_char);
                    in_json->status = JSONS_BODY;
                }
                break;
            case JSONS_UTF8_4b:
                if ((in_json->tail_position) < 2) {
                    in_json->tail[++in_json->tail_position] = in_data_new[i];
                } else {
//                 полный, правильный способ
#ifdef JSON_UNICODE_4BYTE_PROCESS
                    unicode_char =
                            ((in_json->tail[0] & 15u) << 18u) +
                            ((in_json->tail[1] & 63) << 12u) +
                            ((in_json->tail[1] & 63) << 6u) +
                            (in_data_new[i] & 63);
                    cp866_char = uni_widechar_to_cp866(unicode_char);
                    if (cp866_char) _put_byte(cp866_char);
#else
#if UNICODE_NO_CP866_CHAR > 0
                    _put_byte(UNICODE_NO_CP866_CHAR);
#endif
                    in_json->status = JSONS_BODY;
#endif //JSON_UNICODE_4BYTE_PROCESS
                }
                break;
            case JSONS_ESCAPE:
                json_simplify_process_escape(in_json, in_data_new[i]);
                break;
            case JSONS_ESCAPE_U:
                if ((in_json->tail_position) < 3) {
                    in_json->tail[in_json->tail_position++] = in_data_new[i];
                } else {
                    in_json->status = JSONS_BODY;
                    in_json->tail[in_json->tail_position++] = in_data_new[i];
                    unicode_char = uni_escape_hextoi(in_json->tail);

                    if (unicode_char == UNICODE_ERROR_ESCAPE) {
                        // ошибка последовательности
                        in_json->error_code = JSONS_INVALID_ESCAPE;
                        in_json->in_progress = false;
                        return;
                    } else {
                        cp866_char = uni_widechar_to_cp866(unicode_char);
                        // экранируем символы \ и "
                        if (cp866_char == '\"') {
                            _put_byte('\\');
                            _put_byte('\"');
                            break;
                        }
                        if (cp866_char == '\\') {
                            _put_byte('\\');
                            _put_byte('\\');
                            break;
                        }
                        // выкидываем все до пробела
                        if (cp866_char > 31) _put_byte(cp866_char);
                    }
                }
                break;
        }
        i++;
        if (i >= length) break;
    }
}

uint8_t json_simplify_check(Json_Simplify_T *in_json) {

    return 0;
}
