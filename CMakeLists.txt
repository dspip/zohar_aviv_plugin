cmake_minimum_required(VERSION 3.10)

set(PROJECT test)
set(VERSION 0.0.1)

project("${PROJECT}" VERSION "${VERSION}")


find_package(PkgConfig REQUIRED)

pkg_check_modules(GST REQUIRED IMPORTED_TARGET
    "gstreamer-1.0"
    "gstreamer-base-1.0"
    "gstreamer-app-1.0"
    "gstreamer-rtp-1.0"
    "gstreamer-audio-1.0"
    "gstreamer-video-1.0"
    "gstreamer-pbutils-1.0"
    "gstreamer-tag-1.0"
    "gstreamer-webrtc-1.0"
    "gstreamer-sdp-1.0"
)

add_subdirectory("plugins")