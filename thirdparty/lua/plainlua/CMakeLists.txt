
set (AX_LUA_VERSION "5.4.4")

set(LUA_SRC_PATH ${CMAKE_CURRENT_LIST_DIR})

set(lib_name plainlua)
set(target_name ${lib_name})

project(${lib_name})

include(AXPlatform)

aux_source_directory(${LUA_SRC_PATH} LUA_CORE)
list(REMOVE_ITEM LUA_CORE ${LUA_SRC_PATH}/lua.c ${LUA_SRC_PATH}/luac.c)

set(LUA_BUILD_AS_DLL ${BUILD_SHARED_LIBS})

if(WIN32)
    set(LUA_BUILD_AS_DLL TRUE)
    add_library(${target_name} SHARED ${LUA_CORE} ${LUA_SRC_PATH}/plainlua.def)
    if(WINRT)
        target_link_libraries(${target_name} PRIVATE runtimeobject)
    endif()
else()
    add_library(${target_name} ${LUA_CORE})
endif()

target_compile_definitions(${target_name} 
    PRIVATE _CRT_SECURE_NO_WARNINGS 
    PRIVATE _WINSOCK_DEPRECATED_NO_WARNINGS
    )

if(${AX_LUA_VERSION} VERSION_GREATER_EQUAL "5.4.0") # !important: traditional lua bindings solution tolua++ not support yet
  target_compile_definitions(${target_name} 
      PUBLIC LUA_COMPAT_APIINTCASTS=1
      PUBLIC LUA_COMPAT_MATHLIB=1
    )
elseif(${AX_LUA_VERSION} VERSION_GREATER_EQUAL "5.3.0")
  target_compile_definitions(${target_name} 
      PUBLIC LUA_COMPAT_5_1=1
      PUBLIC LUA_COMPAT_5_2=1
      PUBLIC LUA_COMPAT_MODULE=1
    )
elseif(${AX_LUA_VERSION} VERSION_GREATER_EQUAL "5.2.0")
  target_compile_definitions(${target_name} 
      PUBLIC LUA_COMPAT_ALL=1
      PUBLIC LUA_COMPAT_MODULE=1
    )
endif()

if(LUA_BUILD_AS_DLL)
    target_compile_definitions(${target_name} PUBLIC LUA_BUILD_AS_DLL=1)
endif()

set_target_properties(${target_name} PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES "${LUA_SRC_PATH}"
)

set_target_properties(${target_name}
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    FOLDER "ThirdParty"
)

if(WIN32 OR MACOSX)
  
  function (add_lua_bin target_name src_files)
    add_executable(${target_name} ${src_files})
    if(${AX_LUA_VERSION} VERSION_GREATER_EQUAL "5.4.0") # !important: traditional lua bindings solution tolua++ not support yet
      target_compile_definitions(${target_name} PUBLIC LUA_COMPAT_APIINTCASTS=1)
    elseif(${AX_LUA_VERSION} VERSION_GREATER_EQUAL "5.3.0")
      target_compile_definitions(${target_name} 
        PUBLIC LUA_COMPAT_5_1=1
        PUBLIC LUA_COMPAT_5_2=1
        )
    elseif(${AX_LUA_VERSION} VERSION_GREATER_EQUAL "5.2.0")
      target_compile_definitions(${target_name} PUBLIC LUA_COMPAT_ALL=1)
    endif()

    target_link_libraries(${target_name} plainlua)

    set_target_properties(${target_name}
        PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
        FOLDER "Tools"
    )
  endfunction()

  # tool:luac the lua bytecode compiler
  add_lua_bin(luac "${LUA_SRC_PATH}/luac.c;${LUA_SRC_PATH}/ldump.c;${LUA_SRC_PATH}/lopcodes.c")

  # tool:lua the lua interpreter
  add_lua_bin(lua "${LUA_SRC_PATH}/lua.c")
endif()
