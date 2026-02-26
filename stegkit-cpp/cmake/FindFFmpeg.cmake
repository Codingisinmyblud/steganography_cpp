# Find FFmpeg library
#
# Offers:
# FFmpeg::avcodec
# FFmpeg::avformat
# FFmpeg::avutil
# FFmpeg::swscale

include(FindPackageHandleStandardArgs)

foreach(COMPONENT ${FFmpeg_FIND_COMPONENTS})
    string(TOUPPER ${COMPONENT} UPPER_COMPONENT)
    string(TOLOWER ${COMPONENT} LOWER_COMPONENT)

    find_path(FFmpeg_${COMPONENT}_INCLUDE_DIR
        NAMES lib${LOWER_COMPONENT}/${LOWER_COMPONENT}.h
        PATHS /usr/include /usr/local/include /opt/homebrew/include $ENV{HOME}/.local/include
    )

    find_library(FFmpeg_${COMPONENT}_LIBRARY
        NAMES ${LOWER_COMPONENT}
        PATHS /usr/lib /usr/local/lib /opt/homebrew/lib $ENV{HOME}/.local/lib
    )

    if(FFmpeg_${COMPONENT}_INCLUDE_DIR AND FFmpeg_${COMPONENT}_LIBRARY)
        set(FFmpeg_${COMPONENT}_FOUND TRUE)
        if(NOT TARGET FFmpeg::${LOWER_COMPONENT})
            add_library(FFmpeg::${LOWER_COMPONENT} UNKNOWN IMPORTED)
            set_target_properties(FFmpeg::${LOWER_COMPONENT} PROPERTIES
                IMPORTED_LOCATION "${FFmpeg_${COMPONENT}_LIBRARY}"
                INTERFACE_INCLUDE_DIRECTORIES "${FFmpeg_${COMPONENT}_INCLUDE_DIR}"
            )
        endif()
    else()
        set(FFmpeg_${COMPONENT}_FOUND FALSE)
    endif()

    list(APPEND FFmpeg_REQUIRED_VARS FFmpeg_${COMPONENT}_LIBRARY FFmpeg_${COMPONENT}_INCLUDE_DIR)
endforeach()

find_package_handle_standard_args(FFmpeg
    REQUIRED_VARS ${FFmpeg_REQUIRED_VARS}
    HANDLE_COMPONENTS
)
