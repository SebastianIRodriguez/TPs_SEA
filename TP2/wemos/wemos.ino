/**
 * @file wemos.ino
 * @author Luciano Raffagnini - Sebastián Rodriguez
 * @brief
 * @version 0.1
 * @date 2024-06-11
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "sistema.h"

void setup()
{
  // Esperamos que la KL arranque todo
  delay(2000);
  sistema_begin();
}

void loop()
{
  sistema_run();
}