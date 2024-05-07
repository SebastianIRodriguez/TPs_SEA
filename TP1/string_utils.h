/**
 * @file string_utils.h
 * @author Raffagnini - Rodriguez
 * @brief Aquí se encuentran declaradas las funciones que se definen en "string_utils.c"
 * @version 0.1
 * @date 2024-05-05
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef STRING_UTILS_H
#define STRING_UTILS_H

int read_match(const char *origin, const char *match_exp, const char end_c, char *destiny);
int str_replace(char *orig, char *rep, char *with);

#include "string_utils.c"
#endif