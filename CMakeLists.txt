cmake_minimum_required (VERSION 3.15)

set (CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

project (ABComparison VERSION 1.5.0)
set (CMAKE_CXX_STANDARD 17)

add_subdirectory (JUCE)

set (ABC_TARGETS Standalone) # add VST3 here if you want to build it

if (DEFINED VST2PATH)
    juce_set_vst2_sdk_path (${VST2PATH})
    list (APPEND ABC_TARGETS VST)
else ()
    message (WARNING "VST2PATH not defined, not building VST2 version.")
endif ()

juce_add_plugin (
    ABComparison
    PLUGIN_MANUFACTURER_CODE
    daru
    PLUGIN_CODE
    ABCo
    COMPANY_NAME
    "Daniel Rudrich"
    PRODUCT_NAME
    "ABComparison"
    BUNDLE_ID
    "de.danielrudrich.ABComparison"
    FORMATS
    ${ABC_TARGETS}
    COPY_PLUGIN_AFTER_BUILD
    TRUE
)

juce_generate_juce_header (ABComparison)

target_sources (
    ABComparison
    PRIVATE Source/PluginEditor.cpp
            Source/PluginEditor.h
            Source/PluginProcessor.cpp
            Source/PluginProcessor.h
            Source/OSCReceiverPlus.h
            Source/SettingsComponent.h
)

target_compile_definitions (
    ABComparison PUBLIC JUCE_WEB_BROWSER=0 JUCE_USE_CURL=0 JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries (ABComparison PRIVATE juce::juce_audio_utils juce::juce_osc)

set_property (
    TARGET ABComparison PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
)
