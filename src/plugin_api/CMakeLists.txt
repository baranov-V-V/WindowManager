target_sources(main
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/PluginApi.h
    ${CMAKE_CURRENT_SOURCE_DIR}/NewPluginApi.h
    ${CMAKE_CURRENT_SOURCE_DIR}/PluginApiClasses.h
    
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/PluginApi.cpp
)