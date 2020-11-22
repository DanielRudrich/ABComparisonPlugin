cmake_minimum_required (VERSION 3.15)

set (CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

project (ABComparison VERSION 1.4.0)
set (CMAKE_CXX_STANDARD 17)

add_subdirectory (JUCE)


if (DEFINED VST2PATH)
    juce_set_vst2_sdk_path (${VST2PATH})
else()
    message (FATAL_ERROR "You have to specify the VST2PATH variable with the path to the VST2 SDK")
endif()


juce_add_plugin (ABComparison
    PLUGIN_MANUFACTURER_CODE daru
    PLUGIN_CODE ABCo
    COMPANY_NAME "Daniel Rudrich"
    PRODUCT_NAME "ABComparison"
    FORMATS VST
    COPY_PLUGIN_AFTER_BUILD TRUE)

juce_generate_juce_header (ABComparison)

target_sources (ABComparison PRIVATE
    Source/PluginEditor.cpp
    Source/PluginEditor.h
    Source/PluginProcessor.cpp
    Source/PluginProcessor.h
    Source/OSCReceiverPlus.h
    Source/SettingsComponent.h)

target_compile_definitions (ABComparison PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0   
    JUCE_DISPLAY_SPLASH_SCREEN=0
    JUCE_VST3_CAN_REPLACE_VST2=0)

target_link_libraries (ABComparison PRIVATE
    juce::juce_audio_utils
    juce::juce_osc)

set_property (TARGET ABComparison PROPERTY
    MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
