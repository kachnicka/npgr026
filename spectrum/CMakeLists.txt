set(Spectrum_files
    main.cpp
    SpectralData.h Spectrum.h Results.h Sampler.h ColorSpace.h)

add_executable(spectrum ${Spectrum_files})

target_compile_features(spectrum PUBLIC cxx_std_17)
target_link_libraries(spectrum PRIVATE glm::glm)