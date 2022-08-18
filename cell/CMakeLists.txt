cmake_minimum_required(VERSION 3.12)

add_library(cell INTERFACE)

target_sources(cell INTERFACE
  ${CMAKE_CURRENT_LIST_DIR}/envelope.c
  ${CMAKE_CURRENT_LIST_DIR}/oscillator.c
  ${CMAKE_CURRENT_LIST_DIR}/sequencer.c
  ${CMAKE_CURRENT_LIST_DIR}/spawner.c
  ${CMAKE_CURRENT_LIST_DIR}/utility.c
  ${CMAKE_CURRENT_LIST_DIR}/preset.c
  ${CMAKE_CURRENT_LIST_DIR}/fr32/fr32.c


)