set(Spectrum_files
    main.cpp
)

add_executable(spectrum ${Spectrum_files})

target_compile_features(spectrum PUBLIC cxx_std_17)